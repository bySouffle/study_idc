//
// Created by bysou on 2022/4/18.
//

#include "json/st_station_data.h"
#include "gtest/gtest.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

using namespace std;
using namespace rapidjson;

TEST(station_data, gen){
  CStationData data("123","20220418",1,2,3,4,5,6,7);
  JsonWriter writer;
  writer&data;
  cout << writer.GetString() << "\n";
  StringBuffer s;

  Writer<StringBuffer> w(s);
  w.String(writer.GetString());

  cout << s.GetString() << "\n";
}