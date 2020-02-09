#ifndef FILESYSTEM_CLASSES_H
#define FILESYSTEM_CLASSES_H

#include <stdlib.h>

#include "configs.h"

// Sector in the disk.
// size - size of the filled sector
// data - array of data bytes
struct Sector {
    size_t size;
    char data[SECTOR_DATA_SIZE];
};

// parent_inode_index - 8
// is_file - 1
// size - 4
// name - 8
// sector_pointer - 8
//
// full_size = 29
struct Inode {
    struct Inode* parent_inode;
    short is_file;
    size_t size;

    char* name;

    struct Sector* sector_pointer;
};

struct Filesystem {
    struct Sector* sectors;
    unsigned char* sector_map;
    unsigned char* inode_map;
    struct Inode* inode_system;
};

#endif //FILESYSTEM_CLASSES_H
