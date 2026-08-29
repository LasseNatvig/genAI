#!/usr/bin/env python3
# run_comparison_benchmark.py - Run comprehensive Quicksort comparison benchmarks

import subprocess
import sys
import os
from datetime import datetime

def get_timestamp():
    """Get current timestamp for filename"""
    return datetime.now().strftime("%Y%m%d_%H%M%S")

def main():
    # Parameters
    E = 5  # Number of runs per algorithm per test
    
    # Test various problem sizes
    N_values = [1000, 5000, 10000, 20000, 30000, 50000]
    
    # Generate timestamp for CSV file
    timestamp = get_timestamp()
    csv_filename = f"results/comparison_results_{timestamp}.csv"
    
    # Create results directory if it doesn't exist
    os.makedirs("results", exist_ok=True)
    
    # Compile the C program
    print("Compiling C program...")
    compile_result = subprocess.run(["make", "clean"], capture_output=True, text=True, cwd=".")
    compile_result = subprocess.run(["make"], capture_output=True, text=True, cwd=".")
    
    if compile_result.returncode != 0:
        print("Compilation failed:")
        print(compile_result.stderr)
        sys.exit(1)
    
    print("Compilation successful.")
    print(f"Results will be saved to {csv_filename}")
    
    # Run the program for each N value
    for N in N_values:
        print(f"\nRunning benchmark with N={N}, E={E}...")
        result = subprocess.run(["./quicksort_comparison", str(N), str(E), csv_filename, timestamp], 
                              capture_output=True, text=True, cwd=".")
        
        if result.returncode != 0:
            print(f"Error running program for N={N}:")
            print(result.stderr)
            continue
        
        print(result.stdout)
    
    print(f"\nBenchmark complete! Results saved to {csv_filename}")
    print(f"Use 'python plot_comparison_results.py' to generate plots from this data.")

if __name__ == "__main__":
    main()