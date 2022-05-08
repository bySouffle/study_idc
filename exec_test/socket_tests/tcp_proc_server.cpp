//
// Created by bysou on 2022/5/7.
//

#include "_public.h"

CLogFile logfile;
CTcpServer tcp_server;

//  帮助信息
void _help();

//  父进程信号处理函数
void Father_EXIT(int sig);
//  子进程信号处理函数
void Child_EXIT(int signal);
static int login_ed = -1;

//  数据处理函数
enum EXEC_OPT_ENUM {
  Heart = 0x0001,
  Login = 0x1000,
  Inquiry = 0x1001,
  Operate = 0x1002,
  Error = 0x1003,

};
EXEC_OPT_ENUM xml_parse(const char *xml_buffer);
//  业务执行函数
int exec_ctl(const char *xml_buffer, char *out_buffer);
void exec_login(const char *in_data, char *out_data);
void exec_inquiry(const char *in_data, char *out_data);
void exec_operate(const char *in_data, char *out_data);

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
    logfile.Write("server init success[port = %d]", strtol(argv[1], nullptr, 10));
    return -3;
  }
//  static int login_ed = -1;

  while (1) {
    if (tcp_server.Accept()) {
      logfile.Write("server accept %s\n", tcp_server.GetIP());
      if (fork() > 0) {
        //  父进程关闭客户端转到子进程处理
        tcp_server.CloseClient();
        continue;
      }
      // 子进程重新设置退出信号。
      signal(SIGINT,Child_EXIT);
      signal(SIGTERM,Child_EXIT);

      //  子进程关闭监听
      tcp_server.CloseListen();

      char recv_buff[1024]{};
      char send_buff[1024]{};
//      static int login_ed = -1;
      std::cout << __LINE__ << "login_ed " << login_ed << "\n";

      while (true) {
        if (tcp_server.Read(recv_buff) == true) {
          logfile.Write("client [%s] read_data: %s\n", tcp_server.GetIP(), recv_buff);

          exec_ctl(recv_buff, send_buff);

          if (tcp_server.Write(send_buff) == true) {
            logfile.Write("server sendto client [%s] write_data: %s\n", tcp_server.GetIP(), send_buff);
          } else{
            break;
          }

        } else{
          break;
        }
      }
      Child_EXIT(0);
    }

  }

}

void _help() {
  printf("Using:./tcp_proc_server port logfile timeout\n"
         "Example:./tcp_proc_server 54321 /tmp/socket_test/tcp_proc_server.log 35\n\n");
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

EXEC_OPT_ENUM xml_parse(const char *xml_buffer) {
  int code = -1;
  if (GetXMLBuffer(xml_buffer, "opt_code", &code) == true) {
    logfile.Write("opt_code=0x%x。\n", code);

    switch (code) {
      case Heart: return Heart;
      case Login: return Login;
      case Inquiry: return Inquiry;
      case Operate: return Operate;
      default: return Error;
    }
  }
  return Error;
}

int exec_ctl(const char *xml_buffer, char *out_buffer) {

  EXEC_OPT_ENUM exec_code = xml_parse(xml_buffer);

  switch (exec_code) {
    case Heart:
      SPRINTF(out_buffer, 1023, "<opt_code>0</opt_code><message>ON</message>");
      break;
    case Login:
      exec_login(xml_buffer, out_buffer);
      break;
    case Inquiry:
      exec_inquiry(xml_buffer, out_buffer);
      break;
    case Operate:
      exec_operate(xml_buffer, out_buffer);
      break;
    case Error:
      SPRINTF(out_buffer, 1023, "<opt_code>0</opt_code><message>未知错误。</message>");
      break;
    default:
      SPRINTF(out_buffer, 1023, "<opt_code>0</opt_code><message>未知错误。</message>");
      break;
  }
  return 0;

}
void exec_login(const char *in_data, char *out_data) {
  if (in_data == nullptr) {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Login, "未知错误");
    return;
  }
  char username[32]{};
  char passwd[32]{};

  GetXMLBuffer(in_data, "username", username, 31);
  GetXMLBuffer(in_data, "passwd", passwd, 31);
//  static int login_ed;
  std::cout << __LINE__ << "login_ed " << login_ed << "\n";
  if (login_ed == 1) {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Login, "已登录");
    return;
  }

  if (strcmp(username, "bys") == 0 && strcmp(passwd, "zzz") == 0) {

    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Login, "登录成功");
    login_ed = 1;
    std::cout << __LINE__ << "login_ed " << login_ed << "\n";

  }

}

void exec_inquiry(const char *in_data, char *out_data) {
//  static int login_ed;
  std::cout << __LINE__ << "login_ed " << login_ed << "\n";
  if (login_ed != 1) {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Inquiry, "未登录");
    return;
  }

  char card_id[20]{};
  GetXMLBuffer(in_data, "card_id", card_id, 19);
  if (strcmp(card_id, "12345678") == 0) {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Inquiry, "8000");
  } else {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Inquiry, "card error");
  }

}
void exec_operate(const char *in_data, char *out_data) {
//  static int login_ed;
  std::cout << __LINE__ << "login_ed " << login_ed << "\n";
  if (login_ed != 1) {
    SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Operate, "未登录");
    return;
  }

  SPRINTF(out_data, 1023, "<opt_code>0x%x</opt_code><message>%s</message>", Operate, "成功");

}
