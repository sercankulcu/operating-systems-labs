#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_SIZE 128

// Define message structure
struct message {
    long mtype; // message type (must be > 0)
    char mtext[MAX_MSG_SIZE]; // message data
};

int main() {
    key_t key;
    int msgid;
    struct message msg;

    // Generate a unique key
    if ((key = ftok(".", 'A')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Create a message queue
    if ((msgid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(1);
    }

    // Send a message
    msg.mtype = 1; // message type must be greater than 0
    strcpy(msg.mtext, "Hello, message queue!");
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Message sent: %s\n", msg.mtext);

    // Receive a message
    if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Message received: %s\n", msg.mtext);

    // Remove the message queue
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}

