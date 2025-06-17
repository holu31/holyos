#ifndef _CTYPE_H
#define _CTYPE_H

#define _tolower(c) ((c) | 0x20)
#define _toupper(c) ((c) & ~0x20)
#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? _tolower(c) : (c))
#define toupper(c) (((c) >= 'a' && (c) <= 'z') ? _toupper(c) : (c))

#endif // _CTYPE_H
