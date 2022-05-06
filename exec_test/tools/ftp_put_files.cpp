//
// Created by bysou on 2022/5/6.
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
  char match_name[101];     // 待上传文件匹配的规则。
  int  proc_type;              // 上传后服务器文件的处理方式：1-什么也不做；2-删除；3-备份。
  char local_backup_dir[301]; // 上传后服务器文件的备份目录。
  char success_upload_list[301];    // 已上传成功文件名清单。
  int  timeout;            // 进程心跳的超时时间。
  char proc_name[51];          // 进程名，建议用"ftp_put_files_后缀"的方式。
} FtpArgs_t;

//  帮助信息
void help();

//  信号2 15 处理函数
void EXIT(int signal);

//  xml 参数解析
bool xml_parse(const char *xml_buffer);
//  上传文件
typedef struct FileInfo
{
  char filename[301];   // 文件名。
  char mtime[21];       // 文件时间。
}FileInfo_t;

std::vector<FileInfo_t> vec_file_list;              // 存放上传前列出服务器文件名的容器。
std::vector<FileInfo_t> vec_upload_list;        // 已上传成功文件名的容器，从success_upload_list中加载。。
std::vector<FileInfo_t> vec_need_upload_list;     // 本次需要上传的文件的容器。
std::vector<FileInfo_t> vec_no_upload_list;       // 本次不需要上传的文件的容器。

//  加载上传完成的清单到vec_upload_list
bool load_upload_file();

//  比较 vec_file_list | vec_upload_list ==> vec_need_upload_list vec_no_upload_list
bool comp_file_list();

//  把 上传完成的vec_no_upload_list 追加到 vec_upload_list 覆盖旧的文件
bool update_upload_file();

//  if proc_type = 1 把上传成功的文件记录追加到 success_upload_list 文件中。
bool append_uploaded_file(FileInfo_t* file_info);


bool load_list_file();
bool ftp_put_files();

CLogFile logfile;
FtpArgs_t ftp_args{};
Cftp ftp;

CPActive active;  //  进程心跳

int main(int argc, char *argv[]) {
  if (argc != 3) {
    help();
    return -1;
  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
  // 但请不要用 "kill -9 +进程号" 强行终止。
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开日志文件。
  if (logfile.Open(argv[1], "a+") == false) {
    printf("打开日志文件失败（%s）。\n", argv[1]);
    return -1;
  }

  // 解析xml，得到程序运行的参数。
  if (xml_parse(argv[2]) == false) return -1;

  active.AddPInfo(ftp_args.timeout, ftp_args.proc_name);

  // 1. 登录ftp服务器。
  if (ftp.login(ftp_args.host, ftp_args.username, ftp_args.password,
                ftp_args.mode) == false) {
    logfile.Write("ftp.login(%s,%s,%s) failed.[conn %d][login %d][option %d]\n",
                  ftp_args.host, ftp_args.username, ftp_args.password,
                  ftp.m_connectfailed, ftp.m_loginfailed, ftp.m_optionfailed);
    return -1;
  }
  logfile.Write("ftp.login ok.\n");

  ftp_put_files();

  ftp.logout();
  return 0;

}

void help() {
  printf("\n");
  printf("Using:./ftp_put_files logfile_name xml_buffer\n\n");

  printf("Sample:./service_scheduler 30 /${pwd}/tools/ftp_put_files /tmp/idc/proc_data/ftp.log "
         "\"<host>192.168.3.50:2121</host>"
         "<mode>1</mode>"
         "<username>bys</username>"
         "<password>admin</password>"
         "<local_path>/tmp/download_proc_data</local_path>"
         "<remote_path>/Alipay</remote_path>"
         "<match_name>SURF_ZH*.XML,SURF_ZH*.CSV</match_name>"
         "<proc_type>3</proc_type>"
         "<local_backup_dir>/backup</local_backup_dir>"
         "<success_upload_list>/tmp/proc_data/remote_ftp_success_upload.xml</success_upload_list>"
         "<timeout>80</timeout>"
         "<proc_name>ftp_put_files</proc_name>"
         "\"\n\n\n");

  printf("本程序是通用的功能模块，用于把本地目录中的文件上传到远程的ftp服务器。\n");
  printf("logfile_name是本程序运行的日志文件。\n");
  printf("xml_buffer为文件上传的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>bys</username> 远程服务器ftp的用户名。\n");
  printf("<password>admin</password> 远程服务器ftp的密码。\n");
  printf("<remote_path>/tmp/idc/proc_data</remote_path> 远程服务器存放文件的目录。\n");
  printf("<local_path>/tmp/idc/download_proc_data</local_path> 本地文件存放的目录。\n");
  printf("<match_name>SURF_ZH*.XML,SURF_ZH*.CSV</match_name> 待上传文件匹配的规则。"
         "不匹配的文件不会被上传，本字段尽可能设置精确，不建议用*匹配全部的文件。\n\n\n");

  printf("<proc_type>1</proc_type> 文件上传成功后，远程服务器文件的处理方式：1-什么也不做；2-删除；3-备份，"
         "如果为3，还要指定备份的目录。\n");
  printf("<local_backup_dir>/tmp/idc/local_backup_dir</local_backup_dir> "
         "文件上传成功后，本地文件的备份目录，此参数只有当proc_type=3时才有效。\n\n\n");
  printf("<success_upload_list>/tmp/proc_data/remote_ftp_success_upload.xml</success_upload_list> "
         "已上传成功文件名清单，此参数只有当proc_type=1时才有效。\n\n\n");
  printf("<timeout>80</timeout> 上传文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
  printf("<proc_name>ftp_put_files</proc_name> 进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n");

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

  GetXMLBuffer(xml_buffer, "match_name", ftp_args.match_name, 100);   // 待上传文件匹配的规则。
  if (strlen(ftp_args.match_name) == 0) {
    logfile.Write("match_name is null.\n");
    return false;
  }

  // 上传后服务器文件的处理方式：1-什么也不做；2-删除；3-备份。
  GetXMLBuffer(xml_buffer, "proc_type", &ftp_args.proc_type);
  if ( (ftp_args.proc_type!=1) && (ftp_args.proc_type!=2) && (ftp_args.proc_type!=3) ) {
    logfile.Write("proc_type is error.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer, "local_backup_dir", ftp_args.local_backup_dir, 100);   // 待上传文件匹配的规则。
  if ( (ftp_args.proc_type==3) && (strlen(ftp_args.local_backup_dir)==0) ) {
    logfile.Write("local_backup_dir is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer,"success_upload_list",ftp_args.success_upload_list,300); // 已上传成功文件名清单。
  if ( (ftp_args.proc_type==1) && (strlen(ftp_args.success_upload_list)==0) ){
    logfile.Write("success_upload_list is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer,"timeout",&ftp_args.timeout); //  进程心跳的超时时间。
  if (ftp_args.timeout==0) {
    logfile.Write("timeout is null.\n");
    return false;
  }

  GetXMLBuffer(xml_buffer,"proc_name",ftp_args.proc_name,50); // 已上传成功文件名清单。
  if (strlen(ftp_args.proc_name)==0) {
    logfile.Write("proc_name is null.\n");
    return false;
  }

  return true;
}
void EXIT(int signal) {
  printf("程序退出，sig=%d\n\n", signal);
  exit(0);
}
bool ftp_put_files() {

  // 把ftp.nlist()方法获取到的list文件加载到容器vec_file_list中。
  if (load_list_file()==false){
    logfile.Write("LoadListFile() failed.\n");  return false;
  }

  active.UptATime();

  if (ftp_args.proc_type == 1){
    load_upload_file();
    comp_file_list();
    update_upload_file();
    vec_file_list.clear();
    vec_file_list.swap(vec_need_upload_list);
  }

  active.UptATime();

  if (strlen(ftp_args.local_backup_dir) != 0){
    if( ftp.mkdir(ftp_args.local_backup_dir) == false){
      logfile.Write("dir exist or ftp.mkdir(%s) failed.\n",ftp_args.local_backup_dir);
    }
  }

  char local_filename[301],remote_filename[301];

  // 遍历容器vec_file_list。
  for (int ii=0;ii<vec_file_list.size();ii++)
  {
    memset(local_filename,0,sizeof (local_filename));
    memset(remote_filename,0,sizeof (remote_filename));

    SNPRINTF(local_filename,sizeof(local_filename),300,"%s/%s",ftp_args.local_path,vec_file_list[ii].filename);
    SNPRINTF(remote_filename,sizeof(remote_filename),300,"%s/%s",ftp_args.remote_path,vec_file_list[ii].filename);

    // 调用ftp.put()方法上传文件。
    logfile.Write("put %s ...",local_filename);

    if (ftp.put(local_filename,remote_filename)==false)
    {
      logfile.WriteEx("failed.\n"); break;
    }

    logfile.WriteEx("ok.\n");

    active.UptATime();

    // 如果ptype==1，把上传成功的文件记录追加到okfilename文件中。
    if (ftp_args.proc_type==1){
      append_uploaded_file(&vec_file_list[ii]);
    }

    //  del file
    if (ftp_args.proc_type == 2){
      if (REMOVE(local_filename) == false){
        logfile.Write("ftp.ftpdelete(%s) failed.\n",local_filename); return false;
      }
    }

    //  转到备份目录
    if (ftp_args.proc_type == 3){

      char str_local_backup_dir[301];
      SNPRINTF(str_local_backup_dir,sizeof(str_local_backup_dir),300,"%s/%s",ftp_args.local_backup_dir,vec_file_list[ii].filename);
      if (RENAME(local_filename,str_local_backup_dir)==false)
      {
        logfile.Write("ftp.ftprename(%s,%s) failed.\n",local_filename,str_local_backup_dir); return false;
      }
    }
  }
  return true;

}
bool load_list_file() {
  vec_file_list.clear();

  CDir dir;
  dir.SetDateFMT("yyyymmddhh24miss");
  // 不包括子目录。
  // 注意，如果本地目录下的总文件数超过10000，增量上传文件功能将有问题。
  // 建议用delete_files程序及时清理本地目录中的历史文件。

  if (dir.OpenDir(ftp_args.local_path,ftp_args.match_name)==false){
    logfile.Write("Dir.OpenDir(%s) 失败。\n",ftp_args.local_path); return false;
  }

  FileInfo_t file_info{};
    
  while (true)
  {
    memset(&file_info,0,sizeof(FileInfo_t));

    if (dir.ReadDir()==false) break;
    
    strcpy(file_info.filename,dir.m_FileName);   // 文件名，不包括目录名。
    strcpy(file_info.mtime,dir.m_ModifyTime);    // 文件时间。

    vec_file_list.push_back(file_info);
  }
  return true;
}
bool load_upload_file() {
  vec_upload_list.clear();

  CFile file;
  //  第一次上传文件不存在，返回true
  if (file.Open(ftp_args.success_upload_list, "r") == false)  return true;
  char str_buffer[512];
  FileInfo_t file_info{};

  while (true){

    if (file.Fgets(file_info.filename,300, true) == false) break;

    GetXMLBuffer(str_buffer, "filename", file_info.filename);
    GetXMLBuffer(str_buffer, "timestamp", file_info.mtime);
    vec_upload_list.emplace_back(file_info);
  }
  return true;
}
bool comp_file_list() {
  vec_no_upload_list.clear();
  vec_need_upload_list.clear();

  int i,j;
  for ( i = 0; i <  vec_file_list.size(); ++i) {
    //  已经上传的加到vec_no_upload_list
    for ( j = 0; j < vec_upload_list.size(); ++j) {
      if ((strcmp(vec_file_list.at(i).filename, vec_upload_list.at(j).filename) == 0)&&
          (strcmp(vec_file_list.at(i).mtime, vec_upload_list.at(j).mtime) == 0)){
        vec_no_upload_list.emplace_back(vec_file_list.at(i));
        break;
      }
    }
    //  未上传的加到vec_need_upload_list
    if (j == vec_upload_list.size()){
      vec_need_upload_list.emplace_back(vec_file_list.at(i));
    }
  }

  return true;
}
bool update_upload_file() {
  CFile File;

  if (File.Open(ftp_args.success_upload_list,"w")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",ftp_args.success_upload_list); return false;
  }

  for (int i=0; i<vec_no_upload_list.size(); i++)
    File.Fprintf("<filename>%s</filename><timestamp>%s</timestamp>\n",
                 vec_no_upload_list[i].filename, vec_no_upload_list[i].mtime);

  return true;
}

bool append_uploaded_file(FileInfo_t* file_info) {
  CFile File;

  if (File.Open(ftp_args.success_upload_list,"a")==false)
  {
    logfile.Write("File.Open(%s) failed.\n",ftp_args.success_upload_list); return false;
  }

  File.Fprintf("<filename>%s</filename><timestamp>%s</timestamp>\n",
               file_info->filename, file_info->mtime);
  return true;
}

