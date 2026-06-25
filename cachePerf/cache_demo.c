#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MIN_SIZE 1024      // 1KB (256 ints)
#define MAX_SIZE 4194304   // 4MB (1M ints)
#define ITERATIONS 100
#define N_EXPERIMENTS 5    // Number of experiments to run per array size

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
    double avg_seq_time = 0;
    double avg_rand_time = 0;
    int valid_experiments = 0;

    for (int n = 0; n < N_EXPERIMENTS; n++) {
        double seq_time, rand_time;
        single_experiment(array_size, iterations, &seq_time, &rand_time);

        if (seq_time > 0 && rand_time > 0) {
            avg_seq_time += seq_time;
            avg_rand_time += rand_time;
            valid_experiments++;
        }
    }

    if (valid_experiments > 0) {
        avg_seq_time /= valid_experiments;
        avg_rand_time /= valid_experiments;
    }

    printf("%d %f %f\n", array_size * (int)sizeof(int), avg_seq_time, avg_rand_time);
}

int main() {
    srand(time(NULL));

        printf("size of int %d\n", sizeof(int));

     // Test array sizes with finer granularity around cache boundaries
     // L1 cache ~32KB, L2 cache ~1MB for Raspberry Pi4B
     int sizes[] = {
         // Small sizes
         MIN_SIZE, 2048, 4096, 8192, 16384,
         // Around L1 cache (32KB) - doubled from 3 to 6 sizes
         20480, 24576, 28672, 32768, 36864, 40960,
         // Between L1 and L2
         49152, 65536, 98304, 131072, 196608, 262144, 393216, 524288,
         // Around L2 cache (1MB) - doubled from 3 to 6 sizes
         786432, 884736, 983040, 1048576, 1153432, 1258291,
         // Larger sizes
         2097152, 3145728, 4194304
     };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        benchmark(sizes[i], ITERATIONS);
    }

    return 0;
}