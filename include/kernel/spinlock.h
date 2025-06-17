#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <stdint.h>

typedef struct {
	volatile uint32_t lock;
} spinlock_t;

void spinlock_init(spinlock_t* lock);
void spinlock_lock(spinlock_t* lock);
int spinlock_trylock(spinlock_t* lock);
void spinlock_unlock(spinlock_t* lock);

#endif // _SPINLOCK_H
