#include <unistd.h>    // For write() system call
#include <string.h>    // For strlen() function
#include <stdio.h>     // For perror() function
#include <stdlib.h>    // For exit() function

int main() {
    char *message = "Hello, world!\n"; // Message to write
    size_t message_len = strlen(message); // Length of the message

    // Write the message to stdout (file descriptor 1)
    ssize_t bytes_written = write(1, message, message_len);

    if (bytes_written == -1) {
        // Error handling if write fails
        perror("Error writing message");
        return 1;
    }

    return 0;
}

