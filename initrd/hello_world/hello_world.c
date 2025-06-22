void puts(char* str) {
	asm volatile ("int $0x80" :
		: "a" (0), "b" (str)
		: "memory"
	);
}

void main() {
	puts("Ave, Maria!\n");
}
