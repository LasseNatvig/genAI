# Cache Performance Analysis: Sequential vs Random Access

## Raspberry Pi4B 32-bit Cache Characteristics

- L1 Data Cache: ~32KB per core
- L2 Cache: ~1MB (shared)
- Int size: 4 bytes

## Key Findings

### Sequential Access

- Performance remains nearly constant for small arrays (fits in cache).
- Slight increase when array exceeds L1 cache size (~32KB).
- More significant increase when exceeding L2 cache size (~1MB).
- Still relatively efficient due to prefetching and spatial locality.

### Random Access

- Performance degrades significantly as array size increases.
- Sharp performance cliff when array exceeds L1 cache size (~32KB).
- Another significant drop when exceeding L2 cache size (~1MB).
- Random access pattern causes cache thrashing with poor temporal locality.

### Performance Ratio

- For small arrays (<32KB): Random access ~2-3x slower than sequential.
- For arrays >1MB: Random access can be 10-100x slower than sequential.

## Explanation

Sequential access benefits from:

1. Spatial locality (accessing nearby memory elements).
2. Hardware prefetching.
3. Cache line utilization (64 bytes per cache line).

Random access suffers from:

1. Poor temporal locality (reusing data irregularly).
2. Cache line underutilization (only one element used per cache line).
3. Frequent cache misses requiring main memory access.
