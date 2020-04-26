#include <kernel/paging.h>
#include <drivers/fs/initrd.h>

#include <libc/string.h>
#include <libc/mem.h>

Initrd_Header *initrd_header;
Initrd_Entry *initrd_entries;
File_System_Node *initrd_root;
File_System_Node *initrd_dev;
File_System_Node *root_nodes;
usize num_root_nodes;

static Directory_Entry dirent;

FS_READ_PROC(initrd_read)
{
    if (FS_NODE_TYPE(node) == FS_DIRECTORY)
        return 0;
    
    Initrd_Entry entry = initrd_entries[node->inode];
    if (offset > entry.len)
        return 0;
    if (offset+size > entry.len)
        size = entry.len - offset;
    memory_copy(buffer, (byte *)(entry.off + offset), size);
    return size;
}

FS_READ_DIR_PROC(initrd_read_dir)
{
    if (FS_NODE_TYPE(node) != FS_DIRECTORY)
        return 0;
    
    if (node == initrd_root && index == 0)
    {
        strcpy(dirent.name, "dev");
        dirent.inode = 0;
        return &dirent;
    }
    
    index--;
    if (index >= num_root_nodes)
        return 0;
    
    strcpy(dirent.name, root_nodes[index].name);
    dirent.name[sizeof(dirent.name)-1] = 0;
    dirent.inode = root_nodes[index].inode;
    return &dirent;
}

FS_FIND_DIR_PROC(initrd_find_dir)
{
    if (FS_NODE_TYPE(node) != FS_DIRECTORY)
        return 0;
    
    if (node == initrd_root &&
        strcmp(name, "dev") == 0)
        return initrd_dev;
    
    for (usize i = 0; i < num_root_nodes; i++)
    {
        if (strcmp(name, root_nodes[i].name) == 0)
            return &root_nodes[i];
    }
    
    return 0;
}

static File_System_Interface initrd_interface =
{
    .read     = initrd_read,
    .read_dir = initrd_read_dir,
    .find_dir = initrd_find_dir,
};

File_System_Node *init_initrd(usize location)
{
    initrd_header = (Initrd_Header *)location;
    initrd_entries = (Initrd_Entry *)(initrd_header+1);
    
    initrd_root = (File_System_Node *)kmalloc(sizeof(File_System_Node));
    strcpy(initrd_root->name, "initrd");
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->interface = &initrd_interface;
    initrd_root->permissions = 0;
    initrd_root->uid = 0;
    initrd_root->gid = 0;
    initrd_root->inode = 0;
    initrd_root->size = 0;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;
    
    initrd_dev = (File_System_Node *)kmalloc(sizeof(File_System_Node));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->interface = &initrd_interface;
    initrd_dev->permissions = 0;
    initrd_dev->uid = 0;
    initrd_dev->gid = 0;
    initrd_dev->inode = 0;
    initrd_dev->size = 0;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;
    
    root_nodes = (File_System_Node *)kmalloc(sizeof(File_System_Node) * initrd_header->num_entries);
    num_root_nodes = initrd_header->num_entries;
    
    for (usize i = 0; i < num_root_nodes; i++)
    {
        initrd_entries[i].off += location;
        
        strcpy(root_nodes[i].name, "dev");
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].interface = &initrd_interface;
        root_nodes[i].size = initrd_entries[i].len;
        root_nodes[i].inode = i;
        root_nodes[i].permissions = 0;
        root_nodes[i].uid = 0;
        root_nodes[i].gid = 0;
        root_nodes[i].ptr = 0;
        root_nodes[i].impl = 0;
    }
    
    return initrd_root;
}