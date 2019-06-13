#ifndef __BUCKETSORT
#define __BUCKETSORT

#define LOG_DIVISIONS	10
#define DIVISIONS		(1 << LOG_DIVISIONS)

void init_bucketsort(long long listsize);
void finish_bucketsort();
void bucketSort(float *d_input, float *d_output, long long listsize,
				int *sizes, int *nullElements, float minimum, float maximum,
				unsigned int *origOffsets);
void histogram1024GPU(
                      unsigned int *h_Result,
                      float *d_Data,
                      float minimum,
                      float maximum,
                      int dataN);
void histogramInit(long long listsize);
void finish_histogram();
double getBucketTime();

#endif
