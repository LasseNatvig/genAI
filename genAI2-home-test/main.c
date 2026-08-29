// main.c - Sorting algorithms benchmark

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 1000
#define RUNS 5

// Insertion sort implementation
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

// Bubble sort implementation
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

// Quick sort implementation
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
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

void quicksort_helper(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort_helper(arr, low, pi - 1);
        quicksort_helper(arr, pi + 1, high);
    }
}

void quicksort(int arr[], int n) {
    quicksort_helper(arr, 0, n - 1);
}

// Generate random array of size n
void generate_random_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000; // Random numbers between 0 and 9999
    }
}

// Copy array from source to destination
void copy_array(int dest[], int src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

// Measure average time for a sorting function
double measure_sort_time(void (*sort_func)(int[], int), int original[], int n, int runs) {
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return -1;
    }

    clock_t total_time = 0;

    for (int i = 0; i < runs; i++) {
        copy_array(arr, original, n);

        clock_t start = clock();
        sort_func(arr, n);
        clock_t end = clock();

        total_time += (end - start);
    }

    free(arr);

    return ((double)total_time / runs) / CLOCKS_PER_SEC * 1000; // Convert to milliseconds
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    // Allocate arrays
    int *original = malloc(N * sizeof(int));
    if (original == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Generate random array
    generate_random_array(original, N);

    printf("Sorting %d random integers, running each algorithm %d times...\n\n", N, RUNS);

    // Measure insertion sort
    double insertion_avg = measure_sort_time(insertion_sort, original, N, RUNS);
    printf("Insertion Sort Average Time: %.4f ms\n", insertion_avg);

    // Measure bubble sort
    double bubble_avg = measure_sort_time(bubble_sort, original, N, RUNS);
    printf("Bubble Sort Average Time: %.4f ms\n", bubble_avg);

    // Measure quick sort
    double quick_avg = measure_sort_time(quicksort, original, N, RUNS);
    printf("Quick Sort Average Time: %.4f ms\n", quick_avg);

    free(original);

    return 0;
}