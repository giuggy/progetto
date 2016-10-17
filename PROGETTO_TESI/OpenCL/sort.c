#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"
#include "device.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define LIST_SIZE 33

int chiamaKernel(char** hashes, char* s, size_t numLines){ 
	int i;
	int j;
	int flag = 1;

	int *retu= (int*)malloc(sizeof(int)*LIST_SIZE);
	char *C = (char*)malloc(sizeof(char)*LIST_SIZE*numLines);
	
		for (i = 0; i < numLines ; i++) {
			for (j = 0; j < LIST_SIZE; j++) {
				C[i*LIST_SIZE+j] = hashes[i][j];
				//printf("hashes %c \n ", hashes[i][j]);
				//printf("C %c \n ", C[i*LIST_SIZE+j]);
				//printf("S %c \n ", s[j]);
								
			}
		}
		
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
		cl_mem ret_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_READ_ONLY, 
				LIST_SIZE * sizeof(int), NULL, NULL); 
		cl_mem hash_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
				LIST_SIZE* numLines * sizeof(char), C, NULL);
		cl_mem s_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
				LIST_SIZE * sizeof(int), s, NULL);
		
		
		
		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue, hash_mem_obj, CL_TRUE, 0,
				LIST_SIZE* numLines * sizeof(char), C, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, s_mem_obj, CL_TRUE, 0, 
				LIST_SIZE * sizeof(char), s, 0, NULL, NULL);
 
		// Create a program from the kernel source
		cl_program program = clCreateProgramWithSource(context, 1, 
				(const char **)&source_str, (const size_t *)&source_size, &ret);
 
		// Build the program
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
 
		// Create the OpenCL kernel
		cl_kernel kernel = clCreateKernel(program, "search", &ret);
 
		// Set the arguments of the kernel
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&hash_mem_obj);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&ret_mem_obj);
		ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&s_mem_obj);
		ret = clSetKernelArg(kernel, 3, sizeof(size_t), (void *)&numLines);
		
 
		// Execute the OpenCL kernel on the list
		size_t global_item_size = numLines; // Process the entire lists
		size_t local_item_size = 64; // Divide work items into groups of 64
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
				&global_item_size, &local_item_size, 0, NULL, NULL);
	 
		// Read the memory buffer C on the device to the local variable C
		
		ret = clEnqueueReadBuffer(command_queue, ret_mem_obj, CL_TRUE, 0, 
				LIST_SIZE * sizeof(int), retu, 0, NULL, NULL);
		
		
		// Display the result to the screen
		int flagP =0;
		for(j = 0; j < LIST_SIZE; j++){
			//printf("retu %d\n", retu[j]);
			if(retu[j] != 0)
			{
				 flagP = 1;
			 } 
		}
		
		if(flagP == 1) flag=1;
		else flag = 0;
		
		
		//liberazione risorse
		
		ret = clFlush(command_queue);
		ret = clFinish(command_queue);
		ret = clReleaseKernel(kernel);
		ret = clReleaseProgram(program);
		ret = clReleaseMemObject(hash_mem_obj);
		ret = clReleaseMemObject(s_mem_obj);
		ret = clReleaseMemObject(ret_mem_obj);
		ret = clReleaseCommandQueue(command_queue);
		ret = clReleaseContext(context);
		
		free (retu);
		return flag;
}


int textCompare(const void *a, const void *b) {
  char *m1 = *(char **)a;
  char *m2 = *(char **)b;
  return strcmp(m1, m2);
}

void sortArrayOfString(char** strings, size_t numLines) {
  qsort(strings, numLines, sizeof(char*), textCompare);
}  

//rimuove la password dal buffer hashes
size_t rimozione(char* ret,char** strings, size_t numLines)
{
	
		char* tmp = strdup(ret); // analogo alla textCompare()

		memcpy(ret, strings[numLines-1], 32); 
		memcpy(strings[numLines-1], tmp, 32); 
		free(tmp);  
		   
		sortArrayOfString(strings, numLines-1); // riordino l'array per la ricerca binaria
		numLines--;

		return numLines; // rimozione lazy
}  

//cerca le password

size_t find(char** strings, char* s, size_t numLines, size_t stringLength){
		
		int flag ;

		
		flag = chiamaKernel(strings, s, numLines);
				
		if (flag != 1){		
			numLines= rimozione(s, strings, numLines);

		}
		
		return numLines;
}   




