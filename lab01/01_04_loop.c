// Include standard input-output library for printf and scanf functions
#include <stdio.h>

int main() {
    // Declare variables
    int number;           // To store user input
    int i;               // Loop counter
    int sum = 0;         // To store sum of numbers
    
    // Prompt user for input
    printf("Enter a positive number: ");
    
    // Read user input and store it in 'number' variable
    // &number provides the memory address to store the input
    scanf("%d", &number);
    
    // Print confirmation of the entered number
    printf("You entered: %d\n", number);
    
    // If clause to check if number is positive
    if (number > 0) {
        // For loop to iterate from 1 to the entered number
        for (i = 1; i <= number; i++) {
            // Add current value of i to sum
            sum = sum + i;
            
            // Print current iteration details
            printf("Adding %d, current sum = %d\n", i, sum);
        }
        
        // Print final result after loop completes
        printf("The sum of numbers from 1 to %d is: %d\n", number, sum);
    } 
    else {
        // Execute if number is not positive
        printf("Please enter a positive number next time!\n");
    }
    
    // Return 0 to indicate successful program completion
    return 0;
}
