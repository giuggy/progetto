// =====================================================================
//  convolution.c
// =====================================================================

//  Author:         (c) 2013 Camil Demetrescu
//  License:        See the end of this file for license information
//  Created:        May 27, 2013

//  Last changed:   $Date: 2013/05/27 15:00:07 $
//  Changed by:     $Author: demetres $
//  Revision:       $Revision: 1.00 $


#include "convolution.h"

#define LOCAL_SIZE  8
#define KERNEL_NAME "convolution"


// ---------------------------------------------------------------------
// convolution
// ---------------------------------------------------------------------
// data-parallel GPU version

void convolution(unsigned char* I, unsigned char* O, int h, int w, 
                 float* F, float factor, float bias, int s, 
                 clut_device* dev, double* td) {

    int       err;      // error code
    cl_kernel kernel;   // execution kernel
    cl_mem    din;      // input matrix on device
    cl_mem    dout;     // output matrix on device
    cl_mem    dfilter;  // filter matrix on device
    cl_event  evt;      // performance measurement event

    // create the compute kernel
    kernel = clCreateKernel(dev->program, KERNEL_NAME, &err);
    clut_check_err(err, "failed to create kernel");

    // allocate input matrix on device as a copy of input matrix on host
    din = clCreateBuffer(dev->context, 
                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                         h*w*sizeof(unsigned char), I, NULL);
    if (!din) clut_panic("failed to allocate input matrix on device memory");

    // allocate filter matrix on device as a copy of filter matrix on host
    dfilter = clCreateBuffer(dev->context, 
                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                             s*s*sizeof(cl_float), F, NULL);
    if (!dfilter) clut_panic("failed to allocate filter matrix on device memory");

    // allocate output matrix on device
    dout = clCreateBuffer(dev->context, 
                          CL_MEM_WRITE_ONLY, 
                          h*w*sizeof(unsigned char), NULL, NULL);
    if (!dout) clut_panic("failed to allocate output matrix on device memory");

    // set the arguments to our compute kernel
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &din);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &dout);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &h);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &w);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &dfilter);
    err |= clSetKernelArg(kernel, 5, sizeof(cl_float), &factor);
    err |= clSetKernelArg(kernel, 6, sizeof(cl_float), &bias);
    err |= clSetKernelArg(kernel, 7, sizeof(int), &s);
    clut_check_err(err, "failed to set kernel arguments");

    // execute the kernel over the range of our 2D input data set
    size_t local_dim[]  = { LOCAL_SIZE, LOCAL_SIZE };
    size_t global_dim[] = { w, h };
    global_dim[0] = ((global_dim[0]+LOCAL_SIZE-1)/LOCAL_SIZE)*LOCAL_SIZE; // round up
    global_dim[1] = ((global_dim[1]+LOCAL_SIZE-1)/LOCAL_SIZE)*LOCAL_SIZE; // round up

    err = clEnqueueNDRangeKernel(dev->queue, kernel, 2, 
                                 NULL, global_dim, local_dim, 0, NULL, &evt);
    clut_check_err(err, "failed to execute kernel");

    // copy result from device to host
    err = clEnqueueReadBuffer(dev->queue, dout, CL_TRUE, 0, 
                              h*w*sizeof(char), O, 0, NULL, NULL);
    clut_check_err(err, "failed to read output result");

    // return kernel execution time
    *td = clut_get_duration(evt);

    // cleanup
    clReleaseMemObject(din);
    clReleaseMemObject(dout);
    clReleaseMemObject(dfilter);
    clReleaseKernel(kernel);
}


// Copyright (C) 2013 Camil Demetrescu

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
// USA
