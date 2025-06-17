#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
#include <stdint.h>

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n); 
char* strcat(char* dest, const char* src);
char *strncat(char *dest, const char *src, size_t count);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n); 
int stricmp(const char* s1, const char* s2);
int strspn(const char* s1, const char* s2);
int strcspn(const char* s1, const char* s2);
char* strpbrk(const char* s1, const char* s2);
char* strstr(const char* s1, const char* s2);
char* strchr(const char* s, int c);
size_t strlen(const char* s);
char* strdup(const char* str);

char* strtok(char *s, const char* delim);

int memcmp(const void* destptr, const void* srcptr, size_t n);
void* memcpy(void* destptr, const void* srcptr, size_t n);
void* memmove(void* destptr, const void* srcptr, size_t n);
void* memchr(const void* cs, char c, size_t count);
void* memset(void* ptr, int value, size_t size);

void touppercase(char* str);
void tolowercase(char *str);

size_t itoa(int32_t n, char* buf);
size_t itoh(int32_t n, char* buf);
int atoi(const char s[]);
void reverse(char s[]);

#endif // _STRING_H
