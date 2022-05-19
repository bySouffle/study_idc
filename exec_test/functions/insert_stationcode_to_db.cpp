//
// Created by bysou on 2022/5/19.
//

//! 站点数据保存到 mysql T_StationParam表中
/**********************************************************
DROP TABLE
IF
	EXISTS T_StationParam;
CREATE TABLE T_StationParam (
	obtid VARCHAR ( 10 ) NOT NULL COMMENT '站点代码;站点代码',
	cityname VARCHAR ( 30 ) NOT NULL COMMENT '城市名称;城市名称',
	provname VARCHAR ( 30 ) NOT NULL COMMENT '省名称;省名称',
	lat INT NOT NULL COMMENT '纬度;纬度，单位：0.01°',
	lon INT NOT NULL COMMENT '经度;经度，单位：0.01°',
	height INT NOT NULL COMMENT '海拔高度;海拔高度，单位：0.1m',
	upttime TIMESTAMP NOT NULL COMMENT '更新时间;更新时间，数据被插入或者更新的时间',
	keyid INT NOT NULL AUTO_INCREMENT COMMENT '记录编号;记录编号，自动增长列',
	PRIMARY KEY ( obtid ),
    UNIQUE KEY StationParam_KEYID ( keyid )
) COMMENT = '站点参数';
*********************************************************/

#include "_public.h"
#include "db/_mysql.h"

// 站点参数结构体。
typedef struct StationCode {
  char province_name[31]; // 省
  char obt_id[11];        // 站号
  char obt_name[31];      // 站名
  char lat[11];      // 纬度
  char lon[11];      // 经度
  char height[11];   // 海拔高度
} StationCode_t;

// 存放站点参数的容器。
std::vector<StationCode_t> vec_station_code;

// 把站点参数文件中加载到vec_station_code容器中。
bool LoadSTCode(const char *ini_file);

CLogFile logfile;

Connection conn;

CPActive active;

void help();

void EXIT(int sig);

int main(int argc,char *argv[])
{
  // 帮助文档。
  if (argc!=5)
  {
    help();
    return -1;

  }

  // 关闭全部的信号和输入输出。
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
  // 但请不要用 "kill -9 +进程号" 强行终止。
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 打开日志文件。
  if (logfile.Open(argv[4],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[4]); return -1;
  }

  active.AddPInfo(10,"obtcodetodb");   // 进程的心跳，10秒足够。
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
  // PActive.AddPInfo(5000,"obtcodetodb");

  // 把全国站点参数文件加载到vstcode容器中。
  if (LoadSTCode(argv[1])==false) return -1;

  logfile.Write("加载参数文件（%s）成功，站点数（%d）。\n",argv[1],vec_station_code.size());

  // 连接数据库。
  if (conn.connect_to_db(argv[2],argv[3])!=0)
  {
    logfile.Write("connect database(%s) failed.\n%s\n",argv[2],conn.m_cda.message); return -1;
  }

  logfile.Write("connect database(%s) ok.\n",argv[2]);

  StationCode_t station_code;

  // 准备插入表的SQL语句。
  SqlStatement st_insert(&conn);
  st_insert.prepare("insert into T_StationParam(obtid,cityname,provname,lat,lon,height,upttime) values(:1,:2,:3,:4*100,:5*100,:6*10,now())");
  st_insert.bind_in(1,station_code.obt_id,10);
  st_insert.bind_in(2,station_code.obt_name,30);
  st_insert.bind_in(3,station_code.province_name,30);
  st_insert.bind_in(4,station_code.lat,10);
  st_insert.bind_in(5,station_code.lon,10);
  st_insert.bind_in(6,station_code.height,10);

  // 准备更新表的SQL语句。
  SqlStatement st_update(&conn);
  st_update.prepare("update T_StationParam set cityname=:1,provname=:2,lat=:3*100,lon=:4*100,height=:5*10,upttime=now() where obtid=:6");
  st_update.bind_in(1,station_code.obt_name,30);
  st_update.bind_in(2,station_code.province_name,30);
  st_update.bind_in(3,station_code.lat,10);
  st_update.bind_in(4,station_code.lon,10);
  st_update.bind_in(5,station_code.height,10);
  st_update.bind_in(6,station_code.obt_id,10);

  int insert_cnt=0,update_cnt=0;
  CTimer Timer;

  // 遍历vec_station_code容器。
  for (int ii=0;ii<vec_station_code.size();ii++)
  {
    // 从容器中取出一条记录到结构体stcode中。
    memcpy(&station_code,&vec_station_code[ii],sizeof(station_code));

    // 执行插入的SQL语句。
    if (st_insert.execute()!=0)
    {
      if (st_insert.m_cda.rc==1062)
      {
        // 如果记录已存在，执行更新的SQL语句。
        if (st_update.execute()!=0)
        {
          logfile.Write("stmtupt.execute() failed.\n%s\n%s\n",st_update.m_sql,st_update.m_cda.message); return -1;
        }
        else
          update_cnt++;
      }
      else
      {
        logfile.Write("stmtins.execute() failed.\n%s\n%s\n",st_insert.m_sql,st_insert.m_cda.message); return -1;
      }
    }
    else
      insert_cnt++;
  }

  // 把总记录数、插入记录数、更新记录数、消耗时长记录日志。
  logfile.Write("总记录数=%d，插入=%d，更新=%d，耗时=%.2f秒。\n",
                vec_station_code.size(),insert_cnt,update_cnt,Timer.Elapsed());

  // 提交事务。
  conn.commit();

  return 0;
}


void help(){
  printf("\n");
  printf("Using:./insert_stationcode_to_db ini_file mysql_info charset logfile\n");
  printf("Example:./insert_stationcode_to_db "
         "/tmp/study_idc/ini/station_code.ini "
         "\"172.20.92.218,root,2020,idc,3306\" "
         "utf8 "
         "/tmp/study_idc/log_info.log\n\n");

  printf("Example:./service_scheduler 120 "
         "/insert_stationcode_to_db /tmp/study_idc/ini/station_code.ini "
         "\"172.20.92.218,root,2020,idc,3306\" "
         "utf8 "
         "/tmp/study_idc/log_info.log\n\n");

  printf("本程序用于把站点参数数据保存到数据库表中，如果站点不存在则插入，站点已存在则更新。\n");
  printf("ini_file 站点参数文件名（全路径）。\n");
  printf("mysql_info 数据库连接参数：ip,username,password,dbname,port\n");
  printf("charset 数据库的字符集。\n");
  printf("logfile 本程序运行的日志文件名。\n");
  printf("程序每120秒运行一次，由service_scheduler调度。\n\n\n");
}

void EXIT(int sig){
  logfile.Write("程序退出，sig=%d\n\n",sig);
  conn.disconnect();
  exit(0);
}

bool LoadSTCode(const char *ini_file){
  // 打开站点参数文件。
  CFile file_;
  if (file_.Open(ini_file, "r") == false) {
    logfile.Write("file_.Open(%s) failed.\n", ini_file);
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
    CmdStr.GetValue(3, station_code.lat,10);      // 纬度
    CmdStr.GetValue(4, station_code.lon,10);      // 经度
    CmdStr.GetValue(5, station_code.height,10);   // 海拔高度

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
