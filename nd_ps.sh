#!/bin/sh
for k in 256 512 1024
do
	for j in 1 2 3
	do
		for i in 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30
		do
  		./waf --run "scratch/wifi1 --node=$i --psize=$k" >> throughput.txt
		done
	done
done
./read1
chmod +x mac.pg
chmod +x phy.pg
./mac.pg > mac.png
./phy.pg > phy.png
