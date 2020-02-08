#pragma once

#include "sector.h"
#include "configs.h"
#include "sector_map.h"
#include "system.h"

// size 8 + 1 + 8 = 19
struct Inode {
    size_t parent_inode_index;
    bool is_file;
    size_t size;

    char* name;

    Sector* sector_pointer;
};

void clear_inode(struct Filesystem* fs, size_t inode_index);

size_t create_inode(struct Filesystem* fs, size_t parent_inode_index, bool is_file);

size_t get_sector_count_by_indirect_level(size_t indirect_level);

size_t calc_size_with_indirect_level(size_t indirect_level) {
    return get_sector_count_by_indirect_level(indirect_level) * SECTOR_DATA_SIZE;
}

size_t pow(size_t value, size_t p) {
    if (p == 0) {
        return 1;
    } else if (p == 1) {
        return value;
    } else {
        size_t k = pow(value, p / 2);

        return k * k * pow(value, p % 2);
    }
}

size_t needed_inode_indirect_level(size_t size) {
    int i = 1;

    size_t cur_size = calc_size_with_indirect_level(i);
    while (cur_size < size) {
        ++i;
        size_t cur_size = calc_size_with_indirect_level(i);
    }

    return i;
}

size_t get_sector_count_by_indirect_level(size_t indirect_level) {
    return pow(SECTOR_INDIRECT_COUNT, indirect_level + 1);
}

size_t get_pointer_in_cur_indiect_level(size_t indirect_level, size_t full_indirect_level, size_t dest_index) {}

size_t get_sector_index_in_inode(struct Filesystem* fs, size_t inode_index, size_t sector_index_in_inode) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);
    int i;

    Sector* sector_pointer = fs->inode_system[inode_index].sector_pointer;
    size_t sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);

    for (i = 1; i < indirect_level; ++i) {
        size_t index = get_pointer_in_cur_indirect_level(i, indirect_level, sector_index_in_inode);

        sector_pointer = get_pointer_in_sector_by_index(sector_index, index);
        sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);
    }

    return sector_index;
}

void copy_inode(struct Filesystem* fs, size_t to_inode_index, size_t from_inode_index) {
    int i;

    size_t from_inode_size = fs->inode_system[from_inode_index].size;
    size_t from_indirect_level = needed_inode_indirect_level(from_inode_size);
    size_t from_inode_sector_count = get_sector_count_by_indirect_level(from_indirect_level);

    for (i = 0; i < from_inode_sector_count; ++i) {
        size_t from_sector_index = get_sector_index_in_inode(fs, from_inode_index, i);
        size_t to_sector_index = get_sector_index_in_inode(fs, to_inode_index, i);

        copy_sector(fs, to_sector_index, from_sector_index);
    }
}

void delete_inode(struct Filesystem* fs, size_t inode_index) {
    clear_inode(fs, inode_index);

    remove_on_map(fs->inode_map, inode_index);
}

size_t get_inode_real_size(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;

    size_t indirect_level = needed_inode_indirect_level(inode_size);

    return get_sector_count_by_indirect_level(indirect_level) * SECTOR_SIZE;
}

size_t get_filled_sectors(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;

    size_t result = inode_size / SECTOR_DATA_SIZE;
    if (inode_size % SECTOR_DATA_SIZE > 0) {
        ++result;
    }

    return result;
}

size_t create_inode_with_indirect_level(struct Filesystem* fs, size_t parent_inode_index, bool is_file, size_t indirect_level) {
    size_t inode_index = create_inode(fs, parent_inode_index, is_file);
    int i;

    for (i = 0; i < indirect_level; ++i) {
        size_t sector_count = get_sector_count_by_indirect_level(i);

        int j;
        for (j = 0; j < sector_count; ++j) {
            size_t sector_index = get_sector_index_in_inode(fs, inode_index, j);

            make_sector_indirect(sector_index);
        }
    }

    return inode_index;
}

size_t add_indirect_level_to_inode(struct Filesystem* fs, size_t inode_index) {
    size_t cur_indirect_level = needed_indode_indirect_level(fs, inode_index);

    size_t new_inode_index = create_inode_with_indirect_level(cur_indirect_level + 1);

    copy_inode(fs, new_inode_index, inode_index);
    delete_inode(fs, inode_index);

    return new_inode_index;
}

void decrease_indirect_level_from_inode(struct Filesystem* fs, size_t inode_index) {
    size_t size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(size);
    fs->inode_system[inode_index].size = caclc_size_with_indirect_level(indirect_level);
}

void remove_all_sectors_in_inode(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);
    int i;

    while (indirect_level > 0) {
        size_t sector_count = get_sector_count_by_indirect_level(indirect_level);

        for (i = 0; i < sector_count; ++i) {
            size_t sector_index = get_sector_index_in_inode(fs, i);

            remove_sector(fs, sector_index);
        }

        decrease_indirect_level_from_inode(fs, inode_index);
        inode_size = fs->inode_system[inode_index].size;
        indirect_level = needed-inode_indirect_level(inode_size);
    }
}

size_t create_indode(struct Filesystem* fs, size_t parent_inode_index, bool is_file) {
    size_t inode_index = get_number_of_first_empty(fs->inode_map, INODE_CONT);

    reserve_on_map(fs->inode_map, inode_index);

    fs->inode_system[inode_index].name = "";
    fs->inode_system[inode_index].is_file = is_file;
    fs->inode_system[inode_index].size = 0;
    fs->inode_system[inode_index].parent_inode = parent_inode;

    size_t sector_index = make_sector(fs);

    fs->inode_system[indoe_index].sector_pointer = get_pointer_to_sector(fs, sector_index);

    return inode_index;
}

void append_data_to_inode(struct Filesystem* fs, size_t inode_index, char* data, size_t input_data_size) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t new_size = inode_size + input_data_size;

    if (needed_inode_indirect_level(new_size) != needed_inode_indirect_level(inode_size)) {
        add_indirect_level_to_inode(inode_index);
    }

    size_t cur_sector_index_in_inode = get_filed_sectors_in_inode(fs->inode_system[inode_index]);

    while (input_data_size > 0) {
        ++cur_sector_index_in_inode;

        size_t sector_index = get_sector_index_in_inode(fs, cur_sector_index_in_inode);

        append_to_sector(fs, sector_index, data, input_data_size);

        input_data_size -= SECTOR_DATA_SIZE;
        if (input_data_size > 0) {
            data += SECTOR_DATA_SIZE;
        }
    }

    fs->inode_system[inode_index].size = new_size;
}

void clear_inode(struct Filesystem* fs, size_t inode_index) {
    remove_all_sectors_in_inode(fs, inode_index);
    
    fs->inode_system[inode_index].size = 0;
    fs->inode_system[inode_index].filled_sectors = 0;
}

void print_inode(struct Filesystem* fs, size_t inode_index) {
    int i;
    size_t filled_sectors = get_filled_sectors_in_inode(fs->inode_system[inode_index]);

    for (i = 0; i < filled_sectors; ++i) {
        size_t sector_index = get_sector_index_in_inode(fs, i);

        print_sector(fs, sector_index);
    }
}

void print_metadata(struct Filesystem* fs, size_t inode_index) {
    struct Inode* inode = (fs->inode_system + inode_index);

    printf("is_file: %d, name: %s, size: %d, real_size: %d\n", inode->is_file, inode->name, inode->size, get_inode_real_size(fs, inode_index));
}
