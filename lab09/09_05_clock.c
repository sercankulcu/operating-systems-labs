#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Page table entry structure
typedef struct PageTableEntry {
    int pageNumber; // Page number
    bool referenced; // Reference bit
} PageTableEntry;

// Function to initialize the page table
void initializePageTable(PageTableEntry pageTable[], int numPages) {
    for (int i = 0; i < numPages; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].referenced = false;
    }
}

// Function to perform Clock page replacement
int clockReplacement(PageTableEntry pageTable[], int numPages, int *clockHand) {
    while (true) {
        // Check if the page at the current clock hand position has the reference bit set
        if (pageTable[*clockHand].referenced) {
            // Clear the reference bit and move the clock hand to the next page
            pageTable[*clockHand].referenced = false;
            *clockHand = (*clockHand + 1) % numPages;
        } else {
            // Return the index of the page at the current clock hand position to be replaced
            return *clockHand;
        }
    }
}

int main() {
    int numPages = 3; // Number of pages in memory
    PageTableEntry pageTable[numPages];

    // Initialize page table
    initializePageTable(pageTable, numPages);

    // Simulate page references
    int referenceString[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2};
    int numReferences = sizeof(referenceString) / sizeof(referenceString[0]);
    printf("Reference String: ");
    for (int i = 0; i < numReferences; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n");

    printf("\nSimulating Clock Page Replacement:\n");
    int clockHand = 0;
    int pageFaults = 0;

    for (int i = 0; i < numReferences; i++) {
        int pageNumber = referenceString[i];

        // Check if page is already in memory
        bool pageFault = true;
        for (int j = 0; j < numPages; j++) {
            if (pageTable[j].pageNumber == pageNumber) {
                pageTable[j].referenced = true; // Set reference bit
                pageFault = false;
                break;
            }
        }

        // If page is not in memory, perform page replacement
        if (pageFault) {
            pageFaults++;
            int indexToReplace = clockReplacement(pageTable, numPages, &clockHand);
            pageTable[indexToReplace].pageNumber = pageNumber;
            pageTable[indexToReplace].referenced = true; // Set reference bit
            clockHand = (indexToReplace + 1) % numPages; // Update clock hand
        }

        // Display current page table status
        printf("Page Table Status after reference %d: ", pageNumber);
        for (int j = 0; j < numPages; j++) {
            printf("(%d, R=%d) ", pageTable[j].pageNumber, pageTable[j].referenced);
        }
        printf("\n");
    }

    printf("Total page faults: %d\n", pageFaults);

    return 0;
}

