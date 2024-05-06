#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process

        // Define the command and arguments for execve
        char *args[] = {"/bin/ls", "-l", "/", NULL};

        // Execute a new program in the child process
        if (execve(args[0], args, NULL) == -1) {
            perror("Execve failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        system("ps ax");
        // Wait for the child process to terminate
        if (waitpid(pid, &status, 0) == -1) {
            perror("Waitpid failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            // Child process terminated normally
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            // Child process terminated abnormally
            printf("Child process terminated abnormally\n");
        }
    }

    return 0;
}

