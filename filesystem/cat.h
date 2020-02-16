#ifndef FILESYSTEM_CAT_H
#define FILESYSTEM_CAT_H

#include "classes.h"
#include "command_utils.h"

void cat(struct Filesystem* fs, struct Session* sess, char* filename) {
    struct Inode* inode = find_inode_by_name(fs, sess->cur_file, filename);

    if (inode == NULL) {
        printf("this file does not exist\n");
        return;
    }

    printf("metadata: \n");

    size_t inode_index = get_inode_index(fs, inode);

    print_metadata(fs, inode_index);

    printf("\ninode data: \n\n");

    print_inode(fs, inode_index);
}

#endif //FILESYSTEM_CAT_H
