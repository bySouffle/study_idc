//
// Created by bysou on 2022/4/17.
//

#include "gtest/gtest.h"
#include "public/_public.h"

TEST(build_test, test){
  using namespace std;
  char buff[256] = {};
  timetostr(time(nullptr ), buff, nullptr);
  cout << string (buff) << "\n";
}

TEST(logfile, open){
  CLogFile file;
  file.Open("/tmp/study_idc/loginfo.log", "a+", false);
  file.Write("zzzzzzzzzzz\n");
}