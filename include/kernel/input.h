#ifndef _INPUT_H
#define _INPUT_H

#include "stdint.h"
#include "stddef.h"

#define INPUT_BUFFER_SIZE 256

typedef enum {
	KEY_PRESSED,
	KEY_RELEASED
} key_event_type;

typedef struct {
	key_event_type type;
	uint8_t scancode;
	char ascii;
} key_event;

void input_init();
void input_push_event(key_event event);
key_event input_pop_event();
int input_has_events();

#endif // _INPUT_H
