# Sorting Algorithm Benchmark

This project compares the performance of three different sorting algorithms by measuring their execution time on arrays of varying sizes and visualizing the results.

## Algorithms Tested

1. **Bubble Sort** - An O(n²) comparison-based algorithm
2. **Merge Sort** - An O(n log n) divide-and-conquer algorithm
3. **Python's built-in sorted()** - Uses Timsort (a hybrid sorting algorithm)

## Requirements

- Python 3.x
- matplotlib (`pip install matplotlib`)

## Running the Benchmark

```bash
python sorting_algorithms.py
```

## Output

The program will:

1. Generate random arrays of sizes: 1k, 10k, 100k, 500k, and 1M elements
2. Measure and display execution time for each algorithm
3. Create a performance comparison plot named `sorting_performance_comparison.png`
4. Show the plot (if running in a graphical environment)

## Results

The plot uses logarithmic scales on both axes to better visualize the performance differences across different array sizes. Each algorithm is represented with a different line style and markers.

- Bubble Sort shows quadratic growth (O(n²))
- Merge Sort shows linearithmic growth (O(n log n))
- Python's sorted() is highly optimized with excellent real-world performance

## Notes

- Bubble Sort is skipped for very large arrays (500k and 1M elements) due to its O(n²) complexity
- The measured times include the complete sorting operation
- Each test uses a fresh random array to ensure fair comparison
