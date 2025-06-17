#include <kernel/string.h>
#include <kernel/fb.h>
#include <limine.h>
#include "font.h"

static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static struct limine_framebuffer *bootloader_fb;

uint32_t text_color = 0xffff00;
uint32_t bg_color = 0x0000ff;
size_t posx = 0;
size_t posy = 0;

void fb_drawpixel(uint32_t x, uint32_t y, uint32_t color) {
	if (!bootloader_fb || x >= bootloader_fb->width || y >= bootloader_fb->height) return;

	uint8_t* pixel = (uint8_t*)bootloader_fb->address + y * bootloader_fb->pitch + x * (bootloader_fb->bpp / 8);

	uint8_t a = (color >> 24) & 0xFF;
	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = color & 0xFF;

	switch (bootloader_fb->bpp) {
		case 32:
			pixel[2] = r;
			pixel[1] = g;
			pixel[0] = b;
			pixel[3] = a;
			break;
		case 24:
			pixel[2] = r;
			pixel[1] = g;
			pixel[0] = b;
			break;
		case 16:
			*(uint16_t*)pixel = ((r >> 3) << 11) |
				((g >> 2) << 5) |
				(b >> 3);
			break;
	}
}

driver_status_t fb_probe(void) {
	if (framebuffer_request.response->framebuffer_count < 1)
		return DRIVER_STATUS_ERROR;
	return DRIVER_STATUS_OK;
}

driver_status_t fb_putc(const char c) {
	if (!bootloader_fb) return DRIVER_STATUS_DEVICE_NOT_FOUND;

	if (c == '\n') {
		posy += FONT_HEIGHT + 1;
		posx = 0;
		return DRIVER_STATUS_OK;
	}
	
	if ((uint8_t)c < 32 || (uint8_t)c > 127)
		return DRIVER_STATUS_OK;
	
	const uint8_t *glyph = font[(uint8_t)c - 32];

	for (int y = 0; y < FONT_HEIGHT; y++) {
		uint8_t row = glyph[y];
		for (int x = 0; x < FONT_WIDTH; x++) {
			if (row & (1 << x)) {
				fb_drawpixel(posx + x, posy + y, text_color);
			}
		}
	}

	posx += FONT_WIDTH + 1; 
	return DRIVER_STATUS_OK;
}

driver_status_t fb_write(const char *buf) {
	while (*buf) {
		int status = fb_putc(*buf++);
		if (status != DRIVER_STATUS_OK)
		       return status;
	}	
	return DRIVER_STATUS_OK;
}

driver_status_t fb_init(void) {
	bootloader_fb = framebuffer_request.response->framebuffers[0];

	for (uint32_t y = 0; y < bootloader_fb->height; y++) {
		for (uint32_t x = 0; x < bootloader_fb->width; x++) {
			fb_drawpixel(x, y, bg_color);
		}
	}
	return DRIVER_STATUS_OK;
}

