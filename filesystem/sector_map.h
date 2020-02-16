#ifndef FILESYSTEM_MAPPER_H
#define FILESYSTEM_MAPPER_H

#include "classes.h"
#include "configs.h"
#include <stdlib.h>

size_t fill_bit(unsigned char byte, unsigned char bit);
size_t erase_bit(unsigned char byte, unsigned char bit);
size_t get_first_zero_bit(unsigned char byte);
void set_value_on_map(unsigned char* map, size_t sector_number, unsigned char value);

size_t get_number_of_first_empty(unsigned char* map, size_t map_size) {
    int i;
    size_t byte_number = -1;

    for (i = 0; i < map_size; ++i) {
        if (map[i] != 255) {
            byte_number = i;
            break;
        }
    }

    if (byte_number != -1) {
        return byte_number * 8 + get_first_zero_bit(map[byte_number]);
    }
    else {
        exit(27);
    }
}

void reserve_on_map(unsigned char* map, size_t element_number) {
    set_value_on_map(map, element_number, 1);
}

void remove_on_map(unsigned char* map, size_t element_number) {
    set_value_on_map(map, element_number, 0);
}

void set_value_on_map(unsigned char* map, size_t sector_number, unsigned char value) {
    size_t index = sector_number / 8;
    unsigned char bit = sector_number % 8;

    unsigned char new_byte;

    if (value == 0) {
        new_byte = erase_bit(map[index], bit);
    }
    else {
        new_byte = fill_bit(map[index], bit);
    }

    map[index] = new_byte;
}

void remove_first_on_map(unsigned char* map, size_t map_size) {
    size_t index = get_number_of_first_empty(map, map_size);

    remove_on_map(map, index);
}

void reserve_first_on_map(unsigned char* map, size_t map_size) {
    size_t index = get_number_of_first_empty(map, map_size);

    reserve_on_map(map, index);
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

short check_bit(unsigned char byte, unsigned char bit) {
    return (byte & (1 << bit)) > 0;
}

short check_bit_by_index(unsigned char* map, size_t index) {
    return check_bit(map[index / 8], index % 8);
}

#endif //FILESYSTEM_MAPPER_H