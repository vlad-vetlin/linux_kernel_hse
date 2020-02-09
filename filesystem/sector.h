#pragma once

#include "configs.h"
#include <stdio.h>
#include "system.h"
#include "sector_map.h"
#include <stdlib.h>
#include "string.h"

// Sector in the disk.
// size - size of the filled sector
// data - array of data bytes
struct Sector {
    char data[SECTOR_DATA_SIZE];
    size_t size;
};

// return pointer to sector data by sector index
struct Sector* get_pointer_to_sector(struct Filesystem* fs, size_t index);

// save sector in memory
void save_sector(struct Filesystem* fs, size_t sector_index);

// clear sector data
void clear_sector(struct Filesystem* fs, size_t sector_index) {
    fs->sector_map[sector_index].size = 0;
}

// reserve on mapper new sector and return index of it sector
size_t make_sector(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty(fs->sector_map, SECTOR_COUNT);

    reserve_on_map(fs->sector_map, index);

    return index;    
}

// print sector in console
void print_sector(struct Filesystem* fs, size_t sector) {
    struct Sector* sector_pointer = get_pointer_to_sector(fs, sector);
    int i;

    for (i = 0; i < sector_pointer->size; ++i) {
        printf("%c", sector_pointer->data[i]);
    }
}

// return pointer to sector
struct Sector* get_pointer_to_sector(struct Filesystem* fs, size_t index) {
    return (struct Sector*)(fs->sectors + index * SECTOR_SIZE);
}

// return sector index by it pointer
size_t get_index_to_sector_by_pointer(struct Fylesystem* fs, struct Sector* pointer) {
    return (size_t) ((pointer - fs->sectors) / SECTOR_SIZE);
}

// append data to end of the sector. Also increase it size
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

    save_sector(fs, sector_index);
}

// remove sector from mapper
void remove_sector(struct Filesystem* fs, size_t sector_index) {
    remove_on_map(fs->sector_map, sector_index);
}

// copy one sector to another sector
// remove all data from "to sector" before copy
void copy_sector(struct Filesystem* fs, size_t to_sector_index, size_t from_sector_index) {
    clear_sector(fs, to_sector_index);
    struct Sector* sector = fs->sectors[from_sector_index];

    append_to_sector(fs, to_sector_index, sector.data, sector->size);
}

// save sector to memory
void save_sector(struct Filesystem* fs, size_t sector_index) {
}

// get pointer to sector in indirect sector by index
struct Sector* get_pointer_in_sector_by_index(struct Filesystem* fs, size_t sector_index, size_t index) {
    struct Sector* sector_pointer = get_pointer_to_sector(fs, sector_index);

    return ((struct Sector**)(sector_pointer->data))[index];
}

// make sector indirect
void make_sector_indirect(struct Filesystem* fs, size_t sector_index) {
    int i;

    for (i = 0; i < SECTOR_INDIRECT_COUNT; ++i) {
        size_t new_sector_index = make_sector(fs);
        struct Sector* sector_pointer = get_pointer_to_sector(fs, new_sector_index);

        char str_pointer[];
        ltoa(sector_pointer, str_pointer);
        append_to_sector(fs, sector_index, str_pointer, strlen(str_pointer));
    }
}