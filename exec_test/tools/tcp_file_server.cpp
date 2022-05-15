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
bool recv_file(const int sockfd, const FileInfo_t &file_info);
bool ack_message_deal_local_files(const char *recv_buffer);

//  下载业务函数
bool download_proc();
// 把文件的内容发送给对端。
bool send_file(const int sockfd, const char *filename, const int filesize);

//  父进程信号处理函数
void Father_EXIT(int sig);
//  子进程信号处理函数
void Child_EXIT(int signal);

//  登陆信息
bool check_login();



///////////////////////////////
bool b_continue;
CLogFile logfile;
CTcpServer tcp_server;
TcpArgs_t tcp_args{};
CPActive active;  //  进程心跳
///////////////////////////////

//  tcp心跳
bool ActiveTest();

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
      std::cout << tcp_args.client_type;

      if (tcp_args.client_type == 1) {
        upload_proc();
      } else if (tcp_args.client_type == 2) {
        download_proc();
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
    logfile.Write("(%s: %d) [%s] Read failed\n", __FUNCTION__, __LINE__, tcp_server.GetIP());
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
  //  5. 加入心跳报文 超时断开 客户端先发送 在文件发送前校验一次 发送完毕校验一次

  active.AddPInfo(tcp_args.timeout, tcp_args.proc_name);

  char recv_buff[1024]{};
  FileInfo_t file_info{};
  int cnt = 0;
  while (true) {
    memset(recv_buff, 0, sizeof(recv_buff));
    memset(&file_info, 0, sizeof(file_info));
    active.UptATime();
    {
      if (tcp_server.Read(recv_buff, tcp_args.scan_time + 10) == false) {
        if (cnt % 10 == 0) {
          logfile.Write("[%s] upload start Client heart loss\n", tcp_server.GetIP());
        }
        if (cnt > 20) {
          return -1;
        }
        cnt++;
        continue;
      }
      //  parse xml
      std::cout << std::string{recv_buff} << "\n";

      char heart_data[256]{};
      GetXMLBuffer(recv_buff, "activate_test", heart_data, 256);
      if (strcmp(heart_data, "alive") == 0) {
        tcp_server.Write("<activate_test>alive</activate_test>");
        cnt = 0;
      } else {
        return -1;
      }
    }
    memset(recv_buff, 0, sizeof(recv_buff));
    std::cout << __LINE__ << "\n";
    if (tcp_server.Read(recv_buff) == false) {
      logfile.Write("(%s: %d) [%s] Read failed\n", __FUNCTION__, __LINE__, tcp_server.GetIP());
      return -1;
    }
    file_info_parse(recv_buff, file_info);
    char client_filename[301]{};
    memcpy(client_filename, file_info.file_name, sizeof(client_filename));
    UpdateStr(file_info.file_name, tcp_args.client_path, tcp_args.server_path, false);
    if (recv_file(tcp_server.m_conn_fd, file_info) == false) {
      logfile.Write("[%s] recv_file failed\n", tcp_server.GetIP());
    }

    char send_buff[1024]{};
    SPRINTF(send_buff, 1023, "<file_name>%s</file_name><status>success</status>", client_filename);
    if (tcp_server.Write(send_buff) == false) {
      logfile.Write("tcp Write to [%s] failed\n", tcp_server.GetIP());
    }
    {
      if (tcp_server.Read(recv_buff, tcp_args.scan_time + 10) == false) {
        if (cnt % 10 == 0) {
          logfile.Write("[%s] upload start Client heart loss\n", tcp_server.GetIP());
        }
        if (cnt > 20) {
          return -1;
        }
        cnt++;
        sleep(1);
        continue;
      }
      //  parse xml
      char heart_data[256]{};
      GetXMLBuffer(recv_buff, "activate_test", heart_data, 256);
      if (strcmp(heart_data, "alive") == 0) {
        tcp_server.Write("<activate_test>alive</activate_test>");
        cnt = 0;
      }
    }
    active.UptATime();

  }
  return 0;
}

bool download_proc() {
  CDir Dir;

  // 调用OpenDir()打开tcp_args.client_path目录。
  if (Dir.OpenDir(tcp_args.server_path, tcp_args.match_rule, 10000, tcp_args.bool_upload_sub_path) == false) {
    logfile.Write("Dir.OpenDir(%s) 失败。\n", tcp_args.server_path);
    return false;
  }

  int delayed = 0;        // 未收到对端确认报文的文件数量。
  int buff_len = 0;         // 用于存放recv_buffer的长度。

  b_continue = false;

  char send_buffer[1024]{};
  char recv_buffer[1024]{};

  while (true) {
    memset(send_buffer, 0, sizeof(send_buffer));
    memset(recv_buffer, 0, sizeof(recv_buffer));

    // 遍历目录中的每个文件，调用ReadDir()获取一个文件名。
    if (Dir.ReadDir() == false) break;

    b_continue = true;

    // 把文件名、修改时间、文件大小组成报文，发送给对端。
    SNPRINTF(send_buffer, sizeof(send_buffer), 1000,
             "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
             Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    // logfile.Write("send_buffer=%s\n",send_buffer);
    if (tcp_server.Write(send_buffer) == false) {
      logfile.Write("TcpClient.Write() failed.\n");
      return false;
    }

    // 把文件的内容发送给对端。
    logfile.Write("send %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (send_file(tcp_server.m_conn_fd, Dir.m_FullFileName, Dir.m_FileSize) == true) {
      logfile.WriteEx("ok.\n");
      delayed ++;
    } else {
      logfile.WriteEx("failed.\n");
      return false;
    }

    active.UptATime();

    std::cout << __LINE__ <<"\n";

    // 接收对端的确认报文。
    while (delayed > 0) {
      memset(recv_buffer, 0, sizeof(recv_buffer));

      if (TcpRead(tcp_server.m_conn_fd, recv_buffer, &buff_len, -1) == false) {
        break;
      }
      //  TODO  解析应答
//       logfile.Write("recv_buffer=%s\n",recv_buffer);

      // 删除或者转存本地的文件。
      delayed--;
      ack_message_deal_local_files(recv_buffer);

    }
    std::cout << __LINE__ <<"\n";


    // 继续接收对端的确认报文。
    while (delayed > 0) {
      memset(recv_buffer, 0, sizeof(recv_buffer));
      if (TcpRead(tcp_server.m_conn_fd, recv_buffer, &buff_len, 10) == false) break;
      // logfile.Write("recv_buffer=%s\n",recv_buffer);

      // 删除或者转存本地的文件。
      delayed--;
      ack_message_deal_local_files(recv_buffer);
    }

    std::cout << __LINE__ <<"\n";

  }
  return true;
}

bool file_info_parse(const char *file_info_xml, FileInfo_t &file_info) {
  GetXMLBuffer(file_info_xml, "filename", file_info.file_name, 300);
  GetXMLBuffer(file_info_xml, "mtime", file_info.m_ModifyTime, 21);
  GetXMLBuffer(file_info_xml, "size", &file_info.file_size);
  return true;
}
bool recv_file(const int sockfd, const FileInfo_t &file_info) {
  //  1. 生成临时文件名
  char tmp_filename[256]{};
  SPRINTF(tmp_filename, 255, "%s.tmp", file_info.file_name);

  //  2. 循环接收
  int total_bytes = 0;    // 已接收的字节总数。
  int reading = 0;        //本次打算读取的字节数。
  char buffer[1000];    // 存放读取数据的buffer。

  active.UptATime();

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
    if (Readn(sockfd, buffer, reading) == false) {
      logfile.Write("Readn failed\n");
      return false;
    }

    //  写到文件里
    fwrite(buffer, 1, reading, fp);

    //  计算读取的总字节数
    total_bytes += reading;
    if (total_bytes == file_info.file_size) {
      break;
    }
  }

  //  3. 关闭fd
  fclose(fp);

  //  4. 重置文件时间
  UTime(tmp_filename, file_info.m_ModifyTime);

  //  5. 更新文件名
  if (RENAME(tmp_filename, file_info.file_name) == false) {
    logfile.Write("RENAME [%s] ==> [%s] failed\n", tmp_filename, file_info.file_name);
    return false;
  }

  return true;
}
// 把文件的内容发送给对端。
bool send_file(const int sockfd, const char *filename, const int filesize) {
  int reading = 0;        // 每次调用fread时打算读取的字节数。
  int bytes = 0;         // 调用一次fread从文件中读取的字节数。
  char buffer[1000];    // 存放读取数据的buffer。
  int totalbytes = 0;    // 从文件中已读取的字节总数。
  FILE *fp = NULL;

  // 以"rb"的模式打开文件。
  if ((fp = fopen(filename, "rb")) == NULL) return false;

  while (true) {
    memset(buffer, 0, sizeof(buffer));

    // 计算本次应该读取的字节数，如果剩余的数据超过1000字节，就打算读1000字节。
    if (filesize - totalbytes > 1000) reading = 1000;
    else reading = filesize - totalbytes;

    // 从文件中读取数据。
    bytes = fread(buffer, 1, reading, fp);

    // 把读取到的数据发送给对端。
    if (bytes > 0) {
      if (Writen(sockfd, buffer, bytes) == false) {
        fclose(fp);
        return false;
      }
    }

    // 计算文件已读取的字节总数，如果文件已读完，跳出循环。
    totalbytes = totalbytes + bytes;

    if (totalbytes == filesize) break;
  }

  fclose(fp);

  return true;
}

// 删除或者转存本地的文件。
bool ack_message_deal_local_files(const char *recv_buffer) {
  char filename[256];
  char result[16];

  memset(filename, 0, sizeof(filename));
  memset(result, 0, sizeof(result));

  GetXMLBuffer(recv_buffer, "file_name", filename, 255);
  GetXMLBuffer(recv_buffer, "status", result, 15);

  // 如果服务端接收文件不成功，直接返回。
  if (strcmp(result, "success") != 0) return true;

  // proc_type==1，删除文件。
  if (tcp_args.proc_type == 1) {
    if (REMOVE(filename) == false) {
      logfile.Write("REMOVE(%s) failed.\n", filename);
      return false;
    }
  }

  // proc_type==2，移动到备份目录。
  if (tcp_args.proc_type == 2) {
    // 生成转存后的备份目录文件名。
    char backup_filename[256];
    STRCPY(backup_filename, sizeof(backup_filename), filename);
    UpdateStr(backup_filename, tcp_args.client_path, tcp_args.client_path_backup, false);
    if (RENAME(filename, backup_filename) == false) {
      logfile.Write("RENAME(%s,%s) failed.\n",
                    filename,
                    backup_filename);
      return false;
    }
  }

  return true;
}