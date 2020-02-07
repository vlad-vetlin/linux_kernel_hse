#pragma once

#include "configs.h"
#include <stdio.h>
#include "system.h"
#include "sector_map.h"

char* get_pointer_to_sector(struct Filesystem* fs, size_t index);
void save_sector(size_t sector_index);

size_t make_sector(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty_sector(fs);

    reserve_sector_on_map(fs, index);

    return index;    
}

void print_sector(struct Filesystem* fs, size_t sector) {
    char* sector_pointer = get_pointer_to_sector(fs, sector);
    
    printf("%s", sector_pointer);
}

char* get_pointer_to_sector(struct Filesystem* fs, size_t index) {
    return (char*)(fs->sectors + index * SECTOR_SIZE);
}

void update_sector(struct Filesystem* fs, size_t sector_index, char data[]) {
    int i;

    for (i = 0; i < SECTOR_SIZE; ++i) {
        fs->sectors[sector_index * SECTOR_SIZE + i] = data[i];
    }

    save_sector(sector_index);
}

void remove_sector(struct Filesystem* fs, char* sector) {
}

void save_sector(size_t sector_index) {
}
