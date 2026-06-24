#!/usr/bin/env python
"""
Run all debug tests sequentially
"""

import subprocess
import sys
import time

def run_test(script_name, description):
    """Run a single test script"""
    print(f"\n{'='*50}")
    print(f"Running: {description}")
    print(f"{'='*50}\n")

    try:
        result = subprocess.run(
            [sys.executable, script_name],
            check=True,
            capture_output=True,
            text=True,
            cwd="."
        )
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error running {script_name}:")
        print(e.stderr)
        return False
    except FileNotFoundError:
        print(f"Script {script_name} not found!")
        return False

def main():
    """Run all tests"""
    print("Starting Debug Test Suite")
    print("="*50)

    tests = [
        ("led_test.py", "LED Hardware Test"),
        ("input_test.py", "Keyboard Input Test"),
        ("snake_debug.py", "Debug Snake Game")
    ]

    all_passed = True
    for script, description in tests:
        success = run_test(script, description)
        if not success:
            all_passed = False
            print(f"\n❌ {description} FAILED\n")
        else:
            print(f"\n✅ {description} PASSED\n")
        time.sleep(1)  # Pause between tests

    print("="*50)
    print("Test Summary")
    print("="*50)
    if all_passed:
        print("✅ All tests passed!")
    else:
        print("❌ Some tests failed!")
    print("="*50)

if __name__ == "__main__":
    main()