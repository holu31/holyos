void _start() {
	asm volatile ("int $0x80" :
		: "a" (0), "b" ("Hello world\n")
		: "memory"
	);
}
