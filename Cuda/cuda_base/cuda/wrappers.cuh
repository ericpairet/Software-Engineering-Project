#ifndef WRAPPERS_CUH
#define WRAPPERS_CUH

typedef unsigned int uint;

extern "C"
{
    void cudaInit();

    void allocateArray(void **devPtr, int size);
    void freeArray(void *devPtr);

    void copyArrayToDevice(void *device, const void *host, int offset, int size);
    void copyArrayFromDevice(void *host, const void *device, int size);

    uint sumNumbers(uint *dNumbers, uint n);

    // not used here but useful when calling kernel functions
    void computeGridSize(uint n, uint blockSize, uint &numBlocks, uint &numThreads);
}

#endif // WRAPPERS_CUH
