#!/bin/sh

ALL_SERVER="EmxCoreServer
NetServer
NtpClient
EmxMediaServer
RtspServer
SDCardServer
UpdateServer
NightVisionServer
EapilMain
EmxFctToolsV1
EmxAgeingServer"

printname()
{
	printf "%-10s %-20s %-20s %-20s %-10s %-10s %-10s %-10s %-10s\n" "Pid" "ProcName" "PssDeletedMem" "PssSharedLibMem" "PssMem" "RealMem" "ThrNum" "PssMemSum" "RealMemSum"
	return 0
}

RSize=0
VSize=0
PSize=0

printvalue()
{
	RSize=0
	VSize=0
	PSize=0
	cd /proc
	for ecpt in $ALL_SERVER
	do
#		echo $ecpt
		allprocid=`ps | grep $ecpt | grep -v '/bin/sh' | grep -v 'grep' | awk '{print $1}'`
#		echo $allprocid
		for procid in $allprocid
		do
			SizeM=`cat $procid/status|grep 'VmSize'|awk '{print $2}'`
			RssM=`cat $procid/status|grep 'VmRSS'|awk '{print $2}'`
			ThreadsNum=`cat $procid/status |grep 'Threads'|awk '{printf $2}'`
			PssM=`cat $procid/smaps | grep 'Pss'|awk '{print $2}'`
			PssSharedM=`cat $procid/smaps | grep '\.so' -A 3 | grep 'Pss'|awk '{print $2}'`
			PssDeletedM=`cat $procid/smaps | grep 'delete' -A 3 | grep 'Pss'|awk '{print $2}'`
#			echo "$ecpt, $SizeM, $RssM, $ThreadsNum"
			RSize=$(($RSize + $RssM))
			VSize=$(($VSize + $SizeM))
			pss_sum=0
			pss_shared_sum=0
			pss_deleted_sum=0
			#printf "%-20s,%-10u,%-10u,%-10u,%-10u,%-10u\n" $ecpt $SizeM $RssM $ThreadsNum $VSize $RSize
			for pss_sigle in $PssM
			do
				pss_sum=$(($pss_sum + $pss_sigle))
			done
			for pss_sigle in $PssSharedM
			do
				pss_shared_sum=$(($pss_shared_sum + $pss_sigle))
			done
			for pss_sigle in $PssDeletedM
			do
				pss_deleted_sum=$(($pss_deleted_sum + $pss_sigle))
			done

			PSize=$(($PSize + $pss_sum))
			printf "%-10s %-20s %-20u %-20u %-10u %-10u %-10u %-10u %-10u\n" $procid $ecpt $pss_deleted_sum $pss_shared_sum $pss_sum $RssM $ThreadsNum $PSize $RSize
		done
	done
	return 0
}


printinfo()
{
	printname	
	printvalue
	
	return 0
}

printinfo
