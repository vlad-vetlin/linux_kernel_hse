#include "configs.h"
#include <stdio.h>

char* make_sector(Filesystem* fs) {
    size_t index = get_number_of_first_empty_sector(fs);

    reserve_sector_on_map(fs, index);

    return get_pointer_to_sector(index);    
}

void print_sector(size_t sector_index) {
    char* sector = get_pointer_to_sector(sector_index);
    int i;

    for (t = 0; i < SECTOR_SIZE; ++i) {
        printf("%c", sector[i]);
    }
}

char* get_pointer_to_sector(size_t index) {
    return fs->sectors + index * SECTOR_SIZE;
}
