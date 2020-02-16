#ifndef FILESYSTEM_LS_H
#define FILESYSTEM_LS_H

#include "classes.h"
#include "system.h"
#include "session.h"

void ls(struct Filesystem* fs, struct Session* sess) {
    struct Inode* inode = sess->cur_file;
    int i;

    if (inode->is_file) {
        exit(1);
    } else {
        size_t inode_size = inode->size;
        size_t indirect_level = needed_inode_indirect_level(inode_size);
        size_t sector_count = get_sector_count_by_indirect_level(indirect_level);

        size_t inode_index = get_inode_index(fs, inode);
        print_metadata(fs, inode_index);
        for (i = 0; i < sector_count; ++i) {
            int j;
            size_t sector_index = get_sector_index_in_inode(fs, inode_index, i);
            struct Sector* cur_sector_pointer = get_pointer_to_sector(fs, sector_index);

            for (j = 0; j < cur_sector_pointer->size; j += 16) {
                size_t cur_inode_index = get_pointer_in_sector_by_index(fs, sector_index, j / 16);

                print_metadata(fs, cur_inode_index);
            }
        }
    }
}

#endif //FILESYSTEM_LS_H
