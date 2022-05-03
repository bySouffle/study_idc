//
// Created by bysou on 2022/5/3.
//

#include "gtest/gtest.h"
#include "ftp/_ftp.h"

TEST(login, test){
  Cftp ftp;
  bool ret_val = ftp.login("192.168.3.50:2121", "bys", "2020", 1);
  ASSERT_TRUE(ret_val);
  ret_val = ftp.logout();
  ASSERT_TRUE(ret_val);
}

TEST(nlist, test){
  Cftp ftp;
  bool ret_val = ftp.login("192.168.3.50:2121", "bys", "2020", 1);
  ASSERT_TRUE(ret_val);
  char list[1024] = {};
  ret_val = ftp.nlist("/", list);
  ASSERT_TRUE(ret_val);
  ret_val = ftp.logout();
  ASSERT_TRUE(ret_val);
}

TEST(dir, test){
  Cftp ftp;
  bool ret_val = ftp.login("192.168.3.50:2121", "bys", "2020", 1);
  ASSERT_TRUE(ret_val);
  ret_val = ftp.chdir("/");
  ASSERT_TRUE(ret_val);

  ret_val = ftp.nlist("/", "/tmp/zzz.log");
  ASSERT_TRUE(ret_val);
  ret_val = ftp.logout();
  ASSERT_TRUE(ret_val);
}
