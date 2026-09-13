#!/usr/bin/env python3
# plot_perf.py - Plot average execution time vs. array size from CSV files

import glob
import os
import re
import pandas as pd
import matplotlib.pyplot as plt


def list_csv_files(directory):
    """List all CSV files in the given directory."""
    csv_files = glob.glob(os.path.join(directory, "*.csv"))
    return sorted(csv_files)


def extract_array_size(workload):
    """Extract array size (n=...) from the Workload column."""
    match = re.search(r'n=(\d+)', workload)
    if match:
        return int(match.group(1))
    return None


def extract_algorithm_name(workload):
    """Extract algorithm name from the Workload column."""
    # Remove the (n=...) part
    return workload.split(' (n=')[0]


def plot_performance(csv_file):
    """Plot average execution time vs. array size for the given CSV file."""
    # Read the CSV file with the first line as header
    df = pd.read_csv(csv_file, sep=',')
    
    # Clean column names (remove leading/trailing whitespace)
    df.columns = df.columns.str.strip()
    
    # Fix misaligned CSV: The header has Time_RSD% but the data rows have one fewer column
    # So we assume the last column in the data is Time_RSD%
    if 'Time_RSD%' in df.columns and df['Time_RSD%'].isna().all():
        # Drop the empty Time_RSD% column
        df = df.drop(columns=['Time_RSD%'])
        # Rename the last column (CacheMiss_RSD) to Time_RSD%
        last_col = df.columns[-1]
        df = df.rename(columns={last_col: 'Time_RSD%'})
    
    # Extract array size and algorithm name
    df['Array Size'] = df['Workload'].apply(extract_array_size)
    df['Algorithm'] = df['Workload'].apply(extract_algorithm_name)
    
    # Check if required columns exist
    if 'Time(s)' not in df.columns:
        print(f"Error: 'Time(s)' column not found in {csv_file}")
        return
    
    # Plot each algorithm separately with scatter points and error bars for Time_RSD%
    plt.figure(figsize=(10, 6))
    
    algorithms = df['Algorithm'].unique()
    
    # Define colors for each algorithm
    colors = {'InsSort': 'blue', 'BubSort': 'red', 'QckSort': 'green'}
    
    for algo in algorithms:
        algo_data = df[df['Algorithm'] == algo].copy()
        algo_data = algo_data.sort_values('Array Size')
        
        algo_color = colors.get(algo.strip(), 'black')
        
        # Extract Time_RSD% if available
        if 'Time_RSD%' in algo_data.columns:
            # Clean Time_RSD%: Remove % sign and convert to float
            time_rsd_raw = algo_data['Time_RSD%'].astype(str).str.rstrip('%').astype(float) / 100.0
            time_values = algo_data['Time(s)'].values
            time_errors = time_values * time_rsd_raw  # Absolute error = Time(s) * (Time_RSD% / 100)
            
            # Plot with error bars
            plt.errorbar(
                algo_data['Array Size'],
                time_values,
                yerr=time_errors,
                fmt='o',  # Scatter points
                label=algo.strip(),
                color=algo_color,
                capsize=5,  # Size of error bar caps
                elinewidth=1,  # Width of error bars
                markersize=5  # Size of scatter points
            )
        else:
            # Fallback to scatter plot if Time_RSD% is not available
            plt.scatter(
                algo_data['Array Size'],
                algo_data['Time(s)'],
                label=algo.strip(),
                color=algo_color,
                s=50  # Size of the scatter points
            )
        
        # Connect points with a solid line with strong color
        plt.plot(
            algo_data['Array Size'],
            algo_data['Time(s)'],
            color=algo_color,
            alpha=0.8,  # Solid line
            linestyle='-',  # Solid line
            linewidth=1.5
        )
    
    plt.xlabel('Array Size (n)')
    plt.ylabel('Average Execution Time (s)')
    plt.title(f'Performance: Average Execution Time vs. Array Size\n({os.path.basename(csv_file)})')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    
    # Save the plot to a file
    output_dir = "/home/lasse/code/genAI/perfCount/plots"
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, f"plot_{os.path.basename(csv_file).replace('.csv', '.png')}")
    plt.savefig(output_file)
    print(f"\nPlot saved to: {output_file}")
    plt.close()


def main():
    import sys
    
    # Check if a filename was provided as a command-line argument
    if len(sys.argv) > 1:
        # Use the provided filename
        csv_file = sys.argv[1]
        if os.path.exists(csv_file):
            print(f"Plotting data from: {os.path.basename(csv_file)}")
            plot_performance(csv_file)
        else:
            print(f"File not found: {csv_file}")
            return
    else:
        # Interactive mode - original behavior
        res_dir = "/home/lasse/code/genAI/perfCount/res"
        csv_files = list_csv_files(res_dir)
        
        if not csv_files:
            print(f"No CSV files found in {res_dir}")
            return
        
        print("Available CSV files:")
        for i, csv_file in enumerate(csv_files, 1):
            print(f"{i}. {os.path.basename(csv_file)}")
        
        # Ask user to pick a file
        try:
            choice = int(input(f"\nSelect a file (1-{len(csv_files)}): "))
            if 1 <= choice <= len(csv_files):
                selected_file = csv_files[choice - 1]
                print(f"\nPlotting data from: {os.path.basename(selected_file)}")
                plot_performance(selected_file)
            else:
                print("Invalid choice. Exiting.")
        except ValueError:
            print("Invalid input. Please enter a number.")


if __name__ == "__main__":
    main()