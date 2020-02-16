#ifndef FILESYSTEM_RM_H
#define FILESYSTEM_RM_H

#include "classes.h"
#include "command_utils.h"

void remove_command(struct Filesystem* fs, struct Session* sess, char* filename) {
    struct Inode* inode = find_inode_by_name(fs, sess->cur_file, filename);

    if (inode == NULL) {
        printf("this file does not exist\n");
        return;
    }

    if (inode->parent_inode == NULL) {
        printf("yuo cant delete root directory\n");
    }

    short result = remove_pointer_from_inode(fs, inode->parent_inode, inode);

    printf("lalka: %d\n", result);

    if (result == 1) {
        printf("file was deleted successfully\n");
    } else {
        printf("this file does not exist\n");
    }
}

#endif //FILESYSTEM_RM_H
