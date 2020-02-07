#pragma once

#include "configs.h"
#include <stdlib.h>
#include <stdio.h>

struct Filesystem {
    unsigned char* filesystem;
    unsigned char* sector_map;
};

unsigned char* init_filesystem(struct Filesystem* fs) {
    fs->sectors = (unsigned char*)malloc(SECTORS_FULL_SIZE);
    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
    fs->inode_map = (unsigned char*)malloc(INODE_MAP_SIZE);
    fs->inode_system = (unsigned char*)malloc(INODE_FULL_SIZE);
}

unsigned char* get_sector_map(struct Filesystem* fs) {
    return fs->sector_map;
}

unsigned char* get_sectors(struct Filesystem* fs) {
    return fs->sectors;
}

unsigned char* get_inode_map(struct Filesystem* fs) {
    return fs->inode_map;
}

unsigned char* get_inode_system(struct Filesystem* fs) {
    return fs->inode_system;
}

void free_filesystem(struct Filesystem* fs) {
    free(fs->filesystem);
    free(fs->sector_map);
    free(fs->inode_map);
    free(fs->inode_system);
}
