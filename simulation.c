#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 50

typedef struct {
    int start;
    int size;
    char* process_id;
} MemoryBlock;

typedef struct {
    int memory_size;
    MemoryBlock* memory;
    MemoryBlock** free_blocks;
    int free_block_count;
} MemoryManager;

MemoryManager* initialize_memory_manager(int memory_size) {
    MemoryManager* manager = (MemoryManager*)malloc(sizeof(MemoryManager));
    manager->memory_size = memory_size;
    manager->memory = (MemoryBlock*)malloc(memory_size * sizeof(MemoryBlock));
    manager->free_blocks = (MemoryBlock**)malloc(memory_size * sizeof(MemoryBlock*));
    manager->free_blocks[0] = manager->memory;
    manager->free_block_count = 1;
    for (int i = 0; i < memory_size; ++i) {
        manager->memory[i].start = i;
        manager->memory[i].size = 10; // Adjust this based on your simulation
        manager->memory[i].process_id = NULL;
    }
    return manager;
}

int allocate_memory(MemoryManager* manager, char* process_id, int size) {
    for (int i = 0; i < manager->free_block_count; ++i) {
        MemoryBlock* block = manager->free_blocks[i];
        if (block->size >= size) {
            block->process_id = process_id;

            // Update free blocks list
            if (block->size > size) {
                MemoryBlock* new_block = block + size;
                new_block->start = block->start + size;
                new_block->size = block->size - size;
                new_block->process_id = NULL;
                manager->free_blocks[manager->free_block_count++] = new_block;
            }

            // Remove the allocated block from the free blocks list
            for (int j = i; j < manager->free_block_count - 1; ++j) {
                manager->free_blocks[j] = manager->free_blocks[j + 1];
            }
            manager->free_block_count--;

            return 1; // Allocation successful
        }
    }

    return 0; // Allocation failed
}

void deallocate_memory(MemoryManager* manager, char* process_id) {
    for (int i = 0; i < manager->memory_size; ++i) {
        if (manager->memory[i].process_id != NULL && strcmp(manager->memory[i].process_id, process_id) == 0) {
            manager->memory[i].process_id = NULL;
            manager->free_blocks[manager->free_block_count++] = &(manager->memory[i]);
        }
    }
}

void print_memory_status(MemoryManager* manager, int time_unit) {
    printf("Memory Status after Time Unit %d:\n", time_unit);
    printf("Allocated Memory: ");
    for (int i = 0; i < manager->memory_size; ++i) {
        if (manager->memory[i].process_id != NULL) {
            printf("[Start: %d, Size: %d, Process ID: %s] ", manager->memory[i].start, manager->memory[i].size, manager->memory[i].process_id);
        }
    }

    printf("\nFree Memory: ");
    for (int i = 0; i < manager->free_block_count; ++i) {
        printf("[Start: %d, Size: %d] ", manager->free_blocks[i]->start, manager->free_blocks[i]->size);
    }

    int fragmentation_loss = 0;
    int wasted_memory_blocks = manager->free_block_count;

    for (int i = 0; i < manager->free_block_count; ++i) {
        fragmentation_loss += manager->free_blocks[i]->size;
    }

    printf("\nFragmentation Loss: %d", fragmentation_loss);
    printf("\nWasted Memory Blocks: %d\n\n", wasted_memory_blocks);
}

void free_memory_manager(MemoryManager* manager) {
    free(manager->memory);
    free(manager->free_blocks);
    free(manager);
}

int main() {
    MemoryManager* manager = initialize_memory_manager(MEMORY_SIZE);

    // Example processes and sizes
    char process_id[5];
    int process_size;

    // Allocate memory for 3 processes
    for (int i = 1; i <= 3; ++i) {
        sprintf(process_id, "P%d", i);
        process_size = 5; // process size
        if (allocate_memory(manager, process_id, process_size)) {
            printf("Allocated memory for process %s\n", process_id);
        } else {
            printf("Failed to allocate memory for process %s\n", process_id);
        }
    }

    // Display memory status after all allocations
    print_memory_status(manager, 1);

    // Deallocate memory for process P2
    strcpy(process_id, "P2");
    deallocate_memory(manager, process_id);
    printf("Deallocated memory for process %s\n", process_id);

    // Display memory status after deallocation
    print_memory_status(manager, 2);

    free_memory_manager(manager);

    return 0;
}
