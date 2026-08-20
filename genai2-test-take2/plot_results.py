# plot_results.py - Plot performance results from CSV file using matplotlib

import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import re
from datetime import datetime

def find_csv_files():
    """Find all results_*.csv files in the current directory"""
    csv_files = []
    for file in os.listdir('.'):
        if file.startswith('results_') and file.endswith('.csv'):
            csv_files.append(file)
    return sorted(csv_files)

def extract_timestamp_from_filename(filename):
    """Extract timestamp from filename like results_20260820_165625.csv"""
    # Remove 'results_' prefix and '.csv' suffix
    base = filename.replace('results_', '').replace('.csv', '')
    return base

def plot_results(csv_file):
    """Plot the performance results from a CSV file"""
    # Read the CSV file
    df = pd.read_csv(csv_file)

    if df.empty:
        print(f"Error: CSV file {csv_file} is empty")
        return

    # Extract timestamp from filename for output filenames
    timestamp = extract_timestamp_from_filename(csv_file)

    # Create the plot
    plt.figure(figsize=(12, 8))

    # Plot each sorting algorithm
    algorithms = ['insertion_sort_time', 'bubble_sort_time', 'quicksort_time']
    labels = ['Insertion Sort', 'Bubble Sort', 'QuickSort']
    colors = ['red', 'blue', 'green']

    for alg, label, color in zip(algorithms, labels, colors):
        plt.plot(df['N'], df[alg], 'o-', label=label, color=color, linewidth=2, markersize=8)

    plt.xlabel('Array Size (N)', fontsize=14)
    plt.ylabel('Average Time (seconds)', fontsize=14)
    plt.title(f'Sorting Algorithm Performance\n(E={df["E"].iloc[0]} runs per algorithm)', fontsize=16)
    plt.legend(fontsize=12)
    plt.grid(True, which='both', linestyle='--', alpha=0.5)

    # Set x-axis ticks to match our N values
    plt.xticks(df['N'], rotation=45)

    # Adjust layout to prevent label cutoff
    plt.tight_layout()

    # Save the plot as PNG with same timestamp
    png_filename = f"plot_{timestamp}.png"
    plt.savefig(png_filename, dpi=300, bbox_inches='tight')
    print(f"Plot saved as {png_filename}")

    # Also display the plot
    plt.show()

def main():
    # Find available CSV files
    csv_files = find_csv_files()

    if not csv_files:
        print("No results_*.csv files found in current directory")
        print("Please run run_benchmark.py first to generate data")
        sys.exit(1)

    print(f"Available CSV files:")
    for i, file in enumerate(csv_files):
        print(f"  {i+1}. {file}")

    # If there's only one file, use it automatically
    if len(csv_files) == 1:
        csv_file = csv_files[0]
        print(f"\nUsing: {csv_file}")
        plot_results(csv_file)
    else:
        # Let user choose which file to plot
        print(f"\nSelect a file to plot (1-{len(csv_files)}):")
        try:
            choice = int(input().strip())
            if choice < 1 or choice > len(csv_files):
                print(f"Invalid choice. Please select 1-{len(csv_files)}")
                sys.exit(1)
            csv_file = csv_files[choice-1]
            plot_results(csv_file)
        except ValueError:
            print("Invalid input. Please enter a number.")
            sys.exit(1)

if __name__ == "__main__":
    main()