#ifndef __CKATSAK_H__
#define __CKATSAK_H__

#include <stddef.h>  // size_t

typedef enum kern {
	kern_mergeSortFirst,
//	kern_mergeSortPass,
	kern_mergepack,
	kern_histogram1024Kernel,
	kern_bucketcount,
	kern_bucketprefixoffset,
	kern_bucketsort,

	kern_NUM_KERNELS,
} kern_t;

extern float init_time, close_time, total_time;

extern const char *kern_name[kern_NUM_KERNELS];

extern float  mem_alloc_time[kern_NUM_KERNELS];
extern size_t mem_alloc_bytes[kern_NUM_KERNELS];
extern float  h2d_time[kern_NUM_KERNELS];
extern size_t h2d_bytes[kern_NUM_KERNELS];
extern float  exec_time[kern_NUM_KERNELS];
extern float  d2h_time[kern_NUM_KERNELS];
extern size_t d2h_bytes[kern_NUM_KERNELS];

#endif /* __CKATSAK_H__ */
