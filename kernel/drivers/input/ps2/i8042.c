#include <kernel/i8042.h>
#include <asm/io.h>

void i8042_init() {
	i8042_write_cmd(I8042_CMD_DISABLE_KBD);
	i8042_write_cmd(I8042_CMD_DISABLE_MOUSE);

	while (inb(I8042_STATUS_PORT) & 0x1) {
		inb(I8042_DATA_PORT);
	}

	i8042_write_cmd(I8042_CMD_ENABLE_KBD);
	i8042_write_cmd(I8042_CMD_ENABLE_MOUSE);
}

int i8042_check() {
	uint8_t status;
	asm volatile ("inb %1, %0" : "=a"(status) : "Nd"(I8042_STATUS_PORT));

	if (status == 0x00 || status == 0xFF) return -1;

	i8042_write_cmd(I8042_CMD_DISABLE_KBD);
	i8042_write_cmd(I8042_CMD_DISABLE_MOUSE);

	i8042_write_cmd(0xAA);
	uint8_t response = i8042_read_data();
	if (response != 0x55) return -1;

	i8042_write_cmd(I8042_CMD_ENABLE_KBD);
	i8042_write_cmd(I8042_CMD_ENABLE_MOUSE);

	return 0;
}

uint8_t i8042_read_data() {
	while (!(inb(I8042_STATUS_PORT) & I8042_STATUS_OUTPUT_FULL));
	return inb(I8042_DATA_PORT);
}

void i8042_write_data(uint8_t data) {
	while ((inb(I8042_STATUS_PORT) & I8042_STATUS_INPUT_FULL));
	outb(I8042_DATA_PORT, data);
}

void i8042_write_cmd(uint8_t cmd) {
	while ((inb(I8042_STATUS_PORT) & I8042_STATUS_INPUT_FULL));
	outb(I8042_CMD_PORT, cmd);
}
