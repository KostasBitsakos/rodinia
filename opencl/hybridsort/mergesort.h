#ifndef __MERGESORT
#define __MERGESORT

#include "bucketsort.h"

cl_float4 *runMergeSort(long long listsize, int divisions,
					 cl_float4 *d_origList, cl_float4 *d_resultList,
					 int *sizes, int *nullElements,
					 unsigned int *origOffsets);
void init_mergesort(long long listsize);
void finish_mergesort();
double getMergeTime();
#endif
