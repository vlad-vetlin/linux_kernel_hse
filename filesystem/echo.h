#ifndef FILESYSTEM_ECHO_H
#define FILESYSTEM_ECHO_H

#include "classes.h"
#include "command_utils.h"

void echo(struct Filesystem* fs, struct Session* sess, char* filename, char* data) {
    struct Inode* inode = find_inode_by_name(fs, sess->cur_file, filename);

    if (inode == NULL) {
        printf("this file is not exist\n");
        return;
    }

    if (inode->is_file == 0) {
        printf("This is not file\n");
        return;
    }

    size_t len = strlen(data);
    append_data_to_inode(fs, get_inode_index(fs, inode), data, len);

    printf("%ld symbols was successfully added!\n", len);
}

#endif //FILESYSTEM_ECHO_H
