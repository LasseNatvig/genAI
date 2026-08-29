#!/usr/bin/env python3

import os

# Create the main comparison C file
c_code = '''// main_comparison.c - Comprehensive Quicksort Implementation Comparison
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef __x86_64__
#include <x86intrin.h>
#define USE_RDTSC 1
#else
#define USE_RDTSC 0
#endif

#define MAX_PATH_LEN 256
#define TIMESTAMP_LEN 20

// Global counters for detailed performance analysis
static unsigned long long comparison_count = 0;
static unsigned long long swap_count = 0;

typedef struct {
    double execution_time;
    uint64_t cpu_cycles;
    unsigned long long comparisons;
    unsigned long long swaps;
} BenchmarkResult;'''

with open('main_comparison.c', 'w') as f:
    f.write(c_code)

print("Created main_comparison.c")