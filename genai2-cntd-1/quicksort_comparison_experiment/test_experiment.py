#!/usr/bin/env python3
# Test script to verify the experiment setup works

import subprocess
import sys
import os

def test_compilation():
    """Test that the C program compiles"""
    print("Testing compilation...")
    result = subprocess.run(["make", "clean"], capture_output=True, text=True, cwd=".")
    result = subprocess.run(["make"], capture_output=True, text=True, cwd=".")
    
    if result.returncode != 0:
        print("Compilation failed:")
        print(result.stderr)
        return False
    
    print("✓ Compilation successful")
    
    # Check if executable exists
    if not os.path.exists("./quicksort_comparison"):
        print("✗ Executable not found")
        return False
    
    print("✓ Executable created")
    return True

def test_basic_run():
    """Test a basic run of the program"""
    print("Testing basic program run...")
    
    # Use small values for quick testing
    result = subprocess.run(["./quicksort_comparison", "1000", "3", "test_output.csv", "test"], 
                          capture_output=True, text=True, cwd=".")
    
    if result.returncode != 0:
        print("Program execution failed:")
        print(result.stderr)
        return False
    
    print("Program output:")
    print(result.stdout)
    
    # Check if CSV file was created and has content
    if not os.path.exists("test_output.csv"):
        print("✗ CSV output file not created")
        return False
    
    with open("test_output.csv", "r") as f:
        content = f.read()
        if len(content) == 0:
            print("✗ CSV file is empty")
            return False
        
        print("✓ CSV file created and has content")
        print("Sample CSV content:")
        lines = content.split('\n')[:5]  # Show first 5 lines
        for line in lines:
            print(f"  {line}")
    
    # Clean up test file
    os.remove("test_output.csv")
    print("✓ Basic run test passed")
    return True

def test_array_types():
    """Test that all array types are being tested"""
    print("\nTesting different array types...")
    
    result = subprocess.run(["./quicksort_comparison", "2000", "2", "test_types.csv", "test"], 
                          capture_output=True, text=True, cwd=".")
    
    if result.returncode != 0:
        print("Failed to test array types:")
        print(result.stderr)
        return False
    
    # Read CSV and check for different array types
    with open("test_types.csv", "r") as f:
        lines = f.readlines()
        
    array_types_found = set()
    algorithms_found = set()
    
    for line in lines[1:]:  # Skip header
        if line.strip():
            parts = line.strip().split(',')
            if len(parts) >= 4:
                array_types_found.add(parts[3])
                algorithms_found.add(parts[4])
    
    expected_array_types = {'random', 'sorted', 'reverse_sorted', 'with_duplicates'}
    expected_algorithms = {'lomuto_quicksort', 'hoare_quicksort', 'library_qsort'}
    
    print(f"Array types found: {sorted(array_types_found)}")
    print(f"Algorithms found: {sorted(algorithms_found)}")
    
    if array_types_found == expected_array_types:
        print("✓ All expected array types present")
    else:
        print("✗ Missing array types")
        return False
    
    if algorithms_found == expected_algorithms:
        print("✓ All expected algorithms present")
    else:
        print("✗ Missing algorithms")
        return False
    
    # Clean up
    os.remove("test_types.csv")
    return True

def main():
    print("Quicksort Comparison Experiment - Test Suite")
    print("=" * 50)
    
    all_tests_passed = True
    
    # Test compilation
    if not test_compilation():
        all_tests_passed = False
    
    # Test basic functionality
    if not test_basic_run():
        all_tests_passed = False
    
    # Test array types
    if not test_array_types():
        all_tests_passed = False
    
    print("\n" + "=" * 50)
    if all_tests_passed:
        print("✓ All tests passed! The experiment is ready to run.")
        print("\nTo run the full experiment:")
        print("  python3 run_comparison_benchmark.py")
    else:
        print("✗ Some tests failed. Please check the output above.")
        sys.exit(1)

if __name__ == "__main__":
    main()