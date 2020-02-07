#pragma once

#include "configs.h"
#include <stdlib.h>
#include <stdio.h>

struct Filesystem {
    unsigned char* filesystem;
    unsigned char* sector_map;
};

unsigned char* init_filesystem(struct Filesystem* fs) {
    fs->filesystem = (unsigned char*)malloc(SYSTEM_SIZE);
    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
}

unsigned char* get_sector_map(struct Filesystem* fs) {
    return fs->sector_map;
}

unsigned char* get_filesystem(struct Filesystem* fs) {
    return fs->filesystem;
}

void free_filesystem(struct Filesystem* fs) {
    free(fs->filesystem);
    free(fs->sector_map);
}
