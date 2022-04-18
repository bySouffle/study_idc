//
// Created by bysou on 2022/4/18.
//

#ifndef STUDY_IDC_FUNCTION_UNITS_JSON_ST_STATION_DATA_H_
#define STUDY_IDC_FUNCTION_UNITS_JSON_ST_STATION_DATA_H_

#include <utility>

#include "archiver.h"

class CStationData {
 public:
  explicit CStationData(const std::string &obt_id,
                       const std::string &datetime_,
                       int t,
                       int p,
                       int u,
                       int wd,
                       int wf,
                       int r,
                       int vis);
  ~CStationData() = default;

 protected:
  std::string obt_id_;      // 站点代码。
  std::string datetime_;  // 数据时间：格式yyyymmddhh24miss
  int t_;              // 气温：单位，0.1摄氏度。
  int p_;              // 气压：0.1百帕。
  int u_;              // 相对湿度，0-100之间的值。
  int wd_;             // 风向，0-360之间的值。
  int wf_;             // 风速：单位0.1m/s
  int r_;              // 降雨量：0.1mm。
  int vis_;            // 能见度：0.1米。

  template<typename Archiver>
  friend Archiver &operator&(Archiver &ar, CStationData &s);
};

template<typename Archiver>
Archiver &operator&(Archiver &ar, CStationData &s) {
  ar.StartObject();
  ar.Member("obt_id") & s.obt_id_;
  ar.Member("datetime") & s.datetime_;
  ar.Member("t") & s.t_;
  ar.Member("p") & s.p_;
  ar.Member("u") & s.u_;
  ar.Member("wd") & s.wd_;
  ar.Member("wf") & s.wf_;
  ar.Member("r") & s.r_;
  ar.Member("vis") & s.vis_;
  return ar.EndObject();
}


#endif //STUDY_IDC_FUNCTION_UNITS_JSON_ST_STATION_DATA_H_
