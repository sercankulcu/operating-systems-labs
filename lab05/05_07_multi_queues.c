#include <stdio.h>

#define MAX_PROCESSES 100
#define MAX_QUEUE_SIZE 100

// Process structure
struct Process {
    int id;         // Process ID
    int burst;      // Burst time
    int priority;   // Priority
    int completed;  // Flag to check if process completed
};

// Queue structure
struct Queue {
    struct Process *processes[MAX_QUEUE_SIZE];  // Array to store processes
    int front, rear;  // Front and rear pointers of the queue
};

// Function to initialize a queue
void initQueue(struct Queue *q) {
    q->front = q->rear = -1;
}

// Function to check if the queue is empty
int isEmpty(struct Queue *q) {
    return q->front == -1;
}

// Function to add a process to the queue
void enqueue(struct Queue *q, struct Process *p) {
    if (q->rear == MAX_QUEUE_SIZE - 1) {
        printf("Queue overflow\n");
        return;
    }

    if (q->front == -1)
        q->front = 0;
    q->rear++;
    q->processes[q->rear] = p;
}

// Function to remove a process from the queue
struct Process* dequeue(struct Queue *q) {
    if (isEmpty(q)) {
        printf("Queue underflow\n");
        return NULL;
    }

    struct Process *p = q->processes[q->front];
    if (q->front == q->rear)
        q->front = q->rear = -1;
    else
        q->front++;

    return p;
}

// Function to perform Multiple Queues Scheduling
void multiQueueScheduling(struct Queue *queues, int num_queues, int quantum) {
    int currentTime = 0;
    int completed = 0;
    int currentQueue = 0;

    while (completed != num_queues) {
        // If the current queue is empty, move to the next queue
        if (isEmpty(&queues[currentQueue])) {
            currentQueue = (currentQueue + 1) % num_queues;
            continue;
        }

        struct Process *p = dequeue(&queues[currentQueue]);

        // Execute the process for the quantum or its remaining time, whichever is smaller
        if (p->burst <= quantum) {
            currentTime += p->burst;
            printf("Process %d from Queue %d is running from time %d to %d\n", p->id, currentQueue + 1, currentTime - p->burst, currentTime);
            p->completed = 1;
            completed++;
        } else {
            currentTime += quantum;
            printf("Process %d from Queue %d is running from time %d to %d\n", p->id, currentQueue + 1, currentTime - quantum, currentTime);
            p->burst -= quantum;
            enqueue(&queues[currentQueue], p);
        }

        currentQueue = (currentQueue + 1) % num_queues;
    }
}

int main() {

    // Number of processes
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Number of queues and time quantum
    int num_queues = 3;
    int quantum = 2;

    // Initialize queues
    struct Queue queues[num_queues];
    for (int i = 0; i < num_queues; i++) {
        initQueue(&queues[i]);
    }

    // Input process details
    processes[0].id = 1;
    processes[0].burst = 10;
    processes[0].priority = 1;
    processes[0].completed = 0;
    enqueue(&queues[0], &processes[0]);
    processes[1].id = 2;
    processes[1].burst = 8;
    processes[1].priority = 2;
    processes[1].completed = 0;
    enqueue(&queues[1], &processes[1]);
    processes[2].id = 3;
    processes[2].burst = 6;
    processes[2].priority = 5;
    processes[2].completed = 0;
    enqueue(&queues[2], &processes[2]);
    processes[3].id = 4;
    processes[3].burst = 4;
    processes[3].priority = 3;
    processes[3].completed = 0;
    enqueue(&queues[0], &processes[3]);
    processes[4].id = 5;
    processes[4].burst = 2;
    processes[4].priority = 4;
    processes[4].completed = 0;
    enqueue(&queues[1], &processes[4]);

    // Perform Multiple Queues Scheduling
    printf("Sequence of processes execution:\n");
    multiQueueScheduling(queues, num_queues, quantum);

    return 0;
}

