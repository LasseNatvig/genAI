#define _GNU_SOURCE   /* expose syscall(), pid_t under -std=c11 */

/* perf_counters.c — Hardware Performance Counter Demo (Raspberry Pi 4B)
 *
 * Uses perf_event_open() to read ARM Cortex-A72 PMU counters.
 * Runs four workloads that contrast:
 *   - Cache-friendly (sequential) vs cache-thrashing (large stride)
 *   - Branch-predictable (always taken) vs unpredictable (~50/50)
 *
 * The Cortex-A72 PMU provides 1 fixed cycle counter + 6 programmable
 * event counters, which is enough for all six PERF_TYPE_HARDWARE events.
 *
 * Build:  make          (or: gcc -O0 -Wall -o perf_counters perf_counters.c)
 * Run:    ./perf_counters
 *
 * If you see "Operation not permitted", lower the paranoia level:
 *   echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
 */

#include <errno.h>
#include <linux/perf_event.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Array of 4 M ints = 16 MB — comfortably exceeds the 1 MB L2 on the A72 */
#define ARRAY_ELEMS  (1 << 20)
#define ITERATIONS   4

/* ------------------------------------------------------------------ */
/* perf_event_open syscall wrapper                                     */
/* ------------------------------------------------------------------ */
static long perf_event_open(struct perf_event_attr *hw, pid_t pid,
                             int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw, pid, cpu, group_fd, flags);
}

/* ------------------------------------------------------------------ */
/* Counter table                                                       */
/* ------------------------------------------------------------------ */
typedef struct {
    const char *name;
    uint32_t    type;
    uint64_t    config;
    int         fd;        /* -1 = not available on this CPU */
    int         avail;
    uint64_t    value;
} Counter;

static Counter ctrs[] = {
    { "Cycles",        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES,          -1, 0, 0 },
    { "Instructions",  PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS,        -1, 0, 0 },
    { "Cache Refs",    PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES,    -1, 0, 0 },
    { "Cache Misses",  PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES,        -1, 0, 0 },
    { "Branches",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS, -1, 0, 0 },
    { "Branch Misses", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,       -1, 0, 0 },
};
#define NUM_CTRS  ((int)(sizeof(ctrs) / sizeof(ctrs[0])))

/* ------------------------------------------------------------------ */
/* Counter lifecycle helpers                                           */
/* ------------------------------------------------------------------ */
static int init_counters(void)
{
    int any_open = 0;
    for (int i = 0; i < NUM_CTRS; i++) {
        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type           = ctrs[i].type;
        pe.size           = sizeof(pe);
        pe.config         = ctrs[i].config;
        pe.disabled       = 1;
        pe.exclude_kernel = 1;   /* count user-space only */
        pe.exclude_hv     = 1;

        ctrs[i].fd = (int)perf_event_open(&pe, 0 /* this process */,
                                          -1 /* any CPU */,
                                          -1 /* no group */, 0);
        if (ctrs[i].fd < 0) {
            if (errno == EACCES || errno == EPERM) {
                fprintf(stderr, "perf_event_open '%s': %s\n",
                        ctrs[i].name, strerror(errno));
                fprintf(stderr,
                    "  -> Try:  echo -1 | sudo tee "
                    "/proc/sys/kernel/perf_event_paranoid\n");
                return -1;   /* permission error: fatal */
            }
            /* ENOENT / EOPNOTSUPP: event not supported on this CPU — skip */
            ctrs[i].avail = 0;
        } else {
            ctrs[i].avail = 1;
            any_open++;
        }
    }
    if (!any_open) {
        fprintf(stderr, "No hardware counters available on this CPU.\n");
        return -1;
    }
    return 0;
}

static void close_counters(void)
{
    for (int i = 0; i < NUM_CTRS; i++)
        if (ctrs[i].fd >= 0) close(ctrs[i].fd);
}

static void counters_start(void)
{
    for (int i = 0; i < NUM_CTRS; i++) {
        if (!ctrs[i].avail) continue;
        ioctl(ctrs[i].fd, PERF_EVENT_IOC_RESET,  0);
        ioctl(ctrs[i].fd, PERF_EVENT_IOC_ENABLE, 0);
    }
}

static void counters_stop(void)
{
    for (int i = 0; i < NUM_CTRS; i++) {
        if (!ctrs[i].avail) { ctrs[i].value = 0; continue; }
        ioctl(ctrs[i].fd, PERF_EVENT_IOC_DISABLE, 0);
        read(ctrs[i].fd, &ctrs[i].value, sizeof(uint64_t));
    }
}

/* ------------------------------------------------------------------ */
/* Output                                                              */
/* ------------------------------------------------------------------ */
static void fmtcol(char *buf, size_t sz, int avail, uint64_t val)
{
    if (avail)
        snprintf(buf, sz, "%11llu", (unsigned long long)val);
    else
        snprintf(buf, sz, "%11s", "n/a");
}

static void print_header(void)
{
    printf("\n%-26s %13s %13s %11s %11s %11s %11s   %s\n",
           "Workload",
           "Cycles", "Instructions",
           "CacheRefs", "CacheMiss",
           "Branches", "BrMiss",
           "Derived");
    printf("%-26s %13s %13s %11s %11s %11s %11s   %s\n",
           "--------------------------",
           "-------------", "-------------",
           "-----------", "-----------",
           "-----------", "-----------",
           "--------------------------------------");
    /* Warn about any counters not available on this CPU */
    for (int i = 0; i < NUM_CTRS; i++)
        if (!ctrs[i].avail)
            printf("  (note: '%s' not available on this CPU)\n", ctrs[i].name);
}

static void print_row(const char *label)
{
    uint64_t cycles = ctrs[0].value;
    uint64_t instrs = ctrs[1].value;
    uint64_t crefs  = ctrs[2].value;
    uint64_t cmiss  = ctrs[3].value;
    uint64_t brins  = ctrs[4].value;
    uint64_t brmis  = ctrs[5].value;

    char c0[16], c1[16], c2[16], c3[16], c4[16], c5[16];
    fmtcol(c0, sizeof(c0), ctrs[0].avail, cycles);
    fmtcol(c1, sizeof(c1), ctrs[1].avail, instrs);
    fmtcol(c2, sizeof(c2), ctrs[2].avail, crefs);
    fmtcol(c3, sizeof(c3), ctrs[3].avail, cmiss);
    fmtcol(c4, sizeof(c4), ctrs[4].avail, brins);
    fmtcol(c5, sizeof(c5), ctrs[5].avail, brmis);

    /* Derived metrics — only when both operands are available */
    char ipc_s[16]   = "n/a";
    char miss_s[16]  = "n/a";
    char brmis_s[16] = "n/a";
    if (ctrs[0].avail && ctrs[1].avail && cycles)
        snprintf(ipc_s,   sizeof(ipc_s),   "%.2f",
                 (double)instrs / (double)cycles);
    if (ctrs[2].avail && ctrs[3].avail && crefs)
        snprintf(miss_s,  sizeof(miss_s),  "%.1f%%",
                 100.0 * (double)cmiss / (double)crefs);
    if (ctrs[4].avail && ctrs[5].avail && brins)
        snprintf(brmis_s, sizeof(brmis_s), "%.1f%%",
                 100.0 * (double)brmis / (double)brins);

    printf("%-26s %13s %13s %11s %11s %11s %11s"
           "   IPC=%-6s  miss=%-7s  brmiss=%s\n",
           label, c0, c1, c2, c3, c4, c5,
           ipc_s, miss_s, brmis_s);
}

/* ------------------------------------------------------------------ */
/* Workloads                                                           */
/* ------------------------------------------------------------------ */

/* Used as an output sink to prevent the compiler eliminating loops. */
volatile uint64_t sink;

/* 1. Sequential read — stride-1, hardware prefetcher works perfectly */
static void wl_sequential(const int *arr, int n)
{
    uint64_t s = 0;
    for (int it = 0; it < ITERATIONS; it++)
        for (int i = 0; i < n; i++)
            s += arr[i];
    sink = s;
}

/* 2. Strided read — 256-byte hop between accesses, thrashes cache lines */
static void wl_strided(const int *arr, int n)
{
    const int stride = 64;   /* 64 ints × 4 bytes = 256-byte stride */
    uint64_t s = 0;
    int count = n / stride;
    for (int it = 0; it < ITERATIONS; it++)
        for (int i = 0; i < count; i++)
            s += arr[i * stride];
    sink = s;
}

/* 3. Predictable branch — condition is always true (arr[i] >= 0) */
static void wl_predictable(const int *arr, int n)
{
    uint64_t s = 0;
    for (int it = 0; it < ITERATIONS; it++)
        for (int i = 0; i < n; i++)
            if (arr[i] >= 0) s += arr[i];   /* always taken */
    sink = s;
}

/* 4. Unpredictable branch — alternates ~50/50 on odd/even values */
static void wl_unpredictable(const int *arr, int n)
{
    uint64_t s = 0;
    for (int it = 0; it < ITERATIONS; it++)
        for (int i = 0; i < n; i++)
            if (arr[i] & 1) s += arr[i];    /* ~50% taken */
    sink = s;
}

/* ------------------------------------------------------------------ */
/* main                                                                */
/* ------------------------------------------------------------------ */
int main(void)
{
    /* Show paranoia level so the user knows what to expect */
    {
        FILE *f = fopen("/proc/sys/kernel/perf_event_paranoid", "r");
        if (f) {
            int p = 0;
            fscanf(f, "%d", &p);
            fclose(f);
            printf("perf_event_paranoid = %d", p);
            if (p > 1)
                printf("  (WARNING: > 1 may block hw counters;"
                       " needs <= 1 or root)");
            printf("\n");
        }
    }
    printf("ARM Cortex-A72 PMU Demo  —  %d × %d-element array (%d MB), "
           "%d iterations\n",
           ITERATIONS, ARRAY_ELEMS,
           (int)((size_t)ARRAY_ELEMS * sizeof(int) / (1024 * 1024)),
           ITERATIONS);

    if (init_counters() != 0)
        return 1;

    /* Fill array: values 0 .. N-1  (all >= 0,  ~50% odd) */
    int *arr = malloc((size_t)ARRAY_ELEMS * sizeof(int));
    if (!arr) {
        perror("malloc");
        close_counters();
        return 1;
    }
    for (int i = 0; i < ARRAY_ELEMS; i++)
        arr[i] = i;

    print_header();

    counters_start(); wl_sequential  (arr, ARRAY_ELEMS); counters_stop();
    print_row("Sequential (stride=1)");

    counters_start(); wl_strided     (arr, ARRAY_ELEMS); counters_stop();
    print_row("Strided (stride=64)");

    counters_start(); wl_predictable (arr, ARRAY_ELEMS); counters_stop();
    print_row("Predictable branch");

    counters_start(); wl_unpredictable(arr, ARRAY_ELEMS); counters_stop();
    print_row("Unpredictable branch");

    printf("\nWhat to observe:\n");
    printf("  Cache Refs / Misses  — Strided miss%% should be >> Sequential miss%%\n");
    printf("  BrMiss               — Unpredictable should be >> Predictable\n");
    printf("  IPC                  — Higher = more work done per clock cycle\n\n");

    free(arr);
    close_counters();
    return 0;
}
