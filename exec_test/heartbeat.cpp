//
// Created by bysou on 2022/4/20.
//

#include <cstdio>
#include "_public.h"

#define MAX_PROCESS 10

int main(int argc, char *argv[]){
  if(argc < 2) {
    printf("Using: ./single_item_example process_name\n");
    return 0;
  }

  //  创建/获取共享内存，大小n*sizeof(struct st_procinfo)
  int n = MAX_PROCESS;
  int shm_id;
  if (  (shm_id = shmget(0x2020, n * sizeof (struct st_procinfo), 0775|IPC_CREAT)) < 0 ){
    printf("shmget(%x) failed\n", 0x2020);
  }

  //  创建信号量
  CSEM sem;
  if(sem.init(0x2020) == false){
    printf("sem(%x) failed\n", 0x2020);
  }

  //  共享内存连接当前程序地址空间
  struct st_procinfo* heart_info = nullptr;
  heart_info = static_cast<struct st_procinfo*>(shmat(shm_id, nullptr, 0)) ;

  //  填写当前程序心跳信息存入共享内存空的地方
  struct st_procinfo heart_info_{};
  heart_info_.pid = getpid();
  STRNCPY(heart_info_.pname, sizeof (heart_info_.pname), argv[1], 50);
  heart_info_.atime = time(nullptr);
  heart_info_.timeout = 30;

  int m_pos = -1;
  //  异常退出未清理心跳信息 代替原位置
  for (int i = 0; i < MAX_PROCESS; ++i) {
    if ((heart_info + i)->pid == heart_info_.pid ){
      m_pos = i;  //  找到空位
      break;
    }
  }

  sem.P();  // lock

  //  未占用寻找空位置
  if(m_pos == -1){
    for (int i = 0; i < MAX_PROCESS; ++i) {
      if ((heart_info + i)->pid == 0 ){
        m_pos = i;  //  找到空位
        break;
      }
    }
  }
  if (m_pos == -1){
    sem.V();  //  unlock
    printf("共享内存已满\n");
    return -1;
  }

  //  存放当前心跳
  memcpy(&heart_info[m_pos],&heart_info_, sizeof (struct st_procinfo));

  sem.V();  //  unlock

  //  更新心跳
  while (true){
    //  更新心跳时间
    heart_info[m_pos].atime = time(nullptr);
    sleep(5);
  }

  //  当前进程移除共享内存
  memset(&heart_info[m_pos], 0 , sizeof(struct st_procinfo));
  //  分离共享内存
  shmdt(heart_info);



}