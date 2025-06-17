#include <kernel/spinlock.h>
#include <kernel/cpu.h>

void spinlock_init(spinlock_t* lock) {
	lock->lock = 0;
}

void spinlock_lock(spinlock_t* lock) {
	while (atomic_exchange(&lock->lock, 1))
		while (lock->lock)
			cpu_pause();
}

int spinlock_trylock(spinlock_t* lock) {
	return !atomic_exchange(&lock->lock, 1);
}

void spinlock_unlock(spinlock_t* lock) {
	atomic_exchange(&lock->lock, 0);
}
