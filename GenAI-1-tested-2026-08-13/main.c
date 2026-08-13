// main.c   --- TDT4258 - R2026 - RPi4-1 code example: tools setup code example

#include <stdbool.h>
#include <stdio.h>

extern int asmAdd();
extern int asmSub();
extern int asmMultiply();

int main() {
    // Test addition
    int result = asmAdd(2, 3);
    if (result == 5) {
        printf("Addition OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 5 but got %d\n", result);
        fflush(stdout);
    }

    // Test subtraction
    result = asmSub(5, 3);
    if (result == 2) {
        printf("Subtraction OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 2 but got %d\n", result);
        fflush(stdout);
    }

    // Test multiplication - Test case 1: positive numbers (3 * 4 = 12)
    result = asmMultiply(3, 4);
    if (result == 12) {
        printf("Multiplication test 1 (3*4=12) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 12 but got %d\n", result);
        fflush(stdout);
    }

    // Test multiplication - Test case 2: multiply by zero (5 * 0 = 0)
    result = asmMultiply(5, 0);
    if (result == 0) {
        printf("Multiplication test 2 (5*0=0) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 0 but got %d\n", result);
        fflush(stdout);
    }

    // Test multiplication - Test case 3: negative multiplier (2 * -3 = -6)
    result = asmMultiply(2, -3);
    if (result == -6) {
        printf("Multiplication test 3 (2*-3=-6) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected -6 but got %d\n", result);
        fflush(stdout);
    }

}
