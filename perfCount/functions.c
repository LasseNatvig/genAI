// functions.c - Loop and sorting functions for performance measurement

#include <stdint.h>
#include <stdlib.h>

// Used as an output sink to prevent the compiler from eliminating loops
extern volatile uint64_t sink;

// Loop with 1000 iterations
void loop_1000(void) {
    for (int i = 0; i < 1000; i++) {
        int b = i;
        b = b + 2;
        b = b % 56;
    }
}

// Loop with 1000 iterations (sink variant)
void loop_1000_sink(void) {
    for (int i = 0; i < 1000; i++) {
        int b = i;
        b = b + 2;
        b = b % 56;
        sink = b;
    }
}

// Loop with 5000 iterations
void loop_5000(void) {
    for (int i = 0; i < 5000; i++) {
        int b = i;
        b = b + 2;
        b = b % 56;
    }
}

// Loop with 5000 iterations (sink variant)
void loop_5000_sink(void) {
    for (int i = 0; i < 5000; i++) {
        int b = i;
        b = b + 2;
        b = b % 56;
        sink = b;
    }
}

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
