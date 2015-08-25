#!/bin/sh
process=$1
while true 
do
	cnt=$(ps -wwwwaux | grep $process | wc -l)
	if [ 1 -eq $cnt ]
	then
		echo "done"
		break;
	fi
	$(ps -wwwwaux | grep $process | awk '{print $2}' | xargs kill -9) > /dev/null 2>&1
	echo "stoping......"
	sleep 1
done
