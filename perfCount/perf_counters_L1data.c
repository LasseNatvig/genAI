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
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Number of experiments to run for averaging */
int noExp = 5;

/* ------------------------------------------------------------------ */
/* perf_event_open syscall wrapper                                    */
/* ------------------------------------------------------------------ */
static long perf_event_open(struct perf_event_attr *hw, pid_t pid,
                             int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw, pid, cpu, group_fd, flags);
}

/* ------------------------------------------------------------------ */
/* Counter table                                                      */
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
    { "Branches",      PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS,  -1, 0, 0 },
    { "Branch Misses", PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,        -1, 0, 0 },
};
#define NUM_CTRS  ((int)(sizeof(ctrs) / sizeof(ctrs[0])))

// Counter indexes for readability
#define IDX_CYCLES        0
#define IDX_INSTRUCTIONS  1
#define IDX_CACHE_REFS    2
#define IDX_CACHE_MISSES  3
#define IDX_BRANCHES      4
#define IDX_BRANCH_MISSES 5

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

/* Global variable for CSV file */
FILE *csv_file = NULL;

static void fmtcol(char *buf, size_t sz, int avail, uint64_t val) {
    if (avail)
        snprintf(buf, sz, "%11llu", (unsigned long long)val);
    else
        snprintf(buf, sz, "%11s", "n/a");
}

static void calculate_stats(uint64_t values[], int n, double *avg, double *rel_stddev) {
    if (n <= 0) {
        *avg = 0.0;
        *rel_stddev = 0.0;
        return;
    }
    
    // Calculate average
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += values[i];
    }
    *avg = sum / n;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (int i = 0; i < n; i++) {
        variance += (values[i] - *avg) * (values[i] - *avg);
    }
    variance /= n;
    double stddev = sqrt(variance);
    
    // Calculate relative standard deviation
    *rel_stddev = (*avg != 0.0) ? (stddev / *avg) * 100.0 : 0.0;
}

static void calculate_stats_dbl(double values[], int n, double *avg, double *rel_stddev) {
    if (n <= 0) {
        *avg = 0.0;
        *rel_stddev = 0.0;
        return;
    }
    
    // Calculate average
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += values[i];
    }
    *avg = sum / n;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (int i = 0; i < n; i++) {
        variance += (values[i] - *avg) * (values[i] - *avg);
    }
    variance /= n;
    double stddev = sqrt(variance);
    
    // Calculate relative standard deviation
    *rel_stddev = (*avg != 0.0) ? (stddev / *avg) * 100.0 : 0.0;
}

static double print_statistics(const char *label,
                            uint64_t cycles[], uint64_t instrs[], 
                            uint64_t crefs[], uint64_t cmiss[],
                            uint64_t bmisses[],
                            double times[]) {
    // Calculate statistics
    double cycles_avg, instrs_avg, crefs_avg, cmiss_avg, time_avg;
    double bmisses_avg;
    double cycles_relstd, instrs_relstd, crefs_relstd, cmiss_relstd, time_relstd;
    double bmisses_relstd;
    
    calculate_stats(cycles, noExp, &cycles_avg, &cycles_relstd);
    calculate_stats(instrs, noExp, &instrs_avg, &instrs_relstd);
    calculate_stats(crefs, noExp, &crefs_avg, &crefs_relstd);
    calculate_stats(cmiss, noExp, &cmiss_avg, &cmiss_relstd);
    calculate_stats(bmisses, noExp, &bmisses_avg, &bmisses_relstd);
    calculate_stats_dbl(times, noExp, &time_avg, &time_relstd);
    
    // Calculate derived metrics
    double ipc_avg = (cycles_avg > 0) ? instrs_avg / cycles_avg : 0.0;
    double miss_avg = (crefs_avg > 0) ? (cmiss_avg / crefs_avg) * 100.0 : 0.0;
    double bmiss_per_instr = (instrs_avg > 0) ? bmisses_avg / instrs_avg : 0.0;
    
    // Format values with averages and relative standard deviations in compact form
    // Convert to millions for display
    double cycles_M = cycles_avg / 1e6;
    double instrs_M = instrs_avg / 1e6;
    double crefs_M = crefs_avg / 1e6;
    double cmiss_k = cmiss_avg / 1e3;
    double bmisses_k = bmisses_avg / 1e3;
    
    char cycles_str[32], instrs_str[32], crefs_str[32], cmiss_str[32];
    char bmisses_str[32], time_str[32];
    snprintf(cycles_str, sizeof(cycles_str), "%.1fM(%.1f%%)", cycles_M, cycles_relstd);
    snprintf(instrs_str, sizeof(instrs_str), "%.1fM(%.1f%%)", instrs_M, instrs_relstd);
    snprintf(crefs_str, sizeof(crefs_str), "%.1fM(%.1f%%)", crefs_M, crefs_relstd);
    snprintf(cmiss_str, sizeof(cmiss_str), "%.1fk(%.1f%%)", cmiss_k, cmiss_relstd);
    snprintf(bmisses_str, sizeof(bmisses_str), "%.1fk(%.1f%%)", bmisses_k, bmisses_relstd);
    snprintf(time_str, sizeof(time_str), "%.3fs(%.1f%%)", time_avg, time_relstd);
    
    char ipc_str[16], miss_str[16], bmiss_str[20];
    snprintf(ipc_str, sizeof(ipc_str), "%.2f", ipc_avg);
    snprintf(miss_str, sizeof(miss_str), "%.1f%%", miss_avg);
    snprintf(bmiss_str, sizeof(bmiss_str), "*%.1e", bmiss_per_instr);
    
    // Print to console in compact form
    printf("%20s %12s %12s %12s %12s %12s   IPC=%6s  cacheMiss=%7s  branchMiss=%9s  %s\n",
           label, cycles_str, instrs_str, crefs_str, cmiss_str, bmisses_str,
           ipc_str, miss_str, bmiss_str, time_str);
    
    // Write to CSV file
    if (csv_file) {
        fprintf(csv_file, "%s,%.0f,%.0f,%.0f,%.0f,%.0f,%.2f,%.1f%%,%.1e,%.6f,%.1f%%,%.1f%%,%.1f%%,%.1f%%,%.2f%%\n",
                label,
                cycles_avg, instrs_avg, crefs_avg, cmiss_avg, bmisses_avg,
                ipc_avg, miss_avg, bmiss_per_instr, time_avg,
                cycles_relstd, instrs_relstd, crefs_relstd, cmiss_relstd, bmisses_relstd, time_relstd);
    }
    
    return time_avg;
}

/* Used as an output sink to prevent the compiler eliminating loops. */
volatile uint64_t sink;

void generate_random_array(int arr[], int n);
void copy_array(int dest[], int src[], int n);
void insertion_sort(int arr[], int n);
void bubble_sort(int arr[], int n);
void quicksort(int arr[], int n);

int main(int argc, char *argv[]) {
    printf("ARM Cortex-A72 PMU Demo - Sorting Performance\n");

    // Parse command line arguments
    int max_array_size = 20000;
    int start_array_size = 10000;
    int step_size = 1000;
    
    if (argc >= 2) {
        max_array_size = atoi(argv[1]);
    }
    if (argc >= 3) {
        start_array_size = atoi(argv[2]);
    }
    if (argc >= 4) {
        step_size = atoi(argv[3]);
    }
    if (argc >= 5) {
        noExp = atoi(argv[4]);
    }
    printf("Running with start=%d, max=%d, step=%d, experiments: %d\n", 
           start_array_size, max_array_size, step_size, noExp);

    if (init_counters() != 0)
        return 1;

    // Create 'res' directory if it doesn't exist
    if (mkdir("res", 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Failed to create 'res' directory\n");
        return 1;
    }

    // Generate timestamped CSV filename
    char csv_filename[128];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(csv_filename, sizeof(csv_filename), "res/sorting_perf_%Y%m%d_%H%M%S.csv", tm_info);
    
    // Open CSV file for writing
    csv_file = fopen(csv_filename, "w");
    if (!csv_file) {
        fprintf(stderr, "Failed to open CSV file: %s\n", csv_filename);
        return 1;
    }
    
    // Write CSV header
    fprintf(csv_file, "Workload,Cycles,Instructions,CacheRefs,CacheMiss,BranchMisses,IPC,CacheMiss%%,BranchMissPerInstr,Time(s),Cycles_RSD,Instrs_RSD,CacheRefs_RSD,CacheMiss_RSD,BranchMisses_RSD,Time_RSD%%\n");

    // Initialize random seed
    srand(123);  // ensure that it always start on the same seed (reproducibility)

    // Variables to track execution time for the max problem size
    double time_insertion_max = 0.0;
    double time_bubble_max = 0.0;
    double time_quick_max = 0.0;

    // Print column headers with descriptions
    printf("\nPerformance Counter Results:\n");
    printf("  Sorting Algorithm Performance Metrics\n");
    printf("  =====================================\n");
    
    // Warn about any counters not available on this CPU
    for (int i = 0; i < NUM_CTRS; i++)
        if (!ctrs[i].avail)
            printf("  (note: '%s' not available on this CPU)\n", ctrs[i].name);
    
    printf("\n");
    printf("  Metric Descriptions:\n");
    printf("    Cycles      = CPU clock cycles (in millions)\n");
    printf("    Instructions = Retired instructions (in millions)\n");
    printf("    CacheRefs    = Cache references (in millions)\n");
    printf("    CacheMiss   = Cache misses (in thousands)\n");
    printf("    BranchMiss  = Branch instruction misses (in thousands)\n");
    printf("    IPC         = Instructions Per Cycle (avg)\n");
    printf("    cacheMiss   = Cache miss rate (percentage)\n");
    printf("    branchMiss* = Branch misses per instruction (*not usual branch miss rate)\n");
    printf("    Time        = Execution time (in seconds)\n");
    printf("\n  Note: Values shown as Average(RelativeStdDev%%)\n");
    printf("  -----------------------------------------------\n");
    printf("  %20s %12s %12s %12s %12s %12s        %s\n",
           "Workload", "Cycles", "Instructions", "CacheRefs", "CacheMiss", "BranchMiss", "Metrics");

    // Test Insertion Sort for sizes start_array_size to max_array_size in steps of step_size
    
    for (int n = start_array_size; n <= max_array_size; n += step_size) {
        // Arrays to store results for averaging
        uint64_t cycles[noExp], instrs[noExp], crefs[noExp], cmiss[noExp];
        uint64_t bmisses[noExp];
        double times[noExp];
        
        for (int exp = 0; exp < noExp; exp++) {
            int *arr = malloc(n * sizeof(int));
            if (!arr) {
                fprintf(stderr, "Failed to allocate memory\n");
                return 1;
            }
            generate_random_array(arr, n);
            
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            counters_start();
            insertion_sort(arr, n);
            counters_stop();
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            times[exp] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            cycles[exp] = ctrs[IDX_CYCLES].value;
            instrs[exp] = ctrs[IDX_INSTRUCTIONS].value;
            crefs[exp] = ctrs[IDX_CACHE_REFS].value;
            cmiss[exp] = ctrs[IDX_CACHE_MISSES].value;
            bmisses[exp] = ctrs[IDX_BRANCH_MISSES].value;
            free(arr);
        }
        
        char label[21];
        snprintf(label, sizeof(label), "   InsSort (n=%d)", n);
        label[20] = '\0';
        double avg_time = print_statistics(label, cycles, instrs, crefs, cmiss, bmisses, times);
        if (n == max_array_size) {
            time_insertion_max = avg_time;
        }
    }

    // Test Bubble Sort for sizes start_array_size to max_array_size in steps of step_size
    for (int n = start_array_size; n <= max_array_size; n += step_size) {
        // Arrays to store results for averaging
        uint64_t cycles[noExp], instrs[noExp], crefs[noExp], cmiss[noExp];
        uint64_t bmisses[noExp];
        double times[noExp];
        
        for (int exp = 0; exp < noExp; exp++) {
            int *arr = malloc(n * sizeof(int));
            if (!arr) {
                fprintf(stderr, "Failed to allocate memory\n");
                return 1;
            }
            generate_random_array(arr, n);
            
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            counters_start();
            bubble_sort(arr, n);
            counters_stop();
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            times[exp] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            cycles[exp] = ctrs[IDX_CYCLES].value;
            instrs[exp] = ctrs[IDX_INSTRUCTIONS].value;
            crefs[exp] = ctrs[IDX_CACHE_REFS].value;
            cmiss[exp] = ctrs[IDX_CACHE_MISSES].value;
            bmisses[exp] = ctrs[IDX_BRANCH_MISSES].value;
            free(arr);
        }
        
        char label[21];
        snprintf(label, sizeof(label), "   BubSort (n=%d)", n);
        label[20] = '\0';
        double avg_time = print_statistics(label, cycles, instrs, crefs, cmiss, bmisses, times);
        if (n == max_array_size) {
            time_bubble_max = avg_time;
        }
    }

    // Test Quick Sort for sizes start_array_size to max_array_size in steps of step_size
    for (int n = start_array_size; n <= max_array_size; n += step_size) {
        // Arrays to store results for averaging
        uint64_t cycles[noExp], instrs[noExp], crefs[noExp], cmiss[noExp];
        uint64_t bmisses[noExp];
        double times[noExp];
        
        for (int exp = 0; exp < noExp; exp++) {
            int *arr = malloc(n * sizeof(int));
            if (!arr) {
                fprintf(stderr, "Failed to allocate memory\n");
                return 1;
            }
            generate_random_array(arr, n);
            
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            counters_start();
            quicksort(arr, n);
            counters_stop();
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            times[exp] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            cycles[exp] = ctrs[IDX_CYCLES].value;
            instrs[exp] = ctrs[IDX_INSTRUCTIONS].value;
            crefs[exp] = ctrs[IDX_CACHE_REFS].value;
            cmiss[exp] = ctrs[IDX_CACHE_MISSES].value;
            bmisses[exp] = ctrs[IDX_BRANCH_MISSES].value;
            free(arr);
        }
        
        char label[21];
        snprintf(label, sizeof(label), "   QckSort (n=%d)", n);
        label[20] = '\0';
        double avg_time = print_statistics(label, cycles, instrs, crefs, cmiss, bmisses, times);
        if (n == max_array_size) {
            time_quick_max = avg_time;
        }
    }

    // Print summary of execution times for max array size
    printf("\n--- Average Execution Time for n=%d (over %d experiments) ---\n", max_array_size, noExp);
    printf("Insertion Sort:  %.3fs\n", time_insertion_max);
    printf("Bubble Sort:     %.3fs\n", time_bubble_max);
    printf("Quick Sort:      %.3fs\n", time_quick_max);
    
    // Print CSV filename
    printf("\nResults saved to: %s\n", csv_filename);

    // Close CSV file
    if (csv_file) {
        fclose(csv_file);
        csv_file = NULL;
    }

    close_counters();
    return 0;
}
