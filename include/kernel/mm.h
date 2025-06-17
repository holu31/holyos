#ifndef _MM_H
#define _MM_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_MIN_SIZE (1 * 1024 * 1024)

struct mem_block {
    size_t size;
    struct mem_block* next;
};

void mem_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void* kmalloc_aligned(size_t size, size_t alignment);
size_t get_free_memory();

#endif // _MM_H
