#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 512 // Define block size

int main() {
    FILE *file;
    char filename[] = "block_file.bin";
    char buffer[BLOCK_SIZE];
    int num_blocks = 3; // Number of blocks to write

    // Open file in write mode
    file = fopen(filename, "wb");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Write blocks of data to the file
    for (int i = 0; i < num_blocks; i++) {
        // Fill buffer with some data
        for (int j = 0; j < BLOCK_SIZE; j++) {
            buffer[j] = i + 'A'; // Fill buffer with letters A, B, C, ...
        }
        // Write buffer to file
        fwrite(buffer, sizeof(char), BLOCK_SIZE, file);
    }

    // Close file
    fclose(file);

    // Open file in read mode
    file = fopen(filename, "rb");

    // Check if file opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Read blocks of data from the file
    printf("Contents of %s:\n", filename);
    for (int i = 0; i < num_blocks; i++) {
        // Read a block into buffer
        fread(buffer, sizeof(char), BLOCK_SIZE, file);
        // Print contents of buffer
        for (int j = 0; j < BLOCK_SIZE; j++) {
            printf("%c", buffer[j]);
        }
    }
    printf("\n");

    // Close file
    fclose(file);

    return 0;
}

