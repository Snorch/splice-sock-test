#!/bin/bash

echo " time bytes" > times.txt
for i in {1..64}
do
	let tmp=$i*1000000
	for j in {1..10}
	do
		./server $tmp >> times.txt
	done
	echo "" >> times.txt
done

echo " time bytes" >> times.txt
for i in {1..32}
do
	let tmp=$i*1000000*32
	for j in {1..10}
	do
		./server $tmp >> times.txt
	done
	echo "" >> times.txt
done
