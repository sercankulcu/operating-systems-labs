#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096 // Buffer size for reading and writing

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *source_file, *destination_file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Open source file for reading
    source_file = fopen(argv[1], "rb");
    if (source_file == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Open destination file for writing
    destination_file = fopen(argv[2], "wb");
    if (destination_file == NULL) {
        perror("Error opening destination file");
        fclose(source_file);
        exit(EXIT_FAILURE);
    }

    // Copy content from source file to destination file
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        fwrite(buffer, 1, bytes_read, destination_file);
    }

    // Close files
    fclose(source_file);
    fclose(destination_file);

    printf("File copied successfully from %s to %s.\n", argv[1], argv[2]);

    return 0;
}

