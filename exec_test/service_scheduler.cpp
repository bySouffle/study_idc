//
// Created by bysou on 2022/4/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include "_public.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Using:./service_scheduler timer program argv ...\n");
    printf("Example:./service_scheduler 5 /usr/bin/ls -alh /tmp \n\n");

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("timer   运行周期，单位：秒。被调度的程序运行结束后，在timer秒后会被 single_item_example 重新启动。\n");
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("argv    被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");
    return -1;
  }

  //  关闭信号与IO
//  for (int i = 0; i < 64; ++i) {
//    signal(i, SIG_IGN);
//    close(i);
//  }
    CloseIOAndSignal(true);

  //  fork子进程 父进程退出让程序由pid1托管
  if (fork() != 0) exit(0);

  // 启用SIGCHLD信号，让父进程可以wait子进程退出的状态。
  signal(SIGCHLD, SIG_DFL);

  char *param_gv[argc];
  for (int i = 2; i < argc; ++i) {
    param_gv[i - 2] = argv[i];
  }
  param_gv[argc - 2] = nullptr;

  while (true) {
    if (fork() == 0) {
      execv(argv[2], param_gv);
      exit(0);
    } else {
      int status;
      wait(&status);
      std::this_thread::sleep_for(std::chrono::seconds(strtol(argv[1], nullptr, 10)));
    }
  }
}