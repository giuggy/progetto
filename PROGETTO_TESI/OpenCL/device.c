#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "device.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

 
#define MAX_SOURCE_SIZE (0x100000)

/*
const uint32_t k[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };


const uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
                      
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
 
inline void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}                  
 
inline uint32_t to_int32(uint8_t *bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}


void chiamaMD5(uint8_t *initial_msg, size_t initial_len, uint8_t *digest){
	const int LIST_SIZE = 1024;
	
	int i;
	int flag = 1;
	for (i= 0; i < numLines; i ++) {		
		char * hash = hashes[i];
	  // Load the kernel source code into the array source_str
		FILE *fp;
		char *source_str;
		size_t source_size;
 
		fp = fopen("kernel.cl", "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		source_str = (char*)malloc(MAX_SOURCE_SIZE);
		source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
		fclose( fp );
 
		// Get platform and device information
		cl_platform_id platform_id = NULL;
		cl_device_id device_id = NULL;   
		cl_uint ret_num_devices;
		cl_uint ret_num_platforms;
		cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		if (ret != CL_SUCCESS) exit(fprintf(stderr, "error in clGetPlatformIDs\n"));

		ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, 
				&device_id, &ret_num_devices);
		if (ret != CL_SUCCESS) exit(fprintf(stderr, "error in clGetDeviceIDs\n"));
 
		// Create an OpenCL context
		cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
 
		// Create a command queue
		cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
 
		// Create memory buffers on the device for each vector 
		cl_mem h_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
				LIST_SIZE * sizeof(char), NULL, &ret);
		cl_mem s_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
				LIST_SIZE * sizeof(char), NULL, &ret);
		cl_mem r_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
				LIST_SIZE * sizeof(int), NULL, &ret);
 
		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
				LIST_SIZE * sizeof(int), hash, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, 
				LIST_SIZE * sizeof(int), s, 0, NULL, NULL);
 
		// Create a program from the kernel source
		cl_program program = clCreateProgramWithSource(context, 1, 
				(const char **)&source_str, (const size_t *)&source_size, &ret);
 
		// Build the program
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
 
		// Create the OpenCL kernel
		cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
 
		// Set the arguments of the kernel
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&h_mem_obj);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&s_mem_obj);
		ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&m_mem_obj);
		
 
		// Execute the OpenCL kernel on the list
		size_t global_item_size = LIST_SIZE; // Process the entire lists
		size_t local_item_size = 64; // Divide work items into groups of 64
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
				&global_item_size, &local_item_size, 0, NULL, NULL);
	 
		// Read the memory buffer C on the device to the local variable C
		int *risultato = (int*)malloc(sizeof(int)*LIST_SIZE);
		ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, 
				LIST_SIZE * sizeof(int), risultato, 0, NULL, NULL);
	 
		// Display the result to the screen
		int flagP =0;
		int j;
		for(j = 0; j < LIST_SIZE; j++)
			if(risualto[j] == 1) flagP = 1;
		
		if(flagP == 0) flag=0;
		//liberazione risorse
		
		ret = clFlush(command_queue);
		ret = clFinish(command_queue);
		ret = clReleaseKernel(kernel);
		ret = clReleaseProgram(program);
		ret = clReleaseMemObject(h_mem_obj);
		ret = clReleaseMemObject(s_mem_obj);
		ret = clReleaseMemObject(r_mem_obj);
		ret = clReleaseCommandQueue(command_queue);
		ret = clReleaseContext(context);
		
		free (risultato);
	}
		
		return flag;
}*/

void chiamaEncode(uint8_t* buffer, uint8_t* alphabet, size_t charsetLength, size_t stringLength, uint64_t counter){
	
		uint8_t* B = malloc(stringLength*sizeof(uint8_t)+1);

		
		// Load the kernel source code into the array source_str
		FILE *fp;
		char *source_str;
		size_t source_size;
 
		fp = fopen("encode.cl", "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		source_str = (char*)malloc(MAX_SOURCE_SIZE);
		source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
		fclose( fp );
 
		// Get platform and device information
		cl_platform_id platform_id = NULL;
		cl_device_id device_id = NULL;   
		cl_uint ret_num_devices;
		cl_uint ret_num_platforms;
		cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		if (ret != CL_SUCCESS) exit(fprintf(stderr, "error in clGetPlatformIDs\n"));

		ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 1, 
				&device_id, &ret_num_devices);
		if (ret != CL_SUCCESS) exit(fprintf(stderr, "error in clGetDeviceIDs\n"));
 
		// Create an OpenCL context
		cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
 
		// Create a command queue
		cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
 
		// Create memory buffers on the device for each vector 
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
				 stringLength*sizeof(uint8_t)+1, NULL, NULL);
		cl_mem alfa_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
				36 * sizeof(uint8_t), alphabet, NULL);
		
			
 
		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue, alfa_mem_obj, CL_TRUE, 0,
				36 * sizeof(uint8_t), alphabet, 0, NULL, NULL);
		
		
 
		// Create a program from the kernel source
		cl_program program = clCreateProgramWithSource(context, 1, 
				(const char **)&source_str, (const size_t *)&source_size, &ret);
 
		// Build the program
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
 
		
		
		// Create the OpenCL kernel
		cl_kernel kernel = clCreateKernel(program, "encode", &ret);
			
		
		// Set the arguments of the kernel
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&b_mem_obj);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&alfa_mem_obj);
		ret = clSetKernelArg(kernel, 2, sizeof(size_t), (void *)&charsetLength);
		ret = clSetKernelArg(kernel, 3, sizeof(size_t), (void *)&stringLength);
		ret = clSetKernelArg(kernel, 4, sizeof(uint64_t), (void *)&counter);				
		
 
		// Execute the OpenCL kernel on the list
		size_t global_item_size = stringLength*sizeof(uint8_t)+1; // Process the entire lists
		size_t local_item_size = 64; // Divide work items into groups of 64
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
				&global_item_size, &local_item_size, 0, NULL, NULL);
	 
		
		
		// Read the memory buffer C on the device to the local variable C
		ret = clEnqueueReadBuffer(command_queue, b_mem_obj, CL_TRUE, 0, 
				stringLength*sizeof(uint8_t)+1, B, 0, NULL, NULL);
	 
		int l;
		for (l = 0; l < stringLength; l++){
			buffer[l]= B[l];
			
		} 
		
		//liberazione risorse
		
		ret = clFlush(command_queue);
		ret = clFinish(command_queue);
		ret = clReleaseKernel(kernel);
		ret = clReleaseProgram(program);
		ret = clReleaseMemObject(b_mem_obj);
		ret = clReleaseMemObject(alfa_mem_obj);
		ret = clReleaseCommandQueue(command_queue);
		ret = clReleaseContext(context);
		free(B);
		
		return;
}
