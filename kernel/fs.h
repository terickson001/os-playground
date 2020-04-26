#ifndef DRIVERS_FS_H
#define DRIVERS_FS_H

#include <cpu/types.h>

struct File_System_Node;

#define FS_READ_PROC(NAME) \
isize NAME(struct File_System_Node *node , usize offset, usize size, byte *buffer)
#define FS_WRITE_PROC(NAME) \
isize NAME(struct File_System_Node *node, usize offset, usize size, byte *buffer)
#define FS_OPEN_PROC(NAME) \
int NAME(struct File_System_Node *node, b8 read, b8 write)
#define FS_CLOSE_PROC(NAME) \
int NAME(struct File_System_Node *node)
#define FS_READ_DIR_PROC(NAME) \
struct Directory_Entry *NAME(struct File_System_Node *node, usize index)
#define FS_FIND_DIR_PROC(NAME) \
struct File_System_Node *NAME(struct File_System_Node *node, char *name)

typedef FS_READ_PROC(File_System_Read);
typedef FS_WRITE_PROC(File_System_Write);
typedef FS_OPEN_PROC(File_System_Open);
typedef FS_CLOSE_PROC(File_System_Close);
typedef FS_READ_DIR_PROC(File_System_Read_Dir);
typedef FS_FIND_DIR_PROC(File_System_Find_Dir);

typedef struct File_System_Interface
{
    File_System_Read     *read;
    File_System_Write    *write;
    File_System_Open     *open;
    File_System_Close    *close;
    File_System_Read_Dir *read_dir;
    File_System_Find_Dir *find_dir;
} File_System_Interface;

typedef struct File_System_Node
{
    char name[128];
    u32 permissions;
    u32 uid;
    u32 gid;
    u32 flags;
    u32 inode;
    u32 size;
    u32 impl;
    File_System_Interface *interface;
    struct File_System_Node *ptr;
} File_System_Node;

typedef struct Directory_Entry
{
    char name[128];
    u32 inode;
} Directory_Entry;

typedef enum File_System_Flag
{
    // Node Types
    FS_FILE        = 0x01,
    FS_DIRECTORY   = 0x02,
    FS_CHARDEVICE  = 0x03,
    FS_BLOCKDEVICE = 0x04,
    FS_PIPE        = 0x05,
    FS_SYMLINK     = 0x06,
    
    // Flags
    FS_MOUNTPOINT  = 0x08,
} File_System_Flag;
#define FS_NODE_TYPE(NODE) ((NODE->flags) & 0x7)

extern File_System_Node *fs_root;

isize fs_read(struct File_System_Node *node , usize offset, usize size, byte *buffer);
isize fs_write(struct File_System_Node *node, usize offset, usize size, byte *buffer);
int fs_open(struct File_System_Node *node, b8 read, b8 write);
int fs_close(struct File_System_Node *node);
Directory_Entry *fs_read_dir(struct File_System_Node *node, usize index);
File_System_Node *fs_find_dir(struct File_System_Node *node, char *name);

#endif // DRIVERS_FS_H
