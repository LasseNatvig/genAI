# Function Performance Measurement Guide

## Overview
This document describes the methodology for measuring function performance using command-line arguments for **problem size (N)** and **number of experiments (E)**, along with hardware performance counter readings on ARM Cortex-A72 processors.

---

## Methodology

### Command-Line Arguments
The program accepts two command-line arguments:
1. **N (Problem Size)**: Maximum array size to test (default: 20000)
2. **E (Experiments)**: Number of times to run each test for averaging (default: 5)

### Example Usage
```bash
# Run with max array size 30000, 3 experiments
./perf_counters 30000 3

# Run with default values (20000, 5)
./perf_counters

# Via Makefile (runs with N=30000, E=3 and plots results)
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

### Derived Metrics
- **IPC**: Instructions / Cycles
- **Miss Rate**: (CacheMisses / CacheRefs) * 100%
- **Execution Time**: Wall-clock time in seconds
- **RSD%**: Relative standard deviation across E experiments

---

## Measurement Process

### Initialization
Counters are initialized with `exclude_kernel=1` and `exclude_hv=1` to count only user-space events.

### Execution Loop
```c
for (int n = 10000; n <= max_array_size; n += 1000) {
    for (int exp = 0; exp < noExp; exp++) {
        // Generate random array
        int *arr = malloc(n * sizeof(int));
        generate_random_array(arr, n);
        
        // Start measurement
        clock_gettime(CLOCK_MONOTONIC, &start);
        counters_start();
        
        // Execute function under test
        insertion_sort(arr, n);
        
        // Stop measurement
        counters_stop();
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        // Store results
        times[exp] = elapsed_time;
        cycles[exp] = ctrs[0].value;
        instrs[exp] = ctrs[1].value;
        crefs[exp] = ctrs[2].value;
        cmiss[exp] = ctrs[3].value;
        free(arr);
    }
    print_statistics(label, cycles, instrs, crefs, cmiss, times);
}
```

### Output
- Statistics printed to console
- All data saved to `res/sorting_perf_<timestamp>.csv`
- CSV columns: Workload, Cycles, Instructions, CacheRefs, CacheMiss, IPC, Miss%, Time(s), and RSD% for each

---

## Data Analysis and Visualization

### Automatic Plotting
The `make run` target:
1. Executes `./perf_counters 30000 3`
2. Finds latest CSV in `res/`
3. Runs `python3 plot_perf.py` on it
4. Saves plot to `plots/plot_sorting_perf_<timestamp>.png`

### Manual Plotting
```bash
# Plot specific file
python3 plot_perf.py res/sorting_perf_*.csv

# Interactive mode
python3 plot_perf.py
```

---

## Best Practices

### Problem Size (N)
- **Small** (N=1000-5000): Initial testing
- **Moderate** (N=10000-20000): Standard benchmarking
- **Large** (N>20000): Caution with O(n²) algorithms!

### Experiments (E)
- **E=1**: Quick test
- **E=3-5**: Good balance (recommended)
- **E=10-20**: High statistical accuracy

### Warning
O(n²) algorithms (Bubble Sort, Insertion Sort) become very slow for N > 20000. Use `timeout`:
```bash
timeout 10 ./perf_counters 11000 1
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
- `perf_counters_L1data.c`: Main program
- `sorting.c`: Sorting implementations
- `Makefile`: Build system
- `plot_perf.py`: Plotting script
- `res/`: CSV output directory
- `plots/`: Plot output directory
- `measuring.md`: This documentation file

---

## Summary
This framework enables systematic function performance measurement with:
- Configurable N and E via command-line
- Hardware performance counter readings
- Derived metrics (IPC, miss rate)
- CSV output for analysis
- Automatic plotting via Makefile
- Support for extending to any function
