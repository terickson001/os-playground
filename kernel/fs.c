#include <kernel/fs.h>

File_System_Node *fs_root = 0;

FS_READ_PROC(fs_read)
{
    if (node->interface->read)
        return node->interface->read(node, offset, size, buffer);
    else
        return 0;
}

FS_WRITE_PROC(fs_write)
{
    if (node->interface->write)
        return node->interface->write(node, offset, size, buffer);
    else
        return 0;
}

FS_OPEN_PROC(fs_open)
{
    if (node->interface->open)
        return node->interface->open(node, read, write);
    else
        return 0;
}

FS_CLOSE_PROC(fs_close)
{
    if (node->interface->close)
        return node->interface->close(node);
    else
        return 0;
}

FS_READ_DIR_PROC(fs_read_dir)
{
    if (FS_NODE_TYPE(node) == FS_DIRECTORY &&
        node->interface->read_dir)
        return node->interface->read_dir(node, index);
    else
        return 0;
}

FS_FIND_DIR_PROC(fs_find_dir)
{
    if (FS_NODE_TYPE(node) == FS_DIRECTORY &&
        node->interface->find_dir)
        return node->interface->find_dir(node, name);
    else
        return 0;
}