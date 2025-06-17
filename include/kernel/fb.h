#ifndef _FB_H
#define _FB_H

#include <kernel/driver.h>

#include <stddef.h>
#include <stdint.h>

void fb_drawpixel(uint32_t x, uint32_t y, uint32_t color);
driver_status_t fb_probe(void);
driver_status_t fb_write(const char* buf);
driver_status_t fb_init(void);

#endif // _FB_H
