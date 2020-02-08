#pragma once

#include "configs.h"
#include <stdio.h>
#include "system.h"
#include "sector_map.h"

struct Sector {
    char data[SECTOR_DATA_SIZE];
    size_t size;
};

struct Sector* get_pointer_to_sector(struct Filesystem* fs, size_t index);
void save_sector(size_t sector_index);

size_t make_sector(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty(fs->sector_map, SECTOR_COUNT);

    reserve_on_map(fs->sector_map, index);

    return index;    
}

void print_sector(struct Filesystem* fs, size_t sector) {
    char* sector_pointer = get_pointer_to_sector(fs, sector);
    
    printf("%s", sector_pointer);
}

struct Sector* get_pointer_to_sector(struct Filesystem* fs, size_t index) {
    return (char*)(fs->sectors + index * SECTOR_SIZE);
}

size_t get_index_to_sector_by_pointer(struct Fylesystem* fs, char* pointer) {
    return (size_t) ((pointer - fs->sectors) / SECTOR_SIZE);
}

void append_to_sector(struct Filesystem* fs, size_t sector_index, char data[], size_t input_data_size) {
    int i;

    size_t sector_size = fs->sectors[sector_index].size;
    size_t size = SECTOR_DATA_SIZE - sector_size;
    if (input_data_size < size) {
        size = input_data_size;
    }

    for (i = 0; i < size; ++i) {
        fs->sectors[sector_index][sector_size + i] = data[i];
    }

    fs->sectors[sector_index].size += size;

    save_sector(sector_index);
}

void remove_sector(struct Filesystem* fs, size_t sector_index) {
    remove_on_map(fs->sector_map, sector_index);
}

void copy_sector(struct Filesystem* fs, size_t to_sector_index, size_t from_sector_index) {}

void save_sector(size_t sector_index) {
}

struct Sector* get_pointer_in_sector_by_index(struct Filesystem* fs, size_t sector_index, size_t index) {
    struct Sector* sector_pointer = get_pointer_to_sector(fs, sector_index);

    return ((struct Sector**)(sector_pointer->data))[index];

    
}

void make_sector_indirect(struct Filesystem* fs, size_t sector_index) {
    int i;

    for (i = 0; i < SECTOR_DATA_SIZE; ++i) {
        size_t new_sector_index = make_sector(fs);
        Sector* sector_pointer

        append_to_sector(fs, sector_index, )
    }
}