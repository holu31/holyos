#include <kernel/vfs.h>
#include <kernel/string.h>
#include <kernel/printf.h>
#include <kernel/mm.h>

static struct vfs_mount_info mounts[32];
static int mount_count = 0;

static struct vfs_mount_info* vfs_find_mount(const char* path) {
	for (int i = 0; i < mount_count; i++) {
		size_t len = strlen(mounts[i].path);
		if (strncmp(path, mounts[i].path, len) == 0) {
			return &mounts[i];
		}
	}
	return NULL;
}

static char* vfs_find_filepath(const char* path) {
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount || !mount->path)
		return strdup(path);

	size_t mount_path_len = strlen(mount->path);
	if (strncmp(path, mount->path, mount_path_len) != 0) {
		return strdup(path);
	}

	const char* remaining_path = path + mount_path_len;

	while (*remaining_path == '/')
		remaining_path++;

	if (*remaining_path == '\0')
		return strdup("/");

	return strdup(remaining_path);
}

int vfs_mount(char* path, struct vfs_filesystem_ops ops, fs_type_t type) {
	for (int i = 0; i < mount_count; i++) {
		if (strcmp(mounts[i].path, path) == 0) return -1;
	}

	if (mount_count >= 32) return -1;

	mounts[mount_count].path = path;
	mounts[mount_count].ops = ops;
	mounts[mount_count].type = type;

	mount_count++;

	return 0;
}

void vfs_adjust_path(char* adjusted, const char* original) {
	if (strcmp(original, "/") == 0)
		strcpy(adjusted, "./");
	else {
		strcpy(adjusted, "./");
		strcat(adjusted, original);
	}
}

int vfs_read(char* path, void* buf, size_t count) {
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.read) {
		ret = mount->ops.read(rel_path, buf, count);
	}
	kfree(rel_path);
	return ret;
}

int vfs_write(char* path, void* buf, size_t count) {
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.write) {
		ret = mount->ops.write(rel_path, buf, count);
	}
	kfree(rel_path);
	return ret;
}

int vfs_size(char* path) {
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.size) {
		ret = mount->ops.size(rel_path);
	}
	kfree(rel_path);
	return ret;
}

int vfs_list(const char* path) {
	if (!path[0] || strcmp(path, "/") == 0) {
		for (int i = 0; i < mount_count; i++)
			printf("%s TYPE: %d\n", mounts[i].path, mounts[i].type);
		return 0;
	}	
	
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.list) {
		ret = mount->ops.list(rel_path);
	}
	kfree(rel_path);
	return ret;
}

int vfs_isdir(const char* path) {
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.isdir) {
		ret = mount->ops.isdir(rel_path);
	}
	kfree(rel_path);
	return ret;
}

int vfs_exists(const char* path) {
	if (!path[0] || strcmp(path, "/") == 0) return 0; 
	
	struct vfs_mount_info* mount = vfs_find_mount(path);
	if (!mount) return -1;

	char* rel_path = vfs_find_filepath(path);
	int ret = -1;
	if (mount->ops.exists) {
		ret = mount->ops.exists(rel_path);
	}
	kfree(rel_path);
	return ret;
}
