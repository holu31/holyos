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
	vfs_read((char*)path, buf, size);

	elf64_ehdr* ehdr = (elf64_ehdr*)buf;

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

	// printf("ELF: phnum=%d phoff=0x%x\n", ehdr->e_phnum, ehdr->e_phoff);

	elf64_phdr* phdrs = (elf64_phdr*)((uint8_t*)buf + ehdr->e_phoff);
	for (int i = 0; i < ehdr->e_phnum; ++i) {
		if (phdrs[i].p_type != PT_LOAD) continue;
		if (phdrs[i].p_vaddr == 0) continue;

		uintptr_t vaddr = phdrs[i].p_vaddr;
		size_t memsz = phdrs[i].p_memsz;
		size_t filesz = phdrs[i].p_filesz;
		uintptr_t offset = phdrs[i].p_offset;

		// WARNING: IT'S WORKING, BUT YOU NEED TO ALLOCATE PAGES
		// TODO: ALLOCATE PAGES
		
		memcpy((void*)vaddr, (uint8_t*)buf + offset, filesz);
		
		if (memsz > filesz) {
			memset((void*)(vaddr + filesz), 0, memsz - filesz);
		}
	}

	// printf("ELF: entry point = 0x%x\n", (uintptr_t)ehdr->e_entry);
	uintptr_t entry = ehdr->e_entry;
	kfree(buf);
	return (int)entry;
}

void elf_execute(const char* path) {	
	int(*entry_point)() = (int(*)())elf_load(path);
	if (entry_point == NULL) {
		printf("ELF: FAILED TO LOAD\n");
		return;
	}

	entry_point();
}
