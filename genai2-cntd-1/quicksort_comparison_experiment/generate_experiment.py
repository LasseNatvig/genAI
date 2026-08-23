#!/usr/bin/env python3

# Generate main_comparison.c file
c_content = """// QuickSort Comparison Experiment
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define TIMESTAMP_LEN 20

static unsigned long long comparison_count = 0;
static unsigned long long swap_count = 0;

typedef struct {
    double execution_time;
    unsigned long long comparisons;
    unsigned long long swaps;
} BenchmarkResult;

double get_time_in_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
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
"""

with open('main_comparison.c', 'w') as f:
    f.write(c_content)

print("Created basic main_comparison.c")