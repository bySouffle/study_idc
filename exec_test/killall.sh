#!/bin/bash
###################### 停止项目脚本 ######################

killall5 -9 service_scheduler
killall5 gzip_files study_idc delete_files

sleep 3

killall5 -9 gzip_files study_idc delete_files
