# run_benchmark.py - Python script to run C sorting benchmark

import subprocess
import sys
import os

def main():
    # Parameters
    N = 10000  # Number of random integers
    E = 5      # Number of runs per algorithm

    # Compile the C program
    print(f"Compiling C program...")
    compile_result = subprocess.run(["make", "clean"], capture_output=True, text=True)
    compile_result = subprocess.run(["make", "test"], capture_output=True, text=True)

    if compile_result.returncode != 0:
        print(f"Compilation failed:")
        print(compile_result.stderr)
        sys.exit(1)

    print(f"Compilation successful.")

    # Run the program with parameters N={N} and E={E}
    print(f"\nRunning benchmark with N={N}, E={E}...")
    result = subprocess.run(["./test", str(N), str(E)], capture_output=True, text=True)

    if result.returncode != 0:
        print(f"Error running program:")
        print(result.stderr)
        sys.exit(1)

    print(result.stdout)

if __name__ == "__main__":
    main()