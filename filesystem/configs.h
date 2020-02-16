#ifndef FILESYSTEM_CONFIGS_H
#define FILESYSTEM_CONFIGS_H

#include <stdio.h>

// SECTORS
const size_t SECTOR_SIZE = 68;
const size_t SECTOR_COUNT = 64;
const size_t SECTOR_FULL_SIZE = 4352;
#define SECTOR_DATA_SIZE 64
const size_t SECTOR_INDIRECT_COUNT = 4;

// MAPPERS
const size_t SECTOR_MAP_SIZE = 8;
const size_t INODE_MAP_SIZE = 8;

// INODES
const size_t INODE_SIZE = 29;
const size_t INODE_COUNT = 64;
const size_t INODE_FULL_SIZE = 1856;

#endif //FILESYSTEM_CONFIGS_H