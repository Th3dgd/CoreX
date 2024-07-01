// src/mm/malloc.c

#include "../include/kernel.h"

typedef struct MemoryBlock {
    uint32_t size;
    struct MemoryBlock *next;
    int free;
} MemoryBlock;

#define MEMORY_SIZE 1024 * 1024

MemoryBlock *freeList;

static char memory[MEMORY_SIZE];

void memory_init()
{
    freeList = (MemoryBlock *)memory;
    freeList->size = MEMORY_SIZE - sizeof(MemoryBlock);
    freeList->next = NULL;
    freeList->free = 1;
}

void *malloc(uint32_t size)
{
    MemoryBlock *current = freeList;

    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            if (current->size > size + sizeof(MemoryBlock))
            {
                MemoryBlock *newBlock = (MemoryBlock *)((char *)current + sizeof(MemoryBlock) + size);
                newBlock->size = current->size - size - sizeof(MemoryBlock);
                newBlock->next = current->next;
                newBlock->free = 1;
                current->size = size;
                current->next = newBlock;
            }
            current->free = 0;
            return (void *)((char *)current + sizeof(MemoryBlock));
        }
        current = current->next;
    }

    return NULL;
}


void free(void *ptr)
{
    if (ptr == NULL)
        return;

    MemoryBlock *block = (MemoryBlock *)((char *)ptr - sizeof(MemoryBlock));
    block->free = 1;

    MemoryBlock *current = freeList;
    while (current != NULL)
    {
        if (current->free && current->next != NULL && current->next->free)
        {
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
        }
        current = current->next;
    }
}
