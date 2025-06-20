#include <kernel/spinlock.h>
#include <kernel/string.h>
#include <kernel/mm.h>
#include <limine.h>

#define ALIGN(size) (((size) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))

volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

spinlock_t lock;
static struct mem_block* free_list = NULL;

void mem_init() {
	spinlock_init(&lock);

	struct limine_memmap_response* memmap = memmap_request.response;
	if (!memmap) return;

	for (size_t i = 0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry *entry = memmap->entries[i];

		if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= HEAP_MIN_SIZE) {
			struct mem_block* block = (struct mem_block*)entry->base;
			block->size = entry->length - sizeof(struct mem_block);
			block->next = free_list;
			free_list = block;
		}
	}
}

void* kmalloc(size_t size) {
	if (size == 0 || free_list == NULL) {	
		return NULL;
	}

	spinlock_lock(&lock);	
	size = ALIGN(size);

	struct mem_block* prev = NULL;
	struct mem_block* curr = free_list;
	while (curr != NULL) {
		if (curr->size >= size) {
			if (curr->size > size + sizeof(struct mem_block)) {
				struct mem_block *new_block = (struct mem_block*)((uintptr_t)curr + sizeof(struct mem_block) + size);
				new_block->size = curr->size - size - sizeof(struct mem_block);
				new_block->next = curr->next;
				
				curr->size = size;
				curr->next = new_block;
			}
			if (prev == NULL) {
				free_list = curr->next;
			} else {
				prev->next = curr->next;
			}

			spinlock_unlock(&lock);
			return (void*)((uintptr_t)curr + sizeof(struct mem_block));
		}
		
		prev = curr;
		curr = curr->next;
	}
	
	spinlock_unlock(&lock);
	return NULL;
}

void* kmalloc_aligned(size_t size, size_t alignment) {
	void* ptr = kmalloc(size + alignment - 1 + sizeof(void*));
	if (ptr == NULL) {
		return NULL;
	}

	spinlock_lock(&lock);
	
	uintptr_t aligned_ptr = ((uintptr_t)ptr + sizeof(void*) + alignment - 1) & ~(alignment - 1);
	*((void **)(aligned_ptr - sizeof(void *))) = ptr;
	
	spinlock_unlock(&lock);

	return (void *)aligned_ptr;
}

void kfree(void* ptr) {
	if (ptr == NULL) {
		return;
	}

	spinlock_lock(&lock);

	struct mem_block* block = (struct mem_block *)((uintptr_t)ptr - sizeof(struct mem_block));
	struct mem_block* curr = free_list;
	struct mem_block* prev = NULL;

	while (curr != NULL && curr < block) {
		prev = curr;
		curr = curr->next;
	}
	
	if (prev != NULL && (uintptr_t)prev + sizeof(struct mem_block) + prev->size == (uintptr_t)block) {
		prev->size += sizeof(struct mem_block) + block->size;
		block = prev;
	} else {
		block->next = curr;
		if (prev != NULL) {
			prev->next = block;
		} else {
			free_list = block;
		}
	}

	if (block->next != NULL &&
		(uintptr_t)block + sizeof(struct mem_block) + block->size == (uintptr_t)block->next) {
		block->size += sizeof(struct mem_block) + block->next->size;
		block->next = block->next->next;
	}

	spinlock_unlock(&lock);
}

size_t get_free_memory() {
	size_t total = 0;
	struct mem_block* curr = free_list;

	while (curr != NULL) {
		total += curr->size;
		curr = curr->next;
	}

	return total;
}
