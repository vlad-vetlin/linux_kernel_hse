#ifndef FILESYSTEM_SESSION_H
#define FILESYSTEM_SESSION_H

#include "classes.h"
#include "inode.h"

void set_root_file_as_cur(struct Filesystem* fs, struct Session* sess) {
    size_t index = find_index_of_root_inode(fs);

    sess->cur_file = get_inode_pointer(fs, index);
}

#endif //FILESYSTEM_SESSION_H
