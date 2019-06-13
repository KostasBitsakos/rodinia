#!/bin/bash

make clean
make OUTPUT=Y VERIFY=Y TIMER=Y
make HISTO_WG_SIZE_0="-DHISTO_WG_SIZE_0=96"
make BUCKET_WG_SIZE_0="-DBUCKET_WG_SIZE_0=128"
make BUCKET_WG_SIZE_1="-DBUCKET_WG_SIZE_1=32"
make MERGE_WG_SIZE_0="-DMERGE_WG_SIZE_0=256"
make MERGE_WG_SIZE_1="-DMERGE_WG_SIZE_1=208"


