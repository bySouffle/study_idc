#!/bin/bash
###################### 启动项目脚本 ######################

# 检查服务程序是否超时，配置在/etc/rc.local中由root用户执行。
#/exec_test/service_scheduler 30 /exec_test/heart_check
#su - bys -c "/bin/bash /tmp/study_idc/exec_test/start.sh"

# 压缩后台服务程序的备份日志。
/tmp/study_idc/exec_test/service_scheduler 30 /tmp/study_idc/exec_test/tools/gzip_files  /tmp/proc_data "*.log.20*" 0.02

# 生成用于测试的全国气象站点观测的分钟数据。
/tmp/study_idc/exec_test/service_scheduler 60 /tmp/study_idc/study_idc /tmp/study_idc/ini/station_code.ini /tmp/proc_data /tmp/study_idc/log_info.log xml,json,csv

# 清理原始的全国气象站点观测的分钟数据目录 /tmp/proc_data 中的历史数据文件。
/tmp/study_idc/exec_test/service_scheduler 300 /tmp/study_idc/exec_test/tools/delete_files /tmp/proc_data "*" 0.02
