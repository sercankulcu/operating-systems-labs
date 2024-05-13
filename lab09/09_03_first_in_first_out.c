#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Function to find the index of the oldest page in memory
int findOldestPage(int *pages, int numFrames) {
    int oldestIndex = 0;
    for (int i = 1; i < numFrames; i++) {
        if (pages[i] < pages[oldestIndex])
            oldestIndex = i;
    }
    return oldestIndex;
}

// Function to simulate FIFO page replacement
void fifoPageReplacement(int *referenceString, int numReferences, int numFrames) {
    int pageFaults = 0;
    int *frames = (int *)malloc(numFrames * sizeof(int));

    for (int i = 0; i < numFrames; i++)
        frames[i] = -1; // Initialize frames as empty

    int oldestPageIndex = 0;

    for (int i = 0; i < numReferences; i++) {
        bool pageFault = true;

        // Check if page is already in frames
        for (int j = 0; j < numFrames; j++) {
            if (frames[j] == referenceString[i]) {
                pageFault = false;
                break;
            }
        }

        // If page is not in frames, replace the oldest page with the new page
        if (pageFault) {
            pageFaults++;
            frames[oldestPageIndex] = referenceString[i];
            oldestPageIndex = (oldestPageIndex + 1) % numFrames; // Update the index of the oldest page
        }

        // Print frames after each memory access
        printf("Memory after accessing page %d: ", referenceString[i]);
        for (int j = 0; j < numFrames; j++) {
            if (frames[j] != -1)
                printf("%d ", frames[j]);
            else
                printf("_ ");
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);

    free(frames);
}

int main() {
    int referenceString[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    int numFrames = 3; // Number of frames in memory

    printf("FIFO Page Replacement Simulation:\n");
    fifoPageReplacement(referenceString, numReferences, numFrames);

    return 0;
}

