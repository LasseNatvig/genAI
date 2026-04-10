#include <stdio.h>

extern int multiply_repeated_addition(const int *value1, const int *value2);
extern char char_array[];

char text[] = "Hello from C!";

int main(void)
{
    char *s = text;
    int value1 = 6;  
    int value2 = -7;  
    int result = multiply_repeated_addition(&value1, &value2);

    printf("%d * %d = %d\n", value1, value2, result);
    printf("String from C: %s\n", s);
    printf("Character array from assembly: %s\n", char_array);
    return 0;
}