__kernel void convolution(__global unsigned char* in, 
                          __global unsigned char* out, 
                          int rows, int cols,
                          __global float* filter, 
                          float factor, float bias, int size) {

    int u, v;
    int x = get_global_id(0);                                           
    int y = get_global_id(1);

    if (x >= cols || y >= rows) return;
    
    float val = 0.0;
    
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

