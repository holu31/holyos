#include <kernel/i8042.h>
#include <kernel/input.h>
#include <kernel/interrupt.h>
#include "keymap.h"

static int shift_pressed = 0;

static void ps2kbd_handler(struct trap_frame *r) {
	uint8_t scancode = i8042_read_data();
	key_event event;

	if (scancode == 0x2A || scancode == 0x36) {
        	shift_pressed = 1;
	} else if (scancode == 0xAA || scancode == 0xB6) {
		shift_pressed = 0;
       	}

	event.type = (scancode & 0x80) ? KEY_RELEASED : KEY_PRESSED;
	event.scancode = scancode & 0x7F;
	event.ascii = (event.type == KEY_PRESSED) ?
		keymap_to_ascii(event.scancode, shift_pressed) : 0;

	input_push_event(event);
}

int ps2kbd_init(void) {
	irq_request(33, ps2kbd_handler);
	return 0;
}
