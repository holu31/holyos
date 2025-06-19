#include <kernel/elf.h>
#include <kernel/mm.h>
#include <kernel/vfs.h>
#include <kernel/printf.h>
#include <kernel/string.h>

int elf_load(const char* path) {
	if (vfs_exists((char*)path) != 0) {
		printf("ELF: FILE NOT FOUND '%s'", path);
		return -1;
	}

	int size = vfs_size((char*)path);
	if (size <= 0) return -1;

	void* buf = kmalloc(size);
	elf64_ehdr* ehdr = (elf64_ehdr*)buf;

	vfs_read((char*)path, buf, sizeof(elf64_ehdr));

	if (ehdr->e_ident[0] != 0x7F || ehdr->e_ident[1] != 'E' ||
			ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
		printf("ELF: INVALID MAGIC\n");
		kfree(buf);
		return -1;
	}

	if (ehdr->e_machine != 0x3E) {
		printf("ELF: NOT x86_64\n");
		kfree(buf);
		return -1;
	}

	for (int i = 0; i < ehdr->e_phnum; i++) {
		elf64_phdr* phdr = (elf64_phdr*)(buf + ehdr->e_phoff + i * sizeof(elf64_phdr));
		
		if (phdr->p_type == 1) {
			void* segment = (void*)phdr->p_vaddr;
			vfs_read((char*)path, segment, phdr->p_filesz);

			memset(segment + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
		}
	}

	kfree(buf);
	return ehdr->e_entry;
}

void elf_execute(const char* path) {	
	int(*entry_point)() = (int(*)())elf_load(path);
	if (entry_point == NULL) {
		printf("ELF: FAILED TO LOAD\n");
		return;
	}

	entry_point();
}
