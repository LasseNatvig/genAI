#!/usr/bin/env python3
# plot_comparison_results.py - Plot Quicksort comparison results

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys
import os
from datetime import datetime

def find_csv_files():
    """Find all comparison results files"""
    csv_files = []
    for file in os.listdir('results'):
        if file.startswith('comparison_results_') and file.endswith('.csv'):
            csv_files.append(f"results/{file}")
    return sorted(csv_files)

def extract_timestamp_from_filename(filename):
    """Extract timestamp from filename"""
    base = filename.replace('comparison_results_', '').replace('.csv', '')
    return base

def plot_results(csv_file):
    """Plot the performance results"""
    df = pd.read_csv(csv_file)
    
    if df.empty:
        print(f"Error: CSV file {csv_file} is empty")
        return
    
    timestamp = extract_timestamp_from_filename(os.path.basename(csv_file))
    
    # Create multiple plots
    fig, axes = plt.subplots(3, 1, figsize=(14, 16))
    
    # Plot 1: Execution time comparison
    sns.lineplot(data=df, x='N', y='execution_time', hue='algorithm', 
                style='array_type', markers=True, ax=axes[0])
    axes[0].set_xlabel('Array Size (N)')
    axes[0].set_ylabel('Execution Time (seconds)')
    axes[0].set_title('Execution Time Comparison by Array Type and Algorithm')
    axes[0].grid(True, alpha=0.3)
    axes[0].set_facecolor('#f9f9f9')
    
    # Plot 2: Comparisons count
    sns.lineplot(data=df, x='N', y='comparisons', hue='algorithm', 
                style='array_type', markers=True, ax=axes[1])
    axes[1].set_xlabel('Array Size (N)')
    axes[1].set_ylabel('Number of Comparisons')
    axes[1].set_title('Comparison Count by Array Type and Algorithm')
    axes[1].grid(True, alpha=0.3)
    axes[1].set_facecolor('#f9f9f9')
    
    # Plot 3: Swaps count
    sns.lineplot(data=df, x='N', y='swaps', hue='algorithm', 
                style='array_type', markers=True, ax=axes[2])
    axes[2].set_xlabel('Array Size (N)')
    axes[2].set_ylabel('Number of Swaps')
    axes[2].set_title('Swap Count by Array Type and Algorithm')
    axes[2].grid(True, alpha=0.3)
    axes[2].set_facecolor('#f9f9f9')
    
    plt.tight_layout()
    
    # Save the plot
    png_filename = f"results/plot_comparison_{timestamp}.png"
    plt.savefig(png_filename, dpi=300, bbox_inches='tight')
    print(f"Plot saved as {png_filename}")
    
    # Show the plot
    plt.show()
    
    # Also create detailed analysis for each array type
    print("\nDetailed Analysis:")
    print("=" * 80)
    
    array_types = df['array_type'].unique()
    algorithms = df['algorithm'].unique()
    
    for array_type in array_types:
        print(f"\nArray Type: {array_type}")
        print("-" * 40)
        array_df = df[df['array_type'] == array_type]
        
        for alg in algorithms:
            alg_df = array_df[array_df['algorithm'] == alg]
            if not alg_df.empty:
                exec_time = alg_df['execution_time'].iloc[-1]  # Last (largest) N
                comparisons = alg_df['comparisons'].iloc[-1]
                swaps = alg_df['swaps'].iloc[-1]
                print(f"  {alg:<25}: Time: {exec_time:.6f}s, Comparisons: {comparisons:>10}, Swaps: {swaps:>10}")

def main():
    csv_files = find_csv_files()
    
    if not csv_files:
        print("No comparison_results_*.csv files found in results directory")
        print("Please run run_comparison_benchmark.py first to generate data")
        sys.exit(1)
    
    print("Available CSV files:")
    for i, file in enumerate(csv_files):
        print(f"  {i+1}. {file}")
    
    if len(csv_files) == 1:
        plot_results(csv_files[0])
    else:
        print(f"\nSelect a file to plot (1-{len(csv_files)}):")
        try:
            choice = int(input().strip())
            if choice < 1 or choice > len(csv_files):
                print(f"Invalid choice. Please select 1-{len(csv_files)}")
                sys.exit(1)
            plot_results(csv_files[choice-1])
        except ValueError:
            print("Invalid input. Please enter a number.")
            sys.exit(1)

if __name__ == "__main__":
    main()