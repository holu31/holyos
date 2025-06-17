#include <kernel/vsprintf.h>
#include <kernel/printf.h>
#include <kernel/fb.h>
#include <stdarg.h>

void printf(char* fmt, ...) {
	char buf[PRINTF_BUFSIZE];
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	fb_write(buf);
}
