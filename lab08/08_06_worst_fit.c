#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 100

// Structure to represent a memory block
typedef struct MemoryBlock {
    int size;             // Size of the block
    int isFree;           // Flag to indicate whether the block is free (1) or allocated (0)
    struct MemoryBlock *next;  // Pointer to the next block
} MemoryBlock;

MemoryBlock *memory = NULL;

// Function to initialize memory with a single free block
void initializeMemory() {
    memory = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory->size = MEMORY_SIZE;
    memory->isFree = 1;
    memory->next = NULL;
}

// Function to allocate memory using worst fit strategy
void *worstFitAllocate(int size) {
    MemoryBlock *current = memory;
    MemoryBlock *worstFit = NULL;
    int maxFragmentation = -1; // Initialize with a value smaller than any possible fragmentation

    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            int fragmentation = current->size - size;
            if (fragmentation > maxFragmentation) {
                maxFragmentation = fragmentation;
                worstFit = current;
            }
        }
        current = current->next;
    }

    if (worstFit != NULL) {
        // Allocate memory
        if (worstFit->size > size) {
            // Split the block if it's larger than requested
            MemoryBlock *newBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
            newBlock->size = worstFit->size - size;
            newBlock->isFree = 1;
            newBlock->next = worstFit->next;
            worstFit->next = newBlock;
            worstFit->size = size;
        }
        worstFit->isFree = 0;
        printf("Memory (%d) allocated.\n", size);
        return (void *)(worstFit + 1); // Return pointer to the data area of the block
    }
    printf("Memory (%d) allocation failed.\n", size);
    return NULL; // Memory allocation failed
}

// Function to free allocated memory
void deallocate(void *ptr) {
    if (ptr == NULL) return;
    MemoryBlock *block = (MemoryBlock *)ptr - 1;
    block->isFree = 1;
    printf("Memory (%d) deallocated.\n", block->size);
}

// Function to display memory blocks
void displayMemory() {
    MemoryBlock *current = memory;
    printf("Memory Layout: ");
    while (current != NULL) {
        printf("(%d, %s)", current->size, current->isFree ? "free" : "used");
        current = current->next;
    }
    printf("\n");
}

int main() {
    initializeMemory();
    
    displayMemory();
    // Allocate memory using next fit
    void *ptr1 = worstFitAllocate(30);
    displayMemory();
    void *ptr2 = worstFitAllocate(20);
    displayMemory();
    void *ptr3 = worstFitAllocate(40);
    displayMemory();
    void *ptr4 = worstFitAllocate(20);

    // Display memory layout after allocation
    displayMemory();

    // Free memory
    deallocate(ptr1);
    
    // Display memory layout after deallocation
    displayMemory();

    // Free memory
    deallocate(ptr2);
    
    // Display memory layout after deallocation
    displayMemory();

    ptr4 = worstFitAllocate(20);
    
    displayMemory();

    // Free memory allocated for memory block
    free(memory);

    return 0;
}

/* We've implemented the worstFitAllocate() function to search for the worst-fit free block that maximizes internal fragmentation.
It iterates through the memory blocks, calculates the fragmentation for each free block, and selects the block with the maximum fragmentation that is large enough to hold the requested size.
*/
