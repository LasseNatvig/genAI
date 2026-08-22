# run_benchmark.py - Python script to run C sorting benchmark

import subprocess
import sys
import os
from datetime import datetime

def get_timestamp():
    """Get current timestamp for filename"""
    return datetime.now().strftime("%Y%m%d_%H%M%S")

def main():
    # Parameters
    E = 5      # Number of runs per algorithm

    # Test N values from 10000 to 20000 in steps of 1000
    N_values = range(10000, 20001, 1000)

    # Generate timestamp for CSV file
    timestamp = get_timestamp()
    csv_filename = f"results_{timestamp}.csv"

    # Compile the C program
    print(f"Compiling C program...")
    compile_result = subprocess.run(["make", "clean"], capture_output=True, text=True)
    compile_result = subprocess.run(["make", "test"], capture_output=True, text=True)

    if compile_result.returncode != 0:
        print(f"Compilation failed:")
        print(compile_result.stderr)
        sys.exit(1)

    print(f"Compilation successful.")
    print(f"Results will be saved to {csv_filename}")

    # Run the program for each N value
    for N in N_values:
        print(f"\nRunning benchmark with N={N}, E={E}...")
        result = subprocess.run(["./test", str(N), str(E), csv_filename, timestamp], capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Error running program for N={N}:")
            print(result.stderr)
            sys.exit(1)

        print(result.stdout)

    print(f"\nBenchmark complete! Results saved to {csv_filename}")
    print(f"Use 'python plot_results.py' to generate plots from this data.")

if __name__ == "__main__":
    main()