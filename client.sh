#!/bin/bash
if [ -z "$1" ]
then
	echo "please enter ip"
	exit 1
fi
ip="$1"

for i in {1..64}
do
	let size=$i*1000000
	for j in {1..10}
	do
		./client $ip $size
		sleep 0.2
	done
done

for i in {1..32}
do
	let tmp=$i*1000000*32
	for j in {1..10}
	do
		./client $ip $tmp
		sleep 0.2
	done
done
