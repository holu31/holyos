#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
	FS_TYPE_TARFS,
} fs_type_t;

struct vfs_filesystem_ops {
	int (*read)(char* path, void* buf, size_t count);
	int (*write)(char* path, void* buf, size_t count);	

	int (*size)(char* path);
	int (*list)(const char* path);

	int (*isdir)(const char* path);
	int (*exists)(const char* path);
};

struct vfs_mount_info {
	char* path;
	fs_type_t type;
	struct vfs_filesystem_ops ops;
};

int vfs_mount(char* path, struct vfs_filesystem_ops ops, fs_type_t type);
void vfs_adjust_path(char* adjusted, const char* original); 

int vfs_read(char* path, void* buf, size_t count);
int vfs_write(char* path, void* buf, size_t count);

int vfs_size(char* path);
int vfs_list(const char* path);

int vfs_isdir(const char* path);
int vfs_exists(const char* path);

#endif // _VFS_H
