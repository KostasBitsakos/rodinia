#!/bin/bash

echo "32M nodes CPU"
mkdir cpu_32M
./bfs.out /various/gmytil/graph32M.txt -p 1 -d 0 &> cpu_32M/cpu_32M_attempt_1.txt
./bfs.out /various/gmytil/graph32M.txt -p 1 -d 0 &> cpu_32M/cpu_32M_attempt_2.txt
./bfs.out /various/gmytil/graph32M.txt -p 1 -d 0 &> cpu_32M/cpu_32M_attempt_3.txt
./bfs.out /various/gmytil/graph32M.txt -p 1 -d 0 &> cpu_32M/cpu_32M_attempt_4.txt
./bfs.out /various/gmytil/graph32M.txt -p 1 -d 0 &> cpu_32M/cpu_32M_attempt_5.txt


echo "32M nodes GPU"
mkdir gtx_32M
./bfs.out /various/gmytil/graph32M.txt -p 0 -d 0 &> gtx_32M/gtx_32M_attempt_1.txt
./bfs.out /various/gmytil/graph32M.txt -p 0 -d 0 &> gtx_32M/gtx_32M_attempt_2.txt
./bfs.out /various/gmytil/graph32M.txt -p 0 -d 0 &> gtx_32M/gtx_32M_attempt_3.txt
./bfs.out /various/gmytil/graph32M.txt -p 0 -d 0 &> gtx_32M/gtx_32M_attempt_4.txt
./bfs.out /various/gmytil/graph32M.txt -p 0 -d 0 &> gtx_32M/gtx_32M_attempt_5.txt


echo "64M nodes CPU"
mkdir cpu_64M
./bfs.out /various/gmytil/graph64M.txt -p 1 -d 0 &> cpu_64M/cpu_64M_attempt_1.txt
./bfs.out /various/gmytil/graph64M.txt -p 1 -d 0 &> cpu_64M/cpu_64M_attempt_2.txt
./bfs.out /various/gmytil/graph64M.txt -p 1 -d 0 &> cpu_64M/cpu_64M_attempt_3.txt
./bfs.out /various/gmytil/graph64M.txt -p 1 -d 0 &> cpu_64M/cpu_64M_attempt_4.txt
./bfs.out /various/gmytil/graph64M.txt -p 1 -d 0 &> cpu_64M/cpu_64M_attempt_5.txt


echo "64M nodes GPU"
mkdir gtx_64M
./bfs.out /various/gmytil/graph64M.txt -p 0 -d 0 &> gtx_64M/gtx_64M_attempt_1.txt
./bfs.out /various/gmytil/graph64M.txt -p 0 -d 0 &> gtx_64M/gtx_64M_attempt_2.txt
./bfs.out /various/gmytil/graph64M.txt -p 0 -d 0 &> gtx_64M/gtx_64M_attempt_3.txt
./bfs.out /various/gmytil/graph64M.txt -p 0 -d 0 &> gtx_64M/gtx_64M_attempt_4.txt
./bfs.out /various/gmytil/graph64M.txt -p 0 -d 0 &> gtx_64M/gtx_64M_attempt_5.txt


echo "128M nodes CPU"
mkdir cpu_128M
./bfs.out /various/gmytil/graph128M.txt -p 1 -d 0 &> cpu_128M/cpu_128M_attempt_1.txt
./bfs.out /various/gmytil/graph128M.txt -p 1 -d 0 &> cpu_128M/cpu_128M_attempt_2.txt
./bfs.out /various/gmytil/graph128M.txt -p 1 -d 0 &> cpu_128M/cpu_128M_attempt_3.txt
./bfs.out /various/gmytil/graph128M.txt -p 1 -d 0 &> cpu_128M/cpu_128M_attempt_4.txt
./bfs.out /various/gmytil/graph128M.txt -p 1 -d 0 &> cpu_128M/cpu_128M_attempt_5.txt


echo "128M nodes GPU"
mkdir gtx_128M
./bfs.out /various/gmytil/graph128M.txt -p 0 -d 0 &> gtx_128M/gtx_128M_attempt_1.txt
./bfs.out /various/gmytil/graph128M.txt -p 0 -d 0 &> gtx_128M/gtx_128M_attempt_2.txt
./bfs.out /various/gmytil/graph128M.txt -p 0 -d 0 &> gtx_128M/gtx_128M_attempt_3.txt
./bfs.out /various/gmytil/graph128M.txt -p 0 -d 0 &> gtx_128M/gtx_128M_attempt_4.txt
./bfs.out /various/gmytil/graph128M.txt -p 0 -d 0 &> gtx_128M/gtx_128M_attempt_5.txt


echo "256M nodes CPU"
mkdir cpu_256M
./bfs.out /various/gmytil/graph256M.txt -p 1 -d 0 &> cpu_256M/cpu_256M_attempt_1.txt
./bfs.out /various/gmytil/graph256M.txt -p 1 -d 0 &> cpu_256M/cpu_256M_attempt_2.txt
./bfs.out /various/gmytil/graph256M.txt -p 1 -d 0 &> cpu_256M/cpu_256M_attempt_3.txt
./bfs.out /various/gmytil/graph256M.txt -p 1 -d 0 &> cpu_256M/cpu_256M_attempt_4.txt
./bfs.out /various/gmytil/graph256M.txt -p 1 -d 0 &> cpu_256M/cpu_256M_attempt_5.txt


echo "256M nodes GPU"
mkdir gtx_256M
./bfs.out /various/gmytil/graph256M.txt -p 0 -d 0 &> gtx_256M/gtx_256M_attempt_1.txt
./bfs.out /various/gmytil/graph256M.txt -p 0 -d 0 &> gtx_256M/gtx_256M_attempt_2.txt
./bfs.out /various/gmytil/graph256M.txt -p 0 -d 0 &> gtx_256M/gtx_256M_attempt_3.txt
./bfs.out /various/gmytil/graph256M.txt -p 0 -d 0 &> gtx_256M/gtx_256M_attempt_4.txt
./bfs.out /various/gmytil/graph256M.txt -p 0 -d 0 &> gtx_256M/gtx_256M_attempt_5.txt
