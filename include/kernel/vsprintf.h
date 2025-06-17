#ifndef _VSPRINTF_H
#define _VSPRINTF_H

#include <stdarg.h>
#include <stddef.h>

int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);
int vsnprintf(char* buf, size_t size, const char* fmt, va_list args);
int snprintf(char* buf, size_t size, const char* fmt, ...);

#endif // VSPRINTF_H
