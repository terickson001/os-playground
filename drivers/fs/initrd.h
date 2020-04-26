#ifndef FS_INITRD_H
#define FS_INITRD_H

#include <cpu/types.h>
#include <kernel/fs.h>

typedef struct Initrd_Header
{
    usize num_entries;
} Initrd_Header;

typedef struct Initrd_Entry
{
    u16 magic; // 0xDEAD
    char name[32];
    usize off;
    usize len;
} Initrd_Entry;

File_System_Node *init_initrd(usize location);
#endif // FS_INITRD_H
