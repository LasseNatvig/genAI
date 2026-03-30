import random
import string

def generate_character_counts(n, output_file='character_counts.txt'):
    """
    Generate a list of n tuples where each tuple contains:
    - a character c
    - a textstring ts with printable ASCII characters (no newlines/special chars)
    - an integer r representing the count of c in ts

    The list is written to a .txt file.
    """
    results = []

    # Define safe printable characters: letters, digits, space, and basic punctuation
    safe_chars = string.ascii_letters + string.digits + " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"

    for _ in range(n):
        # Generate random character and textstring
        c = random.choice(safe_chars)
        ts_length = random.randint(5, 40)  # Compact text string length
        ts = ''.join(random.choices(safe_chars, k=ts_length))

        # Calculate occurrences
        r = ts.count(c)

        results.append((c, ts, r))

    # Write to file in assembly format
    with open(output_file, 'w') as f:
        for i, (c, ts, r) in enumerate(results, 1):
            # Escape quotes in text string for assembly format
            escaped_ts = ts.replace('"', '\\"')
            f.write(f"  char{i}: .byte '{c}'\n")
            f.write(f"  input{i}: .asciz \"{escaped_ts}\"\n")
            f.write("  .align 4\n")
            f.write(f"  result{i}: .word {r}\n")
            f.write("\n")  # Add empty line between tuples for readability

    # Write to ARM assembly file for loading tuples
    arm_file = 'load_tuples.s'
    with open(arm_file, 'w') as f:
        for i, (c, ts, r) in enumerate(results, 1):
            f.write(f"  LDR R0, =result{i}\n")
            f.write(f"  PUSH {{R0}}\n")
            f.write(f"  LDR R0, =input{i}\n")
            f.write(f"  PUSH {{R0}}\n")
            f.write(f"  LDR R0, =char{i}\n")
            f.write(f"  PUSH {{R0}}\n")
            f.write("\n")  # Add empty line between tuples for readability

    print(f"Generated {n} tuples and saved to {output_file}")
    print(f"Generated ARM assembly code and saved to {arm_file}")

# Example usage
if __name__ == "__main__":
    generate_character_counts(20)