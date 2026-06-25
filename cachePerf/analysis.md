# Cache Performance Analysis: Sequential vs Random Access

Initially written by cline w/mistral-medium @idun-NTNU, revised and commented in boldface by Lasse.

## Raspberry Pi4B 32-bit Cache Characteristics

- L1 Data Cache: ~32KB per core
- L2 Cache: ~1MB (shared)
- Int size: 4 bytes
- **All confirmed OK**

## Key Findings

### Sequential Access

- Performance remains nearly constant for small arrays (fits in cache).
- Slight increase when array exceeds L1 cache size (~32KB). **Not seen in graph. Action is to use log scale y-axis**. Done
- More significant increase when exceeding L2 cache size (~1MB). **Not so visible as expected. Action is to increase array size more slowly on both sides of the cache size thresholds**
    * plan from cline looked OK, but updated plot_results.py crashed. Debugging **TODO**...
    * Maybe also split into four plots, one overall with only x-axis as log scale, one overalle x and y log-scale, and one for each cache size border without log-scales?
- Still relatively efficient due to prefetching and spatial locality.

### Random Access

- Performance degrades significantly as array size increases.
- Sharp performance cliff when array exceeds L1 cache size (~32KB). **Not seen, same comment as above**
- Another significant drop when exceeding L2 cache size (~1MB). **Same comment as above**
- Random access pattern causes cache thrashing with poor temporal locality.

### Performance Ratio

- For small arrays (<32KB): Random access ~2-3x slower than sequential. **Maybe, ask for tabular summary**
- For arrays >1MB: Random access can be 10-100x slower than sequential. **same**

## Explanation

Sequential access benefits from:

1. Spatial locality (accessing nearby memory elements).
2. Hardware prefetching.
3. Cache line utilization (64 bytes per cache line).

Random access suffers from:

1. Poor temporal locality (reusing data irregularly).
2. Cache line underutilization (only one element used per cache line).
3. Frequent cache misses requiring main memory access.
