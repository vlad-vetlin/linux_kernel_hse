#ifndef FILESYSTEM_MAKE_H
#define FILESYSTEM_MAKE_H

#include "classes.h"
#include "inode.h"
#include "sector.h"
#include "command_utils.h"
#include "ls.h"

void make(struct Filesystem* fs, struct Session* sess, char* filename, short is_file) {

    struct Inode* inode = find_inode_by_name(fs, sess->cur_file, filename);

    if (inode != NULL) {
        printf("inode with name %s has already exist\n", filename);
        return;
    }

    size_t inode_index = create_inode_with_name(fs, sess->cur_file, is_file, filename);

    char* str_pointer = ltoa(inode_index, 16);
    size_t cur_inode_index = get_inode_index(fs, sess->cur_file);

    size_t test = append_data_to_inode(fs, cur_inode_index, str_pointer, 16);

    sess->cur_file = get_inode_pointer(fs, test);

    printf("\n");
    printf("successfully created: \n");
    print_metadata(fs, inode_index);
}

#endif //FILESYSTEM_MAKE_H
