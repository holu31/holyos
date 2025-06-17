#include <kernel/string.h>
#include <kernel/ctype.h>
#include <kernel/mm.h>

char* strcpy(char* dest, const char* src) {
	char* tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}

char* strncpy(char* dest, const char* src, size_t n) {
	char* tmp = dest;

	while (n) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		n--;
	}
	return dest;
}

char* strcat(char* dest, const char* src) {
	char* tmp = dest;
	
	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');

	return tmp;
}

char* strncat(char* dest, const char* src, size_t count) {
	char *tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != 0) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}
	return tmp;
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}

	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	if (n == 0) return 0;
	
	while (n-- != 0) {
		unsigned char c1 = *s1++;
		unsigned char c2 = *s2++;
		
		if (c1 != c2)
			return c1 - c2;
		if (c1 == '\0')
			break;
	}
    
	return 0;
}

int stricmp(const char* s1, const char* s2) {
	while (*s1 && *s2) {
		int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
		if (diff != 0) return diff;
		s1++;
		s2++;
	}
	return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int strspn(const char* s1, const char* s2) {
	const char* p;

	for (p = s1; *p != '\0'; ++p) {
		if (!strchr(s2, *p))
			break;
	}
	return p - s1;
}

int strcspn(const char* s1, const char * s2) {
	const char* p;

	for (p = s1; *p != '\0'; ++p) {
		if (strchr(s2, *p))
			break;
	}
	return p - s1;
}

char* strpbrk(const char* s1, const char * s2) {
	const char* p;

	for (p = s1; *p != '\0'; ++p) {
		if (strchr(s2, *p))
			return (char*)p;
	}
	return NULL;
}

char* strstr(const char* s1, const char* s2) {
	size_t len;

	len = strlen(s2);
	if (!len) return (char*)s1;
	
	for (; *s1 != '\0'; *s1++) {
		if (*s1 == *s2 && !memcmp(s1, s2, len))
			return (char*)s1;
	}
	return NULL;
}

char* strchr(const char* s, int c) {
	for (; *s != '\0' && *s != (char)c; ++s);

	return (*s == (char)c) ? (char*)s : NULL;
}

size_t strlen(const char* s) {
	size_t len = 0;
	while (*s++)
		len++;
	return len;
}

char* strdup(const char* str) {
	if (!str) return NULL;

	size_t len = strlen(str) + 1;
	char* new = kmalloc(len);
	if (new) {
        	memcpy(new, str, len);
       	}

	return new;
}

char* strtok(char* s, const char* delim) {
	static char *saved_ptr = NULL;
	char *start;	

	if (!s) {
		if (!saved_ptr || !*saved_ptr)
			return NULL;
		s = saved_ptr;
	}

	s += strspn(s, delim);
	if (!*s) {
		saved_ptr = NULL;
		return NULL;
	}

	start = s;
	s = start + strcspn(start, delim);
	
	if (*s) {
		*s = '\0';
		saved_ptr = s + 1;
	} else {
		saved_ptr = NULL;
	}

	return start;
}

int memcmp(const void* destptr, const void* srcptr, size_t n) {
	const unsigned char* dest = destptr, *src = srcptr;

	while (n--) {
		if (*dest != *src)
			return (*dest < *src) ? -1 : 1;
		dest++; src++;
	}

	return 0;
}

void* memcpy(void* destptr, const void* srcptr, size_t n) {
	char* dest = destptr;
	const char* src = srcptr;
	while (n--)
		*dest++ = *src++;

	return destptr;
}

void* memmove(void* destptr, const void* srcptr, size_t n) {
	char* dest;
	const char* src;

	if (destptr < srcptr) {
		dest = destptr;
		src = srcptr;
		while (n--)
			*dest = *src++;
	} else {
		dest = destptr;
		dest += n;
		src = srcptr;
		src += n;
		while (n--)
			*--dest = *--src;
	}

	return destptr;
}

void* memchr(const void* cs, char c, size_t count) {
	const unsigned char* ptr = cs;
	while (count-- != 0) {
		if ((unsigned char)c == *ptr++) {
			return (void*)(ptr - 1);
		}
	}

	return NULL;
}

void* memset(void* ptr, int value, size_t size) {
	char* buf = ptr;
	while (size--)
		*buf++ = value;

	return ptr;
}

void touppercase(char* str) {	
	for (; *str != '\0'; str++) {
		*str = toupper((unsigned char)*str);
	}
}

void tolowercase(char *str) {
	for (; *str != '\0'; str++) {
		*str = tolower((unsigned char)*str);
	}
}

size_t itoa(int32_t n, char* buf) {
	int i, sign;
	
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		buf[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	
	if (sign < 0)
		buf[i++] = '-';
	
	buf[i] = '\0';
	reverse(buf);

	return i;
}

size_t itoh(int32_t n, char* buf) {
	const char hex_chars[] = "0123456789ABCDEF";
	uint32_t i, d = 0x10000000;
	char* ptr = buf;

	while ((n / d == 0) && (d >= 0x10))
		d /= 0x10;
	i = n;

	while (d >= 0xF) {
		*ptr++ = hex_chars[i / d];
		i = i % d;
		d /= 0x10;
	}

	*ptr++ = hex_chars[i];
	*ptr = 0;

	return strlen(buf);
}

int atoi(const char s[]) {
	int val = 0, sign = 1;
	
	if (*s == '-') {
		sign = -1;
		s++;
	}
	
	while (*s >= '0' && *s <= '9') {
		val = val * 10 + (*s - '0');
		s++;
	}
	
	return sign * val;
}

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
