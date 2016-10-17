__kernel void matrix_mul(__global const int *A, 
                         __global const int *B, 
                         __global int *C,
                         int n) {
    
    // Get the index of the current element to be processed
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i>=n || j>=n) return;
    
    // Do the operation
    int k, sum = 0;
    for (k=0; k<n; k++) sum += A[i*n+k] * B[k*n+j];
    C[i*n+j] = sum;
}
