#include "public/_public.h"
#include "json/st_station_data.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

CPActive active_;  //  进程心跳

CLogFile logfile_; //  文件操作class

CFile file_;   // 文件操作对象

// 站点参数结构体。
typedef struct StationCode {
  char province_name[31]; // 省
  char obt_id[11];        // 站号
  char obt_name[31];      // 站名
  double lat;             // 纬度
  double lon;             // 经度
  double height;          // 海拔高度
} StationCode_t;

// 存放站点参数的容器。
std::vector<StationCode_t> vec_station_code;

// 把站点参数文件中加载到vec_station_code容器中。
bool LoadSTCode(const char *ini_file);

// 站点分钟观测数据结构
typedef struct StationData
{
  char obt_id[11];      // 站点代码。
  char date_time[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
}StationData_t;

std::vector<StationData_t> vec_station_data;  // 存放站点分钟观测数据的容器
char str_date_time[21] = {};

// 模拟生成站点分钟观测数据，存放在 vec_station_data 中。
void simulation_gen_data();

//  把 vec_station_data 数据写入文件
bool save_station_data(const char *out_path,const char *data_fmt);

//  程序退出信号处理函数
void EXIT(int signal);

int main(int argc, char *argv[]) {

  std::cout << "Hello, World!" << std::endl;
  if ((argc != 5) && (argc != 6)) {
    // 如果参数非法，给出帮助文档。
    printf("Using:   ./study_idc ini_file out_path logfile_ data_fmt [date_time]\n");
    printf("Example: ./study_idc /tmp/study_idc/ini/station_code.ini /tmp/proc_data /tmp/study_idc/log_info.log\n\n");
    printf("         ./study_idc /tmp/study_idc/ini/station_code.ini /tmp/proc_data /tmp/study_idc/log_info.log xml,json,csv\n");
    printf("         ./study_idc /tmp/study_idc/ini/station_code.ini /tmp/proc_data /tmp/study_idc/log_info.log xml,json,csv 20220423204000\n");

    printf("inifile  参数文件名。\n");
    printf("outpath  数据文件存放的目录。\n");
    printf("logfile_  本程序运行的日志文件名。\n");
    printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。\n\n");


    return -1;
  }

  //  关闭全部信号与IO
  CloseIOAndSignal(true);
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
  // 但请不要用 "kill -9 +进程号" 强行终止。
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);
  
  // 打开程序的日志文件。
  if (logfile_.Open(argv[3], "a+", false) == false) {
    printf("logfile_.Open(%s) failed.\n", argv[3]);
    return -1;
  }
  logfile_.Write("study_idc 开始运行。\n");

  active_.AddPInfo(20, "study_idc"); //  添加心跳信息到SHM
  
  // 把站点参数文件中加载到vec_station_code容器中。
  if (LoadSTCode(argv[1]) == false) return -1;

  //  获取当前时间作为观测时间
  memset(str_date_time, 0, sizeof (str_date_time));
  if (argc == 5) LocalTime(str_date_time, "yyyymmddhh24miss");
  else STRCPY(str_date_time, sizeof (str_date_time), argv[5]);
  
  // 模拟生成站点分钟观测数据。
  simulation_gen_data();

  // 把 vec_station_data 写入文件。
  if(strstr(argv[4], "xml")!= nullptr) save_station_data(argv[2], "xml");
  if(strstr(argv[4], "json")!= nullptr) save_station_data(argv[2], "json");
  if(strstr(argv[4], "csv")!= nullptr) save_station_data(argv[2], "csv");

  logfile_.Write("study_idc 运行结束。\n");
  return 0;
}

// 把站点参数文件中加载到vec_station_code容器中。
bool LoadSTCode(const char *ini_file) {
  // 打开站点参数文件。
  if (file_.Open(ini_file, "r") == false) {
    logfile_.Write("file_.Open(%s) failed.\n", ini_file);
    return false;
  }

  char strBuffer[256];

  CCmdStr CmdStr;

  StationCode_t station_code;

  while (true) {
    // 从站点参数文件中读取一行，如果已读取完，跳出循环。
    if (file_.Fgets(strBuffer, 255, true) == false) break;

    // 把读取到的一行拆分。
    CmdStr.SplitToCmd(strBuffer, ",", true);

    if (CmdStr.CmdCount() != 6) continue;     // 扔掉无效的行。

    // 把站点参数的每个数据项保存到站点参数结构体中。
    memset(&station_code, 0, sizeof(StationCode_t));
    CmdStr.GetValue(0, station_code.province_name, 30); // 省
    CmdStr.GetValue(1, station_code.obt_id, 10);    // 站号
    CmdStr.GetValue(2, station_code.obt_name, 30);  // 站名
    CmdStr.GetValue(3, &station_code.lat);         // 纬度
    CmdStr.GetValue(4, &station_code.lon);         // 经度
    CmdStr.GetValue(5, &station_code.height);      // 海拔高度

    // 把站点参数结构体放入站点参数容器。
    vec_station_code.push_back(station_code);
  }

  for (auto & it : vec_station_code){\
    logfile_.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",
                  it.province_name,
                  it.obt_id,
                  it.obt_name,
                  it.lat,
                  it.lon,
                  it.height);
  }

  return true;
}

// 模拟生成站点分钟观测数据，存放在 vec_station_data 中。
void simulation_gen_data() {
//  memset(str_date_time,0,sizeof (str_date_time));
//  LocalTime(str_date_time, "yyyymmddhh24miss");

  srand(time(nullptr));
  StationData_t station_data;

  for (auto & i : vec_station_code) {
    memset(&station_data,0, sizeof(StationData_t));

    //  随机填充
    strncpy(station_data.obt_id, i.obt_id, strlen(i.obt_id));
    strncpy(station_data.date_time, str_date_time, strlen(str_date_time));
    station_data.t=rand()%351;       // 气温：单位，0.1摄氏度
    station_data.p=rand()%265+10000; // 气压：0.1百帕
    station_data.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    station_data.wd=rand()%360;      // 风向，0-360之间的值。
    station_data.wf=rand()%150;      // 风速：单位0.1m/s
    station_data.r=rand()%16;        // 降雨量：0.1mm
    station_data.vis=rand()%5001+100000;  // 能见度：0.1米

    //  加入vector
    vec_station_data.push_back(station_data);
  }

  //  写入日志文件
  for (auto & it : vec_station_data){
    logfile_.Write("obt_id=%s,date_time=%s,t=%d,p=%d,u=%d,wd=%d,wf=%d,r=%d,vis=%d\n",
                  it.obt_id,
                  it.date_time,
                  it.t,
                  it.p,
                  it.u,
                  it.wd,
                  it.wf,
                  it.r,
                  it.vis);
  }

}

//  把 vec_station_data 数据写入文件
bool save_station_data(const char *out_path, const char *data_fmt) {
//  CFile file_;
  //  设置文件名
//  std::string save_filename;
//  save_filename  = save_filename + out_path +
//      "/StationData_" + str_date_time + "_" + std::to_string(getpid()) + "." + data_fmt;
  char save_filename[512] = {};
  sprintf(save_filename,"%s/StationData_%s_%d.%s",out_path,str_date_time,getpid(),data_fmt);

  //  打开文件
  if(file_.OpenForRename(save_filename, "w") == false){
    logfile_.Write("file_.OpenForRename(%s) failed.\n",save_filename); return false;
  }

  //  写入table
  if(strcmp(data_fmt, "csv") == 0){
    file_.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
  }

  if(strcmp(data_fmt, "xml") == 0){
    file_.Fprintf("<data>\n");
  }

  if(strcmp(data_fmt, "json") == 0){
  }

  for (auto &it: vec_station_data) {
    if(strcmp(data_fmt, "csv") == 0) {
      file_.Fprintf("%s,%s,%.1d,%.1d,%d,%d,%.1d,%.1d,%.1d\n",
                    it.obt_id,
                    it.date_time,
                    it.t,
                    it.p,
                    it.u,
                    it.wd,
                    it.wf,
                    it.r,
                    it.vis);
    }
    if(strcmp(data_fmt, "xml") == 0){
      file_.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1d</t><p>%.1d</p>"\
                   "<u>%d</u><wd>%d</wd><wf>%.1d</wf><r>%.1d</r><vis>%.1d</vis><endl/>\n",
                    it.obt_id,
                    it.date_time,
                    it.t,
                    it.p,
                    it.u,
                    it.wd,
                    it.wf,
                    it.r,
                    it.vis);
    }
    if(strcmp(data_fmt, "json") == 0){
      CStationData data(
                   it.obt_id,
                   it.date_time,
                   it.t,
                   it.p,
                   it.u,
                   it.wd,
                   it.wf,
                   it.r,
                   it.vis);
      JsonWriter writer;
      writer & data;

      rapidjson::StringBuffer s;

      rapidjson::Writer<rapidjson::StringBuffer> w(s);
      w.String(writer.GetString());

      file_.Fprintf(s.GetString());
      file_.Fprintf("\n");

    }

    }
  if (strcmp(data_fmt,"xml")== 0) file_.Fprintf("</data>\n");
  if (strcmp(data_fmt,"json")==0) {}

  //  关闭文件
  file_.CloseAndRename();

  //  修改文件时间属性
  UTime(save_filename, str_date_time);

  logfile_.Write("生成数据文件%s成功，数据时间%s，记录数%d。\n", save_filename, str_date_time, vec_station_data.size());
  return true;

}

void EXIT(int signal){
  logfile_.Write("程序退出，sig=%d\n\n",signal);
  exit(0);
}