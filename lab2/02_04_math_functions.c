#include "02_04_math_functions.h"

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        // Division by zero, handle error
        return 0; // For simplicity, returning 0
    }
    return a / b;
}

