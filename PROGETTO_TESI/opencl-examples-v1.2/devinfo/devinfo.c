#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_PLATFORMS 32
#define MAX_DEVICES   32
#define MAX_STR_LEN   1024

int main() {
    
    unsigned int i, j;	// iterator variables for loops
    
    cl_int err;
    cl_platform_id platforms[MAX_PLATFORMS]; // IDs of all the platforms    
    cl_uint num_platforms;                   // number of platforms on this machine
    char vendor[MAX_STR_LEN];                // platforms vendor string
    char opencl_version[MAX_STR_LEN];        // opencl version string
    cl_device_id devices[MAX_DEVICES];       // number of devices for each platform 
    cl_uint num_devices;                     // number of devices on this machine
    char deviceName[MAX_STR_LEN];            // devices name    
    cl_uint numberOfCores;                   // number of cores of on a device
    cl_long amountOfMemory;                  // amount of memory on a device
    cl_uint clockFreq;                       // clock frequency of a device
    cl_ulong maxAllocatableMem;              // maximum allocatable memory
    cl_ulong localMem;                       // local memory for a device
    cl_bool available;                       // tells if the device is available
    size_t device_wg_size;                   // max number of work items in a work group

    // get the number of platforms
    err = clGetPlatformIDs (32, platforms, &num_platforms);
    
    int fs1 = 32, fs2=fs1-4, fs3=fs2-4;
    printf("\n%-*s %u\n\n", fs1, "Number of platforms:", num_platforms);    

    // iterate over platforms
    for(i = 0; i < num_platforms; i++) {

        printf("Platform: %u\n\n", i);
        err = clGetPlatformInfo (platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
        if (err != CL_SUCCESS) exit((printf("Error in clGetPlatformInfo"),1));

        printf("    %-*s %s\n", fs2, "Platform Vendor:", vendor);

        err = clGetDeviceIDs (platforms[i], CL_DEVICE_TYPE_ALL, sizeof(devices), devices, &num_devices);
        if (err != CL_SUCCESS) exit((printf("Error in clGetDeviceIDs"),1));
        
        printf("    %-*s %u\n\n", fs2, "Number of devices:", num_devices);
        
        // iterate over devices
        for (j = 0; j < num_devices; j++) {

            // scan in device information
            err = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_NAME"),1));
            
            err = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_VENDOR"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, sizeof(opencl_version), opencl_version, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_OPENCL_C_VERSION"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(numberOfCores), &numberOfCores, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_MAX_COMPUTE_UNITS"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(amountOfMemory), &amountOfMemory, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_GLOBAL_MEM_SIZE"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFreq), &clockFreq, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_MAX_CLOCK_FREQUENCY"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAllocatableMem), &maxAllocatableMem, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_MAX_MEM_ALLOC_SIZE"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem), &localMem, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_LOCAL_MEM_SIZE"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(available), &available, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_AVAILABLE"),1));

            err = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(device_wg_size), &device_wg_size, NULL);
            if (err != CL_SUCCESS) exit((printf("Error in retrieving CL_DEVICE_MAX_WORK_GROUP_SIZE"),1));

            
            
            // print out device info
            printf("    %-*s %u\n", fs2, "Device:", j);
            printf("        %-*s %s\n", fs3, "Name:", deviceName);
            printf("        %-*s %s\n", fs3, "Vendor:", vendor);
            printf("        %-*s %s\n", fs3, "OpenCL version:", opencl_version);
            printf("        %-*s %s\n", fs3, "Available:", available ? "Yes" : "No");
            printf("        %-*s %u\n", fs3, "Compute Units:", numberOfCores);
            printf("        %-*s %u MHz\n", fs3, "Clock Frequency:", clockFreq);
            printf("        %-*s %0.00f MB\n", fs3, "Global Memory:", (double)amountOfMemory/1048576);            
            printf("        %-*s %0.00f MB\n", fs3, "Max Allocatable Memory:", (double)maxAllocatableMem/1048576);
            printf("        %-*s %u KB\n", fs3, "Local Memory:", (unsigned int)localMem);
            printf("        %-*s %lu\n\n", fs3, "Max work group size:", device_wg_size);
        }
    }
    
    return 0;
}
