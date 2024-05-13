#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_REQUESTS 100

// Function to find the index of the request with the shortest seek time
int find_shortest_seek(int requests[], int num_requests, int current_position, int visited[]) {
    int min_distance = INT_MAX;
    int index = -1;

    for (int i = 0; i < num_requests; i++) {
        if (!visited[i]) {
            int distance = abs(requests[i] - current_position);
            if (distance < min_distance) {
                min_distance = distance;
                index = i;
            }
        }
    }

    return index;
}

// Function to simulate SSTF disk arm scheduling
void sstf_disk_scheduling(int requests[], int num_requests, int initial_position) {
    int current_position = initial_position;
    int total_head_movement = 0;
    int visited[MAX_REQUESTS] = {0}; // Array to keep track of visited requests

    printf("SSTF Disk Arm Scheduling\n");

    // Process all requests
    for (int i = 0; i < num_requests; i++) {
        int next_index = find_shortest_seek(requests, num_requests, current_position, visited);
        if (next_index == -1) {
            break; // No more requests to process
        }

        int distance = abs(requests[next_index] - current_position);
        total_head_movement += distance;
        current_position = requests[next_index];
        visited[next_index] = 1;

        printf("Moved to track %d, Head movement: %d\n", current_position, distance);
    }

    printf("Total Head Movement: %d\n", total_head_movement);
}

int main() {
    
    int requests[MAX_REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67};
    int num_requests = 8;
    int initial_position = 53;

    // Simulate SSTF disk arm scheduling
    sstf_disk_scheduling(requests, num_requests, initial_position);

    return 0;
}

