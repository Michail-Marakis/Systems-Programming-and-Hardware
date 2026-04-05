#include <iostream>
#include <cmath>
#include <chrono>
#include <pthread.h>

#define N 100000000
#define NUM_THREADS 8

double a = 0.0;
double b = 10.0;
double h;

double table[NUM_THREADS];

double f(double x) {
    return x * x;
}

void* compute(void* arg) {
    int id = *((int*)arg);

    int pieces = N / NUM_THREADS;

    int start_index = id * pieces;

    int end_index;
    if (id == NUM_THREADS - 1) {
        end_index = N;
    }
    else {
        end_index = (id + 1) * pieces;
    }

    double local_sum = 0.0;

    for (int i = start_index; i < end_index; i++) {
        double x = a + i * h;
        local_sum = local_sum + 2.0 * f(x);
    }

    table[id] = local_sum;

    return NULL;
}

int main() {
    h = (b - a) / N;

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, compute, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double total_sum = 0.0;

    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum = total_sum + table[i];
    }

    total_sum = total_sum + f(a) + f(b);

    double result = total_sum * (h / 2.0);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Integral = " << result << std::endl;
    std::cout << "Time = " << elapsed.count() << " seconds" << std::endl;

    return 0;
}