#!/bin/bash

TYPE="$1"
size=$2

for i in `seq 1 5`; do cat /home/users/gmytil/rodinia/gpu-rodinia-new/opencl/bfs/$TYPE"_"$size/$TYPE"_"$size"_attempt_"$i.txt >> test;done;awk < test -F":" 'BEGIN{sh2d = 0;sexec=0;sd2h=0}{if($1 == "Exec") sexec+=$2;if($1 == "HtoD") sh2d +=$2;if($1 == "DtoH") sd2h +=$2}END{print "Exec:"sexec/5.0;print "HtoD:"sh2d/5.0;print "DtoH:"sd2h/5.0;print "Total:"sexec/5.0+sh2d/5.0+sd2h/5.0}';rm test
