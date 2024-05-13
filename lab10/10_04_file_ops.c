#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char filename[] = "example_file.txt";
    char text[] = "This is a demonstration of file operations in C.";

    // Open file in write mode
    file = fopen(filename, "w");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    // Write text to the file
    fputs(text, file);

    // Close file
    fclose(file);

    // Open file in read mode
    file = fopen(filename, "r");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    // Read and display text from the file
    printf("Contents of %s:\n", filename);
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Close file
    fclose(file);

    return 0;
}

