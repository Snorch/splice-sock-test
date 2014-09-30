#!/bin/bash

for i in {1..128}
do
	let tmp=$i*1000000
	./client 127.0.0.1 $tmp
	sleep 0.1
done

for i in {1..32}
do
	let tmp=$i*1000000*32
	./client 127.0.0.1 $tmp
	sleep 0.1
done
