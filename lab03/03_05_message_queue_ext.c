// Include necessary header files for message queue operations
#include <stdio.h>      // For printf and perror
#include <stdlib.h>     // For exit
#include <string.h>     // For string operations
#include <unistd.h>     // For sleep
#include <sys/types.h>  // For key_t and other types
#include <sys/ipc.h>    // For IPC operations
#include <sys/msg.h>    // For message queue functions
#include <time.h>       // For time functions

// Define constants
#define MAX_MSG_SIZE 128    // Maximum size of message text
#define MSG_TYPE_1   1      // Message type for sender 1
#define MSG_TYPE_2   2      // Message type for sender 2

// Define message structure as required by System V message queues
struct message {
    long mtype;             // Message type (must be > 0)
    char mtext[MAX_MSG_SIZE]; // Message data buffer
};

// Function to display current time with message
void print_timestamp(const char *action, const char *msg) {
    time_t now;
    time(&now);
    printf("%s: %s - %s\n", action, ctime(&now), msg);
}

int main() {
    key_t key;              // Key for message queue
    int msgid;             // Message queue identifier
    struct message msg;    // Message buffer for sending/receiving
    pid_t pid;             // Process ID for forking
    
    // Generate a unique key based on current directory and character 'A'
    if ((key = ftok(".", 'A')) == -1) {
        perror("ftok failed");
        exit(1);
    }
    
    // Create or get message queue with read/write permissions (0666)
    if ((msgid = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }
    
    // Fork to create a child process
    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) { // Child process - Sender
        // Prepare first message
        msg.mtype = MSG_TYPE_1;
        snprintf(msg.mtext, MAX_MSG_SIZE, "Message from child (PID: %d)", getpid());
        
        // Send first message
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd failed in child");
            exit(1);
        }
        print_timestamp("Child sent", msg.mtext);
        
        // Sleep to simulate processing time
        sleep(1);
        
        // Prepare and send second message
        msg.mtype = MSG_TYPE_2;
        strcpy(msg.mtext, "Second message from child");
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd failed in child (second)");
            exit(1);
        }
        print_timestamp("Child sent", msg.mtext);
        
        exit(0); // Child process terminates
    }
    else { // Parent process - Receiver
        // Wait briefly for child to send messages
        sleep(2);
        
        // Receive first message (type 1)
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), MSG_TYPE_1, 0) == -1) {
            perror("msgrcv failed for type 1");
            exit(1);
        }
        print_timestamp("Parent received", msg.mtext);
        
        // Receive second message (type 2)
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), MSG_TYPE_2, 0) == -1) {
            perror("msgrcv failed for type 2");
            exit(1);
        }
        print_timestamp("Parent received", msg.mtext);
        
        // Get and display queue statistics
        struct msqid_ds buf;
        if (msgctl(msgid, IPC_STAT, &buf) == -1) {
            perror("msgctl stat failed");
            exit(1);
        }
        printf("Messages in queue: %lu\n", buf.msg_qnum);
        printf("Last sender PID: %d\n", buf.msg_lspid);
        
        // Remove the message queue
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl remove failed");
            exit(1);
        }
        printf("Message queue removed successfully\n");
    }
    
    return 0; // Successful completion
}
