//
// Created by bysou on 2022/4/20.
//

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>

int main(){

  printf("fork before parent %d\n", getpid());

  if (fork() == 0){
    printf("forking sub %d\n", getpid());
  } else{
    printf("forking parent %d\n", getpid());
    exit(0);
  }

  printf("fork after parent %d\n", getpid());

  signal(SIGCHLD, SIG_IGN);

}
