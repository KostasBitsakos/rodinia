#include "ckatsak.h"

const char *kern_name[] = {
	[kern_mergeSortFirst]      = "mergeSortFirst",
//	[kern_mergeSortPass]       = "mergeSortPass",
	[kern_mergepack]           = "mergepack",
	[kern_histogram1024Kernel] = "histogram1024Kernel",
	[kern_bucketcount]         = "bucketcount",
	[kern_bucketprefixoffset]  = "bucketprefixoffset",
	[kern_bucketsort]          = "bucketsort",
};

float init_time = 0, close_time = 0, total_time = 0;

float  mem_alloc_time[kern_NUM_KERNELS]  = {0};
size_t mem_alloc_bytes[kern_NUM_KERNELS] = {0};
float  h2d_time[kern_NUM_KERNELS]        = {0};
size_t h2d_bytes[kern_NUM_KERNELS]       = {0};
float  exec_time[kern_NUM_KERNELS]       = {0};
float  d2h_time[kern_NUM_KERNELS]        = {0};
size_t d2h_bytes[kern_NUM_KERNELS]       = {0};
