# Function Performance Measurement Guide

## Overview
This document describes the methodology for measuring function performance using command-line arguments for **array size range (start, max, step)** and **number of experiments (E)**, along with hardware performance counter readings on ARM Cortex-A72 processors.

---

## Methodology

### Command-Line Arguments
The program accepts four command-line arguments:
1. **max_array_size**: Maximum array size to test
2. **start_array_size**: Starting array size
3. **step_size**: Increment between array sizes
4. **noExp (E)**: Number of times to run each test for averaging

### Example Usage
```bash
# Run with max array size 30000, starting at 16, step 2000, 3 experiments
./perf_counters 30000 16 2000 3

# Via Makefile (uses: start=16, step=2000, max=30000, experiments=3)
make run
```

---

## Performance Counters

Uses Linux `perf_event_open()` to access ARM Cortex-A72 PMU:

| Counter | Event | Description |
|---------|-------|-------------|
| Cycles | `PERF_COUNT_HW_CPU_CYCLES` | CPU cycles consumed |
| Instructions | `PERF_COUNT_HW_INSTRUCTIONS` | Instructions retired |
| Cache Refs | `PERF_COUNT_HW_CACHE_REFERENCES` | Cache references |
| Cache Misses | `PERF_COUNT_HW_CACHE_MISSES` | Cache misses |
| Branch Misses | `PERF_COUNT_HW_BRANCH_MISSES` | Branch instruction misses |

### Derived Metrics
- **IPC**: Instructions / Cycles
- **CacheMiss%**: (CacheMisses / CacheRefs) * 100%
- **BranchMissPerInstr**: BranchMisses / Instructions (per-instruction rate)
- **Execution Time**: Wall-clock time in seconds
- **RSD%**: Relative standard deviation across E experiments

### Output Value Units
- Cycles, Instructions, CacheRefs: displayed in millions (M)
- CacheMiss, BranchMiss: displayed in thousands (k)
- IPC: ratio
- CacheMiss%: percentage
- BranchMissPerInstr: scientific notation
- Time: seconds

---

## Sorting Algorithms Tested

Three sorting algorithms are benchmarked:

| Algorithm | Complexity | Description |
|-----------|------------|-------------|
| **Insertion Sort** | O(n²) | Efficient for small or nearly-sorted arrays |
| **Bubble Sort** | O(n²) | Simple comparison-based sort |
| **Quick Sort** | O(n log n) avg | Fast divide-and-conquer algorithm |

## Measurement Process

### Initialization
Counters are initialized with `exclude_kernel=1` and `exclude_hv=1` to count only user-space events.

### Execution Loop
For each array size from start to max in steps of step_size:
```c
for (int n = start_array_size; n <= max_array_size; n += step_size) {
    for (int exp = 0; exp < noExp; exp++) {
        // Generate random array
        int *arr = malloc(n * sizeof(int));
        generate_random_array(arr, n);
        
        // Start measurement
        clock_gettime(CLOCK_MONOTONIC, &start);
        counters_start();
        
        // Execute sorting algorithm
        insertion_sort(arr, n);
        // or bubble_sort(arr, n);
        // or quick_sort(arr, 0, n-1);
        
        // Stop measurement
        counters_stop();
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Store results
        times[exp] = elapsed_time;
        cycles[exp] = ctrs[IDX_CYCLES].value;
        instrs[exp] = ctrs[IDX_INSTRUCTIONS].value;
        crefs[exp] = ctrs[IDX_CACHE_REFS].value;
        cmiss[exp] = ctrs[IDX_CACHE_MISSES].value;
        bmisses[exp] = ctrs[IDX_BRANCH_MISSES].value;
        free(arr);
    }
    print_statistics(label, cycles, instrs, crefs, cmiss, bmisses, times);
}
```

### Output
- Statistics printed to console with formatted columns
- All data saved to `res/sorting_perf_<timestamp>.csv`
- CSV columns: Workload, Cycles, Instructions, CacheRefs, CacheMiss, BranchMisses, IPC, CacheMiss%, BranchMissPerInstr, Time(s), and RSD% for each counter

---

## Data Analysis and Visualization

### Automatic Plotting
The `make run` target:
1. Compiles and runs `./perf_counters 30000 16 2000 3`
2. Finds latest CSV in `res/`
3. Runs `python3 plot_perf.py` on it
4. Saves plot to `plots/plot_sorting_perf_<timestamp>.png`

Plot colors:
- **Blue**: Insertion Sort
- **Red**: Bubble Sort  
- **Green**: Quick Sort

### Manual Plotting
```bash
# Plot specific file
python3 plot_perf.py res/sorting_perf_*.csv

# Plot latest file
python3 plot_perf.py res/sorting_perf_$(ls -t res/*.csv | head -1 | cut -d/ -f3)
```

---

## Best Practices

### Array Size Configuration
- **Small** (start=16, max=1000, step=100): Quick testing
- **Moderate** (start=16, max=30000, step=2000): Standard benchmarking (used by `make run`)
- **Custom**: Adjust based on algorithm complexity and patience

### Experiments (E)
- **E=1**: Very quick, no statistical data
- **E=3**: Good balance (recommended, used by `make run`)
- **E=5-10**: High statistical accuracy

### Warning
O(n²) algorithms (Bubble Sort, Insertion Sort) become very slow for large array sizes. Use `timeout` for long-running tests:
```bash
# Run with small array sizes
./perf_counters 1000 16 100 1

# Or use timeout
timeout 30 ./perf_counters 5000 16 500 3
```

---

## Error Handling

### Permission Issues
```bash
echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
```

### Directory Setup
```bash
mkdir -p res plots
```

---

## Extending to Other Functions

```c
for (int exp = 0; exp < noExp; exp++) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    counters_start();
    
    your_function(input);
    
    counters_stop();
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Store and report results
}
```

---

## Files
- `perf_counters_L1data.c`: Main program with performance counter logic and sorting benchmarks
- `sorting.c`: Sorting algorithm implementations (insertion_sort, bubble_sort, quick_sort)
- `Makefile`: Build system with run target
- `plot_perf.py`: Plotting script with algorithm-specific colors
- `res/`: CSV output directory
- `plots/`: Plot output directory (PNG files)
- `measuring.md`: This documentation file

---

## Summary
This framework enables systematic performance measurement with:
1. Configurable array sizes (start, max, step) and experiments via command-line
2. Hardware performance counter readings from ARM Cortex-A72 PMU
3. Derived metrics (IPC, CacheMiss%, BranchMissPerInstr)
4. Terminal output with formatted columns (M for millions, k for thousands)
5. CSV output for data analysis
6. Automatic plotting via Makefile with algorithm-specific colors
7. Support for extending to any function
