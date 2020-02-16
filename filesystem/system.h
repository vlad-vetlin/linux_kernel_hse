#ifndef FILESYSTEM_SYSTEM_H
#define FILESYSTEM_SYSTEM_H

#include "classes.h"
#include "configs.h"
#include "sector_map.h"

#include <stdlib.h>
#include <stdio.h>

void init_filesystem(struct Filesystem* fs) {
    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
    fs->inode_map = (unsigned char*)malloc(INODE_MAP_SIZE);
    fs->inode_system = (struct Inode*)malloc(INODE_FULL_SIZE);
    fs->sectors = (struct Sector*)malloc(SECTOR_FULL_SIZE);
}

unsigned char* get_sector_map(struct Filesystem* fs) {
    return fs->sector_map;
}

struct Sector* get_sectors(struct Filesystem* fs) {
    return fs->sectors;
}

unsigned char* get_inode_map(struct Filesystem* fs) {
    return fs->inode_map;
}

struct Inode* get_inode_system(struct Filesystem* fs) {
    return fs->inode_system;
}

void free_filesystem(struct Filesystem* fs) {
    int i;
    for (i = 0; i < INODE_MAP_SIZE; ++i) {
        if (check_bit_by_index(fs->inode_map, i)) {
            free(fs->inode_system[i].name);
        }
    }

    free(fs->sector_map);
    free(fs->inode_map);
    free(fs->inode_system);
    free(fs->sectors);
}

size_t find_index_of_root_inode(struct Filesystem* fs) {
    int i;
    for (i = 0; i < INODE_COUNT; ++i) {
        if (check_bit_by_index(fs->inode_map, i) && inode_is_root(fs, i)) {
            return i;
        }
    }
}

#endif //FILESYSTEM_SYSTEM_H