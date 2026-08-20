# benchmark.py - Performance benchmarking and plotting

import subprocess
import numpy as np
import matplotlib.pyplot as plt
import os

def run_benchmark(n_value):
    """Compile and run the parameterized C program with a specific N value."""
    try:
        # Compile the parameterized C file (only once)
        if not os.path.exists('test_param'):
            compile_result = subprocess.run(['gcc', '-std=c11', '-g', '-o', 'test_param', 'main_param.c'],
                                           capture_output=True, text=True)
            if compile_result.returncode != 0:
                print(f"Compilation failed: {compile_result.stderr}")
                return None

        # Run the compiled program with N as parameter
        run_result = subprocess.run(['./test_param', str(n_value)], capture_output=True, text=True)

        if run_result.returncode != 0:
            print(f"Execution failed for N={n_value}: {run_result.stderr}")
            return None

        # Parse the output
        lines = run_result.stdout.strip().split('\n')
        if len(lines) >= 4:
            try:
                # Line 0: "N RUNS", Line 1: "Insertion: X", Line 2: "Bubble: Y", Line 3: "Quick: Z"
                insertion_time = float(lines[1].split(':')[1].strip())
                bubble_time = float(lines[2].split(':')[1].strip())
                quick_time = float(lines[3].split(':')[1].strip())

                return {
                    'n': n_value,
                    'insertion': insertion_time,
                    'bubble': bubble_time,
                    'quick': quick_time
                }
            except (IndexError, ValueError) as e:
                print(f"Could not parse output for N={n_value}: {run_result.stdout}")
                return None
        else:
            print(f"Unexpected output for N={n_value}: {run_result.stdout}")
            return None

    except Exception as e:
        print(f"Error running benchmark for N={n_value}: {e}")
        return None

def collect_performance_data():
    """Collect performance data for varying N values."""

    # Generate N values: 1000 to 10000 step 1000, 10000 to 100000 step 10000
    n_values = []
    # First range: 1000 to 10000 in steps of 1000
    for n in range(1000, 10001, 1000):
        n_values.append(n)
    # Second range: 20000 to 100000 in steps of 10000
    for n in range(20000, 100001, 10000):
        n_values.append(n)

    print("Collecting performance data...")
    print("This may take several minutes depending on your system performance.")

    results = []

    for n in n_values:
        print(f"Testing N={n}... ", end='', flush=True)

        result = run_benchmark(n)

        if result:
            print(f"Done - Insertion: {result['insertion']:.2f}ms, Bubble: {result['bubble']:.2f}ms, Quick: {result['quick']:.2f}ms")
            results.append(result)
        else:
            print(f"Skipping N={n} due to errors")

    return results, n_values

def create_performance_plot(results, n_values):
    """Create matplotlib plots for the performance data."""

    if not results:
        print("No data to plot")
        return

    # Extract data
    n_array = np.array([r['n'] for r in results])
    insertion_times = np.array([r['insertion'] for r in results])
    bubble_times = np.array([r['bubble'] for r in results])
    quick_times = np.array([r['quick'] for r in results])

    # Create figure with subplots
    plt.figure(figsize=(14, 6))

    # Plot all three algorithms on the same graph (linear scale)
    plt.subplot(1, 2, 1)
    plt.plot(n_array, insertion_times, 'b-', label='Insertion Sort', marker='o')
    plt.plot(n_array, bubble_times, 'r-', label='Bubble Sort', marker='s')
    plt.plot(n_array, quick_times, 'g-', label='Quick Sort', marker='^')

    plt.xlabel('Array Size (N)')
    plt.ylabel('Time (ms)')
    plt.title('Sorting Algorithm Performance (Linear Scale)')
    plt.legend()
    plt.grid(True, which='both', linestyle='--', alpha=0.7)

    # Create a log-log plot for better visualization of growth rates
    plt.subplot(1, 2, 2)
    plt.plot(n_array, insertion_times, 'b-', label='Insertion Sort', marker='o')
    plt.plot(n_array, bubble_times, 'r-', label='Bubble Sort', marker='s')
    plt.plot(n_array, quick_times, 'g-', label='Quick Sort', marker='^')

    plt.xlabel('Array Size (N)')
    plt.ylabel('Time (ms)')
    plt.title('Sorting Algorithm Performance (Log-Log Scale)')
    plt.legend()
    plt.grid(True, which='both', linestyle='--', alpha=0.7)
    plt.xscale('log')
    plt.yscale('log')

    plt.tight_layout()
    plt.savefig('sorting_performance.png', dpi=300, bbox_inches='tight')
    print("Performance plot saved as 'sorting_performance.png'")

    # Also create a separate full comparison plot
    plt.figure(figsize=(10, 6))
    plt.plot(n_array, insertion_times, 'b-', label='Insertion Sort', marker='o')
    plt.plot(n_array, bubble_times, 'r-', label='Bubble Sort', marker='s')
    plt.plot(n_array, quick_times, 'g-', label='Quick Sort', marker='^')

    plt.xlabel('Array Size (N)')
    plt.ylabel('Time (ms)')
    plt.title('Sorting Algorithm Performance Comparison')
    plt.legend()
    plt.grid(True, which='both', linestyle='--', alpha=0.7)

    plt.savefig('sorting_comparison.png', dpi=300, bbox_inches='tight')
    print("Comparison plot saved as 'sorting_comparison.png'")

    # Show the plots
    plt.show()

def cleanup():
    """Clean up temporary files."""
    if os.path.exists('test_param'):
        os.remove('test_param')

def main():
    """Main function to run the benchmarking and plotting."""

    print("Sorting Algorithm Performance Benchmark")
    print("=" * 50)

    try:
        # Collect performance data
        results, n_values = collect_performance_data()

        if results:
            # Print summary
            print("\nPerformance Summary:")
            print("{:<8} {:<15} {:<15} {:<15}".format("N", "Insertion(ms)", "Bubble(ms)", "Quick(ms)"))
            for result in results:
                print("{:<8} {:<15.4f} {:<15.4f} {:<15.4f}".format(
                    result['n'], result['insertion'], result['bubble'], result['quick']))

            # Create plots
            create_performance_plot(results, n_values)
        else:
            print("Failed to collect performance data")

    finally:
        # Clean up temporary files
        cleanup()

if __name__ == "__main__":
    main()