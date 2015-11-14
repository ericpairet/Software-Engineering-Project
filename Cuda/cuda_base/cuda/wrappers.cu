
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

#include "helper_cuda.h"

typedef unsigned int uint;

extern "C"
{
    void cudaInit()
    {
        int devID;

        // use device with highest Gflops/s
        devID = findCudaDevice();

        if (devID < 0)
        {
            printf("No CUDA Capable devices found, exiting...\n");
            exit(EXIT_SUCCESS);
        }
    }
    void allocateArray(void **devPtr, size_t size)
    {
        checkCudaErrors(cudaMalloc(devPtr, size));
    }

    void freeArray(void *devPtr)
    {
        checkCudaErrors(cudaFree(devPtr));
    }

    void copyArrayToDevice(void *device, const void *host, int offset, int size)
    {
        checkCudaErrors(cudaMemcpy((char *) device + offset, host, size, cudaMemcpyHostToDevice));
    }

    void copyArrayFromDevice(void *host, const void *device, int size)
    {
        checkCudaErrors(cudaMemcpy(host, device, size, cudaMemcpyDeviceToHost));
    }


    uint sumNumbers(uint *dNumbers, uint n)
    {
        // simple reduction from 1 to n
        thrust::device_ptr<uint> dp_numbers(dNumbers);
        return thrust::reduce(dp_numbers, dp_numbers + n);
    }

    //Round a / b to nearest higher integer value
    uint iDivUp(uint a, uint b)
    {
        return (a % b != 0) ? (a / b + 1) : (a / b);
    }

    // compute grid and thread block size for a given number of elements
    void computeGridSize(uint n, uint blockSize, uint &numBlocks, uint &numThreads)
    {
        numThreads = min(blockSize, n);
        numBlocks = iDivUp(n, numThreads);
    }
}
