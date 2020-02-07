#pragma once

#include "configs.h"
#include "system.h"
#include <stdlib.h>

size_t fill_bit(unsigned char byte, unsigned char bit);
size_t erase_bit(unsigned char byte, unsigned char bit);
size_t get_first_zero_bit(unsigned char byte);
void set_value_on_map(struct Filesystem* fs, size_t sector_number, unsigned char value);

size_t get_number_of_first_empty_sector(struct Filesystem* filesystem) {
    unsigned char* sector_map = get_sector_map(filesystem);
    int i;
    size_t byte_number = -1;

    for (i = 0; i < SECTOR_MAP_SIZE; ++i) {
        if (sector_map[i] != 255) {
            byte_number = i;
            break;
        }
    }

    if (byte_number != -1) {
        return byte_number * 8 + get_first_zero_bit(sector_map[byte_number]);
    }
    else {
        exit(27);
    }
}

void reserve_sector_on_map(struct Filesystem* fs, size_t sector_number) {
    set_value_on_map(fs, sector_number, 1);
}

void remove_sector_on_map(struct Filesystem* fs, size_t sector_number) {
    set_value_on_map(fs, sector_number, 0);
}

void set_value_on_map(struct Filesystem* fs, size_t sector_number, unsigned char value) {
    size_t index = sector_number / 8;
    unsigned char bit = sector_number % 8;

    unsigned char new_byte;

    if (value == 0) {
        new_byte = erase_bit(fs->sector_map[index], bit);
    }
    else {
        new_byte = fill_bit(fs->sector_map[index], bit);
    }

    fs->sector_map[index] = new_byte;
}

void reserve_first_sector_on_map(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty_sector(fs);

    reserve_sector_on_map(fs, index);
}

size_t get_first_zero_bit(unsigned char byte) {
    
    size_t number = 0;

    while (byte > 0) {
        byte /= 2;
        ++number;
    }

    return number;
}

size_t fill_bit(unsigned char byte, unsigned char bit) {
    return byte | (1 << bit);
}

size_t erase_bit(unsigned char byte, unsigned char bit) {
    return byte & (255 - (1 << bit));
}
