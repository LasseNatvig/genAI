# Quicksort Implementation Comparison Experiment

A comprehensive comparison of different Quicksort implementations to analyze their performance characteristics across various problem sizes and array types.

## Purpose

This experiment helps understand the performance differences between:
- **Lomuto Partition Quicksort** (classic implementation)
- **Hoare Partition Quicksort** (more efficient partition scheme)
- **Standard Library qsort** (optimized reference implementation)

## Files

- `main_comparison.c` - Main C program with three quicksort implementations
- `Makefile` - Build system for the C program
- `run_comparison_benchmark.py` - Python script to run benchmarks
- `plot_comparison_results.py` - Python script to visualize results
- `analysis_notes.md` - Detailed analysis of implementation differences
- `results/` - Directory for benchmark results and plots

## Setup

### Prerequisites
- GCC compiler
- Python 3 with pandas, matplotlib, and seaborn
- Make

### Installation

```bash
# Navigate to the experiment directory
cd quicksort_comparison_experiment

# Install Python dependencies (if not already installed)
pip install pandas matplotlib seaborn
```

## Running the Experiment

### Compile the C Program

```bash
make
```

This will create the `quicksort_comparison` executable.

### Run Benchmarks

```bash
# Run with default parameters
python3 run_comparison_benchmark.py

# Or run manually for specific parameters
./quicksort_comparison 10000 10 results/my_results.csv my_timestamp
```

The benchmark will test:
- Array sizes: 1000, 5000, 10000, 20000, 30000, 50000, 100000
- Array types: random, sorted, reverse_sorted, with_duplicates
- 10 iterations per test (configurable)

### Generate Plots

```bash
python3 plot_comparison_results.py
```

This will:
1. Find all result CSV files
2. Create comprehensive plots showing:
   - Execution time comparison
   - Comparison count analysis
   - Swap count analysis
3. Provide detailed performance analysis

## Understanding the Results

### CSV Output Format

Each benchmark run produces a CSV file with columns:
- `timestamp` - When the benchmark was run
- `N` - Array size
- `E` - Number of iterations
- `array_type` - Type of array (random, sorted, reverse_sorted, with_duplicates)
- `algorithm` - Algorithm name
- `execution_time` - Average execution time in seconds
- `comparisons` - Average number of element comparisons
- `swaps` - Average number of element swaps

### Key Metrics to Analyze

1. **Execution Time** - Overall performance
2. **Comparison Count** - Efficiency of element comparisons
3. **Swap Count** - Efficiency of element movement
4. **Performance on Different Array Types** - Robustness against worst-case scenarios

## Expected Findings

### Performance Rankings
1. **Library qsort** - Fastest due to optimizations (median-of-three, insertion sort for small arrays, etc.)
2. **Hoare Partition** - Faster than Lomuto due to fewer swaps
3. **Lomuto Partition** - Slowest, especially on sorted/reverse-sorted arrays

### Array Type Impact
- **Random Arrays**: All algorithms perform well
- **Sorted Arrays**: Lomuto performs poorly (O(n²)), others handle better
- **Reverse Sorted**: Similar to sorted arrays
- **Arrays with Duplicates**: Hoare and qsort perform better than Lomuto

### Scalability
- Performance differences become more pronounced with larger array sizes
- The overhead of additional swaps and comparisons in Lomuto becomes more significant

## Customizing the Experiment

### Change Problem Sizes
Edit `run_comparison_benchmark.py` and modify `N_values`:
```python
N_values = [1000, 5000, 10000, 20000, 50000, 100000]
```

### Change Number of Iterations
Edit `run_comparison_benchmark.py` and modify `E`:
```python
E = 20  # More iterations for more accurate averaging
```

### Add More Array Types
Modify `main_comparison.c` to include additional array generation methods.

### Add More Algorithms
Add additional sorting algorithms to `main_comparison.c`:
- Randomized quicksort
- Three-way partitioning quicksort
- Insertion sort (for comparison)
- Merge sort (for reference)

## Implementation Analysis

See `analysis_notes.md` for detailed explanation of:
- How each partitioning scheme works
- Theoretical performance characteristics
- Why performance differences occur
- Optimization opportunities

## Clean Up

```bash
# Clean compiled files
make clean

# Remove old results
rm -f results/*
```

## Advanced Usage

### Profile Individual Runs
For detailed CPU-level analysis, use system profiling tools:

```bash
# Using perf on Linux
perf stat ./quicksort_comparison 10000 10 results/profile.csv profile

# Using time for basic timing
time ./quicksort_comparison 10000 10 results/time_test.csv time_test
```

### Compare with Original Implementation
The original implementation in the parent directory uses Lomuto partition. Compare results:

```bash
# Run original benchmark
cd ..
python3 run_benchmark.py

# Run comparison benchmark  
cd quicksort_comparison_experiment
python3 run_comparison_benchmark.py
```

## Learning Outcomes

By running this experiment, you will learn:

1. **Implementation Differences**: How partitioning schemes affect performance
2. **Algorithm Analysis**: The impact of different algorithmic choices
3. **Performance Measurement**: How to measure and compare algorithm performance
4. **Worst-Case Behavior**: How different implementations handle edge cases
5. **Optimization Techniques**: What makes some implementations faster than others

## References

- Cormen, T. H., et al. "Introduction to Algorithms" (for algorithm analysis)
- Sedgewick, R., Wayne, K. "Algorithms" (for partitioning schemes)
- Hoare, C. A. R. "Quicksort" (original algorithm description)
- Lomuto, N. "On the Analysis of the Partitioning Step in Quicksort"