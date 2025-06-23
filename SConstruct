import os
import SCons.Script

env = SCons.Script.Environment()

env['CC'] = 'gcc'
env['AS'] = 'gcc'
env['LD'] = 'ld'
env['CFLAGS'] = '-m64 -g -O0 \
            -nostdlib -ffreestanding -Wall -Wextra \
            -fno-builtin -fno-pic -fno-stack-protector \
            -mcmodel=large'
env['ASFLAGS'] = env['CFLAGS']
env['LDFLAGS'] = ''

env.Append(CPPPATH=["../include", "../limine"])

kernel_obj = SConscript('kernel/SConstruct', exports='env')
lib_obj = SConscript('lib/SConstruct', exports='env')
mm_obj = SConscript('mm/SConstruct', exports='env')
fs_obj = SConscript('fs/SConstruct', exports='env')

system = env.Program(
    target='system',
    source=[kernel_obj, mm_obj, fs_obj, lib_obj]
)

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

env.Clean(system, 'system')
env.Clean(system, 'build_config.json')
env.Clean(system, 'holyos.iso')