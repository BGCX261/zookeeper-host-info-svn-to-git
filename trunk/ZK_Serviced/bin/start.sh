#!/bin/sh
root=/data3/muyanxue/qdev/ZK_FrameWork/ZK_Serviced/
logs=${root}/logs/
bin=${root}/bin/

daemon=true
#pid=/home/s/var/proc/
pid=${logs}



worker=$1



if [ ${worker} == ZK_Serviced ]
then
	${bin}/ZK_Serviced --query_interval=5000 --log_path=${logs} --daemonize=${daemon} --pid_path=${pid} >/dev/null 2>&1
fi
