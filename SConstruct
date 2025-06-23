import os
import json
import SCons.Script

CONFIG_FILE = 'build_config.json'

def load_config(file):
    if not os.path.exists(file):
        print(f"Configuration file '{file}' not found.")
        return {}

    with open(file, 'r') as f:
        config = json.load(f)
    
    print(f"Loaded configuration: {config}")
    return config

env = SCons.Script.Environment()

config = load_config(CONFIG_FILE)
if not config:
    exit(1)

env['CC'] = config.get('compiler')
env['AS'] = config.get('compiler')
env['LD'] = 'ld.lld' if config.get('compiler') == 'clang' else 'ld'
env['CFLAGS'] = '-m64 -g -O0 \
            -nostdlib -ffreestanding -Wall -Wextra \
            -fno-builtin -fno-pic -fno-stack-protector \
            -mcmodel=large'
env['ASFLAGS'] = env['CFLAGS']
if config.get('compiler') == 'clang':
    env.Append(CFLAGS=' -target x86_64-pc-none-elf -march=x86-64')
env['LDFLAGS'] = '-T kernel/kernel.ld'

env.Append(CPPPATH=["../include", "../limine"])

kernel_obj = SConscript('kernel/SConstruct', exports='env')
lib_obj = SConscript('lib/SConstruct', exports='env')
mm_obj = SConscript('mm/SConstruct', exports='env')
fs_obj = SConscript('fs/SConstruct', exports='env')

system = env.Program(
    target='system',
    source=[kernel_obj, mm_obj, fs_obj, lib_obj],
    LINKFLAGS=env['LDFLAGS'],
)

initrd_tar = 'isodir/boot/initrd.tar'
env.Command(initrd_tar, 'initrd/', [
    Mkdir('isodir/boot'),
    f'tar -cvf {initrd_tar} -C initrd/ .'
])

def limine(target, source, env):
    if not os.path.exists('limine/limine'):
        os.system('git clone https://github.com/limine-bootloader/limine.git --branch=v9.3.2-binary --depth=1')
        os.system('make -C limine')

env.Command('limine', [], limine)

def build_iso(target, source, env):
    os.makedirs('isodir/boot/limine', exist_ok=True)
    os.makedirs('isodir/EFI/BOOT', exist_ok=True)
    os.system('cp system isodir/boot/')
    os.system('cp limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin isodir/boot/limine/')
    os.system('cp limine/BOOTX64.EFI limine/BOOTIA32.EFI isodir/EFI/BOOT/')
    os.system('xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table '
              '--efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label isodir -o holyos.iso')
    os.system('./limine/limine bios-install holyos.iso')

env.Command('holyos.iso', [system, 'limine'], build_iso)

AlwaysBuild(Alias('qemu', None, 'qemu-system-x86_64 -cdrom holyos.iso -m 128M -serial stdio -d guest_errors -boot order=d'))
AlwaysBuild(Alias('qemu-gdb', None, [
    'qemu-system-x86_64 -m 128M -cpu max -serial stdio -d cpu_reset,int -cdrom holyos.iso -s -S &',
    'gdb -ex "file system" -ex "target remote localhost:1234"'
]))

env.Clean(system, 'system')
env.Clean(system, 'build_config.json')
env.Clean(system, 'holyos.iso')