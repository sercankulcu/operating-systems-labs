#include <stdio.h>
#include <stdlib.h>

// Process structure
struct Process {
    int id;         // Process ID
    int weight;     // Weight representing fair share
    int executed;   // Time already executed
};

// Function to perform fair-share scheduling
void fairShareScheduling(struct Process processes[], int n, int totalTime) {
    int remainingTime[n];
    float completionRatio[n];
    int completed = 0;

    // Initialize remaining time for each process
    for (int i = 0; i < n; i++)
        remainingTime[i] = processes[i].weight;

    printf("Sequence of processes execution:\n");
    // Continue until all processes are completed or total time is reached
    for (int currentTime = 0; currentTime < totalTime && completed != n; currentTime++) {
        float maxRatio = 0;
        int maxIndex = -1;
        // Find the process with the maximum completion ratio
        for (int i = 0; i < n; i++) {
            completionRatio[i] = (float)remainingTime[i] / processes[i].weight;
            if (completionRatio[i] > maxRatio) {
                maxRatio = completionRatio[i];
                maxIndex = i;
            }
        }
        // Execute the process with the maximum completion ratio
        if (maxIndex != -1) {
            printf("Process %d (%f) is running time %d\n", processes[maxIndex].id, completionRatio[maxIndex], currentTime);
            processes[maxIndex].executed = 1;
            remainingTime[maxIndex]--;
            if (remainingTime[maxIndex] == 0)
                completed++;
        }
    }
}

int main() {
    // Number of processes and total time
    int n = 5;
    int totalTime = 30;

    // Array to store processes
    struct Process processes[n];

    // Input process details
    processes[0].id = 1;
    processes[0].weight = 10;
    processes[0].executed = 0;
    processes[1].id = 2;
    processes[1].weight = 8;
    processes[1].executed = 0;
    processes[2].id = 3;
    processes[2].weight = 6;
    processes[2].executed = 0;
    processes[3].id = 4;
    processes[3].weight = 4;
    processes[3].executed = 0;
    processes[4].id = 5;
    processes[4].weight = 2;
    processes[4].executed = 0;

    // Perform fair-share scheduling
    fairShareScheduling(processes, n, totalTime);

    return 0;
}

