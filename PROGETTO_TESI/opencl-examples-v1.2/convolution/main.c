// =====================================================================
//  main.c
// =====================================================================

//  Author:         (c) 2013 Camil Demetrescu
//  License:        See the end of this file for license information
//  Created:        May 27, 2013
 
//  Last changed:   $Date: 2013/05/27 15:00:07 $
//  Changed by:     $Author: demetres $
//  Revision:       $Revision: 1.00 $


#include <stdio.h>
#include <stdlib.h>

#include "clut.h"
#include "pgm.h"
#include "convolution.h"

#define IMAGE_DIR       "images/"
#define RESULT_DIR      "results/"
#define IMAGE_FILE      "Colosseo"
#define IMAGE_EXT       ".pgm"
#define PROG_NAME       "convolution.cl"
#define MAX_LEN         1024
#define MAX_FILTER_SIZE (9*9)

// convolution filter type
typedef struct {
    char* name;
    int   size;
    float factor;
    float bias;
    float filter[MAX_FILTER_SIZE];
} filter_t;

// convolution filters
filter_t f[] = {

    {
        "blur",
        5,
        1.0/13.0,
        0.0,
        {
            0, 0, 1, 0, 0,
            0, 1, 1, 1, 0,
            1, 1, 1, 1, 1,
            0, 1, 1, 1, 0,
            0, 0, 1, 0, 0,
        }
    },

    {
        "motion-blur",
        9,
        1.0 / 9.0,
        0.0,
        {
            1, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1,
        }
    },
    
    {
        "sharpen",
        3,
        1.0,
        0.0,
        {
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1,
        }
    },
    
    {
        "sharpen-more",
        3,
        1.0,
        0.0,
        {
            1,  1,  1,
            1, -7,  1,
            1,  1,  1,
        }
    },
    
    {
        "edges",
        3,
        1.0,
        0.0,
        {
            -1, -1, -1,
            -1,  8, -1,
            -1, -1, -1
        }
    },

    {
        "emboss",
        3,
        1.0,
        128.0,
        {
            -1, -1,  0,
            -1,  0,  1,
             0,  1,  1
        }
    },
};


// ---------------------------------------------------------------------
// equal
// ---------------------------------------------------------------------
// compare two matrices

int equal(unsigned char* a, unsigned char* b, int rows, int cols) {
    int i, j;
    for (i=0; i<rows; i++)
        for (j=0; j<cols; j++) 
            if (a[i*rows+j] != b[i*rows+j]) return 0;
    return 1;
}


// ---------------------------------------------------------------------
// convolution_host
// ---------------------------------------------------------------------
// sequential CPU version executed on host

void convolution_host(unsigned char* in, unsigned char* out, 
                      int rows, int cols, 
                      float* filter, float factor, float bias, int size, 
                      double* t) {

    int x, y, u, v;

    // get initial time
    double start = clut_get_real_time();

    // scan pixels of input image
    for (y=0; y<rows; y++)
        for (x=0; x<cols; x++) {

            float val = 0;

            for (u=0; u<size; u++)
                for (v=0; v<size; v++) {
                    int iy = y - size/2 + u; 
                    int ix = x - size/2 + v;
                    if (iy<0 || iy>=rows || ix<0 || ix>=cols) continue;
                    val += in[iy*cols+ix]*filter[u*size+v]; 
                }

            int res = (int)(factor*val+bias);
                 if (res < 0)   res = 0;
            else if (res > 255) res = 255;

            out[y*cols+x] = res;
        }

    // get elapsed time
    *t = clut_get_real_time() - start;
}


// ---------------------------------------------------------------------
// run_test
// ---------------------------------------------------------------------
// perform convolution transformation test on input matrix

void run_test(unsigned char* in, unsigned char* out, int rows, int cols, 
              float* filter, float factor, float bias, int size,
              clut_device* dev, char* msg) {

    double t, td;

    // allocate correct output matrix on host
	unsigned char* out_ok = malloc(rows*cols*sizeof(unsigned char));
	if (out_ok == NULL) 
		clut_panic("failed to allocate output matrices on host");

    // compute convolution on host
    convolution_host(in, out_ok, rows, cols, 
                     filter, factor, bias, size, &t);

    // compute convolution on device
    convolution(in, out, rows, cols, 
                filter, factor, bias, size, dev, &td);

    // validate our results
    printf("%s\n", msg);
    printf("    CPU took %.3f msec\n", t*1E03);
    printf("    Device took %.3f msec\n", td*1E03);
    printf("    Correctness test: %s\n", equal(out, out_ok, rows, cols) ? 
										 "PASSED" : "FAILED");    

    // cleanup
	free(out_ok);
}


// ---------------------------------------------------------------------
// main
// ---------------------------------------------------------------------
int main(int argc, char** argv) {

    clut_device     dev;                // device structure
    unsigned char*  in;                 // input matrix on host
    unsigned char*  out;                // output matrix on host
    char*           img_file;			// image file name
    int             rows, cols;         // number of rows and columns
    int             err, i;             // misc vars
    char            in_path[MAX_LEN];   // pathname of input image file
    char            out_path[MAX_LEN];  // pathname of output image file

    // set image file name
    if (argc > 1) img_file = argv[1];
    else          img_file = IMAGE_FILE;

    // open device
    clut_open_device(&dev, PROG_NAME);

    // build pathname of input image file
    sprintf(in_path, "%s%s%s", IMAGE_DIR, img_file, IMAGE_EXT);

    // load image file in pgm format
    printf("Open image file: %s\n", in_path);
    err = pgm_load(&in, &rows, &cols, in_path);
    if (err) clut_panic("failed to load input image file");
    else     printf("Loaded %dx%d image\n", cols, rows);

    // print device info
    clut_print_device_info(&dev);

    // allocate output matrix on host
    out = malloc(rows*cols*sizeof(unsigned char));
    if (out == NULL) clut_panic("failed to allocate output matrix on host");

    // perform a test for each filter
    for (i=0; i<sizeof(f)/sizeof(filter_t); i++) {

        char msg[64];
        sprintf(msg, "Test %d: %s (%d x %d filter)", 
                i, f[i].name, f[i].size, f[i].size );
        
        // run test
        run_test(in, out, rows, cols, 
                 f[i].filter, f[i].factor, f[i].bias, f[i].size, 
                 &dev, msg);

        // build pathname of output image file
        sprintf(out_path, "%s%s.%s%s", 
                RESULT_DIR, img_file, f[i].name, IMAGE_EXT);

        // save output image
        printf("    Saving image: %s\n", out_path);
        err = pgm_save(out, rows, cols, out_path);
        if (err) clut_panic("failed to save output image file");
    }
    
    // cleanup
    free(in);
    free(out);
    clut_close_device(&dev);
    
    return 0;
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
