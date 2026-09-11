// GenAI-3-2026-winPC (= CPUlator 6) 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function to generate a random lowercase letter
char random_char() {
    return 'a' + (rand() % 26);
}

// Function to generate a random string of length len
void random_string(char *str, int len) {
    for (int i = 0; i < len; i++) {
        str[i] = random_char();
    }
    str[len] = '\0';
}

// Function to count occurrences of a character in a string
int count_char_occurrences(char c, const char *str) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == c) {
            count++;
        }
    }
    return count;
}

int main(void) {
    int n;
    printf("Enter the number of tuples (n): ");
    scanf_s("%d", &n);

    // Seed the random number generator
    srand(time(NULL));

    // Open the output file
    FILE *file;
    errno_t err = fopen_s(&file, "output.txt", "w");
    if (err != 0 || file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    // Generate and write n tuples
    for (int i = 0; i < n; i++) {
        char c = random_char();
        char ts[51]; // Max length of 50 characters + null terminator
        int ts_len = 5 + (rand() % 21); // Random length between 5 and 25
        random_string(ts, ts_len);
        int r = count_char_occurrences(c, ts);

        // Write the tuple to the file in the specified format
        fprintf(file, "char%d: .byte '%c'\n", i, c);
        fprintf(file, "input%d: .asciz \"%s\"\n", i, ts);
        fprintf(file, ".align 4\n");
        fprintf(file, "result%d: .word %d\n", i, r);
    }

    // Close the file
    fclose(file);

    // Open the assembly file for writing
    FILE *asm_file;
    errno_t asm_err = fopen_s(&asm_file, "output.s", "w");
    if (asm_err != 0 || asm_file == NULL) {
        printf("Error opening output.s!\n");
        return 1;
    }

    // Write the assembly instructions for each tuple
    for (int i = 0; i < n; i++) {
        fprintf(asm_file, "LDR R0, =result%d\n", i);
        fprintf(asm_file, "PUSH {R0}\n");
        fprintf(asm_file, "LDR R0, =input%d\n", i);
        fprintf(asm_file, "PUSH {R0}\n");
        fprintf(asm_file, "LDR R0, =char%d\n", i);
        fprintf(asm_file, "PUSH {R0}\n");
    }

    // Close the assembly file
    fclose(asm_file);
    printf("List of %d tuples written to output.txt and output.s.\n", n);

    return 0;
}