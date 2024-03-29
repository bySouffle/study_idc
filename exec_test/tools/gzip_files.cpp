//
// Created by bysou on 2022/4/23.
//

#include "_public.h"

void EXIT(int signal);

int main(int argc,char *argv[]){

  if (argc != 4)
  {
    printf("\n");
    printf("Using: ./gzip_files path_name match_str timeout\n\n");

    printf("Example:./gzip_files /log/idc \"*.log.20*\" 0.02\n");
    printf("        ./gzip_files /tmp/proc_data \"*.xml,*.json\" 0.01\n");
    printf("        ./service_scheduler 300 /${pwd}/tools/gzip_files /log/idc \"*.log.20*\" 0.02\n");
    printf("        ./service_scheduler 300 /${pwd}/tools/gzip_files /tmp/idc/proc_data \"*.xml,*.json\" 0.01\n\n\n");

    printf("这是一个工具程序，用于压缩历史的数据文件或日志文件。\n");
    printf("本程序把pathname目录及子目录中timeout天之前的匹配matchstr文件全部压缩，timeout可以是小数。\n");
    printf("本程序不写日志文件，也不会在控制台输出任何信息。\n");
    printf("本程序调用/usr/bin/gzip命令压缩文件。\n\n\n");

    return -1;
  }

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  // CloseIOAndSignal();
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 获取timeout参数指定的时间点。
  char strTimeOut[21];
  LocalTime(strTimeOut,"yyyy-mm-dd hh24:mi:ss",0-(int)(atof(argv[3])*24*60*60));

  CDir Dir;

  // 打开目录，读取文件，包括它的子目录
  if (Dir.OpenDir(argv[1],argv[2],10000,true) == false)
  {
    printf("Dir.OpenDir(%s) failed.\n",argv[1]); return -1;
  }

  char strCmd[1024];  // 存放gzip压缩文件的命令。

  while (Dir.ReadDir() == true)
  {
    // 判断文件的时间，如果在timeout参数指定的时间点之前，就压缩它。
    if ( (strcmp(Dir.m_ModifyTime,strTimeOut) < 0) && (MatchStr(Dir.m_FileName,"*.gz") == false) )
    {
      SNPRINTF(strCmd,sizeof(strCmd),1024,"/usr/bin/gzip -f %s 1>/dev/null 2>/dev/null",Dir.m_FullFileName);
      if (system(strCmd)==0)
        printf("gzip %s success.\n",Dir.m_FullFileName);
      else
        printf("gzip %s failed.\n",Dir.m_FullFileName);
    }
  }

  return 0;
}

void EXIT(int signal)
{
  printf("程序退出，sig=%d\n\n",signal);

  exit(0);
}
