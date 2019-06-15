////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <fcntl.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <CL/cl.h>
#include "mergesort.h"
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////
#define BLOCKSIZE	256
#define ROW_LENGTH	BLOCKSIZE * 4
#define ROWS		4096
#define DATA_SIZE	(1024)
#define MAX_SOURCE_SIZE	(0x10000000)


cl_device_id device_id;             // compute device id
cl_context mergeContext;                 // compute context
cl_command_queue mergeCommands;
cl_program mergeProgram;                 // compute program
cl_kernel mergeFirstKernel;                   // compute kernel
cl_kernel mergePassKernel;
cl_kernel mergePackKernel;
cl_int err;
cl_mem d_resultList_first_buff;
cl_mem d_origList_first_buff;
cl_mem constStartAddr;
cl_mem finalStartAddr;
cl_mem nullElems;
cl_mem d_orig;
cl_mem d_res;
cl_float4 *d_resultList_first_altered = NULL;
cl_event mergeFirstEvent;
cl_event mergePassEvent;
cl_event mergePackEvent;
double mergesum = 0;

extern int platform_id_inuse;
extern int device_id_inuse;

#ifdef TIMING
#include "timing.h"

extern struct timeval tv;
extern struct timeval tv_total_start, tv_total_end;
extern struct timeval tv_init_start, tv_init_end;
extern struct timeval tv_h2d_start, tv_h2d_end;
extern struct timeval tv_d2h_start, tv_d2h_end;
extern struct timeval tv_kernel_start, tv_kernel_end;
extern struct timeval tv_mem_alloc_start, tv_mem_alloc_end;
extern struct timeval tv_close_start, tv_close_end;

#include "ckatsak.h"
static float mem_alloc_time_pass;
static size_t mem_alloc_bytes_pass;
#if 0
extern float init_time, mem_alloc_time, h2d_time, kernel_time,
       d2h_time, close_time, total_time;
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
// The mergesort algorithm
////////////////////////////////////////////////////////////////////////////////
void init_mergesort(long long listsize){
	cl_uint num = 0;
	clGetPlatformIDs(0,NULL,&num);
	cl_platform_id platformID[num];
	clGetPlatformIDs(num,platformID,NULL);
	clGetDeviceIDs(platformID[platform_id_inuse],CL_DEVICE_TYPE_ALL,0,NULL,&num);
	cl_device_id devices[num];
	err = clGetDeviceIDs(platformID[platform_id_inuse],CL_DEVICE_TYPE_ALL,num,devices,NULL);

	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to create a device group! (Init_mergesort)\n");
		exit(1);
	}
	char name[128];


	clGetDeviceInfo(devices[device_id_inuse],CL_DEVICE_NAME,128,name,NULL);

	mergeContext = clCreateContext(0, 1, &devices[device_id_inuse], NULL, NULL, &err);

	mergeCommands = clCreateCommandQueue(mergeContext, devices[device_id_inuse], CL_QUEUE_PROFILING_ENABLE, &err);

	d_resultList_first_altered = (cl_float4 *)malloc(listsize*sizeof(float));

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_start, NULL);
#endif

	// mem_alloc: mergeSortFirst, mergeSortPass
	d_resultList_first_buff = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, listsize * sizeof(float), NULL, NULL);
	// mem_alloc: mergeSortFirst, mergeSortPass
	d_origList_first_buff = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, listsize * sizeof(float), NULL, NULL);

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_end, NULL);
	tvsub(&tv_mem_alloc_end, &tv_mem_alloc_start, &tv);
	mem_alloc_time_pass = tv.tv_sec * 1000.0 + (float) tv.tv_usec / 1000.0;
	mem_alloc_bytes_pass = 2 * listsize * sizeof(float);
	mem_alloc_time[kern_mergeSortFirst] += mem_alloc_time_pass;
	mem_alloc_bytes[kern_mergeSortFirst] += 2 * listsize * sizeof(float);

	gettimeofday(&tv_mem_alloc_start, NULL);
#endif

	// mem_alloc: mergepack
	d_orig = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, listsize * sizeof(float), NULL, NULL);
	// mem_alloc: mergepack
	d_res = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, listsize * sizeof(float), NULL, NULL);

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_end, NULL);
	tvsub(&tv_mem_alloc_end, &tv_mem_alloc_start, &tv);
	mem_alloc_time[kern_mergepack] += tv.tv_sec * 1000.0 + (float) tv.tv_usec / 1000.0;
	mem_alloc_bytes[kern_mergepack] += 2 * listsize * sizeof(float);
#endif

	FILE *fp;
	const char fileName[] = "./mergesort.cl";
	long long source_size;
	char *source_str;

	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load mergesort kernel.\n");
		exit(1);
	}

	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);

	fclose(fp);
	mergeProgram = clCreateProgramWithSource(mergeContext, 1, (const char **) &source_str, (const long long *)&source_size, &err);
	if (!mergeProgram)
	{
		printf("Error: Failed to create merge compute program!\n");
		exit(1);
	}

	err = clBuildProgram(mergeProgram, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		long long len;
		char buffer[2048];

		printf("Error: Failed to build merge program executable!\n");
		clGetProgramBuildInfo(mergeProgram, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		exit(1);
	}
}

void finish_mergesort() {
	clReleaseMemObject(constStartAddr);
	clReleaseMemObject(nullElems);
	clReleaseMemObject(finalStartAddr);
	clReleaseMemObject(d_orig);
	clReleaseMemObject(d_res);
	clReleaseMemObject(d_origList_first_buff);
	clReleaseMemObject(d_resultList_first_buff);
	clReleaseProgram(mergeProgram);
	clReleaseKernel(mergeFirstKernel);
	clReleaseCommandQueue(mergeCommands);
	clReleaseContext(mergeContext);
}

cl_float4 *runMergeSort(long long listsize, int divisions,
		cl_float4 *d_origList, cl_float4 *d_resultList,
		int *sizes, int *nullElements,
		unsigned int *origOffsets){

	int *startaddr = (int *)malloc((divisions + 1)*sizeof(int));
	int largestSize = -1;
	startaddr[0] = 0;
	for(int i=1; i<=divisions; i++)
	{
		startaddr[i] = startaddr[i-1] + sizes[i-1];
		if(sizes[i-1] > largestSize) largestSize = sizes[i-1];
	}
	largestSize *= 4;

	mergeFirstKernel = clCreateKernel(mergeProgram, "mergeSortFirst", &err);
	if (!mergeFirstKernel || err != CL_SUCCESS)
	{
		printf("Error: Failed to create merge sort first compute kernel!\n");
		exit(1);
	}

	cl_event write_event[4], read_event[2];
	// h2d: mergeSortFirst
	err = clEnqueueWriteBuffer(mergeCommands, d_resultList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_resultList, 0, NULL, &write_event[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to d_resultList_first_buff source array!\n");
		exit(1);
	}
	// h2d: mergeSortFirst
	err = clEnqueueWriteBuffer(mergeCommands, d_origList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_origList, 0, NULL, &write_event[1]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to d_origList_first_buff source array!\n");
		exit(1);
	}

	err = 0;
	err = clSetKernelArg(mergeFirstKernel, 0, sizeof(cl_mem), &d_origList_first_buff);
	err = clSetKernelArg(mergeFirstKernel, 1, sizeof(cl_mem), &d_resultList_first_buff);
	err = clSetKernelArg(mergeFirstKernel, 2, sizeof(cl_int), &listsize);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to set merge first kernel arguments! %d\n", err);
		exit(1);
	}
#ifdef MERGE_WG_SIZE_0
	const int THREADS = MERGE_WG_SIZE_0;
#else
	const int THREADS = 256;
#endif
	size_t local[] = {THREADS,1,1};
	int blocks = ((listsize/4)%THREADS == 0) ? (listsize/4)/THREADS : (listsize/4)/THREADS + 1;
	size_t global[] = {blocks*THREADS,1,1};
	size_t grid[] = {blocks,1,1,1};

	// exec: mergeSortFirst
	err = clEnqueueNDRangeKernel(mergeCommands, mergeFirstKernel, 3, NULL, global, local, 0, NULL, &mergeFirstEvent);
	if (err)
	{
		printf("Error: Failed to execute mergeFirst kernel!\n");
		exit(1);
	}
	clWaitForEvents(1, &mergeFirstEvent);
	clFinish(mergeCommands);

	// d2h: mergeSortFirst
	err = clEnqueueReadBuffer(mergeCommands, d_resultList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_resultList, 0, NULL, &read_event[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to read prefix output array! %d\n", err);
		exit(1);
	}

#ifdef TIMING
	h2d_time[kern_mergeSortFirst] += probe_event_time(write_event[0], mergeCommands);
	h2d_time[kern_mergeSortFirst] += probe_event_time(write_event[1], mergeCommands);
	h2d_bytes[kern_mergeSortFirst] += listsize * sizeof(float);

	d2h_time[kern_mergeSortFirst] += probe_event_time(read_event[0], mergeCommands);
	d2h_bytes[kern_mergeSortFirst] += listsize * sizeof(float);

	exec_time[kern_mergeSortFirst] += probe_event_time(mergeFirstEvent, mergeCommands);
#if 0
	h2d_time += probe_event_time(write_event[0], mergeCommands);
	h2d_time += probe_event_time(write_event[1], mergeCommands);
	d2h_time += probe_event_time(read_event[0], mergeCommands);
	kernel_time += probe_event_time(mergeFirstEvent, mergeCommands);
#endif
#endif
	clReleaseEvent(write_event[0]);
	clReleaseEvent(write_event[1]);
	clReleaseEvent(read_event[0]);

	//    for(int i = 0; i < listsize/4;i++) {
	//        printf("RESULT %f \n", d_resultList[i].s[0]);
	//        printf("RESULT %f \n", d_resultList[i].s[1]);
	//        printf("RESULT %f \n", d_resultList[i].s[2]);
	//        printf("RESULT %f \n", d_resultList[i].s[3]);
	//    }
	cl_ulong time_start, time_end;
	double total_time;
	clGetEventProfilingInfo(mergeFirstEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(mergeFirstEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;
	mergesum+= total_time / 1000000;
	printf("Merge First Kernel Time: %0.3f \n", total_time/1000000);

	//    for(int i =0; i < listsize/4; i++) {
	//        printf("TEST %f \n", d_resultList[i].s[0]);
	//        printf("TEST %f \n", d_resultList[i].s[1]);
	//        printf("TEST %f \n", d_resultList[i].s[2]);
	//        printf("TEST %f \n", d_resultList[i].s[3]);
	//    }

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_start, NULL);
#endif

	// mem_alloc: mergeSortPass, mergepack
	constStartAddr = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, (divisions + 1) * sizeof(int), NULL, NULL);

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_end, NULL);
	tvsub(&tv_mem_alloc_end, &tv_mem_alloc_start, &tv);

	mem_alloc_time_pass += tv.tv_sec * 1000.0 + (float) tv.tv_usec / 1000.0;
	mem_alloc_bytes_pass += (divisions + 1) * sizeof(int);
	mem_alloc_time[kern_mergepack] += tv.tv_sec * 1000.0 + (float) tv.tv_usec / 1000.0;
	mem_alloc_bytes[kern_mergepack] += (divisions + 1) * sizeof(int);
#endif

	// h2d: mergeSortPass
	err = clEnqueueWriteBuffer(mergeCommands, constStartAddr, CL_TRUE, 0, (divisions+1)*sizeof(int), startaddr, 0, NULL, &write_event[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to constStartAddr source array!\n");
		exit(1);
	}

#ifdef TIMING
	float h2d_time_cSA = probe_event_time(write_event[0], mergeCommands);
	//h2d_time += probe_event_time(write_event[0], mergeCommands);
#endif

	clReleaseEvent(write_event[0]);

	mergePassKernel = clCreateKernel(mergeProgram, "mergeSortPass", &err);
	if (!mergePassKernel || err != CL_SUCCESS)
	{
		printf("Error: Failed to create merge sort pass compute kernel!\n");
		exit(1);
	}

	double mergePassTime = 0;
	int nrElems = 2;
	while (1) {
		int floatsperthread = (nrElems*4);
		//printf("FPT %d \n", floatsperthread);
		int threadsPerDiv = (int)ceil(largestSize/(float)floatsperthread);
		//printf("TPD %d \n",threadsPerDiv);
		int threadsNeeded = threadsPerDiv * divisions;
		//printf("TN %d \n", threadsNeeded);

#ifdef MERGE_WG_SIZE_1
		local[0] = MERGE_WG_SIZE_1;
#else
		local[0] = 208;
#endif
		grid[0] = ((threadsNeeded%local[0]) == 0) ?
			threadsNeeded/local[0] :
			(threadsNeeded/local[0]) + 1;
		if(grid[0] < 8){
			grid[0] = 8;
			local[0] = ((threadsNeeded%grid[0]) == 0) ?
				threadsNeeded / grid[0] :
				(threadsNeeded / grid[0]) + 1;
		}
		// Swap orig/result list
		cl_float4 *tempList = d_origList;
		d_origList = d_resultList;
		d_resultList = tempList;

		// h2d: mergeSortPass(loop)
		err = clEnqueueWriteBuffer(mergeCommands, d_resultList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_resultList, 0, NULL, &write_event[0]);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to write to d_resultList_first_buff source array!\n");
			exit(1);
		}

		// h2d: mergeSortPass(loop)
		err = clEnqueueWriteBuffer(mergeCommands, d_origList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_origList, 0, NULL, &write_event[1]);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to write to d_origList_first_buff source array!\n");
			exit(1);
		}
		err = 0;
		err  = clSetKernelArg(mergePassKernel, 0, sizeof(cl_mem), &d_origList_first_buff);
		err  = clSetKernelArg(mergePassKernel, 1, sizeof(cl_mem), &d_resultList_first_buff);
		err  = clSetKernelArg(mergePassKernel, 2, sizeof(cl_int), &nrElems);
		err  = clSetKernelArg(mergePassKernel, 3, sizeof(int), &threadsPerDiv);
		err  = clSetKernelArg(mergePassKernel, 4, sizeof(cl_mem), &constStartAddr);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to set merge pass kernel arguments! %d\n", err);
			exit(1);
		}

		global[0] = grid[0]*local[0];
		// exec: mergeSortPass(loop)
		err = clEnqueueNDRangeKernel(mergeCommands, mergePassKernel, 3, NULL, global, local, 0, NULL, &mergePassEvent);
		if (err)
		{
			printf("Error: Failed to execute mergePass kernel!\n");
			exit(1);
		}

		clFinish(mergeCommands);
		// d2h: mergeSortPass(loop)
		err = clEnqueueReadBuffer(mergeCommands, d_resultList_first_buff, CL_TRUE, 0, listsize*sizeof(float), d_resultList, 0, NULL, &read_event[0]);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to read prefix output array! %d\n", err);
			exit(1);
		}
#ifdef TIMING
		float h2d_time_pass = probe_event_time(write_event[0], mergeCommands);
		h2d_time_pass += probe_event_time(write_event[1], mergeCommands);
		size_t h2d_bytes_pass = 2 * listsize * sizeof(float);
		float d2h_time_pass = probe_event_time(read_event[0], mergeCommands);
		size_t d2h_bytes_pass = listsize * sizeof(float);
		float exec_time_pass = probe_event_time(mergePassEvent, mergeCommands);

		static int mergeSortPass_iter = 0;
		fprintf(stderr, "%s_%d,%zd,%f,%zd,%f,%f,%zd,%f,%f,%d,%d\n",
			"mergeSortPass", mergeSortPass_iter++,
			mem_alloc_bytes_pass, mem_alloc_time_pass,
			h2d_bytes_pass, h2d_time_pass,
			exec_time_pass,
			d2h_bytes_pass, d2h_time_pass,
			mem_alloc_time_pass + h2d_time_pass + exec_time_pass + d2h_time_pass,
			platform_id_inuse, device_id_inuse);
#if 0
		h2d_time += probe_event_time(write_event[0], mergeCommands);
		h2d_time += probe_event_time(write_event[1], mergeCommands);
		d2h_time += probe_event_time(read_event[0], mergeCommands);
		kernel_time += probe_event_time(mergePassEvent, mergeCommands);
#endif
#endif
		clReleaseEvent(write_event[0]);
		clReleaseEvent(write_event[1]);
		clReleaseEvent(read_event[0]);

		clFinish(mergeCommands);
		clGetEventProfilingInfo(mergePassEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(mergePassEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
		total_time = time_end - time_start;
		mergesum+= total_time /1000000;
		mergePassTime+= total_time/1000000;
		printf("Merge Pass Kernel Iteration Time: %0.3f \n", total_time/1000000);
		nrElems *= 2;
		floatsperthread = (nrElems*4);

		if(threadsPerDiv == 1) break;
	}
	printf("Merge Pass Kernel Time: %0.3f \n", mergePassTime);

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_start, NULL);
#endif

	// mem_alloc: mergepack
	finalStartAddr = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, (divisions+1) * sizeof(int), NULL, NULL);
	// mem_alloc: mergepack
	nullElems = clCreateBuffer(mergeContext, CL_MEM_READ_WRITE, (divisions) * sizeof(int), NULL, NULL);

#ifdef TIMING
	gettimeofday(&tv_mem_alloc_end, NULL);
	tvsub(&tv_mem_alloc_end, &tv_mem_alloc_start, &tv);
	mem_alloc_time[kern_mergepack] += tv.tv_sec * 1000.0 + (float) tv.tv_usec / 1000.0;
	mem_alloc_bytes[kern_mergepack] += (2 * divisions + 1) * sizeof(int);
#endif

	// h2d: mergepack
	err = clEnqueueWriteBuffer(mergeCommands, finalStartAddr, CL_TRUE, 0, (divisions+1)*sizeof(int), origOffsets, 0, NULL, &write_event[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to finalStartAddr source array!\n");
		exit(1);
	}

	// h2d: mergepack
	err = clEnqueueWriteBuffer(mergeCommands, nullElems, CL_TRUE, 0, (divisions)*sizeof(int), nullElements, 0, NULL, &write_event[1]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to nullElements source array!\n");
		exit(1);
	}
#ifdef MERGE_WG_SIZE_0
	local[0] = MERGE_WG_SIZE_0;
#else
	local[0] = 256;
#endif
	grid[0] = ((largestSize%local[0]) == 0) ?
		largestSize/local[0] :
		(largestSize/local[0]) + 1;
	grid[1] = divisions;
	//grid[0] = 17;
	global[0] = grid[0]*local[0];
	global[1] = grid[1]*local[1];

	mergePackKernel = clCreateKernel(mergeProgram, "mergepack", &err);
	if (!mergePackKernel || err != CL_SUCCESS)
	{
		printf("Error: Failed to create merge sort pack compute kernel!\n");
		exit(1);
	}
	// h2d: mergepack
	err = clEnqueueWriteBuffer(mergeCommands, d_res, CL_TRUE, 0, listsize*sizeof(float), d_resultList, 0, NULL, &write_event[2]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to d_resultList_first_buff source array!\n");
		exit(1);
	}

	// h2d: mergepack
	err = clEnqueueWriteBuffer(mergeCommands, d_orig, CL_TRUE, 0, listsize*sizeof(float), d_origList, 0, NULL, &write_event[3]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to write to d_origList_first_buff source array!\n");
		exit(1);
	}
	err = 0;
	err  = clSetKernelArg(mergePackKernel, 0, sizeof(cl_mem), &d_res);
	err  = clSetKernelArg(mergePackKernel, 1, sizeof(cl_mem), &d_orig);
	err  = clSetKernelArg(mergePackKernel, 2, sizeof(cl_mem), &constStartAddr);
	err  = clSetKernelArg(mergePackKernel, 3, sizeof(cl_mem), &nullElems);
	err  = clSetKernelArg(mergePackKernel, 4, sizeof(cl_mem), &finalStartAddr);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to set merge pack kernel arguments! %d\n", err);
		exit(1);
	}
	// exec: mergepack
	err = clEnqueueNDRangeKernel(mergeCommands, mergePackKernel, 3, NULL, global, local, 0, NULL, &mergePackEvent);
	if (err)
	{
		printf("Error: Failed to execute merge pack kernel!\n");
		exit(1);
	}

#ifdef TIMING
	for (int event_id = 0; event_id < 4; event_id++)
		h2d_time[kern_mergepack] += probe_event_time(write_event[event_id], mergeCommands);
	h2d_bytes[kern_mergepack] += 2 * listsize * sizeof(float) + (2 * divisions + 1) * sizeof(int);
	exec_time[kern_mergepack] += probe_event_time(mergePackEvent, mergeCommands);
#endif

	clReleaseEvent(write_event[0]);
	clReleaseEvent(write_event[1]);
	clReleaseEvent(write_event[2]);
	clReleaseEvent(write_event[3]);

	clFinish(mergeCommands);
	clGetEventProfilingInfo(mergePackEvent, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(mergePackEvent, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	total_time = time_end - time_start;
	mergesum+= total_time / 1000000;
	printf("Merge Pack Kernel Time: %0.3f \n", total_time / 1000000);

	// d2h: mergepack
	err = clEnqueueReadBuffer(mergeCommands, d_orig, CL_TRUE, 0, listsize*sizeof(float), d_origList, 0, NULL, &read_event[0]);
	if (err != CL_SUCCESS)
	{
		printf("Error: Failed to read origList array! %d\n", err);
		exit(1);
	}

#ifdef TIMING
	d2h_time[kern_mergepack] += probe_event_time(read_event[0], mergeCommands);
	d2h_bytes[kern_mergepack] += listsize * sizeof(float);
#endif
	clReleaseEvent(read_event[0]);

	free(startaddr);
	return d_origList;

}

double getMergeTime() {
	return mergesum;
}
