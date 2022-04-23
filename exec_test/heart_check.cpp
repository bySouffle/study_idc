//
// Created by bysou on 2022/4/23.
//

#include "_public.h"


int main(int argc, char*argv[]){

  //  日志类
  CLogFile logfile;

  const int kMAX_PROCESS = 10;

  //  1. 帮助文档
  if (argc < 2){
    printf("\n");
    printf("Using: ./heart_check logfile_name\n");

    printf("Example: ./service_scheduler 10 heart_check /tmp/log/heart_check.log\n\n");

    printf("本程序用于检查后台服务程序是否超时，如果已超时，就终止它。\n");
    printf("注意：\n");
    printf("  1）本程序由service_scheduler启动，运行周期建议为10秒。\n");
    printf("  2）为了避免被普通用户误杀，本程序应该用root用户启动。\n");
    printf("  3）如果要停止本程序，只能用killall -9 终止。\n\n\n");

    return 0;
  }

  CloseIOAndSignal(true);

  //  2. 打开日志文件
  if ( logfile.Open(argv[1], "a+") == false ){
    printf("logfile.Open(%s) failed.\n",argv[1]); return -1;
  }
  //  3. 创建/获取共享内存
  int shm_id = 0;
  if( (shm_id = shmget(0x2020, kMAX_PROCESS* sizeof(struct st_procinfo), 0666|IPC_CREAT)) == -1 ){
    logfile.Write("创建/获取共享内存(%x)失败。\n",SHMKEYP); return false;
  }

  //  4. 链接共享内存
  struct st_procinfo *p_proc_all = static_cast<st_procinfo*>(shmat(shm_id, nullptr, 0)) ;

  //  5. 遍历共享内存中全部的记录
  for (int i = 0; i < kMAX_PROCESS; ++i) {
    //  pid 为 0 跳过
    if (p_proc_all[i].pid == 0) continue;

    //  日志记录心跳
    logfile.Write("i=%d,pid=%d,proc_name=%s,timeout=%d,atime=%d\n",\
                   i,p_proc_all[i].pid,p_proc_all[i].pname,p_proc_all[ i].timeout,p_proc_all[ i].atime);

    //  向进程发送信号0判断是否存活
    int ret_val = kill(p_proc_all[i].pid, 0);
    if (ret_val == -1){
      logfile.Write("进程pid=%d(%s)已经不存在。\n",(p_proc_all + i)->pid,(p_proc_all + i)->pname);
      memset(p_proc_all+i,0,sizeof(struct st_procinfo)); // 从共享内存中删除该记录。
      continue;
    }

    //  计算超时
    time_t now = time(nullptr);
    if (now - p_proc_all[i].atime < p_proc_all->timeout)  continue;

    logfile.Write("进程pid=%d(%s)已经超时。\n",(p_proc_all+i)->pid,(p_proc_all+i)->pname);
    kill(p_proc_all[i].pid, 15);  //  正常终止进程

    //  周期1s判断是否退出
    for (int j = 0; j < 5; ++j) {
      sleep(1);
      ret_val = kill(p_proc_all[i].pid, 0);
      if (ret_val == -1)  break;
    }

    //  依然未退出
    if (ret_val == -1){
      logfile.Write("进程pid=%d(%s)已经正常终止。\n",(p_proc_all+i)->pid,(p_proc_all+i)->pname);
    } else{
      kill(p_proc_all[i].pid, 9); //  强行终止
      logfile.Write("进程pid=%d(%s)已经强制终止。\n",(p_proc_all+i )->pid,(p_proc_all+i )->pname);

    }
    //  清除超时进程信息
    memset(p_proc_all+i, 0, sizeof (struct st_procinfo));
  }
  //  分离
  shmdt(p_proc_all);
  return 0;

}