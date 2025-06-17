#ifndef _I8042_H
#define _I8042_H

#include "stdint.h"

#define I8042_DATA_PORT		0x60
#define I8042_STATUS_PORT	0x64
#define I8042_CMD_PORT		0x64

#define I8042_STATUS_OUTPUT_FULL	(1 << 0)
#define I8042_STATUS_INPUT_FULL		(1 << 1)

#define I8042_CMD_DISABLE_KBD	0xAD
#define I8042_CMD_ENABLE_KBD	0xAE
#define I8042_CMD_DISABLE_MOUSE	0xA7
#define I8042_CMD_ENABLE_MOUSE	0xA8

void i8042_init();
int i8042_check();
uint8_t i8042_read_data();
void i8042_write_data(uint8_t data);
void i8042_write_cmd(uint8_t cmd);

#endif // _I8042_H
