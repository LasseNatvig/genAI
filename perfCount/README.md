# Hardware Performance Counter Demo — Raspberry Pi 4B

Demonstrates reading ARM Cortex-A72 PMU (Performance Monitoring Unit) hardware
counters from user-space using the Linux `perf_event_open()` syscall.
No external tools required — the syscall is part of the kernel.

## Build & Run

```bash
make
./perf_counters max_array_size start_array_size step_size noExp
```

Example:
```bash
# Run with array sizes from 16 to 30000 in steps of 2000, 3 experiments
./perf_counters 30000 16 2000 3
```

If you see *Operation not permitted*:

```bash
echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
```

Via Makefile:
```bash
# Uses: start=16, step=2000, max=30000, experiments=3
make run

# Plot results with interactive matplotlib window
make plot
```

## Counters

The Cortex-A72 PMU provides 1 fixed cycle counter and 6 programmable event
counters. This program uses these `PERF_TYPE_HARDWARE` events:

| Counter | Description |
|---|---|
| **Cycles** | CPU clock cycles elapsed |
| **Instructions** | Instructions retired |
| **Cache Refs** | L1 data cache accesses (reads + writes) |
| **Cache Misses** | L1 data cache misses that went to L2/RAM |
| **Branch Misses** | Branches mispredicted by the branch predictor |

Derived metrics printed per workload:

- **IPC** (Instructions Per Cycle) — higher is better; drops when the CPU stalls waiting on memory.
- **CacheMiss%** — cache miss rate (`Cache Misses / Cache Refs`); high values mean the working set exceeds the cache.
- **BranchMissPerInstr** — branch misses per instruction (`Branch Misses / Instructions`).

## Workloads

Three sorting algorithms are measured and compared:

| Workload | Description |
|---|---|
| **Insertion Sort** | O(n²) algorithm, good for small or nearly-sorted arrays |
| **Bubble Sort** | O(n²) algorithm, simple but inefficient for large arrays |
| **Quick Sort** | O(n log n) average case, much faster for larger arrays |

## Hardware Notes

- **SoC:** Broadcom BCM2711 (Raspberry Pi 4B)
- **Core:** ARM Cortex-A72, ARMv8-A
- **L1D cache:** 32 KB per core
- **L2 cache:** 1 MB unified (shared across all 4 cores)
- **PMU:** 1 cycle counter + 6 programmable counters per core

The program gracefully skips any counter that is not supported on the current
CPU (e.g. when running inside a VM with a restricted PMU).

## Output

- Terminal: Formatted table with column headers and metrics
- CSV file: `res/sorting_perf_<timestamp>.csv` with raw counter values
- Plot: `plots/plot_sorting_perf_<timestamp>.png` with execution time vs array size
- Interactive window: `make plot` opens a matplotlib window where you can zoom and pan

Terminal values are displayed as:
- Cycles, Instructions, CacheRefs: in millions (M)
- CacheMiss, BranchMiss: in thousands (k)
- IPC: ratio
- CacheMiss%: percentage
- BranchMissPerInstr: per-instruction rate (scientific notation)
- Time: in seconds

All values show: Average(RelativeStdDev%)

### Interactive Plot
Running `make plot` displays an interactive matplotlib window that allows:
- Zooming in/out on specific regions
- Panning to explore different parts of the plot
- Hovering to see data points

**Note:** The matplotlib window might appear under other open windows. Check your taskbar/dock if you don't see it.

## Files

- `perf_counters_L1data.c`: Main program with performance counter logic and sorting benchmarks
- `sorting.c`: Sorting algorithm implementations (insertion_sort, bubble_sort, quick_sort)
- `Makefile`: Build system with run target
- `plot_perf.py`: Python script for plotting CSV results with blue (Insertion Sort), red (Bubble Sort), green (Quick Sort) lines
- `res/`: CSV output directory
- `plots/`: Plot output directory
