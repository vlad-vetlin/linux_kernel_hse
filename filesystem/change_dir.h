#ifndef FILESYSTEM_CHANGE_DIR_H
#define FILESYSTEM_CHANGE_DIR_H

#include "string.h"
#include "classes.h"
#include "inode.h"
#include "sector.h"
#include "command_utils.h"

void change_dir(struct Filesystem* fs, struct Session* sess, char* filename) {

    if (strcmp(filename, "..") == 0) {
        if (sess->cur_file->parent_inode == NULL) {
            printf("current directory is a root\n");
            return;
        }

        sess->cur_file = sess->cur_file->parent_inode;

        printf("current directory is: \n");
        print_metadata(fs, get_inode_index(fs, sess->cur_file));

        return;
    }

    struct Inode* inode_pointer = find_inode_by_name(fs, sess->cur_file, filename);

    if (inode_pointer == NULL) {
        printf("Такого файла не существует\n");
        return;
    }

    if (inode_pointer->is_file == 1) {
        printf("this is file!\n");
        return;
    }

    sess->cur_file = inode_pointer;

    printf("current directory is: \n");
    print_metadata(fs, get_inode_index(fs, sess->cur_file));
}

#endif //FILESYSTEM_CHANGE_DIR_H
