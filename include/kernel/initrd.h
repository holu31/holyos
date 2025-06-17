#ifndef _INITRD_H
#define _INITRD_H

#include <stddef.h>

#define INITRD_NORMAL_FILE	'0'
#define INITRD_HARD_LINK	'1'
#define INITRD_SYMBOLIC_LINK	'2'
#define INITRD_CHARACTER_DEVICE	'3'
#define INITRD_BLOCK_DEVICE	'4'
#define INITRD_DIRECTORY	'5'
#define INITRD_NAMED_PIPE	'6'

struct tar_header {
	char filename[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag[1];
};

int initrd_tarfs(void);

#endif // _INITRD_H
