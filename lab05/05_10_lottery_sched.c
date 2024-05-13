#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 100
#define MAX_TICKETS   1000

// Process structure
struct Process {
    int id;           // Process ID
    int tickets;      // Number of lottery tickets
    int completed;    // Flag to check if process completed
};

// Function to perform lottery scheduling
void lotteryScheduling(struct Process processes[], int n) {
    int totalTickets = 0;
    int completed = 0;
    int winner;

    // Calculate total number of tickets
    for (int i = 0; i < n; i++)
        totalTickets += processes[i].tickets;

    printf("Sequence of processes execution:\n");
    // Continue until all processes are completed
    while (completed != n) {
        // Generate a random number between 0 and totalTickets
        winner = rand() % totalTickets;
        int sum = 0;
        // Find the winning process based on the lottery ticket
        for (int i = 0; i < n; i++) {
            sum += processes[i].tickets;
            if (sum > winner && !processes[i].completed) {
                printf("Process %d is running\n", processes[i].id);
                processes[i].completed = 1;
                completed++;
                break;
            }
        }
    }
}

int main() {
    // Number of processes
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Array to store processes
    struct Process processes[MAX_PROCESSES];

    // Input process details
    printf("Enter number of tickets for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d", &processes[i].tickets);
        processes[i].completed = 0;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Perform lottery scheduling
    lotteryScheduling(processes, n);

    return 0;
}

