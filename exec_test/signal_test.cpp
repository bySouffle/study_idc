//
// Created by bysou on 2022/4/18.
//

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

void EXIT(int num){
  printf("信号来啦 %d\n", num);
  exit(0);
}

typedef struct SHM_DATA_INFO{
  int pid{};
  char data[];
}SHM_DATA_INFO_t;

int main(){
  for (int i = 0; i <= 64; ++i) {
    signal(i, SIG_IGN);
  }
  signal(SIGINT, EXIT);
  printf("pid: %d\n", getpid());
  while (1){

  }
}
