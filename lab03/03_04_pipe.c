#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h> // Include for pid_t

#define BUFFER_SIZE 256

int main() {
    int fd[2]; // File descriptors for the pipe
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process (reads from the pipe)
        close(fd[1]); // Close the write end of the pipe

        // Read from the pipe
        printf("Child process reading from pipe...\n");
        ssize_t num_bytes = read(fd[0], buffer, BUFFER_SIZE);
        if (num_bytes == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Null-terminate the string
        buffer[num_bytes] = '\0';
        printf("Child process received message: %s\n", buffer);

        // Close the read end of the pipe
        close(fd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process (writes to the pipe)
        close(fd[0]); // Close the read end of the pipe

        // Write to the pipe
        printf("Parent process writing to pipe...\n");
        const char *message = "Hello, child process!";
        ssize_t num_bytes = write(fd[1], message, strlen(message));
        if (num_bytes == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        // Close the write end of the pipe
        close(fd[1]);

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}

