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
    // Number of queues and time quantum
    int num_queues, quantum;

    printf("Enter the number of queues: ");
    scanf("%d", &num_queues);

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    // Initialize queues
    struct Queue queues[num_queues];
    for (int i = 0; i < num_queues; i++)
        initQueue(&queues[i]);

    // Input processes for each queue
    for (int i = 0; i < num_queues; i++) {
        int num_processes;
        printf("Enter the number of processes in Queue %d: ", i + 1);
        scanf("%d", &num_processes);

        printf("Enter burst time and priority for each process in Queue %d:\n", i + 1);
        for (int j = 0; j < num_processes; j++) {
            struct Process p;
            printf("Process %d: ", j + 1);
            p.id = j + 1;
            scanf("%d %d", &p.burst, &p.priority);
            enqueue(&queues[i], &p);
        }
    }

    // Perform Multiple Queues Scheduling
    printf("Sequence of processes execution:\n");
    multiQueueScheduling(queues, num_queues, quantum);

    return 0;
}

