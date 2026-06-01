// main.c   --- TDT4258 - R2026 - RPi4-1 code example: tools setup code example

#include <stdbool.h>
#include <stdio.h>

extern bool asmAdd(); 
extern int asmRoutine(int a, int b); // from exam 2024
 
int main() {
    int result = asmAdd(2, 3);
    if (result == 5) {
        printf(" OK\n");  
        fflush(stdout);
    } else {
        printf("\nERROR: expected 5 but got %d\n", result);
        fflush(stdout);
    }
    //**************************************************************
    // Exam 2024 problem 17
    int n1 = 0x0000FFFF;
    int n2 = 0x00010000;
    printf("ResultA: %d\n", asmRoutine(n1, n2));
    n2 = 0x00000001;
    printf("ResultB: %d\n", asmRoutine(n1, n2));
    return 0;
}