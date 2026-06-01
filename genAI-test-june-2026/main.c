// main.c   --- TDT4258 - R2026 - RPi4-1 code example: tools setup code example

#include <stdbool.h>
#include <stdio.h>

extern int asmAdd(int a, int b);
extern int asmSub(int a, int b);
extern int asmRoutine(int a, int b);
extern int asmMul(int a, int b);

int main() {
    // Test addition
    int add_result = asmAdd(2, 3);
    if (add_result == 5) {
        printf("Addition OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 5 but got %d\n", add_result);
        fflush(stdout);
    }

    // Test subtraction
    int sub_result = asmSub(10, 4);
    if (sub_result == 6) {
        printf("Subtraction OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 6 but got %d\n", sub_result);
        fflush(stdout);
    }

    // Test subtraction with negative result
    int sub_result2 = asmSub(5, 10);
    if (sub_result2 == -5) {
        printf("Negative subtraction OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected -5 but got %d\n", sub_result2);
        fflush(stdout);
    }

    // Test multiplication: 2 * 3 = 6
    int mul_result1 = asmMul(2, 3);
    if (mul_result1 == 6) {
        printf("Multiplication (2*3) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 6 but got %d\n", mul_result1);
        fflush(stdout);
    }

    // Test multiplication: 5 * 0 = 0
    int mul_result2 = asmMul(5, 0);
    if (mul_result2 == 0) {
        printf("Multiplication (5*0) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 0 but got %d\n", mul_result2);
        fflush(stdout);
    }

    // Test multiplication: 7 * 4 = 28
    int mul_result3 = asmMul(7, 4);
    if (mul_result3 == 28) {
        printf("Multiplication (7*4) OK\n");
        fflush(stdout);
    } else {
        printf("\nERROR: expected 28 but got %d\n", mul_result3);
        fflush(stdout);
    }

}
