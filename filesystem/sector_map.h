#pragma once

#include "configs.h"
#include "system.h"
#include <stdlib.h>

// TODO rewrite for work with bits

size_t get_number_of_first_empty_sector(struct Filesystem* filesystem) {
    char* sector_map = get_sector_map(filesystem);
    char* i;

    for (i = sector_map; i < sector_map + SECTOR_MAP_SIZE; ++i) {
        if ((*(i)) == 0) {
            return i - sector_map;
        }
    }

    exit(27);
}

void reserve_sector_on_map(struct Filesystem* filesystem, size_t sector_number) {
    filesystem->sector_map[sector_number] = 1;
}

void reserve_first_sector_on_map(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty_sector(fs);

    reserve_sector_on_map(fs, index);
}
