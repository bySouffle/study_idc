#include "public/_public.h"

CLogFile logfile; //  文件操作class

int main(int argc, char *argv[]) {

  std::cout << "Hello, World!" << std::endl;
  if ((argc != 5) && (argc != 6)) {
    // 如果参数非法，给出帮助文档。
    printf("Using:   ./study_idc inifile outpath logfile\n");
    printf("Example: ./study_idc /tmp/study_idc/ini/param_config.ini /tmp/proc_data /tmp/study_idc/log_info.log\n\n");
    printf("         ./study_idc 1 2 /tmp/study_idc/log_info.log 4 5\n");

    printf("inifile  参数文件名。\n");
    printf("outpath  数据文件存放的目录。\n");
    printf("logfile  本程序运行的日志文件名。\n");


    return -1;
  }

  // 打开程序的日志文件。
  if (logfile.Open(argv[3],"a+",false)==false){
    printf("logfile.Open(%s) failed.\n",argv[3]); return -1;
  }
  logfile.Write("study_idc 开始运行。\n");


  logfile.Write("study_idc 运行结束。\n");
  return 0;
}
