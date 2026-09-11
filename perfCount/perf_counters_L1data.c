#define _GNU_SOURCE   /* expose syscall(), pid_t under -std=c11 */

/* Initial code written by github copilot, early summer 2026. Further edited by Lasse Natvig for teaching purposes
 * perf_counters.c — Hardware Performance Counter Demo (Raspberry Pi 4B)
 *
 * Uses perf_event_open() to read ARM Cortex-A72 PMU counters.
 * The Cortex-A72 PMU provides 1 fixed cycle counter + 6 programmable
 * event counters, which is enough for all six PERF_TYPE_HARDWARE events. I
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
static int init_counters(void) {
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
static void fmtcol(char *buf, size_t sz, int avail, uint64_t val) {
    if (avail)
        snprintf(buf, sz, "%11llu", (unsigned long long)val);
    else
        snprintf(buf, sz, "%11s", "n/a");
}

static void print_header(void) {
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

static void print_row(const char *label) {
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

/* Used as an output sink to prevent the compiler eliminating loops. */
volatile uint64_t sink;

int main(void) {
    printf("ARM Cortex-A72 PMU Demo\n");

    if (init_counters() != 0)
        return 1;

    print_header();

    counters_start();
    for (int i = 0; i < 1000; i++) {
        int b = i;
        b = b +2;
        b = b % 56;
    }
    counters_stop();
    print_row("Loop 1000 iterations");

    counters_start();
    for (int i = 0; i < 5000; i++) {
        int b = i;
        b = b +2;
        b = b % 56;
    }
    counters_stop();
    print_row("Loop 5000 iterations");

    close_counters();
    return 0;
}
