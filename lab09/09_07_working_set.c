#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct PageTableEntry {
    int pageNumber;        // Page number
    int lastReferenced;    // Time of last reference
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].lastReferenced = -1; // No reference yet
    }
}

// Function to perform Working Set page replacement
int workingSetReplacement(PageTableEntry pageTable[], int numPages, int currentTime, int timeWindow) {
    int pageToReplace = -1;

    // Find a page outside the working set (last referenced before the window)
    for (int i = 0; i < numPages; i++) {
        if (pageTable[i].pageNumber != -1 && (currentTime - pageTable[i].lastReferenced) > timeWindow) {
            pageToReplace = i;
            break;
        }
    }

    // If all pages are within the working set or no page is found, pick the oldest page
    if (pageToReplace == -1) {
        int oldestTime = currentTime + 1; // Set to a value greater than currentTime
        for (int i = 0; i < numPages; i++) {
            if (pageTable[i].pageNumber == -1) { // Prefer empty slot
                pageToReplace = i;
                break;
            } else if (pageTable[i].lastReferenced < oldestTime) {
                oldestTime = pageTable[i].lastReferenced;
                pageToReplace = i;
            }
        }
    }

    return pageToReplace;
}

int main() {
    int numPages = 3; // Number of pages in memory
    PageTableEntry pageTable[numPages];
    int timeWindow = 3; // Working set window size
    int currentTime = 0;

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Working Set Page Replacement (Window = %d):\n", timeWindow);
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory
        bool pageFound = false;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].lastReferenced = currentTime; // Update last reference time
                pageFound = true;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (!pageFound) {
            pageFaults++;
            int pageToReplace = workingSetReplacement(pageTable, numPages, currentTime, timeWindow);
            printf("Page %d replaced by %d\n", 
                   pageTable[pageToReplace].pageNumber == -1 ? -1 : pageTable[pageToReplace].pageNumber, 
                   pageNumber);
            pageTable[pageToReplace].pageNumber = pageNumber;
            pageTable[pageToReplace].lastReferenced = currentTime;
        }

        // Update current time
        currentTime++;

        // Print page table status
        printf("Page Table Status after reference %d (t=%d): ", pageNumber, currentTime);
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber != -1) {
                bool inWorkingSet = (currentTime - pageTable[j].lastReferenced) <= timeWindow;
                printf("(%d, T=%d, WS=%d) ", pageTable[j].pageNumber, pageTable[j].lastReferenced, inWorkingSet);
            } else {
                printf("_ ");
            }
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);
    return 0;
}
