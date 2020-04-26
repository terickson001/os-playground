#ifndef BOOT_MULTIBOOT_H
#define BOOT_MULTIBOOT_H

#include <cpu/types.h>

#define MB_FLAG_MEM  0x001
#define MB_FLAG_DEV  0x002
#define MB_FLAG_CMD  0x004
#define MB_FLAG_MODS 0x008
#define MB_FLAG_AOUT 0x010
#define MB_FLAG_ELF  0x020
#define MB_FLAG_MMAP 0x040
#define MB_FLAG_CONF 0x080
#define MB_FLAG_LOAD 0x100
#define MB_FLAG_APM  0x200
#define MB_FLAG_VBE  0x400

typedef struct Multiboot
{
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u32 vbe_mode;
    u32 vbe_interface_seg;
    u32 vbe_interface_off;
    u32 vbe_interface_len;
} Multiboot;

#endif // BOOT_MULTIBOOT_H
