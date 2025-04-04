// Include the standard input-output library for using printf function
#include <stdio.h>

// Main function - entry point of the program
int main() {
    // Print a welcome message to the console
    // \n adds a new line after the text
    printf("Hello, world! Welcome to the world of programming.\n");
    
    // Declare and initialize a variable to store a number
    int number = 42;
    
    // Print the value of the number variable
    // %d is a format specifier for integers
    printf("Here's a number for you: %d\n", number);
    
    // Declare a character variable
    char letter = 'A';
    
    // Print the character
    // %c is a format specifier for characters
    printf("Here's a letter: %c\n", letter);
    
    // Return 0 to indicate successful program completion
    return 0;
}
