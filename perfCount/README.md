# Hardware Performance Counter Demo — Raspberry Pi 4B

Demonstrates reading ARM Cortex-A72 PMU (Performance Monitoring Unit) hardware
counters from user-space using the Linux `perf_event_open()` syscall.
No external tools required — the syscall is part of the kernel.

## Build & Run

```bash
make
./perf_counters
```

If you see *Operation not permitted*:

```bash
echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
```

## Counters

The Cortex-A72 PMU provides 1 fixed cycle counter and 6 programmable event
counters. This program opens all six standard `PERF_TYPE_HARDWARE` events:

| Counter | Description |
|---|---|
| **Cycles** | CPU clock cycles elapsed |
| **Instructions** | Instructions retired |
| **Cache Refs** | L1 data cache accesses (reads + writes) |
| **Cache Misses** | L1 data cache misses that went to L2/RAM |
| **Branches** | Branch instructions executed |
| **Branch Misses** | Branches mispredicted by the branch predictor |

Derived metrics printed per workload:

- **IPC** (Instructions Per Cycle) — higher is better; drops when the CPU stalls waiting on memory.
- **miss%** — cache miss rate (`Cache Misses / Cache Refs`); high values mean the working set exceeds the cache.
- **brmiss%** — branch misprediction rate (`Branch Misses / Branches`); high values hurt the instruction pipeline.

## Workloads

Four synthetic workloads are measured to contrast hardware behaviours:

| Workload | What it demonstrates |
|---|---|
| **Sequential read** | Stride-1 array scan; the hardware prefetcher runs ahead of the CPU, so cache misses are near zero and IPC is high. |
| **Strided read (stride=64)** | Accesses every 64th element (256-byte hops), defeating the prefetcher and causing frequent cache misses. IPC drops sharply. |
| **Predictable branch** | `if (arr[i] >= 0)` is always true; the branch predictor learns the pattern immediately and mispredictions stay near zero. |
| **Unpredictable branch** | `if (arr[i] & 1)` alternates true/false on consecutive elements; the branch predictor is wrong ~50% of the time. |

The array contains 4 M `int` values (16 MB), which exceeds the 1 MB L2 cache
of the Cortex-A72, ensuring the strided workload reliably spills to DRAM.

## Hardware Notes

- **SoC:** Broadcom BCM2711 (Raspberry Pi 4B)
- **Core:** ARM Cortex-A72, ARMv8-A
- **L1D cache:** 32 KB per core
- **L2 cache:** 1 MB unified (shared across all 4 cores)
- **PMU:** 1 cycle counter + 6 programmable counters per core

The program gracefully skips any counter that is not supported on the current
CPU (e.g. when running inside a VM with a restricted PMU).
