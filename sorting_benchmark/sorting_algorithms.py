#!/usr/bin/env python3
"""
Sorting Algorithm Benchmark
Compares execution time of three sorting algorithms:
- Bubble Sort (O(n²))
- Merge Sort (O(n log n))
- Python's built-in sorted() (Timsort)
"""

import time
import random
import matplotlib.pyplot as plt

def bubble_sort(arr):
    """Bubble Sort implementation - O(n²) time complexity"""
    n = len(arr)
    for i in range(n):
        for j in range(0, n-i-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]

def merge_sort(arr):
    """Merge Sort implementation - O(n log n) time complexity"""
    if len(arr) > 1:
        mid = len(arr) // 2
        left = arr[:mid]
        right = arr[mid:]

        merge_sort(left)
        merge_sort(right)

        i = j = k = 0

        while i < len(left) and j < len(right):
            if left[i] < right[j]:
                arr[k] = left[i]
                i += 1
            else:
                arr[k] = right[j]
                j += 1
            k += 1

        while i < len(left):
            arr[k] = left[i]
            i += 1
            k += 1

        while j < len(right):
            arr[k] = right[j]
            j += 1
            k += 1

def measure_sorting_time(sort_func, arr, func_name):
    """Measure execution time of a sorting function"""
    # Create a copy of the array to avoid modifying the original
    arr_copy = arr.copy()
    start_time = time.perf_counter()
    sort_func(arr_copy)
    end_time = time.perf_counter()
    return end_time - start_time

def generate_test_data(size):
    """Generate random list of integers for testing"""
    return [random.randint(0, 1000000) for _ in range(size)]

def main():
    # Test data sizes
    sizes = [1000, 10000, 100000, 500000, 1000000]

    # Store timing results
    results = {
        'Bubble Sort': [],
        'Merge Sort': [],
        'Python sorted()': []
    }

    print("Running sorting algorithm benchmark...")
    print("=" * 50)

    for size in sizes:
        print(f"Testing with {size:,} elements...")

        # Generate test data
        test_data = generate_test_data(size)

        # Test Bubble Sort (limit to smaller sizes due to O(n²) complexity)
        if size <= 100000:  # Skip bubble sort for very large arrays
            bubble_time = measure_sorting_time(bubble_sort, test_data, "Bubble Sort")
            results['Bubble Sort'].append(bubble_time)
            print(f"  Bubble Sort: {bubble_time:.6f} seconds")
        else:
            print(f"  Bubble Sort: Skipped (too large for O(n²) algorithm)")
            results['Bubble Sort'].append(float('nan'))

        # Test Merge Sort
        merge_time = measure_sorting_time(merge_sort, test_data, "Merge Sort")
        results['Merge Sort'].append(merge_time)
        print(f"  Merge Sort: {merge_time:.6f} seconds")

        # Test Python's built-in sorted()
        sorted_time = measure_sorting_time(sorted, test_data, "Python sorted()")
        results['Python sorted()'].append(sorted_time)
        print(f"  Python sorted(): {sorted_time:.6f} seconds")

        print()

    # Create plot
    plt.figure(figsize=(12, 8))
    plt.title('Sorting Algorithm Performance Comparison', fontsize=16)
    plt.xlabel('Array Size', fontsize=14)
    plt.ylabel('Execution Time (seconds)', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xscale('log')  # Use log scale for better visualization
    plt.yscale('log')  # Use log scale for time

    # Plot each algorithm
    line_styles = ['--', '-.', '-']
    for i, (algorithm, times) in enumerate(results.items()):
        plt.plot(sizes, times, marker='o', linestyle=line_styles[i], markersize=8, label=algorithm, linewidth=2)

    plt.legend(fontsize=12)
    plt.tight_layout()

    # Save plot as image
    plot_filename = 'sorting_performance_comparison.png'
    plt.savefig(plot_filename)
    print(f"Plot saved as '{plot_filename}'")

    # Show plot (optional - comment out if running in headless environment)
    try:
        plt.show()
    except Exception as e:
        print(f"Could not display plot (may be running headless): {e}")

    print("\n" + "=" * 50)
    print("Benchmark completed successfully!")
    print(f"\nResults summary:")
    for algorithm, times in results.items():
        valid_times = [t for t in times if not (isinstance(t, float) and str(t) == 'nan')]
        if valid_times:
            avg_time = sum(valid_times) / len(valid_times)
            print(f"{algorithm}: {len(valid_times)} tests, avg time: {avg_time:.6f}s")
        else:
            print(f"{algorithm}: No valid test results")

if __name__ == "__main__":
    main()