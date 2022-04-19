//
// Created by bysou on 2022/4/17.
//

#include "gtest/gtest.h"
#include "public/_public.h"

TEST(build_test, test) {
  using namespace std;
  char buff[256] = {};
  timetostr(time(nullptr), buff, nullptr);
  cout << string(buff) << "\n";
}

TEST(logfile, open) {
  CLogFile file;
  file.Open("/tmp/study_idc/loginfo.log", "a+", false);
  file.Write("zzzzzzzzzzz\n");
}

typedef struct SHM_DATA_INFO {
  int pid{};
  char data[];
} SHM_DATA_INFO_t;

#include <sys/ipc.h>
#include <sys/shm.h>

TEST(shm, test_write) {
  //  获取共享内存
  int shm_id = shmget(0x2020, sizeof(SHM_DATA_INFO_t) + 20, 0775 | IPC_CREAT);
  if (shm_id == -1) std::cout << "shmget err\n";
  SHM_DATA_INFO_t *shm_data = nullptr;
  //  映射
  shm_data = static_cast<SHM_DATA_INFO_t *>(shmat(shm_id, nullptr, 0));
  if ((void *) shm_data == (void *) -1) std::cout << "shmat err\n";
  shm_data->pid = 100;
  memcpy(shm_data->data, "1234567890", 11);
  //  分离
  if (shmdt(shm_data) == -1) std::cout << "shmdt err\n";
  //  删除
  shmctl(shm_id, IPC_RMID, nullptr);
}

TEST(shm, test_read) {
  //  获取共享内存
  int shm_id = shmget(0x2020, sizeof(SHM_DATA_INFO_t) + 20, 0775 | IPC_CREAT);
  if (shm_id == -1) std::cout << "shmget err\n";
  SHM_DATA_INFO_t *shm_data = nullptr;
  //  映射
  shm_data = static_cast<SHM_DATA_INFO_t *>(shmat(shm_id, nullptr, 0));
  if ((void *) shm_data == (void *) -1) std::cout << "shmat err\n";

  std::cout << "shm_id = " << shm_id << " pid = " << shm_data->pid << " shm_data = " << std::string(shm_data->data)
            << "\n";

  //  分离
  if (shmdt(shm_data) == -1) std::cout << "shmdt err\n";
  //  删除
  shmctl(shm_id, IPC_RMID, nullptr);
}

TEST(sem, pv_opt){

}
