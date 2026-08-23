// QuickSort Comparison Experiment
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define TIMESTAMP_LEN 20

static unsigned long long comparison_count = 0;
static unsigned long long swap_count = 0;

typedef struct {
    double execution_time;
    unsigned long long comparisons;
    unsigned long long swaps;
} BenchmarkResult;

double get_time_in_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

void copy_array(int dest[], int src[], int n) {
    for (int i = 0; i < n; i++) dest[i] = src[i];
}

int lomuto_partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        comparison_count++;
        if (arr[j] < pivot) {
            i++;
            swap_count++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    swap_count++;
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

void quicksort_lomuto(int arr[], int low, int high) {
    if (low < high) {
        int pi = lomuto_partition(arr, low, high);
        quicksort_lomuto(arr, low, pi - 1);
        quicksort_lomuto(arr, pi + 1, high);
    }
}

void quicksort_lomuto_wrapper(int arr[], int n) {
    comparison_count = 0;
    swap_count = 0;
    quicksort_lomuto(arr, 0, n - 1);
}

// --- HOARE PARTITION (More efficient) ---
int hoare_partition(int arr[], int low, int high) {
    int pivot = arr[low];
    int i = low - 1;
    int j = high + 1;
    while (1) {
        do { i++; comparison_count++; } while (arr[i] < pivot);
        do { j--; comparison_count++; } while (arr[j] > pivot);
        comparison_count++;
        if (i >= j) return j;
        swap_count++;
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void quicksort_hoare(int arr[], int low, int high) {
    if (low < high) {
        int pi = hoare_partition(arr, low, high);
        quicksort_hoare(arr, low, pi);
        quicksort_hoare(arr, pi + 1, high);
    }
}

void quicksort_hoare_wrapper(int arr[], int n) {
    comparison_count = 0;
    swap_count = 0;
    quicksort_hoare(arr, 0, n - 1);
}

// Library qsort for reference
int compare_ints(const void *a, const void *b) {
    comparison_count++;
    return (*(int *)a - *(int *)b);
}

void library_qsort_wrapper(int arr[], int n) {
    comparison_count = 0;
    swap_count = 0;
    qsort(arr, n, sizeof(int), compare_ints);
}

// Array generation functions
void generate_random_array(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = rand();
}

void generate_sorted_array(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = i;
}

void generate_reverse_sorted_array(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = n - i;
}

void generate_array_with_duplicates(int arr[], int n, int dup_factor) {
    for (int i = 0; i < n; i++) arr[i] = rand() % (n / dup_factor);
}

typedef void (*SortFunction)(int[], int);

BenchmarkResult benchmark_sort(SortFunction sort_func, int original[], int n, int iterations) {
    BenchmarkResult result = {0};
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        result.execution_time = -1;
        return result;
    }

    double total_time = 0;
    unsigned long long total_comparisons = 0;
    unsigned long long total_swaps = 0;

    for (int i = 0; i < iterations; i++) {
        copy_array(arr, original, n);
        comparison_count = 0;
        swap_count = 0;

        double start_time = get_time_in_seconds();
        sort_func(arr, n);
        double end_time = get_time_in_seconds();

        total_time += (end_time - start_time);
        total_comparisons += comparison_count;
        total_swaps += swap_count;
    }

    free(arr);
    result.execution_time = total_time / iterations;
    result.comparisons = total_comparisons / iterations;
    result.swaps = total_swaps / iterations;
    return result;
}

typedef enum {
    ARRAY_RANDOM, ARRAY_SORTED, ARRAY_REVERSE_SORTED, ARRAY_WITH_DUPLICATES
} ArrayType;

const char* get_array_type_name(ArrayType type) {
    const char* names[] = {"random", "sorted", "reverse_sorted", "with_duplicates"};
    return names[type];
}

void generate_array_by_type(int arr[], int n, ArrayType type, int param) {
    switch (type) {
        case ARRAY_RANDOM: generate_random_array(arr, n); break;
        case ARRAY_SORTED: generate_sorted_array(arr, n); break;
        case ARRAY_REVERSE_SORTED: generate_reverse_sorted_array(arr, n); break;
        case ARRAY_WITH_DUPLICATES: generate_array_with_duplicates(arr, n, param); break;
    }
}

typedef struct {
    const char *name;
    SortFunction func;
} Algorithm;

void get_timestamp(char *timestamp, int len) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, len, "%Y%m%d_%H%M%S", tm_info);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <N> <E> <output_file> [timestamp]\n", argv[0]);
        fprintf(stderr, "  N: Number of elements\n");
        fprintf(stderr, "  E: Number of iterations\n");
        fprintf(stderr, "  output_file: CSV file to append results to\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int E = atoi(argv[2]);
    const char *output_file = argv[3];
    char timestamp[TIMESTAMP_LEN];
    if (argc >= 5) {
        snprintf(timestamp, TIMESTAMP_LEN, "%s", argv[4]);
    } else {
        get_timestamp(timestamp, TIMESTAMP_LEN);
    }

    if (N <= 0 || E <= 0) {
        fprintf(stderr, "Error: N and E must be positive integers\n");
        return 1;
    }

    Algorithm algorithms[] = {
        {"lomuto_quicksort", quicksort_lomuto_wrapper},
        {"hoare_quicksort", quicksort_hoare_wrapper},
        {"library_qsort", library_qsort_wrapper}
    };
    int num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);

    printf("Benchmarking with N=%d, E=%d\n", N, E);
    srand(time(NULL));

    int *original_array = (int *)malloc(N * sizeof(int));
    if (!original_array) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    ArrayType array_types[] = {ARRAY_RANDOM, ARRAY_SORTED, ARRAY_REVERSE_SORTED, ARRAY_WITH_DUPLICATES};
    int duplicate_factor = 10;

    FILE *csv_file = fopen(output_file, "a");
    if (!csv_file) {
        fprintf(stderr, "Error: Could not open CSV file\n");
        free(original_array);
        return 1;
    }

    if (ftell(csv_file) == 0) {
        fprintf(csv_file, "timestamp,N,E,array_type,algorithm,execution_time,comparisons,swaps\n");
    }

    for (int t = 0; t < 4; t++) {
        ArrayType array_type = array_types[t];
        printf("\nTesting %s arrays:\n", get_array_type_name(array_type));
        generate_array_by_type(original_array, N, array_type, duplicate_factor);

        for (int a = 0; a < num_algorithms; a++) {
            Algorithm alg = algorithms[a];
            printf("  Testing %s... ", alg.name);
            fflush(stdout);

            // Warm-up
            int *warmup = malloc(N * sizeof(int));
            copy_array(warmup, original_array, N);
            alg.func(warmup, N);
            free(warmup);

            BenchmarkResult result = benchmark_sort(alg.func, original_array, N, E);
            if (result.execution_time < 0) continue;

            printf("Done. Time: %.6fs, Comparisons: %llu, Swaps: %llu\n",
                   result.execution_time, result.comparisons, result.swaps);

            fprintf(csv_file, "%s,%d,%d,%s,%s,%.9f,%llu,%llu\n",
                    timestamp, N, E, get_array_type_name(array_type), alg.name,
                    result.execution_time, result.comparisons, result.swaps);
            fflush(csv_file);
        }
    }

    fclose(csv_file);
    free(original_array);
    printf("\nBenchmark complete! Results saved to %s\n", output_file);
    return 0;
}
