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
        manager->memory[i].size = 30; // Adjust this based on your simulation
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
    //system("cls"); // Clear the console screen (for Unix-like systems)

    //printf("Welcome to My Memory Manager\n");
    printf("Memory Status after Time Unit %d:\n", time_unit);

    printf("+-----------------+-------+------------+\n");
    printf("|   Process ID    | Size  |   Start    |\n");
    printf("+-----------------+-------+------------+\n");

    for (int i = 0; i < manager->memory_size; ++i) {
        if (manager->memory[i].process_id != NULL) {
            printf("| %15s | %5d | %10d |\n", manager->memory[i].process_id, manager->memory[i].size, manager->memory[i].start);
        }
    }

    printf("+-----------------+-------+------------+\n");

    printf("\nFree Memory Blocks:\n");

    printf("+-------+------------+\n");
    printf("| Size  |   Start    |\n");
    printf("+-------+------------+\n");

    for (int i = 0; i < manager->free_block_count; ++i) {
        printf("| %5d | %10d |\n", manager->free_blocks[i]->size, manager->free_blocks[i]->start);
    }

    printf("+-------+------------+\n");

    int fragmentation_loss = 0;

    for (int i = 0; i < manager->free_block_count; ++i) {
        fragmentation_loss += manager->free_blocks[i]->size;
    }

    printf("\nFragmentation Loss: %d\n", fragmentation_loss);
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

    printf("Welcome to My Memory Manager\n");

    // Allocate memory for 3 processes
    for (int i = 1; i <= 3; ++i) {
        printf("Enter the process name for process %d: ", i);
        scanf("%s", process_id);
        printf("Enter the size for process %d: ", i);
        scanf("%d", &process_size);

        if (allocate_memory(manager, process_id, process_size)) {
            printf("Allocated memory for process %s\n", process_id);
        } else {
            printf("Failed to allocate memory for process %s\n", process_id);
        }

        print_memory_status(manager, i);
    }

    // Deallocate memory for process P1
    strcpy(process_id, "P1");
    deallocate_memory(manager, process_id);
    printf("Deallocated memory for process %s\n", process_id);

    // Display memory status after deallocation
    print_memory_status(manager, 4);

    free_memory_manager(manager);

    return 0;
}
