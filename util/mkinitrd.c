#include <drivers/fs/initrd.h>
#include <cpu/types.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

usize file_size(char *name)
{
    FILE *file = fopen(name, "r");
    if(file == 0)
    {
        printf("Error: file not found: %s\n", name);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    usize size = ftell(file);
    fclose(file);
    return size;
}

typedef struct String
{
    char *data;
    usize len;
} String;

String path_base_name(char *path)
{
    int len = strlen(path);
    if (!len) return (String){};
    if (path[len-1] == '/') len--;
    
    char *slash = 0;
    char *end = &path[len-1];
    
    for (int i = len-1; i >= 0; i--)
    {
        if (path[i] == '/')
            slash = &path[i];
    }
    
    if (!slash)
        return (String){path, (end-path)+1};
    else
        return (String){slash+1, end-slash};
}

Initrd_Entry mk_entry(char *path, usize *off)
{
    Initrd_Entry entry = {};
    
    entry.magic = 0xDEAD;
    
    String name = path_base_name(path);
    strncpy(entry.name, name.data, name.len);
    
    entry.len = file_size(path);
    entry.off = *off;
    *off += entry.len;
    
    return entry;
}

int main(int argc, char **argv)
{
    int num_files = argc-1;
    char **files = argv+1;
    Initrd_Entry *entries = malloc(sizeof(Initrd_Entry) * num_files);
    usize off = sizeof(Initrd_Header) + (sizeof(Initrd_Entry) * num_files);
    
    Initrd_Header header = {};
    header.num_entries = num_files;
    
    for (int i = 0; i < num_files; i++)
        entries[i] = mk_entry(files[i], &off);
    
    usize total_size = off;
    
    byte *data = malloc(total_size);
    byte *cursor = data;
    
    memcpy(cursor, &header, sizeof(Initrd_Header));
    cursor += sizeof(Initrd_Header);
    memcpy(cursor, entries, sizeof(Initrd_Entry)*num_files);
    cursor += sizeof(Initrd_Entry) * num_files;
    
    for (int i = 0; i < num_files; i++)
    {
        FILE *file = fopen(files[i], "r");
        cursor += fread(cursor, 1, entries[i].len, file);
        fclose(file);
    }
    
    FILE *img = fopen("initrd.img", "w");
    fwrite(data, 1, total_size, img);
    fclose(img);
    
    free(data);
    
    return 0;
}