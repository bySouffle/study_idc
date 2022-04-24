//
// Created by bysou on 2022/4/23.
//

#include "_public.h"

void EXIT(int signal);

int main(int argc, char *argv[]) {

  if (argc != 4) {
    printf("\n");
    printf("Using: ./delete_files path_name match_str timeout\n\n");

    printf("Example:./delete_files /log/idc \"*.log.20*\" 0.02\n");
    printf("        ./delete_files /tmp/proc_data \"*.xml,*.json\" 0.01\n");
    printf("        ./service_scheduler 300 /${pwd}/tools/delete_files /log/idc \"*.log.20*\" 0.02\n");
    printf("        ./service_scheduler 300 /${pwd}/tools/delete_files /tmp/idc/proc_data \"*.xml,*.json\" 0.01\n\n\n");

    printf("这是一个工具程序，用于删除历史的数据文件或日志文件。\n");
    printf("本程序把 path_name 目录及子目录中 timeout 天之前的匹配 match_str 文件全部删除，timeout可以是小数。\n");
    printf("本程序不写日志文件，也不会在控制台输出任何信息。\n\n\n");

    return -1;
  }

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  // CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 获取timeout参数指定的时间点。
  char strTimeOut[21];
  LocalTime(strTimeOut, "yyyy-mm-dd hh24:mi:ss", 0 - (int) (atof(argv[3]) * 24 * 60 * 60));

  CDir Dir;

  // 打开目录，读取文件，包括它的子目录
  if (Dir.OpenDir(argv[1], argv[2], 10000, true) == false) {
    printf("Dir.OpenDir(%s) failed.\n", argv[1]);
    return -1;
  }

  //  遍历目录文件名
  while (true) {
    //  得到文件信息
    if (Dir.ReadDir() == false) break;
    printf("=>\t%s\t<=\n", Dir.m_FullFileName);
    //  与超时时间比较，更早就删除
    if (strcmp(Dir.m_ModifyTime, strTimeOut) < 0) {
      if (REMOVE(Dir.m_FullFileName)) {
        printf("REMOVE\t%s\tsuccess.\n", Dir.m_FullFileName);
      } else {
        printf("REMOVE\t%s\tfailed.\n", Dir.m_FullFileName);
      }
    }
  }

  return 0;
}

void EXIT(int signal) {
  printf("程序退出，sig=%d\n\n", signal);

  exit(0);
}
