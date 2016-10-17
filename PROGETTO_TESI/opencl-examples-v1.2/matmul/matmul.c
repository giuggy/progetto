#include <stdio.h>
#include <stdlib.h>
#include "clut.h"

#define N                   1000
#define LOCAL_SIZE          8 // work items per work group

int main(void) {
    
    clut_device dev;	// context, device queue & program
    int       err;      // error code
    cl_kernel kernel;   // execution kernel
    cl_event  event;

    // create the two input vectors + output vector
    int i, j, k, n = N;
    int *A = (int*)malloc(sizeof(int)*n*n);
    int *B = (int*)malloc(sizeof(int)*n*n);
    int *C = (int*)malloc(sizeof(int)*n*n);

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            A[i*n+j] = i;
            B[i*n+j] = n - i;
        }
    }

	clut_open_device(&dev, "matmul.cl");
    
    clut_print_device_info(&dev);

    // create memory buffers on the device for each vector 
    cl_mem a_mem_obj = clCreateBuffer(dev.context, 
    						CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
    						n*n * sizeof(int), A, NULL);
    if (!a_mem_obj) clut_panic("failed to allocate input vec on device memory");

    cl_mem b_mem_obj = clCreateBuffer(dev.context, 
    						CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
    						n*n * sizeof(int), B, NULL);
    if (!b_mem_obj) clut_panic("failed to allocate input vec on device memory");

    cl_mem c_mem_obj = clCreateBuffer(dev.context, CL_MEM_WRITE_ONLY, 
    						n*n * sizeof(int), NULL, NULL);
    if (!c_mem_obj) clut_panic("failed to allocate output vec on device memory");

    // create an OpenCL kernel
    kernel = clCreateKernel(dev.program, "matrix_mul", &err);
    clut_check_err(err, "clCreateKernel failed");

    // set the arguments of the kernel
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
    err |= clSetKernelArg(kernel, 3, sizeof(int), (void *)&n);
    clut_check_err(err, "clSetKernelArg failed");

    // execute the OpenCL kernel on the list
    size_t local_item_size[2]  = { LOCAL_SIZE, LOCAL_SIZE };
    size_t global_item_size[2] = 
        { ((n+LOCAL_SIZE-1)/LOCAL_SIZE)*LOCAL_SIZE,
          ((n+LOCAL_SIZE-1)/LOCAL_SIZE)*LOCAL_SIZE } ;

    printf("data size: %d\n", n);
    printf("global size: %lu\n", global_item_size[0]);
    
    err = clEnqueueNDRangeKernel(dev.queue, kernel, 2, NULL, 
                                 global_item_size, local_item_size,
                                 0, NULL, &event);
    clut_check_err(err, "clEnqueueNDRangeKernel failed");

    // read the memory buffer C on the device to the local variable C
    err = clEnqueueReadBuffer(dev.queue, c_mem_obj, CL_TRUE, 0, 
                              n*n * sizeof(int), C, 0, NULL, NULL);
    clut_check_err(err, "clEnqueueReadBuffer failed");

    printf("Tempo esecuzione su Device: %f sec\n", 
           clut_get_duration(event));

    // check correctness
    double start = clut_get_real_time();
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            int sum = 0;
            for (k = 0; k < n; k++) sum += A[i*n+k]*B[k*n+j];
            if (sum != C[i*n+j]) break;
        }
    }
    double stop = clut_get_real_time();
    printf("Tempo esecuzione su Host: %f sec\n", stop-start);

	if (i < n || j < n) printf("correctness test: FAILED\n");
	else                printf("correctness test: PASSED\n");

    clut_close_device(&dev);

    free(A);
    free(B);
    free(C);
    return 0;
}
