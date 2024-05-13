#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = NULL;
    int size = 5;

    // Allocate memory dynamically
    ptr = (int *)malloc(size * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Fill the allocated memory with values
    for (int i = 0; i < size; i++) {
        ptr[i] = i + 1;
    }

    // Print the values
    printf("Values stored in dynamically allocated memory:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", ptr[i]);
    }
    printf("\n");

    // Free the allocated memory
    free(ptr);

    return 0;
}

/*
 * This program demonstrates how memory is allocated dynamically using malloc() and then freed using free().
 * */
