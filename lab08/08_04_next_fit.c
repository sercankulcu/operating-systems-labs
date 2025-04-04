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
MemoryBlock *lastAllocated = NULL; // Pointer to the last allocated block

// Function to initialize memory with a single free block
void initializeMemory() {
    memory = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    memory->size = MEMORY_SIZE;
    memory->isFree = 1;
    memory->next = NULL;
    lastAllocated = memory;
}

// Function to allocate memory using next fit strategy
void *nextFitAllocate(int size) {
    MemoryBlock *current = lastAllocated;
    while (current != NULL) {
        if (current->isFree && current->size >= size) {
            // Allocate memory
            if (current->size > size) {
                // Split the block if it's larger than requested
                MemoryBlock *newBlock = (MemoryBlock *)malloc(sizeof(MemoryBlock));
                newBlock->size = current->size - size;
                newBlock->isFree = 1;
                newBlock->next = current->next;
                current->next = newBlock;
                current->size = size;
            }
            current->isFree = 0;
            lastAllocated = current->next;
            printf("Memory (%d) allocated.\n", size);
            return (void *)(current + 1); // Return pointer to the data area of the block
        }
        current = current->next;
        if (current == NULL) {
            current = memory; // Wrap around to the beginning of the memory
        }
        if (current == lastAllocated) {
            break; // We've looped through all blocks and found no suitable one
        }
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
    void *ptr1 = nextFitAllocate(20);
    displayMemory();
    void *ptr2 = nextFitAllocate(30);
    displayMemory();
    nextFitAllocate(40);
    displayMemory();
    nextFitAllocate(20);

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

    nextFitAllocate(10);
    
    displayMemory();

    // Free memory allocated for memory block
    free(memory);

    return 0;
}
