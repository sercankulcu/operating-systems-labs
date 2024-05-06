#include <stdio.h>
#include "02_04_math_functions.h"

int main() {
    int a = 10;
    int b = 5;
    double c = 12.5;
    double d = 2.5;

    // Using math functions
    printf("Addition: %d + %d = %d\n", a, b, add(a, b));
    printf("Subtraction: %d - %d = %d\n", a, b, subtract(a, b));
    printf("Multiplication: %d * %d = %d\n", a, b, multiply(a, b));
    printf("Division: %.1f / %.1f = %.2f\n", c, d, divide(c, d));

    return 0;
}

