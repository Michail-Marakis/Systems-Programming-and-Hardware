#include <iostream>
#include <cmath>
#include <chrono>
#include <cuda.h>

#define N 100000000

__device__ double f(double x) {
    return x * x;
}

// =======================================================
//  ACTIVE CASE: REGISTER / LOCAL VARIABLES
// =======================================================
__global__ void kernel_registers(double a, double h, double *out) {

    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < N) {
        double x1 = a + tid * h;
        double x2 = a + (tid + 1) * h;

        double val = (f(x1) + f(x2)) * h / 2.0;
        out[tid] = val;
    }
}

/*
// =======================================================
//  CASE 2: GLOBAL MEMORY ONLY (same but explicit)
// =======================================================
__global__ void kernel_global(double a, double h, double *out) {

    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < N) {
        double x1 = a + tid * h;
        double x2 = a + (tid + 1) * h;

        out[tid] = (f(x1) + f(x2)) * h / 2.0;
    }
}
*/

/*
// =======================================================
//  CASE 3: SHARED MEMORY (block-level buffer)
// =======================================================
__global__ void kernel_shared(double a, double h, double *out) {

    __shared__ double cache[256];

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int local = threadIdx.x;

    double val = 0.0;

    if (tid < N) {
        double x1 = a + tid * h;
        double x2 = a + (tid + 1) * h;

        val = (f(x1) + f(x2)) * h / 2.0;
    }

    cache[local] = val;

    __syncthreads();

    if (tid < N) {
        out[tid] = cache[local];
    }
}
*/

// =======================================================
// MAIN
// =======================================================
int main() {

    double a = 0.0, b = 10.0;
    double h = (b - a) / N;

    double *h_out = new double[N];
    double *d_out;

    cudaMalloc(&d_out, N * sizeof(double));

    int threadsPerBlock = 256;
    int blocks = (N + threadsPerBlock - 1) / threadsPerBlock;

    auto start = std::chrono::high_resolution_clock::now();

    // ACTIVE KERNEL CALL
    kernel_registers<<<blocks, threadsPerBlock>>>(a, h, d_out);

    cudaDeviceSynchronize();

    cudaMemcpy(h_out, d_out, N * sizeof(double), cudaMemcpyDeviceToHost);

    double sum = 0.0;
    for (int i = 0; i < N; i++) {
        sum += h_out[i];
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Execution Time = " << elapsed.count() << " sec" << std::endl;

    cudaFree(d_out);
    delete[] h_out;

    return 0;
}