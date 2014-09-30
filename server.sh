#!/bin/bash

echo "   time     bytes" > times.txt
for i in {1..128}
do
	let tmp=$i*1000000
	./server $tmp >> times.txt
done

echo "   time     bytes" >> times.txt
for i in {1..32}
do
	let tmp=$i*1000000*32
	./server $tmp >> times.txt
done
