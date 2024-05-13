#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char filename[] = "test.txt";
    char ch;

    // Open file in write mode
    file = fopen(filename, "w");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Write characters to the file
    fprintf(file, "Hello, this is a character-based file demonstration.\n");
    fprintf(file, "The quick brown fox jumps over the lazy dog.\n");

    // Close file
    fclose(file);

    // Open file in read mode
    file = fopen(filename, "r");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Read and display characters from the file
    printf("Contents of %s:\n", filename);
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Close file
    fclose(file);

    return 0;
}

