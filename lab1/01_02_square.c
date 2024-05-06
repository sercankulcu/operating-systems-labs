#include <stdio.h>

int main() {
    int number;
    
    // Prompt the user to enter a number
    printf("Enter a number: ");
    scanf("%d", &number);
    
    // Calculate the square of the number
    int square = number * number;
    
    // Print the square
    printf("The square of %d is: %d\n", number, square);
    
    return 0;
}

