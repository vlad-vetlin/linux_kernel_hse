#pragma once

#include <stdio.h>

// SECTORS
const size_t SECTOR_SIZE = 32;
const size_t SECTOR_COUNT = 64; // 2KB (1024/32)
const size_t SECTOR_FULL_SIZE = 2048;
const size_t SECTOR_DATA_SIZE = 28;
const size_t SECTOR_INDIRECT_COUNT = 7;

// MAPPERS
const size_t SECTOR_MAP_SIZE = 4;
const size_t INODE_MAP_SIZE = 4;

// INODES
const size_t INODE_SIZE = 25;
const size_t INODE_COUNT = 64;
const size_t INODE_FULL_SIZE = 1600;