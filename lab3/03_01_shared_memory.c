#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

int main() {
    int shmid;
    char *shmaddr;
    pid_t pid;

    // Create a shared memory segment
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment to the process's address space
    if ((shmaddr = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Write data to the shared memory
    sprintf(shmaddr, "Hello, shared memory!");

    // Fork a child process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process reads from shared memory
        printf("Child process reads from shared memory: %s\n", shmaddr);
        exit(0);
    } else {
        // Parent process waits for the child to terminate
        wait(NULL);
        printf("Parent process finishes.\n");

        // Detach and remove the shared memory segment
        shmdt(shmaddr);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

