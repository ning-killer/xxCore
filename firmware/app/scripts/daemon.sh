#!/bin/sh
AddRecoveryLog(){
  logfile="/tmp/daemon.log"
  if [ -f $logfile ];then
    size=$(ls -l $logfile | awk '{print $5}')
    if [ $size -gt 4096 ];then
      rm $logfile
    fi
  fi
  echo "restart $1 on `date`" >> $logfile
}
AppCheckAndRecovery(){
  App=$(ps | grep $1 | grep -v grep)
  #application disappeared
  if [[ "${App/$1//}" == "$App" ]];then
    $1 $2
    AddRecoveryLog $1
  fi
}

while true
do
  sleep 5
  # if the core service EmxMsgServer is not running, restart all
  App=$(ps | grep EmxMsgServer | grep -v grep)
  if [[ "${App/EmxMsgServer//}" == "$App" ]];then
    allstop.sh
    allstart.sh
    AddRecoveryLog "all"
  else
    AppCheckAndRecovery EmxLogServer
    AppCheckAndRecovery EmxEnvServer
    AppCheckAndRecovery EmxNtpClient
    AppCheckAndRecovery EmxSDCardServer
    AppCheckAndRecovery EmxUpdateServer
    AppCheckAndRecovery EmxMediaServer
    AppCheckAndRecovery EmxNetServer
    AppCheckAndRecovery EmxTools
    AppCheckAndRecovery EmxMain
  fi
done