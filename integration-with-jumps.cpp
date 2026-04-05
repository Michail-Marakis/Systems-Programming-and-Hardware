#include <iostream>
#include <cmath>
#include <chrono>
#include <pthread.h>

#define N 100000000
#define NUM_THREADS 8

double a = 0.0, b = 10.0;
double h;
double sum = 0.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f(double x) {
    return x * x;
}

void* compute(void* arg) {
    int id = *(int*)arg;

    double local_sum = 0.0;



    for (int i = id; i < N; i = i + NUM_THREADS) {
        double x = a + i * h;
        local_sum += 2 * f(x);
    }

    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    h = (b - a) / N;

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, compute, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sum += f(a) + f(b);
    double final_res = sum * (h / 2);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Integral = " << final_res << std::endl;
    std::cout << "Time = " << elapsed.count() << " sec\n";

    pthread_mutex_destroy(&mutex);

    return 0;
}