#pragma once

#include "configs.h"
#include <stdlib.h>
#include <stdio.h>

struct Filesystem {
    char* filesystem;
    char* sector_map;
};

char* init_filesystem(struct Filesystem* fs) {
    fs->filesystem = (char*)malloc(SYSTEM_SIZE);
    fs->sector_map = (char*)malloc(SECTOR_MAP_SIZE);
}

char* get_sector_map(struct Filesystem* fs) {
    return fs->sector_map;
}

char* get_filesystem(struct Filesystem* fs) {
    return fs->filesystem;
}

void free_filesystem(struct Filesystem* fs) {
    free(fs->filesystem);
    free(fs->sector_map);
}
