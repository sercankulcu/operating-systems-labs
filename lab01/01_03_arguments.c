#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check if exactly 2 arguments are passed (excluding the program name)
    if (argc != 3) {
        printf("Usage: %s <num1> <num2>\n", argv[0]);
        return 1;
    }

    // Convert arguments from strings to integers
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);

    // Calculate the sum
    int sum = num1 + num2;

    // Print the sum
    printf("The sum of %d and %d is %d\n", num1, num2, sum);

    return 0;
}

