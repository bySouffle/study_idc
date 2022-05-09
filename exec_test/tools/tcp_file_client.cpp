//
// Created by bysou on 2022/5/8.
//

#include "_public.h"
#include <thread>
//  tcp 上传文件客户端 client_type = 1

// 程序运行的参数结构体。
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

CLogFile logfile;
TcpArgs_t tcp_args{};

// 程序退出和信号2、15的处理函数。
void EXIT(int sig);

void _help();

// 把xml解析到参数tcp_args结构中。
bool xml_parse(char *xml_buffer);

CTcpClient TcpClient;

bool login(const char *argv);    // 登录业务。

bool ActiveTest();    // 心跳。

char recv_buffer[1024];   // 发送报文的buffer。
char send_buffer[1024];   // 接收报文的buffer。

// 文件上传的主函数，执行一次文件上传的任务。
bool _tcp_put_files();
bool b_continue = true;   // 如果调用_tcp_put_files发送了文件，b_continue为true，初始化为true。

// 把文件的内容发送给对端。
bool send_file(const int sockfd, const char *filename, const int filesize);

// 删除或者转存本地的文件。
bool ack_message_deal_local_files(const char *recv_buffer);

CPActive active;  // 进程心跳。

int main(int argc, char *argv[]) {
  if (argc != 3) {
    _help();
    return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
  // 但请不要用 "kill -9 +进程号" 强行终止。
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开日志文件。
  if (logfile.Open(argv[1], "a+") == false) {
    printf("打开日志文件失败（%s）。\n", argv[1]);
    return -1;
  }

  // 解析xml，得到程序运行的参数。
  if (xml_parse(argv[2]) == false) return -1;

  active.AddPInfo(tcp_args.timeout, tcp_args.proc_name);  // 把进程的心跳信息写入共享内存。

  // 向服务端发起连接请求。
  if (TcpClient.ConnectToServer(tcp_args.ip, tcp_args.port) == false) {
    logfile.Write("TcpClient.ConnectToServer(%s,%d) failed.\n", tcp_args.ip, tcp_args.port);
    EXIT(-1);
  }

  // 登录业务。
  if (login(argv[2]) == false) {
    logfile.Write("login() failed.\n");
    EXIT(-1);
  }

  while (true) {
    // 调用文件上传的主函数，执行一次文件上传的任务。
    if (_tcp_put_files() == false) {
      logfile.Write("_tcp_put_files() failed.\n");
      EXIT(-1);
    }
//    break;
    if (b_continue == false) {
      std::this_thread::sleep_for(std::chrono::milliseconds(tcp_args.scan_time));
//      sleep(tcp_args.scan_time);

      if (ActiveTest() == false) break;
    }

    active.UptATime();
  }

  EXIT(0);
}

// 心跳。
bool ActiveTest() {
  memset(send_buffer, 0, sizeof(send_buffer));
  memset(recv_buffer, 0, sizeof(recv_buffer));

  SPRINTF(send_buffer, sizeof(send_buffer), "<activetest>ok</activetest>");
  // logfile.Write("发送：%s\n",send_buffer);
  if (TcpClient.Write(send_buffer) == false) return false; // 向服务端发送请求报文。

  if (TcpClient.Read(recv_buffer, 20) == false) return false; // 接收服务端的回应报文。
  // logfile.Write("接收：%s\n",recv_buffer);

  return true;
}

// 登录业务。
bool login(const char *argv) {
  memset(send_buffer, 0, sizeof(send_buffer));
  memset(recv_buffer, 0, sizeof(recv_buffer));

  SPRINTF(send_buffer, sizeof(send_buffer), "%s<client_type>1</client_type>", argv);
  logfile.Write("发送：%s\n", send_buffer);
  if (TcpClient.Write(send_buffer) == false) return false; // 向服务端发送请求报文。

  if (TcpClient.Read(recv_buffer, 20) == false) return false; // 接收服务端的回应报文。
  logfile.Write("接收：%s\n", recv_buffer);

  logfile.Write("登录(%s:%d)成功。\n", tcp_args.ip, tcp_args.port);

  return true;
}

void EXIT(int sig) {
  logfile.Write("程序退出，sig=%d\n\n", sig);

  exit(0);
}

void _help() {
  printf("\n");
  printf("Using:./tcp_file_client logfile_name xml_buffer\n\n");

  printf("Sample:./service_scheduler 30 /${pwd}/tools/tcp_file_client /tmp/idc/proc_data/tcp_file_client.log "
         "\"<ip>127.0.0.1</ip>"
         "<port>54321</port>"
         "<proc_type>1</proc_type>"
         "<client_path>/tmp/tcp/client_dir</client_path>"
         "<bool_upload_sub_path>true</bool_upload_sub_path>"
         "<match_rule>*.XML,*.CSV,*.JSON</match_rule>"
         "<server_path>/tmp/tcp/server_dir</server_path>"
         "<scan_time>10</scan_time>"
         "<timeout>50</timeout>"
         "<proc_name>tcp_file_client</proc_name>\"\n");
  printf("       ./service_scheduler 30 /${pwd}/tools/tcp_file_client /tmp/idc/proc_data/tcp_file_client.log \""
         "\"<ip>127.0.0.1</ip>"
         "<port>54321</port>"
         "<proc_type>2</proc_type>"
         "<client_path>/tmp/tcp/client_dir2</client_path>"
         "<client_path_backup>/tmp/tcp/client_dir2_backup</client_path_backup>"
         "<bool_upload_sub_path>true</bool_upload_sub_path>"
         "<match_rule>*.XML,*.CSV,*.JSON</match_rule>"
         "<server_path>/tmp/tcp/server_dir2</server_path>"
         "<scan_time>10</scan_time>"
         "<timeout>50</timeout>"
         "<proc_name>tcp_file_client</proc_name>\"\n\n\n");

  printf("本程序是数据中心的公共功能模块，采用tcp协议把文件上传给服务端。\n");
  printf("logfile_name   本程序运行的日志文件。\n");
  printf("xml_buffer     本程序运行的参数，如下：\n");
  printf("ip            服务端的IP地址。\n");
  printf("port          服务端的端口。\n");
  printf("proc_type         文件上传成功后的处理方式：1-删除文件；2-移动到备份目录。\n");
  printf("client_path    本地文件存放的根目录。\n");
  printf("client_path_backup 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。\n");
  printf("bool_upload_sub_path      是否上传client_path目录下各级子目录的文件，true-是；false-否，缺省为false。\n");
  printf("match_rule     待上传文件名的匹配规则，如\"*.TXT,*.XML\"\n");
  printf("server_path       服务端文件存放的根目录。\n");
  printf("scan_time       扫描本地目录文件的时间间隔，单位：秒，取值在1-30之间。\n");
  printf("timeout       本程序的超时时间，单位：秒，视文件大小和网络带宽而定，建议设置50以上。\n");
  printf("proc_name         进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}

// 把xml解析到参数tcp_args结构
bool xml_parse(char *xml_buffer) {
  memset(&tcp_args, 0, sizeof(struct TcpArgs));

  GetXMLBuffer(xml_buffer, "ip", tcp_args.ip);
  if (strlen(tcp_args.ip) == 0) {
    logfile.Write("ip is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "port", &tcp_args.port);
  if (tcp_args.port == 0) {
    logfile.Write("port is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "proc_type", &tcp_args.proc_type);
  if ((tcp_args.proc_type != 1) && (tcp_args.proc_type != 2)) {
    logfile.Write("proc_type not in (1,2).\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "client_path", tcp_args.client_path);
  if (strlen(tcp_args.client_path) == 0) {
    logfile.Write("client_path is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "client_path_backup", tcp_args.client_path_backup);
  if ((tcp_args.proc_type == 2) && (strlen(tcp_args.client_path_backup) == 0)) {
    logfile.Write("client_path_backup is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "bool_upload_sub_path", &tcp_args.bool_upload_sub_path);

  GetXMLBuffer(xml_buffer, "match_rule", tcp_args.match_rule);
  if (strlen(tcp_args.match_rule) == 0) {
    logfile.Write("match_rule is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "server_path", tcp_args.server_path);
  if (strlen(tcp_args.server_path) == 0) {
    logfile.Write("server_path is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "scan_time", &tcp_args.scan_time);
  if (tcp_args.scan_time == 0) {
    logfile.Write("scan_time is null.\n");
    return false;
  }

  // 扫描本地目录文件的时间间隔，单位：秒。
  // tcp_args.scan_time没有必要超过30秒。
  if (tcp_args.scan_time > 30) tcp_args.scan_time = 30;

  // 进程心跳的超时时间，一定要大于tcp_args.timeout，没有必要小于50秒。
  GetXMLBuffer(xml_buffer, "timeout", &tcp_args.timeout);
  if (tcp_args.timeout == 0) {
    logfile.Write("timeout is null.\n");
    return false;
  }
  if (tcp_args.timeout < 50) tcp_args.timeout = 50;

  GetXMLBuffer(xml_buffer, "proc_name", tcp_args.proc_name, 50);
  if (strlen(tcp_args.proc_name) == 0) {
    logfile.Write("proc_name is null.\n");
    return false;
  }

  return true;
}

// 文件上传的主函数，执行一次文件上传的任务。
bool _tcp_put_files() {
  CDir Dir;

  // 调用OpenDir()打开tcp_args.client_path目录。
  if (Dir.OpenDir(tcp_args.client_path, tcp_args.match_rule, 10000, tcp_args.bool_upload_sub_path) == false) {
    logfile.Write("Dir.OpenDir(%s) 失败。\n", tcp_args.client_path);
    return false;
  }

  int delayed = 0;        // 未收到对端确认报文的文件数量。
  int buff_len = 0;         // 用于存放recv_buffer的长度。

  b_continue = false;

  while (true) {
    memset(send_buffer, 0, sizeof(send_buffer));
    memset(recv_buffer, 0, sizeof(recv_buffer));

    // 遍历目录中的每个文件，调用ReadDir()获取一个文件名。
    if (Dir.ReadDir() == false) break;

    b_continue = true;

    //  心跳
    std::cout << __LINE__ <<"\n";
    {
      if (TcpClient.Write("<activate_test>alive</activate_test>") == false) {
        logfile.Write("[%s] Client heart send error\n");
        return false;
      }
      char recv_heart[256]{};
      if (TcpClient.Read(recv_heart) == false) {
        logfile.Write("[%s] Client heart recv error\n");
        return false;
      }

      if (strcmp(recv_heart, "<activate_test>alive</activate_test>") != 0) {
        logfile.Write("[%s] Client heart recv error\n");
        return false;
      }
    }


    // 把文件名、修改时间、文件大小组成报文，发送给对端。
    SNPRINTF(send_buffer, sizeof(send_buffer), 1000,
             "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
             Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    // logfile.Write("send_buffer=%s\n",send_buffer);
    if (TcpClient.Write(send_buffer) == false) {
      logfile.Write("TcpClient.Write() failed.\n");
      return false;
    }

    // 把文件的内容发送给对端。
    logfile.Write("send %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (send_file(TcpClient.m_conn_fd, Dir.m_FullFileName, Dir.m_FileSize) == true) {
      logfile.WriteEx("ok.\n");
      delayed++;
    } else {
      logfile.WriteEx("failed.\n");
      TcpClient.Close();
      return false;
    }

    active.UptATime();


    // 接收对端的确认报文。
    while (delayed > 0) {
      memset(recv_buffer, 0, sizeof(recv_buffer));

      if (TcpRead(TcpClient.m_conn_fd, recv_buffer, &buff_len, -1) == false) {
        break;
      }
      //  TODO  解析应答
//       logfile.Write("recv_buffer=%s\n",recv_buffer);

      // 删除或者转存本地的文件。
      delayed--;
      ack_message_deal_local_files(recv_buffer);

    }


    // 继续接收对端的确认报文。
    while (delayed > 0) {
      memset(recv_buffer, 0, sizeof(recv_buffer));
      if (TcpRead(TcpClient.m_conn_fd, recv_buffer, &buff_len, 10) == false) break;
      // logfile.Write("recv_buffer=%s\n",recv_buffer);

      // 删除或者转存本地的文件。
      delayed--;
      ack_message_deal_local_files(recv_buffer);
    }
    //
    {
      if (TcpClient.Write("<activate_test>alive</activate_test>") == false) {
        logfile.Write("[%s] Client heart send error\n");
        return false;
      }
      char recv_heart[256]{};
      if (TcpClient.Read(recv_heart) == false) {
        logfile.Write("[%s] Client heart recv error\n");
        return false;
      }

      if (strcmp(recv_heart, "<activate_test>alive</activate_test>") != 0) {
        logfile.Write("[%s] Client heart recv error\n");
        return false;
      }
    }
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

