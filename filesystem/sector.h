#ifndef FILESYSTEM_SECTOR_H
#define FILESYSTEM_SECTOR_H

#include "classes.h"
#include "configs.h"
#include "sector_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// return pointer to sector data by sector index
struct Sector* get_pointer_to_sector(struct Filesystem* fs, size_t index);

// save sector in memory
void save_sector(struct Filesystem* fs, size_t sector_index);

// clear sector data
void clear_sector(struct Filesystem* fs, size_t sector_index) {
    fs->sectors[sector_index].size = 0;
}

// reserve on mapper new sector and return index of it sector
size_t make_sector(struct Filesystem* fs) {
    size_t index = get_number_of_first_empty(fs->sector_map, SECTOR_COUNT);

    reserve_on_map(fs->sector_map, index);

    struct Sector* sector = get_pointer_to_sector(fs, index);
    sector->size = 0;

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
    return &(fs->sectors[index]);
}

// return sector index by it pointer
size_t get_index_to_sector_by_pointer(struct Filesystem* fs, struct Sector* pointer) {
    return (size_t) ((pointer - fs->sectors));
}

// append data to end of the sector. Also increase it size
size_t append_to_sector(struct Filesystem* fs, size_t sector_index, char data[], size_t input_data_size) {
    int i;

    size_t sector_size = fs->sectors[sector_index].size;
    size_t size = SECTOR_DATA_SIZE - sector_size;

    if (input_data_size < size) {
        size = input_data_size;
    }

    for (i = 0; i < size; ++i) {
        fs->sectors[sector_index].data[sector_size + i] = data[i];
    }

    fs->sectors[sector_index].size += size;

    save_sector(fs, fs->sectors[sector_index].size);

    return size;
}

// remove sector from mapper
void remove_sector(struct Filesystem* fs, size_t sector_index) {
    remove_on_map(fs->sector_map, sector_index);
}

// copy one sector to another sector
// remove all data from "to sector" before copy
void copy_sector(struct Filesystem* fs, size_t to_sector_index, size_t from_sector_index) {
//    printf("DEBUG: copy from sector %ld to %ld\n", from_sector_index, to_sector_index);

    clear_sector(fs, to_sector_index);
    struct Sector* sector = get_pointer_to_sector(fs, from_sector_index);

    append_to_sector(fs, to_sector_index, sector->data, sector->size);
}

// save sector to memory
void save_sector(struct Filesystem* fs, size_t sector_index) {
}

long long my_atol(char* data, int len){

    unsigned long long ans = 0;
    unsigned long long p = 1;
    int i;

    for(i = len - 1; i >= 0; --i) {
        int cur_value;

        if (data[i] >= '0' && data[i] <= '9') {
            cur_value = data[i] - '0';
        } else {
            cur_value = 10 + (data[i] - 'a');
        }

        ans += p * cur_value;

        p *= 16;
    }

    return ans;
}

// get pointer to sector in indirect sector by index
struct Sector* get_pointer_in_sector_by_index(struct Filesystem* fs, size_t sector_index, size_t index) {
    struct Sector* sector_pointer = get_pointer_to_sector(fs, sector_index);

    int i;
    char s_index[17];

    if (index > 3) {
        exit(1);
    }
    for (i = 0; i < 16; ++i) {
        s_index[i] = sector_pointer->data[16 * index + i];
    }

    s_index[16] = '\0';

    return my_atol(s_index, 16);
}

char* ltoa(long long val, int len){

    static char buf[16] = {0};

    for(; len > 0 ; --len, val /= 16) {
        buf[len - 1] = "0123456789abcdef"[val % 16];
    }

    return &buf[len];

}

// make sector indirect
void make_sector_indirect(struct Filesystem* fs, size_t sector_index) {
    int i;

    for (i = 0; i < SECTOR_INDIRECT_COUNT; ++i) {
        size_t new_sector_index = make_sector(fs);
        struct Sector* sector_pointer = get_pointer_to_sector(fs, new_sector_index);

        char* str_pointer = ltoa((long long)sector_pointer, 16);

        append_to_sector(fs, sector_index, str_pointer, 16);
    }
}

#endif //FILESYSTEM_SECTOR_H