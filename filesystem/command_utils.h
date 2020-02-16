#ifndef FILESYSTEM_COMMAND_UTILS_H
#define FILESYSTEM_COMMAND_UTILS_H

#include "classes.h"
#include "inode.h"
#include "sector.h"

struct Inode* find_inode_by_name(struct Filesystem* fs, struct Inode* inode, char* filename) {
    size_t inode_size = inode->size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);
    size_t sector_count = get_sector_count_by_indirect_level(indirect_level);
    int i;


    size_t inode_index = get_inode_index(fs, inode);

    for (i = 0; i < sector_count; ++i) {
        int j;
        size_t sector_index = get_sector_index_in_inode(fs, inode_index, i);
        struct Sector* sector_pointer = get_pointer_to_sector(fs, sector_index);

        for (j = 0; j < sector_pointer->size; j += 16) {
            size_t new_inode_index = get_pointer_in_sector_by_index(fs, sector_index, j / 16);
            struct Inode* inode_pointer = get_inode_pointer(fs, new_inode_index);

            if (strcmp(filename, inode_pointer->name) == 0) {
                return inode_pointer;
            }
        }
    }

    return NULL;
}

char* get_string() {
    char data[100];

    scanf("%s", data);

    size_t len = strlen(data) + 1;
    char* data_in_memory = malloc(len);

    int i;
    for (i = 0; i < len - 1; ++i) {
        data_in_memory[i] = data[i];
    }

    data_in_memory[len - 1] = '\0';

    return data_in_memory;
}

#endif //FILESYSTEM_COMMAND_UTILS_H
