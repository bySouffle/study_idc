//
// Created by bysou on 2022/5/3.
//

#include "_public.h"
#include "_ftp.h"

typedef struct FtpArgs {
  char host[31];           // 远程服务器的IP和端口。
  int mode;               // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];       // 远程服务器ftp的用户名。
  char password[31];       // 远程服务器ftp的密码。
  char remote_path[301];    // 远程服务器存放文件的目录。
  char local_path[301];     // 本地文件存放的目录。
  char match_name[101];     // 待下载文件匹配的规则。
  char list_filename[301];  // 下载前列出服务器文件名的文件。
  int  proc_type;              // 下载后服务器文件的处理方式：1-什么也不做；2-删除；3-备份。
  char remote_backup_dir[301]; // 下载后服务器文件的备份目录。
} FtpArgs_t;

//  帮助信息
void help();

//  信号2 15 处理函数
void EXIT(int signal);

//  xml 参数解析
bool xml_parse(const char *xml_buffer);
//  下载文件
typedef struct FileInfo
{
  char filename[301];   // 文件名。
  char mtime[21];       // 文件时间。
}FileInfo_t;
std::vector<FileInfo_t> vec_file_list;    // 存放下载前列出服务器文件名的容器。

bool load_list_file();
bool ftp_get_files();

CLogFile logfile;
FtpArgs_t ftp_args{};
Cftp ftp;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    help();
    return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
  // 但请不要用 "kill -9 +进程号" 强行终止。
  // CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开日志文件。
  if (logfile.Open(argv[1], "a+") == false) {
    printf("打开日志文件失败（%s）。\n", argv[1]);
    return -1;
  }

  // 解析xml，得到程序运行的参数。
  if (xml_parse(argv[2]) == false) return -1;


  // 1. 登录ftp服务器。
  if (ftp.login(ftp_args.host, ftp_args.username, ftp_args.password,
                ftp_args.mode) == false) {
    logfile.Write("ftp.login(%s,%s,%s) failed.[conn %d][login %d][option %d]\n",
                  ftp_args.host, ftp_args.username, ftp_args.password,
                  ftp.m_connectfailed, ftp.m_loginfailed, ftp.m_optionfailed);
    return -1;
  }
  logfile.Write("ftp.login ok.\n");

  // 2. 进入ftp服务器存放文件的目录。
  // 3. 调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件中。
  // 4. 把ftp.nlist()方法获取到的list文件加载到容器vfilelist中。
  // 5. 遍历容器vfilelist。
/*
  for (int ii=0;ii<vlistfile.size();ii++)
  {
    // 调用ftp.get()方法从服务器下载文件。
  }
*/
  ftp_get_files();

  ftp.logout();
  return 0;

}

void help() {
  printf("\n");
  printf("Using:./ftp_get_files logfile_name xml_buffer\n\n");

  printf("Sample:./service_scheduler 30 /${pwd}/tools/ftp_get_files /tmp/idc/proc_data/ftp.log "
         "\"<host>192.168.3.50:2121</host>"
         "<mode>1</mode>"
         "<username>bys</username>"
         "<password>admin</password>"
         "<local_path>/tmp/download_proc_data</local_path>"
         "<remote_path>/Alipay</remote_path>"
         "<match_name>SURF_ZH*.XML,SURF_ZH*.CSV</match_name>"
         "<list_filename>/tmp/proc_data/remote_ftp_file.list</list_filename>"
         "<proc_type>3</proc_type>"
         "<remote_backup_dir>/backup</remote_backup_dir>"
         "\"\n\n\n");

  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
  printf("logfile_name是本程序运行的日志文件。\n");
  printf("xml_buffer为文件下载的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>bys</username> 远程服务器ftp的用户名。\n");
  printf("<password>admin</password> 远程服务器ftp的密码。\n");
  printf("<remote_path>/tmp/idc/proc_data</remote_path> 远程服务器存放文件的目录。\n");
  printf("<local_path>/tmp/idc/download_proc_data</local_path> 本地文件存放的目录。\n");
  printf("<match_name>SURF_ZH*.XML,SURF_ZH*.CSV</match_name> 待下载文件匹配的规则。"
         "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n\n\n");

  printf("<list_filename>/tmp/proc_data/remote_ftp_file.list</list_filename> 下载前列出服务器文件名的文件。\n\n\n");

  printf("<proc_type>1</proc_type> 文件下载成功后，远程服务器文件的处理方式：1-什么也不做；2-删除；3-备份，"
         "如果为3，还要指定备份的目录。\n");
  printf("<remote_backup_dir>/tmp/idc/remote_backup_dir</remote_backup_dir> "
         "文件下载成功后，服务器文件的备份目录，此参数只有当proc_type=3时才有效。\n\n\n");

}

bool xml_parse(const char *xml_buffer) {
  memset(&ftp_args, 0, sizeof(ftp_args));

  GetXMLBuffer(xml_buffer, "host", ftp_args.host, 30);   // 远程服务器的IP和端口。
  if (strlen(ftp_args.host) == 0) {
    logfile.Write("host is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "mode", &ftp_args.mode);   // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  if (ftp_args.mode != 2) ftp_args.mode = 1;

  GetXMLBuffer(xml_buffer, "username", ftp_args.username, 30);   // 远程服务器ftp的用户名。
  if (strlen(ftp_args.username) == 0) {
    logfile.Write("username is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "password", ftp_args.password, 30);   // 远程服务器ftp的密码。
  if (strlen(ftp_args.password) == 0) {
    logfile.Write("password is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "remote_path", ftp_args.remote_path, 300);   // 远程服务器存放文件的目录。
  if (strlen(ftp_args.remote_path) == 0) {
    logfile.Write("remote_path is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "local_path", ftp_args.local_path, 300);   // 本地文件存放的目录。
  if (strlen(ftp_args.local_path) == 0) {
    logfile.Write("local_path is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "match_name", ftp_args.match_name, 100);   // 待下载文件匹配的规则。
  if (strlen(ftp_args.match_name) == 0) {
    logfile.Write("match_name is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "list_filename", ftp_args.list_filename, 100);   // 待下载文件匹配的规则。
  if (strlen(ftp_args.match_name) == 0) {
    logfile.Write("list_filename is null.\n");
    return false;
  }

  // 下载后服务器文件的处理方式：1-什么也不做；2-删除；3-备份。
  GetXMLBuffer(xml_buffer, "proc_type", &ftp_args.proc_type);
  if ( (ftp_args.proc_type!=1) && (ftp_args.proc_type!=2) && (ftp_args.proc_type!=3) ) {
    logfile.Write("proc_type is error.\n");
    return false; 
  }


  GetXMLBuffer(xml_buffer, "remote_backup_dir", ftp_args.remote_backup_dir, 100);   // 待下载文件匹配的规则。
  if ( (ftp_args.proc_type==3) && (strlen(ftp_args.remote_backup_dir)==0) ) {
    logfile.Write("remote_backup_dir is null.\n");
    return false;
  }
  
  return true;
}
void EXIT(int signal) {
  printf("程序退出，sig=%d\n\n", signal);
  exit(0);
}
bool ftp_get_files() {
  // 进入ftp服务器存放文件的目录。
  if (ftp.chdir(ftp_args.remote_path)==false){
    logfile.Write("ftp.chdir(%s) failed.\n",ftp_args.remote_path); return false;
  }
  
  // 调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件中。
  if (ftp.nlist(".",ftp_args.list_filename)==false){
    logfile.Write("ftp.nlist(%s) failed.\n",ftp_args.remote_path); return false;
  }

  // 把ftp.nlist()方法获取到的list文件加载到容器vec_file_list中。
  if (load_list_file()==false){
    logfile.Write("LoadListFile() failed.\n");  return false;
  }

  if (strlen(ftp_args.remote_backup_dir) != 0){
    if( ftp.mkdir(ftp_args.remote_backup_dir) == false){
      logfile.Write("dir exist or ftp.mkdir(%s) failed.\n",ftp_args.remote_backup_dir);
    }
  }

  char remote_filename[301],local_filename[301];

  // 遍历容器vec_file_list。
  for (int ii=0;ii<vec_file_list.size();ii++)
  {
    memset(remote_filename,0,sizeof (remote_filename));
    memset(local_filename,0,sizeof (local_filename));

    SNPRINTF(remote_filename,sizeof(remote_filename),300,"%s/%s",ftp_args.remote_path,vec_file_list[ii].filename);
    SNPRINTF(local_filename,sizeof(local_filename),300,"%s/%s",ftp_args.local_path,vec_file_list[ii].filename);

    // 调用ftp.get()方法从服务器下载文件。
    logfile.Write("get %s ...",remote_filename);

    if (ftp.get(remote_filename,local_filename)==false)
    {
      logfile.WriteEx("failed.\n"); break;
    }

    logfile.WriteEx("ok.\n");

    //  del file
    if (ftp_args.proc_type == 2){
      if (ftp.ftpdelete(remote_filename) == false){
        logfile.Write("ftp.ftpdelete(%s) failed.\n",remote_filename); return false;
      }
    }

    //  转到备份目录
    if (ftp_args.proc_type == 3){

      char str_remote_backup_dir[301];
      SNPRINTF(str_remote_backup_dir,sizeof(str_remote_backup_dir),300,"%s/%s",ftp_args.remote_backup_dir,vec_file_list[ii].filename);
      if (ftp.ftprename(remote_filename,str_remote_backup_dir)==false)
      {
        logfile.Write("ftp.ftprename(%s,%s) failed.\n",remote_filename,str_remote_backup_dir); return false;
      }
    }
  }
  return true;

}
bool load_list_file() {
  vec_file_list.clear();
  CFile file;

  if (file.Open(ftp_args.list_filename,"r")==false){
    logfile.Write("File.Open(%s) 失败。\n",ftp_args.list_filename); return false;
  }

  FileInfo_t file_info;
  while (true)
  {
    memset(&file_info,0,sizeof(FileInfo_t));

    if (file.Fgets(file_info.filename,300,true)==false) break;

    if (MatchStr(file_info.filename,ftp_args.match_name)==false) continue;

    vec_file_list.push_back(file_info);
  }
  return true;
}
