#include <kernel/initrd.h>
#include <kernel/vfs.h>
#include <kernel/printf.h>
#include <kernel/string.h>
#include <kernel/mm.h>
#include <limine.h>

static volatile struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0
};

static uintptr_t data;

static uint32_t tar_parse_size(const char *size) {
	uint32_t val = 0;
	while (*size)
		val = val * 8 + (*size++ - '0');
	
	return val;
}

struct tar_header* initrd_find(const char* filename) {
	if (!data) return NULL;

	char adjusted_path[256];
	vfs_adjust_path(adjusted_path, filename);

	struct tar_header* header = (struct tar_header*)data;
	while (header->filename[0] != '\0') {
		// TEMP
		char path[256];
	       	strcpy(path, adjusted_path);
		if (header->typeflag[0] != INITRD_DIRECTORY) {
			int size = strlen(path);
			if (path[size-1] == '/')
				path[size-1] = '\0';
		}
		if (strcmp(header->filename, path) == 0) {
			return header;
		}

		size_t size = tar_parse_size(header->size);
		header = (struct tar_header*)((char*)header + 512 + ((size + 511) / 512) * 512);
	}

	return NULL;
}

int initrd_read(char* path, void* buf, size_t count) {
	struct tar_header* header = initrd_find(path);
	if (!header) return -1;

	size_t filesize = tar_parse_size(header->size);

	size_t data_offset = sizeof(struct tar_header);
	if (data_offset % 512 != 0) {
		data_offset += 512 - (data_offset % 512);
	}

	size_t to_read = count;
	if (to_read > filesize) {
		to_read = filesize;
	}

	memcpy(buf, (char*)header + data_offset, to_read);

	return to_read;
}

int initrd_size(char* path) {
	struct tar_header* header = initrd_find(path);
	if (!header) return -1;

	return tar_parse_size(header->size);
}

int initrd_list(const char* path) {
	if (!data) return -1;
	
	struct tar_header* header = initrd_find(path);	
	if (header && header->typeflag[0] != INITRD_DIRECTORY) {
		printf("%s (FILE) [size: %d bytes]\n", path, tar_parse_size(header->size));
		return 0;
	}
		
	char dir_path[256];
	vfs_adjust_path(dir_path, path);

	header = (struct tar_header*)data;	
	while (header->filename[0] != '\0') {
		if (strncmp(header->filename, dir_path, strlen(dir_path)) == 0) {
			const char* name = header->filename + strlen(dir_path);
			const char* next_slash = strchr(name, '/');	

			if (next_slash == NULL && *name != '\0') {
				printf("%s%s (FILE)\n", dir_path, name);
			}
			else if (next_slash != NULL) {
				 if (strcmp(next_slash, path) == 0)
					 printf("%s%s (DIRECTORY)\n", dir_path, name);
			}
		}
        
		size_t size = tar_parse_size(header->size);
		header = (struct tar_header*)((char*)header + 512 + ((size + 511) / 512) * 512);
	}
	
	return 0;
}

int initrd_isdir(const char* path) {
	struct tar_header* header = initrd_find(path);
	if (!header) return -1;

	if (header->typeflag[0] == INITRD_DIRECTORY)
		return 0;

	return -1;
}

int initrd_exists(const char* path) {
	struct tar_header* header = initrd_find(path);
	if (!header) return -1;

	return 0;
}

struct vfs_filesystem_ops initrd = {
	.read = initrd_read,
	.size = initrd_size,
	.list = initrd_list,
	.isdir = initrd_isdir,
	.exists = initrd_exists,
};

int initrd_tarfs(void) {
	struct limine_module_response* module_response = module_request.response;
	
	if (!module_response || module_response->module_count == 0) {
		return -1;
	}

	for (size_t i = 0; i < module_response->module_count; i++) {
		struct limine_file *module = module_response->modules[i];
		void *addr = module->address;
	
		if (memcmp(addr + 257, "ustar", 5) == 0) {
			data = (uintptr_t)addr;
			break;	
		}
	}

	if (!data) return -1;

	vfs_mount("/initrd/", initrd, FS_TYPE_TARFS);

	return 0;
}
