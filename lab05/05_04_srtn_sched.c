#include <stdio.h>
#include <limits.h>

// Process structure
struct Process {
    int id;      // Process ID
    int burst;   // Burst time
    int arrival; // Arrival time
    int remaining; // Remaining burst time
};

// Function to find the process with the shortest remaining time
int findShortestProcess(struct Process processes[], int n, int currentTime) {
    int shortest = -1;
    int shortestTime = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (processes[i].arrival <= currentTime && processes[i].remaining < shortestTime && processes[i].remaining > 0) {
            shortest = i;
            shortestTime = processes[i].remaining;
        }
    }

    return shortest;
}

// Function to perform SRTN scheduling
void srtNScheduling(struct Process processes[], int n) {
    int currentTime = 0;
    int completed = 0;
    int prevProcess = -1;

    while (completed != n) {
        int shortest = findShortestProcess(processes, n, currentTime);

        if (shortest == -1) {
            currentTime++;
        } else {
            // Execute the process with shortest remaining time
            if (prevProcess != shortest) {
                printf("Process %d is running from time %d to ", processes[shortest].id, currentTime);
            }
            processes[shortest].remaining--;

            // If process is completed, update completion time and print
            if (processes[shortest].remaining == 0) {
                printf("%d\n", currentTime + 1);
                completed++;
            }

            prevProcess = shortest;
            currentTime++;
        }
    }
}

int main() {
    // Number of processes
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Array to store processes
    struct Process processes[n];

    // Input process details
    printf("Enter burst time and arrival time for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        processes[i].id = i + 1;
        scanf("%d %d", &processes[i].burst, &processes[i].arrival);
        processes[i].remaining = processes[i].burst;
    }

    // Perform SRTN scheduling
    printf("Sequence of processes execution:\n");
    srtNScheduling(processes, n);

    return 0;
}

