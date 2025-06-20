#include <kernel/mm.h>
#include <kernel/cpu.h>
#include <kernel/gdt.h>
#include <kernel/interrupt.h>
#include <kernel/pic.h>
#include <kernel/fb.h>
#include <kernel/printf.h>
#include <kernel/image.h>
#include <kernel/string.h>
#include <kernel/pci.h>
#include <kernel/initrd.h>
#include <kernel/vfs.h>
#include <kernel/i8042.h>
#include <kernel/ps2kbd.h>
#include <kernel/input.h>
#include <kernel/syscall.h>
#include <kernel/cli.h>

void main(void) {
	cpu_disable_interrupts();
	gdt_init();
	idt_init();
	pic_remap();
	fb_init();
	mem_init();
	// WARNING: THIS FUNCTION CRASHES
	// WHEN OS IS COMPILED WITH CLANG
	initrd_tarfs();
	syscall_init();

	i8042_init();
	ps2kbd_init();
	input_init();

	printf("HOLY OS VERSION 0.1\n");
	printf("\n\"IN THE BEGINNING WAS THE WORD, AND THE WORD WAS WITH GOD, AND THE WORD WAS GOD\"\n");
	printf("- JOHN 1:1\n\n");

	size_t free_mem = get_free_memory();
	int kb = free_mem / 1024;
	printf("FREE MEMORY: %d KB\n\n", kb);

	cpu_enable_interrupts();

	// TEST SYSCALL
	asm volatile ("int $0x80" :
		: "a" (0), "b" ("Hello\n")
		: "memory"
	);

	for (int y = 0; y < jesus_medal_height; y++) {
		for (int x = 0; x < jesus_medal_width; x++) {
			int idx = (y * jesus_medal_width + x) * 3;
			
			uint8_t r = jesus_medal_data[idx];
			uint8_t g = jesus_medal_data[idx + 1];
			uint8_t b = jesus_medal_data[idx + 2];		
			uint32_t color = (r << 16) | (g << 8) | b;
			if (color <= 0xffffff && color >= 0x646464) continue;
			color = 0xFFFF00;

			fb_drawpixel(x + 960, y, color);
		}
	}

	cli_run();

	for (;;) asm("hlt");
}
