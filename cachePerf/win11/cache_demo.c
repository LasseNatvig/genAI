#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define K 1024      // 1KB (256 ints)
#define ITERATIONS 100
#define N_EXPERIMENTS 10    // Number of experiments to run per array size

void single_experiment(int array_size, int iterations, double *seq_time, double *rand_time) {
    int *array = malloc(array_size * sizeof(int));
    if (!array) {
        fprintf(stderr, "Memory allocation failed for size %d\n", array_size);
        *seq_time = -1;
        *rand_time = -1;
        return;
    }

    // Initialize array
    for (int i = 0; i < array_size; i++) {
        array[i] = i;
    }

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    // Sequential access
    QueryPerformanceCounter(&start);
    for (int i = 0; i < iterations; i++) {
        volatile int sum = 0;
        for (int j = 0; j < array_size; j++) {
            sum += array[j];
        }
    }
    QueryPerformanceCounter(&end);
    *seq_time = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

    // Random access
    int *indices = malloc(array_size * sizeof(int));
    if (!indices) {
        fprintf(stderr, "Memory allocation failed for indices\n");
        free(array);
        *seq_time = -1;
        *rand_time = -1;
        return;
    }

    for (int i = 0; i < array_size; i++) {
        indices[i] = rand() % array_size;
    }

    QueryPerformanceCounter(&start);
    for (int i = 0; i < iterations; i++) {
        volatile int sum = 0;
        for (int j = 0; j < array_size; j++) {
            sum += array[indices[j]];
        }
    }
    QueryPerformanceCounter(&end);
    *rand_time = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;

    free(array);
    free(indices);
}

void benchmark(int array_size, int iterations) {
    for (int n = 0; n < N_EXPERIMENTS; n++) {
        double seq_time, rand_time;
        single_experiment(array_size, iterations, &seq_time, &rand_time);

        // Print individual experiments instead of averages
        if (seq_time > 0 && rand_time > 0) {
            printf("%d %f %f\n", array_size * (int)sizeof(int), seq_time, rand_time);
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));

    printf("size of int: %d bytes\n", (int)sizeof(int));

    // Test array sizes with finer granularity around cache boundaries
    // L1 data ~32KB per core, L2 ~2MB per P-core, L3 ~30MB shared (Intel i7-13700)
    int sizes[] = { K, 2*K, 3*K, 4*K, 5*K, 6*K, 7*K, 8*K, 9*K, 10*K, 11*K, 12*K, 16*K, 32*K, 64*K,
       128*K, 140*K, 180*K, 220*K, 260*K, 300*K,
       320*K, 340*K, 360*K, 380*K, 400*K, 450*K, 500*K, 550*K, 600*K, 800*K, 1000*K, 1400*K,
       2000*K, 3000*K, 4000*K, 8000*K, 12000*K, 16000*K, 20000*K}; 
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        benchmark(sizes[i], ITERATIONS);
    }

    return 0;
}
