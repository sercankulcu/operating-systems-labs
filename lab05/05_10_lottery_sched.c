#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    printf("Sequence of processes execution (tickets %d):\n", totalTickets);
    // Continue until all processes are completed
    while (completed != n) {
        // Generate a random number between 0 and totalTickets
        winner = rand() % totalTickets;
        int sum = 0;
        // Find the winning process based on the lottery ticket
        for (int i = 0; i < n; i++) {
            sum += processes[i].tickets;
            if (winner <= sum && !processes[i].completed) {
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
    int n = 5;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].tickets = 10;
    processes[0].completed = 0;
    processes[1].id = 2;
    processes[1].tickets = 8;
    processes[1].completed = 0;
    processes[2].id = 3;
    processes[2].tickets = 6;
    processes[2].completed = 0;
    processes[3].id = 4;
    processes[3].tickets = 4;
    processes[3].completed = 0;
    processes[4].id = 5;
    processes[4].tickets = 2;
    processes[4].completed = 0;

    // Display processes along with all details
    printf("Process\t\tTickets\n");

    // Calculate total waiting time and turnaround time
    for (int i = 0; i < n; i++) {
        printf("%d\t\t%d\n", processes[i].id, processes[i].tickets);
    }

    // Seed the random number generator
    srand(time(NULL));

    // Perform lottery scheduling
    lotteryScheduling(processes, n);

    return 0;
}

