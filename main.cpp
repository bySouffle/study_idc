#include "public/_public.h"

CLogFile logfile; //  文件操作class

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

// 模拟生成站点分钟观测数据，存放在 vec_station_data 中。
void simulation_gen_data();

int main(int argc, char *argv[]) {

  std::cout << "Hello, World!" << std::endl;
  if ((argc != 5) && (argc != 6)) {
    // 如果参数非法，给出帮助文档。
    printf("Using:   ./study_idc inifile outpath logfile\n");
    printf("Example: ./study_idc /tmp/study_idc/ini/station_code.ini /tmp/proc_data /tmp/study_idc/log_info.log\n\n");
    printf("         ./study_idc /tmp/study_idc/ini/station_code.ini 2 /tmp/study_idc/log_info.log 4 5\n");

    printf("inifile  参数文件名。\n");
    printf("outpath  数据文件存放的目录。\n");
    printf("logfile  本程序运行的日志文件名。\n");

    return -1;
  }

  // 打开程序的日志文件。
  if (logfile.Open(argv[3], "a+", false) == false) {
    printf("logfile.Open(%s) failed.\n", argv[3]);
    return -1;
  }
  logfile.Write("study_idc 开始运行。\n");

  // 把站点参数文件中加载到vec_station_code容器中。
  if (LoadSTCode(argv[1]) == false) return -1;

  // 模拟生成站点分钟观测数据。
  {
    simulation_gen_data();
    for (auto & it : vec_station_data){
    logfile.Write("obt_id=%s,date_time=%s,t=%d,p=%d,u=%d,wd=%d,wf=%d,r=%d,vis=%d\n",
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

  logfile.Write("study_idc 运行结束。\n");
  return 0;
}

// 把站点参数文件中加载到vec_station_code容器中。
bool LoadSTCode(const char *ini_file) {
  CFile File;

  // 打开站点参数文件。
  if (File.Open(ini_file, "r") == false) {
    logfile.Write("File.Open(%s) failed.\n", ini_file);
    return false;
  }

  char strBuffer[256];

  CCmdStr CmdStr;

  StationCode_t station_code;

  while (true) {
    // 从站点参数文件中读取一行，如果已读取完，跳出循环。
    if (File.Fgets(strBuffer, 255, true) == false) break;

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
    logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",
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
  srand(time(nullptr));
  char str_date_time[21] = {};
  LocalTime(str_date_time, "yyyymmddhh24miss");

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

}
