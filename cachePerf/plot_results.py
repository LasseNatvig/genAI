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

    # Parse output: size seq_time rand_time
    data = []
    for line in result.stdout.strip().split('\n'):
        if line:
            parts = line.split()
            if len(parts) == 3:
                data.append((int(parts[0]), float(parts[1]), float(parts[2])))

    return np.array(data)

def plot_results():
    """Generate and save the performance plot"""
    if not os.path.exists('results.txt'):
        # Run benchmark and save results
        result = subprocess.run(["./cache_demo"], capture_output=True, text=True)
        with open('results.txt', 'w') as f:
            f.write(result.stdout)

    # Load data from file
    data = np.loadtxt('results.txt')
    sizes = data[:, 0] / 1024  # Convert to KB
    seq_times = data[:, 1]
    rand_times = data[:, 2]

    plt.figure(figsize=(10, 6))

    # Plot sequential access with lines and markers
    plt.plot(sizes, seq_times, 'b-', linewidth=2, label='Sequential Access')
    plt.plot(sizes, seq_times, 'bo', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Plot random access with lines and markers
    plt.plot(sizes, rand_times, 'r-', linewidth=2, label='Random Access')
    plt.plot(sizes, rand_times, 'ro', markersize=6, markerfacecolor='none', markeredgewidth=1.5)

    # Use logarithmic scale on x-axis and y-axis
    plt.xscale('log', base=2)
    plt.yscale('log')

    # Add cache boundary lines for Raspberry Pi4B
    plt.axvline(x=32, color='cyan', linestyle='--', linewidth=1.5, label='L1 Cache (~32KB)')
    plt.axvline(x=1024, color='green', linestyle='--', linewidth=1.5, label='L2 Cache (~1MB)')

    plt.xlabel('Array Size (KB)', fontsize=12)
    plt.ylabel('Time (seconds)', fontsize=12)
    plt.title('Cache Performance: Sequential vs Random Access (Raspberry Pi4B)', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3, which="both", axis="both")
    plt.tight_layout()

    plt.savefig('cache_performance.png', dpi=150, bbox_inches='tight')
    print(f"Plot saved to cache_performance.png")

    # also show the plot
    plt.show()

if __name__ == "__main__":
    plot_results()