// main.c - Sorting algorithms benchmark with insertion sort, bubble sort, and quicksort
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PATH_LEN 256
#define TIMESTAMP_LEN 20

// Function prototypes
void insertion_sort(int arr[], int n);
void bubble_sort(int arr[], int n);
void quicksort(int arr[], int low, int high);
int partition(int arr[], int low, int high);
void generate_random_array(int arr[], int n);
void copy_array(int dest[], int src[], int n);
double bench_sort(void (*sort_func)(int[], int), int arr[], int n, int iterations);
double bench_quicksort(void (*sort_func)(int[], int, int), int arr[], int n, int iterations);
void get_timestamp(char *timestamp, int len);
void write_results_to_csv(const char *filename, int N, int E, double insertion_time, double bubble_time, double quicksort_time, const char *timestamp);

int main(int argc, char *argv[]) {
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: %s <N> <E> [csv_filename] [timestamp]\n", argv[0]);
        fprintf(stderr, "  N: Number of random integers to generate\n");
        fprintf(stderr, "  E: Number of runs for each algorithm\n");
        fprintf(stderr, "  csv_filename: (optional) CSV file to append results to\n");
        fprintf(stderr, "  timestamp: (optional) Timestamp for this run\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int E = atoi(argv[2]);

    if (N <= 0 || E <= 0) {
        fprintf(stderr, "Error: N and E must be positive integers\n");
        return 1;
    }

    // Determine CSV filename and timestamp
    char csv_filename[MAX_PATH_LEN];
    char provided_timestamp[TIMESTAMP_LEN] = "";
    if (argc == 5) {
        // Use provided CSV filename and timestamp
        snprintf(csv_filename, MAX_PATH_LEN, "%s", argv[3]);
        snprintf(provided_timestamp, TIMESTAMP_LEN, "%s", argv[4]);
    } else if (argc == 4) {
        // Use provided CSV filename, generate timestamp
        char timestamp[TIMESTAMP_LEN];
        get_timestamp(timestamp, TIMESTAMP_LEN);
        snprintf(csv_filename, MAX_PATH_LEN, "%s", argv[3]);
        snprintf(provided_timestamp, TIMESTAMP_LEN, "%s", timestamp);
    } else {
        // Generate timestamp for CSV file
        char timestamp[TIMESTAMP_LEN];
        get_timestamp(timestamp, TIMESTAMP_LEN);
        snprintf(csv_filename, MAX_PATH_LEN, "results_%s.csv", timestamp);
        snprintf(provided_timestamp, TIMESTAMP_LEN, "%s", timestamp);
    }

    // Seed random number generator
    srand(time(NULL));

    // Allocate memory for arrays
    int *original_array = (int *)malloc(N * sizeof(int));
    int *copy1 = (int *)malloc(N * sizeof(int));
    int *copy2 = (int *)malloc(N * sizeof(int));
    int *copy3 = (int *)malloc(N * sizeof(int));

    if (!original_array || !copy1 || !copy2 || !copy3) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    printf("Benchmarking sorting algorithms with N=%d, E=%d\n", N, E);

    // Warm-up run to avoid cold start bias
    generate_random_array(original_array, N);
    copy_array(copy1, original_array, N);
    insertion_sort(copy1, N);

    // Benchmark insertion sort
    generate_random_array(original_array, N);
    double avg_insertion = bench_sort(insertion_sort, original_array, N, E);
    printf("Insertion Sort: %.6f seconds average\n", avg_insertion);

    // Benchmark bubble sort
    generate_random_array(original_array, N);
    double avg_bubble = bench_sort(bubble_sort, original_array, N, E);
    printf("Bubble Sort: %.6f seconds average\n", avg_bubble);

    // Benchmark quicksort
    generate_random_array(original_array, N);
    double avg_quick = bench_quicksort(quicksort, original_array, N, E);
    printf("QuickSort: %.6f seconds average\n", avg_quick);

    // Write results to CSV file
    write_results_to_csv(csv_filename, N, E, avg_insertion, avg_bubble, avg_quick, provided_timestamp);
    printf("Results written to %s\n", csv_filename);

    // Free memory
    free(original_array);
    free(copy1);
    free(copy2);
    free(copy3);

    return 0;
}

// Get current timestamp as string
void get_timestamp(char *timestamp, int len) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, len, "%Y%m%d_%H%M%S", tm_info);
}

// Write results to CSV file
void write_results_to_csv(const char *filename, int N, int E, double insertion_time, double bubble_time, double quicksort_time, const char *timestamp) {
    FILE *file = fopen(filename, "a");

    if (file == NULL) {
        fprintf(stderr, "Error: Could not open %s for writing\n", filename);
        return;
    }

    // Check if file is empty (first write)
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    if (file_size == 0) {
        // Write header
        fprintf(file, "timestamp,N,E,insertion_sort_time,bubble_sort_time,quicksort_time\n");
    }

    // Write data with provided timestamp
    fprintf(file, "%s,%d,%d,%.6f,%.6f,%.6f\n",
            timestamp, N, E, insertion_time, bubble_time, quicksort_time);

    fclose(file);
}

// Generate array of N random integers
void generate_random_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (2 * n + 1) - n; // Random numbers in range [-n, n]
    }
}

// Copy array
void copy_array(int dest[], int src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

// Insertion Sort
void insertion_sort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Bubble Sort
void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// QuickSort - main function
void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

// QuickSort - partition function
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// Benchmark function for sorting algorithms that take (int[], int)
double bench_sort(void (*sort_func)(int[], int), int original[], int n, int iterations) {
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        return -1;
    }

    clock_t total_time = 0;

    for (int i = 0; i < iterations; i++) {
        copy_array(arr, original, n);

        clock_t start = clock();
        sort_func(arr, n);
        clock_t end = clock();

        total_time += (end - start);
    }

    free(arr);
    return (double)total_time / (CLOCKS_PER_SEC * iterations);
}

// Benchmark function for quicksort which takes (int[], int, int)
double bench_quicksort(void (*sort_func)(int[], int, int), int original[], int n, int iterations) {
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        return -1;
    }

    clock_t total_time = 0;

    for (int i = 0; i < iterations; i++) {
        copy_array(arr, original, n);

        clock_t start = clock();
        sort_func(arr, 0, n - 1);
        clock_t end = clock();

        total_time += (end - start);
    }

    free(arr);
    return (double)total_time / (CLOCKS_PER_SEC * iterations);
}