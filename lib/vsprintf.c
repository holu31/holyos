#include <kernel/vsprintf.h>
#include <kernel/string.h>
#include <limits.h>

int vsprintf(char* buf, const char* fmt, va_list args) {
	return vsnprintf(buf, SIZE_MAX, fmt, args);
}

int sprintf(char* buf, const char* fmt, ...) {
	va_list args;
	int r;

	va_start(args, fmt);
	r = vsnprintf(buf, SIZE_MAX, fmt, args);
	va_end(args);

	return r;
}

int vsnprintf(char* buf, size_t size, const char* fmt, va_list args) {
	char* str = buf;
	const char* end = buf + size - 1;

	for (; *fmt && str < end; fmt++) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		fmt++;
		if (!fmt) break;

		switch (*fmt) {
			case 'd': {
				int num = va_arg(args, int);
				char buffer[32];
				int len = itoa(num, buffer);
				for (int i = 0; i < len && str < end; i++)
					*str++ = buffer[i];
				break;
			}
			case 'x': {
				uint32_t num = va_arg(args, uint32_t);
				char buffer[32];
				int len = itoh(num, buffer);
				for (int i = 0; i < len && str < end; i++)
					*str++ = buffer[i];
				break;
			}
			case 'c': {
				const char c = va_arg(args, int);
				*str++ = c;
				break;
			}
			case 's': {
				const char* s = va_arg(args, const char*);
				if (!s) s = "(null)";
				while (*s && str < end) {
					*str++ = *s++;
				}
				break;
			}
			case '%': {
				if (str < end) {
					*str++ = '%';
				}
				break;
			}
			default: {
				if (str < end) {
					*str++ = '%';
					if (str < end) {
						*str++ = *fmt;
					}
				}
				break;
			}
		}
	}

	*str = '\0';
	return str - buf;
}

int snprintf(char* buf, size_t size, const char* fmt, ...) {
	va_list args;
	int r;

	va_start(args, fmt);
	r = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return r;
}
