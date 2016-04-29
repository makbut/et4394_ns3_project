#!/bin/sh
for i in 0 5 10
do
	for k in 0 10 20
	do
		for h in 1 2 3
		do
			for j in 5 15 25 35 45 55 65 75 85 95 105 115 125 135 145 155 165 175 185 195 205 215 225 235 245 255 265
			do
  				./waf --run "scratch/wifi0 --node=2 --rnf=$k --dist=$j --rxg=$i" >> Distance.txt
			done
		done
	done
done
./read
chmod +x 0.pg
chmod +x 5.pg
chmod +x 10.pg
./0.pg > 0.png
./5.pg > 5.png
./10.pg > 10.png

