#!/bin/sh

AddRecoveryLog(){
  logfile="/root/data/daemon.log"
  if [ -f $logfile ];then
    size=$(ls -l $logfile | awk '{print $5}')
    if [ $size -gt 4096 ];then
      rm $logfile
    fi
  fi
  echo "$1[$2]: restart." >> $logfile
}

TagMemory(){
  memlogfile="/root/data/mem.log"
  mmalogfile="/root/data/mma.log"
  if [ -f $memlogfile ];then
    size=$(ls -l $memlogfile | awk '{print $5}')
    if [ $size -gt 524288 ];then
      rm $memlogfile
    fi
  fi
   if [ -f $mmalogfile ];then
    size=$(ls -l $mmalogfile | awk '{print $5}')
    if [ $size -gt 524288 ];then
      rm $mmalogfile
    fi
  fi
  memMessage=$(echo 1 > /proc/sys/vm/drop_caches ; free -m | grep Mem:)
  mmaMessage=$(cat /sys/kernel/debug/ion/cvi_carveout_heap_dump/summary | grep usage;)
  echo "mem[$1]: $memMessage" >> $memlogfile
  echo "mma[$1]: $mmaMessage" >> $mmalogfile
}

AppCheckAndRecovery(){
  App=$(ps | grep $1 | grep -v grep)
  if [[ "${App/$1//}" == "$App" ]];then
    # 写入日志文件
    AddRecoveryLog "$1" "$2"
    if [[ $1 == "EmxCoreServer" ]];then #|| $1 == "EmxMediaServer"
      # 不可恢复进程，重启系统
      reboot
    else
      # 服务重新启动
      $1 -b
    fi
  fi
}

while true
do
  sleep 10
  currentTime=$(date +"%Y-%m-%d %H:%M:%S")
  TagMemory "$currentTime"
  AppCheckAndRecovery EmxCoreServer "$currentTime"
  AppCheckAndRecovery EmxMediaServer "$currentTime"
  AppCheckAndRecovery EmxModulesServer "$currentTime"
  AppCheckAndRecovery EapilMain "$currentTime"
  # AppCheckAndRecovery EmxAgeingServer "$currentTime"
  AppCheckAndRecovery EmxFctToolsV1 "$currentTime"
done
