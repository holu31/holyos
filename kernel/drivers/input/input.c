#include <kernel/input.h>
#include <kernel/spinlock.h>

static key_event buffer[INPUT_BUFFER_SIZE];
static uint32_t head = 0;
static uint32_t tail = 0;
static spinlock_t lock;

void input_init() {
	spinlock_init(&lock);
	head = tail = 0;
}

void input_push_event(key_event event) {
	spinlock_lock(&lock);
	uint32_t next = (head + 1) % INPUT_BUFFER_SIZE;
	if (next != tail) {
		buffer[head] = event;
		head = next;
	}
	spinlock_unlock(&lock);
}

key_event input_pop_event() {
	spinlock_lock(&lock);
	key_event event = {0};
	if (tail != head) {
		event = buffer[tail];
		tail = (tail + 1) % INPUT_BUFFER_SIZE;
	}
	spinlock_unlock(&lock);
	return event;
}

int input_has_events() {
	return head != tail;
}
