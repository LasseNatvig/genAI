#!/usr/bin/env python3
import matplotlib.pyplot as plt
import subprocess
import numpy as np
import os

def run_benchmark():
    """Run the C benchmark and capture output"""
    print("Running cache benchmark...")
    result = subprocess.run(["./cache_demo"], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running benchmark: {result.stderr}")
        return None

    # Parse output: size seq_time rand_time (skip non-numeric lines)
    data = []
    for line in result.stdout.strip().split('\n'):
        if line and not line.startswith('size of int'):
            parts = line.split()
            if len(parts) == 3:
                data.append((int(parts[0]), float(parts[1]), float(parts[2])))

    return np.array(data)

def load_data():
    """Load data from file - now with multiple samples per size"""
    if not os.path.exists('results.txt'):
        # Run benchmark and save results
        result = subprocess.run(["./cache_demo"], capture_output=True, text=True)
        with open('results.txt', 'w') as f:
            f.write(result.stdout)

    # Load data from file - group by size, creating arrays of samples
    data = []
    with open('results.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('size of int'):
                try:
                    parts = line.split()
                    if len(parts) == 3:
                        data.append((float(parts[0]), float(parts[1]), float(parts[2])))
                except ValueError:
                    continue

    data = np.array(data)
    sizes = data[:, 0] / 1024  # Convert to KB
    seq_times = data[:, 1]
    rand_times = data[:, 2]
    return sizes, seq_times, rand_times

def load_summary():
    """Load min values per size from summary.txt"""
    data = []
    with open('summary.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('size_kb') or not line:
                continue
            parts = line.split()
            if len(parts) == 7:
                data.append((float(parts[0]), float(parts[1]), float(parts[4])))
    data = np.array(data)
    sizes    = data[:, 0]  # already in KB
    seq_mins = data[:, 1]
    rand_mins = data[:, 2]
    return sizes, seq_mins, rand_mins

def create_plot_1_x_log_only(sizes, seq_times, rand_times):
    """Plot with only x-axis as log scale"""
    plt.figure(figsize=(10, 6))

    # Plot sequential access
    plt.plot(sizes, seq_times, 'b-', linewidth=2, label='Sequential Access')
    plt.plot(sizes, seq_times, 'bo', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Plot random access
    plt.plot(sizes, rand_times, 'r-', linewidth=2, label='Random Access')
    plt.plot(sizes, rand_times, 'ro', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Only x-axis log scale
    plt.xscale('log', base=2)

    # Add cache boundary lines
    plt.axvline(x=32, color='cyan', linestyle='--', linewidth=1.5, label='L1 Cache (~32KB)')
    plt.axvline(x=1024, color='green', linestyle='--', linewidth=1.5, label='L2 Cache (~1MB)')

    plt.xlabel('Array Size (KB)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Cache Performance: Sequential vs Random Access (X-axis Log Scale) - Min Value', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3, which="both", axis="both")
    plt.tight_layout()

    plt.savefig('c_perf_x_log.png', dpi=150, bbox_inches='tight')
    print(f"Plot 1 saved to c_perf_x_log.png")

def create_plot_2_both_log(sizes, seq_times, rand_times):
    """Plot with both x and y log scale"""
    plt.figure(figsize=(10, 6))

    # Plot sequential access
    plt.plot(sizes, seq_times, 'b-', linewidth=2, label='Sequential Access')
    plt.plot(sizes, seq_times, 'bo', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Plot random access
    plt.plot(sizes, rand_times, 'r-', linewidth=2, label='Random Access')
    plt.plot(sizes, rand_times, 'ro', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Both axes log scale
    plt.xscale('log', base=2)
    plt.yscale('log')

    # Add cache boundary lines
    plt.axvline(x=32, color='cyan', linestyle='--', linewidth=1.5, label='L1 Cache (~32KB)')
    plt.axvline(x=1024, color='green', linestyle='--', linewidth=1.5, label='L2 Cache (~1MB)')

    plt.xlabel('Array Size (KB)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Cache Performance: Sequential vs Random Access (Both Axes Log Scale) - Min Value', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3, which="both", axis="both")
    plt.tight_layout()

    plt.savefig('c_perf_both_log.png', dpi=150, bbox_inches='tight')
    print(f"Plot 2 saved to c_perf_both_log.png")

def create_plot_3_l1_border(sizes, seq_times, rand_times):
    """Focus on L1 cache border (32KB) without log scales"""
    plt.figure(figsize=(10, 6))

    # Plot sequential access
    plt.plot(sizes, seq_times, 'b-', linewidth=2, label='Sequential Access')
    plt.plot(sizes, seq_times, 'bo', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Plot random access
    plt.plot(sizes, rand_times, 'r-', linewidth=2, label='Random Access')
    plt.plot(sizes, rand_times, 'ro', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # No log scales
    plt.xscale('linear')
    plt.yscale('linear')

    # Focus on L1 cache border region (16KB to 64KB)
    plt.xlim(16, 64)
    plt.ylim(0, 0.01)  # Zoom y-axis to use full plot area

    # Add L1 cache boundary line
    plt.axvline(x=32, color='cyan', linestyle='--', linewidth=1.5, label='L1 Cache (~32KB)')

    plt.xlabel('Array Size (KB)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Cache Performance Focus: L1 Cache Border (~32KB) - Min Value', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3, which="both", axis="both")
    plt.tight_layout()

    plt.savefig('c_perf_l1_focus.png', dpi=150, bbox_inches='tight')
    print(f"Plot 3 saved to c_perf_l1_focus.png")

def create_plot_4_l2_border(sizes, seq_times, rand_times):
    """Focus on L2 cache border (1024KB) without log scales"""
    plt.figure(figsize=(10, 6))

    # Plot sequential access
    plt.plot(sizes, seq_times, 'b-', linewidth=2, label='Sequential Access')
    plt.plot(sizes, seq_times, 'bo', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Plot random access
    plt.plot(sizes, rand_times, 'r-', linewidth=2, label='Random Access')
    plt.plot(sizes, rand_times, 'ro', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # No log scales
    plt.xscale('linear')
    plt.yscale('linear')

    # Focus on L2 cache border region (512KB to 2048KB)
    plt.xlim(512, 2048)
    plt.ylim(0, 3.0)  # Zoom y-axis to use full plot area

    # Add L2 cache boundary line
    plt.axvline(x=1024, color='green', linestyle='--', linewidth=1.5, label='L2 Cache (~1MB)')

    plt.xlabel('Array Size (KB)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Cache Performance Focus: L2 Cache Border (~1MB) - Min Value', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3, which="both", axis="both")
    plt.tight_layout()

    plt.savefig('c_perf_l2_focus.png', dpi=150, bbox_inches='tight')
    print(f"Plot 4 saved to c_perf_l2_focus.png")

def write_summary():
    """Write summary.txt with one line per array size: min, mean, RSD for seq and rand times"""
    from collections import defaultdict

    groups = defaultdict(lambda: {'seq': [], 'rand': []})
    with open('results.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('size of int'):
                parts = line.split()
                if len(parts) == 3:
                    try:
                        size = int(parts[0])
                        groups[size]['seq'].append(float(parts[1]))
                        groups[size]['rand'].append(float(parts[2]))
                    except ValueError:
                        continue

    with open('summary.txt', 'w') as f:
        f.write(f"{'size_kb':>10}  {'seq_min':>10}  {'seq_mean':>10}  {'seq_rsd%':>10}  {'rand_min':>10}  {'rand_mean':>10}  {'rand_rsd%':>10}\n")
        for size in sorted(groups):
            seq = np.array(groups[size]['seq'])
            rand = np.array(groups[size]['rand'])
            seq_rsd  = (seq.std()  / seq.mean()  * 100) if seq.mean()  != 0 else 0
            rand_rsd = (rand.std() / rand.mean() * 100) if rand.mean() != 0 else 0
            f.write(f"{size // 1024:>10}  {seq.min():>10.6f}  {seq.mean():>10.6f}  {seq_rsd:>10.2f}  {rand.min():>10.6f}  {rand.mean():>10.6f}  {rand_rsd:>10.2f}\n")

    print("Summary written to summary.txt")

def plot_results():
    """Generate all four plots"""
    write_summary()
    sizes, seq_mins, rand_mins = load_summary()

    # Create all four plots
    create_plot_1_x_log_only(sizes, seq_mins, rand_mins)
    create_plot_2_both_log(sizes, seq_mins, rand_mins)
    create_plot_3_l1_border(sizes, seq_mins, rand_mins)
    create_plot_4_l2_border(sizes, seq_mins, rand_mins)

    print("All plots generated successfully!")

if __name__ == "__main__":
    plot_results()