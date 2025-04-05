#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Function to simulate FIFO page replacement
void fifoPageReplacement(int *referenceString, int numReferences, int numFrames) {
    int pageFaults = 0;
    int *frames = (int *)malloc(numFrames * sizeof(int));
    for (int i = 0; i < numFrames; i++)
        frames[i] = -1; // Initialize frames as empty

    int oldestPageIndex = 0;
    int frameCount = 0; // Track how many frames are filled

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];
        bool pageFault = true;

        // Check if page is already in frames
        for (int j = 0; j < numFrames; j++) {
            if (frames[j] == pageNumber) {
                pageFault = false;
                break;
            }
        }

        // If page is not in frames
        if (pageFault) {
            pageFaults++;
            if (frameCount < numFrames) { // Fill empty frames first
                frames[frameCount] = pageNumber;
                frameCount++;
            } else { // Replace the oldest page
                printf("Page %d replaced by %d\n", frames[oldestPageIndex], pageNumber);
                frames[oldestPageIndex] = pageNumber;
                oldestPageIndex = (oldestPageIndex + 1) % numFrames;
            }
        }

        // Print frames after each memory access
        printf("Memory after accessing page %d: ", pageNumber);
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
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    int numFrames = 3; // Number of frames in memory

    printf("FIFO Page Replacement Simulation:\n");
    fifoPageReplacement(referenceString, numReferences, numFrames);

    return 0;
}
