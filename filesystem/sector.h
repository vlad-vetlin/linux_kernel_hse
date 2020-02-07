#include "configs.h"
#include <stdio.h>

char* make_sector(Filesystem* fs) {
    size_t index = get_number_of_first_empty_sector(fs);

    reserve_sector_on_map(fs, index);

    return get_pointer_to_sector(index);    
}

char* get_pointer_to_sector(size_t index) {
    return fs->sectors + index * SECTOR_SIZE;
}
