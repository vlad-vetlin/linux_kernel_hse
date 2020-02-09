#ifndef FILESYSTEM_CONFIGS_H
#define FILESYSTEM_CONFIGS_H

#include <stdio.h>

// SECTORS
const size_t SECTOR_SIZE = 64;
const size_t SECTOR_COUNT = 64;
const size_t SECTOR_FULL_SIZE = 4096;
#define SECTOR_DATA_SIZE 60
const size_t SECTOR_INDIRECT_COUNT = 3;

// MAPPERS
const size_t SECTOR_MAP_SIZE = 4;
const size_t INODE_MAP_SIZE = 4;

// INODES
const size_t INODE_SIZE = 29;
const size_t INODE_COUNT = 64;
const size_t INODE_FULL_SIZE = 1856;

#endif //FILESYSTEM_CONFIGS_H