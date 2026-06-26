#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MIN_SIZE 1024      // 1KB (256 ints) Should always be 1K
// #define MAX_SIZE 4194304   // 4MB (1M ints)
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

    // Sequential access
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < iterations; i++) {
        volatile int sum = 0;
        for (int j = 0; j < array_size; j++) {
            sum += array[j];
        }
    }
    gettimeofday(&end, NULL);
    *seq_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

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

    gettimeofday(&start, NULL);
    for (int i = 0; i < iterations; i++) {
        volatile int sum = 0;
        for (int j = 0; j < array_size; j++) {
            sum += array[indices[j]];
        }
    }
    gettimeofday(&end, NULL);
    *rand_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

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
    srand(time(NULL));

    // printf("size of int %d\n", sizeof(int)); // Answer was 4, as expected for 32 bit

     // Test array sizes with finer granularity around cache boundaries
     // L1 cache ~32KB, L2 cache ~1MB for Raspberry Pi4B
     int sizes[] = { MIN_SIZE, 2*MIN_SIZE, 3*MIN_SIZE, 4*MIN_SIZE, 5*MIN_SIZE, 6*MIN_SIZE, 7*MIN_SIZE, 8*MIN_SIZE,
        9*MIN_SIZE, 10*MIN_SIZE, 11*MIN_SIZE, 12*MIN_SIZE, 16*MIN_SIZE, 32*MIN_SIZE, 64*MIN_SIZE, 
        128*MIN_SIZE, 140*MIN_SIZE, 160*MIN_SIZE, 180*MIN_SIZE, 200*MIN_SIZE, 220*MIN_SIZE, 240*MIN_SIZE, 260*MIN_SIZE, 280*MIN_SIZE, 300*MIN_SIZE,
        320*MIN_SIZE, 340*MIN_SIZE, 360*MIN_SIZE, 380*MIN_SIZE, 400*MIN_SIZE, 500*MIN_SIZE, 600*MIN_SIZE, 
        800*MIN_SIZE, 1000*MIN_SIZE   
     };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        benchmark(sizes[i], ITERATIONS);
    }

    return 0;
}