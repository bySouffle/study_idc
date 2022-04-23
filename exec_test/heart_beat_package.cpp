//
// Created by bysou on 2022/4/23.
//

#include "_public.h"

//  心跳检测
CPActive active;

void EXIT(int sig){
  printf("sig=%d\n", sig);
  exit(0);  //  不会调用局部对象的析构函数，会调用全局变量的析构函数
}

int main(int argc, char *argv[]){

  if (argc!=3) {
    printf("Using: ./heart_beat_package proc_name timeout\n");
    return 0;
  }
  signal(2, EXIT);
  signal(15, EXIT);

  active.AddPInfo(atoi(argv[2]),argv[1]);

  while (true){
    active.UptATime();
    sleep(10);
  }
  return 0;
}