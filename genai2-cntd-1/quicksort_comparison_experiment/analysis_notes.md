# Quicksort Implementation Comparison Analysis

## Overview

This experiment compares three different Quicksort implementations to understand their performance characteristics:

1. **Lomuto Partition (Original)** - The classic implementation
2. **Hoare Partition** - More efficient partition scheme  
3. **Library qsort** - Standard library implementation for reference

## Implementation Details

### Lomuto Partition Quicksort

**Algorithm:**
- Uses the last element as the pivot
- Single pointer `i` that tracks the position where elements less than pivot should go
- Iterates through array with pointer `j`, swapping elements when `arr[j] < pivot`

**Pros:**
- Simpler to understand and implement
- Consistent partitioning behavior

**Cons:**
- More swaps than Hoare partition (approximately 3x more swaps)
- Makes more comparisons when there are duplicate elements
- Less efficient on average

**Characteristics:**
- Average comparisons: ~2n ln n
- Average swaps: ~n ln n
- Worst case: O(n²) when array is already sorted

### Hoare Partition Quicksort

**Algorithm:**
- Uses the first element as the pivot
- Two pointers starting from both ends, moving toward each other
- Swaps elements when both pointers find elements that are on the wrong side of the pivot
- Continues until pointers cross

**Pros:**
- Approximately 3x fewer swaps than Lomuto partition
- Better performance on arrays with duplicates
- Generally faster in practice
- More efficient cache usage

**Cons:**
- Slightly more complex to implement
- Partition point is not as intuitive

**Characteristics:**
- Average comparisons: ~2n ln n
- Average swaps: ~1/3 n ln n (much better than Lomuto)
- Worst case: O(n²) but less likely

### Library qsort

**Algorithm:**
- Implementation-specific (typically uses optimized quicksort)
- May use introsort (hybrid of quicksort, heapsort, and insertion sort)
- Often includes optimizations like:
  - Insertion sort for small partitions
  - Median-of-three pivot selection
  - Three-way partitioning for duplicates

**Pros:**
- Highly optimized
- Handles edge cases well
- Often includes additional optimizations

**Cons:**
- Implementation details are hidden
- May use different strategies on different platforms

## Performance Predictions

### Random Arrays
- **Lomuto vs Hoare**: Hoare should be faster due to fewer swaps
- **Both vs qsort**: qsort should be competitive or better due to optimizations

### Already Sorted Arrays
- **Lomuto**: Worst case O(n²) performance - very slow
- **Hoare**: Still O(n²) but faster than Lomuto due to fewer swaps
- **qsort**: Likely has optimizations to detect and handle sorted arrays better

### Reverse Sorted Arrays
- **Lomuto**: Worst case O(n²) performance - very slow
- **Hoare**: Better than Lomuto, but still O(n²)
- **qsort**: Should handle this well with pivot selection optimizations

### Arrays with Duplicates
- **Lomuto**: Poor performance due to many unnecessary comparisons
- **Hoare**: Better performance but still not optimal
- **qsort**: Likely uses three-way partitioning which is optimal for duplicates

## Metrics Collected

1. **Execution Time** - Wall-clock time in seconds
2. **CPU Cycles** - Processor cycles (hardware-dependent)
3. **Comparisons** - Number of element comparisons
4. **Swaps** - Number of element swaps

## Expected Results

1. **Hoare should consistently outperform Lomuto** due to fewer swaps
2. **qsort should be the fastest** due to additional optimizations
3. **Lomuto should perform worst on sorted/reverse-sorted arrays** due to O(n²) behavior
4. **Performance differences should increase with array size**

## Code Analysis

The key differences in the implementation are:

### Lomuto Partition:
```c
int lomuto_partition(int arr[], int low, int high) {
    int pivot = arr[high];  // Pivot is last element
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        comparison_count++;
        if (arr[j] < pivot) {
            i++;
            swap_count++;
            // swap arr[i] and arr[j]
        }
    }
    swap_count++;
    // swap arr[i+1] and arr[high] to put pivot in place
    return i + 1;
}
```

### Hoare Partition:
```c
int hoare_partition(int arr[], int low, int high) {
    int pivot = arr[low];  // Pivot is first element
    int i = low - 1;
    int j = high + 1;
    while (1) {
        do { i++; comparison_count++; } while (arr[i] < pivot);
        do { j--; comparison_count++; } while (arr[j] > pivot);
        comparison_count++;
        if (i >= j) return j;
        swap_count++;
        // swap arr[i] and arr[j]
    }
}
```

**Key Differences:**
1. **Pivot Selection**: Lomuto uses last element, Hoare uses first
2. **Pointer Movement**: Lomuto uses one pointer, Hoare uses two pointers moving toward each other
3. **Swap Frequency**: Hoare swaps once per iteration, Lomuto may do many swaps
4. **Partition Result**: Lomuto puts pivot in final position, Hoare doesn't necessarily

## Optimization Opportunities

The current implementations could be enhanced with:

1. **Insertion Sort for Small Partitions** - For partitions smaller than a threshold (e.g., 10-20 elements), use insertion sort
2. **Median-of-Three Pivot Selection** - Choose pivot as median of first, middle, and last elements
3. **Randomized Pivot Selection** - Avoid worst-case scenarios
4. **Three-Way Partitioning** - Better handle arrays with many duplicates
5. **Tail Recursion Elimination** - Reduce stack usage
6. **Iterative Implementation** - Avoid recursion overhead completely

This experiment provides the foundation for understanding why these optimizations are beneficial and how they affect performance.