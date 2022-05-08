//
// Created by bysou on 2022/5/8.
//

#include "_public.h"

//  程序运行参数结构体
typedef struct TcpArgs {
  int client_type;          //  客户端类型 1-上传文件 | 2-下载文件
  char ip[32];            //  服务器的IP
  unsigned short port;      //  port
  int proc_type;            //  上传成功后的操作类型 1-删除 | 2-移动到备份目录
  char client_path[256];         //  客户端文件根目录
  char client_path_backup[256];  //  客户端备份根目录  proc_type = 2 时有效
  bool bool_upload_sub_path;     //  上传子目录?
  char match_rule[256];               //  文件名匹配规则
  char server_path[256];              //  server根目录
  int scan_time;                //  扫描本地目录周期
  int timeout;                  //  心跳超时时间
  char proc_name[64];            // 进程名，建议用"tcp_file_server_后缀"的方式。
} TcpArgs_t;

//  上传/下载文件的信息
typedef struct FileInfo {
  char file_name[301];
  char m_ModifyTime[21];      // 文件最后一次被修改的时间，即stat结构体的st_mtime成员。
  int file_size;
} FileInfo_t;

//  帮助信息
void _help();

//  xml 参数解析
bool xml_parse(const char *xml_buffer);

//  上传业务函数
int upload_proc();
bool file_info_parse(const char *file_info_xml, FileInfo_t &file_info);
bool recv_file(const int sockfd, const FileInfo_t& file_info);
//  下载业务函数
int download_proc();

//  父进程信号处理函数
void Father_EXIT(int sig);
//  子进程信号处理函数
void Child_EXIT(int signal);

//  登陆信息
bool check_login();

///////////////////////////////
CLogFile logfile;
CTcpServer tcp_server;
TcpArgs_t tcp_args{};
///////////////////////////////


int main(int argc, char *argv[]) {
  //  忽略信号
  CloseIOAndSignal();
  signal(SIGINT, Father_EXIT);
  signal(SIGTERM, Father_EXIT);

  //  参数初始化
  if (argc != 3) {
    _help();
    return -1;
  }

  if (logfile.Open(argv[2], "w") == false) {
    return -2;
  }

  if (tcp_server.InitServer(strtol(argv[1], nullptr, 10)) == false) {
    logfile.Write("server init failed[port = %d]", strtol(argv[1], nullptr, 10));
    return -3;
  }

  while (true) {
    if (tcp_server.Accept() == false) {
      logfile.Write("server accept failed\n");
      Father_EXIT(-1);
    }

    if (fork() > 0) {
      //  父进程关闭客户端转到子进程处理
      tcp_server.CloseClient();
      continue;
    }
    // 子进程重新设置退出信号。
    signal(SIGINT, Child_EXIT);
    signal(SIGTERM, Child_EXIT);

    //  子进程关闭监听
    tcp_server.CloseListen();

    logfile.Write("server accept %s\n", tcp_server.GetIP());

    //  业务逻辑
    //  1. login
    //  2. 上传业务 || 下载业务

    while (true) {
      if (check_login() == false) {
        logfile.Write("[%s] check_login failed\n", tcp_server.GetIP());
        break;
      }
      logfile.Write("[%s] check_login success\n", tcp_server.GetIP());
      std::cout <<tcp_args.client_type;


      if (tcp_args.client_type == 1) {
        upload_proc();
      }

      break;
    }
    Child_EXIT(0);

  }
}

void _help() {
  printf("Using:./tcp_file_server port logfile timeout\n"
         "Example:./tcp_file_server 54321 /tmp/socket_test/tcp_file_server.log 35\n\n");
}

void Father_EXIT(int sig) {
  //  以下代码是为了防止信号处理函数在执行的过程中被信号中断
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("父进程退出，sig=%d。\n", sig);

  tcp_server.CloseListen();    // 关闭监听的socket。

  kill(0, 15);  //  通知所有子进程run

  exit(0);
}

void Child_EXIT(int sig) {
  //  以下代码是为了防止信号处理函数在执行的过程中被信号中断
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("子进程退出，sig=%d。\n", sig);

  tcp_server.CloseClient();    // 关闭客户端的socket。

  exit(0);
}

bool check_login() {
  //  1. 读取信息
  //  2. 解析到tcp_args中
  char recv_buff[1024]{};
  if (tcp_server.Read(recv_buff, 20) == false) {
    logfile.Write("[%s] Read failed\n", tcp_server.GetIP());
    return false;
  }
  logfile.Write("[%s] Read: %s", tcp_server.GetIP(), recv_buff);

  xml_parse(recv_buff);

  if (tcp_args.proc_type == 1 || tcp_args.proc_type == 2) {
    if (tcp_server.Write("login success") == false) {
      logfile.Write("[%s] Write failed\n", tcp_server.GetIP());
      return false;
    }
    logfile.Write("[%s] Write: %s\n", tcp_server.GetIP(), "login success");
  }

  return true;
}

bool xml_parse(const char *xml_buffer) {
  GetXMLBuffer(xml_buffer, "client_type", &tcp_args.client_type);
  GetXMLBuffer(xml_buffer, "proc_type", &tcp_args.proc_type);
  GetXMLBuffer(xml_buffer, "client_path", tcp_args.client_path);
  GetXMLBuffer(xml_buffer, "client_path_backup", tcp_args.client_path_backup);
  GetXMLBuffer(xml_buffer, "bool_upload_sub_path", &tcp_args.bool_upload_sub_path);
  GetXMLBuffer(xml_buffer, "match_rule", tcp_args.match_rule);
  GetXMLBuffer(xml_buffer, "server_path", tcp_args.server_path);
  GetXMLBuffer(xml_buffer, "scan_time", &tcp_args.scan_time);
  GetXMLBuffer(xml_buffer, "timeout", &tcp_args.timeout);
  GetXMLBuffer(xml_buffer, "proc_name", tcp_args.proc_name, 50);
  return true;
}

int upload_proc() {
  //  1. 接收文件信息 文件名 大小... ==> 解析  ==> 修改上传路径为本地路径
  //  2. 读取文件到缓冲区
  //  3. 保存文件 ==> 更新临时文件
  //  4. 发送应答给client
  char recv_buff[1024]{};
  if (tcp_server.Read(recv_buff) == false) {
    logfile.Write("[%s] Read failed\n", tcp_server.GetIP());
    return -1;
  }
  FileInfo_t file_info{};
  file_info_parse(recv_buff, file_info);
  UpdateStr(file_info.file_name, tcp_args.client_path, tcp_args.server_path, false);

  if ( recv_file(tcp_server.m_conn_fd, file_info) == false){
    logfile.Write("[%s] recv_file failed\n", tcp_server.GetIP());
    char send_buff[1024]{};
    SPRINTF(send_buff,1023,"<file_name>%s</file_name><status>success</status>", file_info.file_name);
    tcp_server.Write( send_buff);
  }



  return 0;
}

int download_proc() {
  return 0;
}

bool file_info_parse(const char *file_info_xml, FileInfo_t &file_info) {
  GetXMLBuffer(file_info_xml, "filename", file_info.file_name, 300);
  GetXMLBuffer(file_info_xml, "mtime", file_info.m_ModifyTime, 21);
  GetXMLBuffer(file_info_xml, "size", &file_info.file_size);
  return true;
}
bool recv_file(const int sockfd, const FileInfo_t& file_info){
  //  1. 生成临时文件名
  char tmp_filename[256]{};
  SPRINTF(tmp_filename, 255, "%s.tmp", file_info.file_name);

  //  2. 循环接收
  int total_bytes = 0;    // 已接收的字节总数。
  int reading = 0;        //本次打算读取的字节数。
  char buffer[1000];    // 存放读取数据的buffer。

  //  打开临时文件
  FILE *fp = nullptr;
  if ((fp = fopen(tmp_filename, "w")) == nullptr) {
    logfile.Write("fopen %s failed\n", tmp_filename);
    return false;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    //  计算本次读取的字节数
    if (file_info.file_size - total_bytes > 1000) {
      reading = 1000;
    } else {
      reading = file_info.file_size - total_bytes;
    }

    //  读取到缓存区
    if (Readn(sockfd, buffer, reading) == false){
      logfile.Write("Readn failed\n");
      return false;
    }

    //  写到文件里
    fwrite(buffer, 1, reading, fp);

    //  计算读取的总字节数
    total_bytes+=reading;
    if (total_bytes == file_info.file_size){
      break;
    }
  }

  //  3. 关闭fd
  fclose(fp);

  //  4. 重置文件时间
  UTime(tmp_filename,file_info.m_ModifyTime);

  //  5. 更新文件名
  if(RENAME(tmp_filename, file_info.file_name) == false){
    logfile.Write("RENAME [%s] ==> [%s] failed\n", tmp_filename, file_info.file_name);
    return false;
  }

  return true;
}
