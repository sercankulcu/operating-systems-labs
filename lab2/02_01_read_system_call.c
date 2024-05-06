#include <unistd.h> // For the read function
#include <stdio.h>  // For printf

#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];  // Buffer to store input
    int bytes_read;             // Number of bytes read

    // Read input from stdin (keyboard)
    bytes_read = read(0, buffer, BUFFER_SIZE);

    if (bytes_read == -1) {
        perror("Error reading input");
        return 1;
    }

    // Null-terminate the string
    buffer[bytes_read] = '\0';

    // Print the read input
    printf("You entered: %s", buffer);

    return 0;
}

