include common.mk

ARCHIVES = kernel/kernel.o mm/mm.o fs/fs.o
LIBS = lib/lib.a

.PHONY: all $(ARCHIVES) $(LIBS)
all: isodir/boot/initrd.tar holyos.iso

holyos.iso: limine/limine limine.conf system
	@mkdir -p isodir/boot/limine/ isodir/EFI/BOOT/
	@cp system isodir/boot/
	@cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin isodir/boot/limine/	
	@cp limine/BOOTX64.EFI limine/BOOTIA32.EFI isodir/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label isodir -o holyos.iso
	./limine/limine bios-install holyos.iso

limine/limine:
	@if [ ! -d "limine" ]; then \
		git clone https://github.com/limine-bootloader/limine.git --branch=v9.3.2-binary --depth=1; \
	fi
	make -C limine

.PHONY: config
config:
	@python3 scripts/config.py

system: $(ARCHIVES) $(LIBS)
	$(LD) $(LDFLAGS) -T kernel/kernel.ld $(ARCHIVES) $(LIBS) -o system 

kernel/kernel.o:
	@make -C kernel

mm/mm.o:
	@make -C mm

fs/fs.o:
	@make -C fs

lib/lib.a:
	@make -C lib

isodir/boot/initrd.tar: initrd/
	@mkdir -p isodir/boot/
	@tar -cvf isodir/boot/initrd.tar -C initrd/ .

qemu: all
	qemu-system-x86_64 -cdrom holyos.iso -m 128M -serial stdio -d guest_errors -boot order=d

qemu-gdb: all
	qemu-system-x86_64 -m 128M -cpu max -serial stdio -d cpu_reset,int -cdrom holyos.iso -s -S &
	gdb -ex "file system" -ex "target remote localhost:1234"

.PHONY: clean
clean:
	@rm -rf isodir/ limine/ system holyos.iso
	@make -C kernel clean
	@make -C lib clean
	@make -C mm clean
	@make -C fs clean
	@rm -rf common.mk build_config.json
