#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct {
    int pageNumber;       // Page number
    bool referenceBit;    // Reference bit (R)
    int lastReferenced;   // Time of last reference
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].referenceBit = false;
        pageTable[i].lastReferenced = -1;
    }
}

// Function to perform Working Set Clock (WSClock) page replacement
int wsclockReplacement(PageTableEntry pageTable[], int numPages, int* clockHand, int currentTime, int timeWindow) {

    int checkedPages = 0;

    while (checkedPages < numPages) {
        PageTableEntry* currentPage = &pageTable[*clockHand];
        
        if (currentPage->pageNumber == -1) { // Empty slot
            int victim = *clockHand;
            *clockHand = (*clockHand + 1) % numPages;
            return victim;
        }

        // Check if page is outside the working set and not referenced
        bool inWorkingSet = (currentTime - currentPage->lastReferenced) <= timeWindow;
        if (!currentPage->referenceBit && !inWorkingSet) {
            int victim = *clockHand;
            *clockHand = (*clockHand + 1) % numPages;
            return victim;
        }

        // Give a second chance if referenced
        if (currentPage->referenceBit) {
            currentPage->referenceBit = false;
        }

        *clockHand = (*clockHand + 1) % numPages;
        checkedPages++;

        // If we've gone full circle, pick the next non-referenced page or oldest
        if (checkedPages == numPages) {
            while (true) {
                if (!pageTable[*clockHand].referenceBit) {
                    int victim = *clockHand;
                    *clockHand = (*clockHand + 1) % numPages;
                    return victim;
                }
                pageTable[*clockHand].referenceBit = false;
                *clockHand = (*clockHand + 1) % numPages;
            }
        }
    }

    return *clockHand; // Fallback (shouldn’t reach here)
}

int main() {
    int numPages = 3; // Number of pages in memory
    PageTableEntry pageTable[numPages];
    int clockHand = 0;
    int timeWindow = 3; // Time window to define the working set
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

    printf("\nSimulating Working Set Clock Page Replacement (Window = %d):\n", timeWindow);
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory
        bool pageFound = false;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].referenceBit = true;
                pageTable[j].lastReferenced = currentTime;
                pageFound = true;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (!pageFound) {
            pageFaults++;
            int pageToReplace = wsclockReplacement(pageTable, numPages, &clockHand, currentTime, timeWindow);
            printf("Page %d replaced by %d (Clock hand at %d)\n", 
                   pageTable[pageToReplace].pageNumber, pageNumber, clockHand);
            pageTable[pageToReplace].pageNumber = pageNumber;
            pageTable[pageToReplace].referenceBit = true;
            pageTable[pageToReplace].lastReferenced = currentTime;
        }

        // Update current time
        currentTime++;

        // Print page table status
        printf("Page Table Status after reference %d (t=%d): ", pageNumber, currentTime);
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber != -1) {
                bool inWorkingSet = (currentTime - pageTable[j].lastReferenced) <= timeWindow;
                printf("(%d, R=%d, WS=%d) ", pageTable[j].pageNumber, pageTable[j].referenceBit, inWorkingSet);
            } else {
                printf("_ ");
            }
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);
    return 0;
}
