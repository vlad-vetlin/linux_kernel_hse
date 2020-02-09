#pragma once

#include "sector.h"
#include "configs.h"
#include "sector_map.h"
#include "system.h"

// parent_inode_index - 4
// is_file - 1
// size - 4
// name - 8
// sector_pointer - 8
//
// full_size = 25
struct Inode {
    size_t parent_inode_index;
    bool is_file;
    size_t size;

    char* name;

    Sector* sector_pointer;
};

// remove inode from mapped and delete all sectors
void clear_inode(struct Filesystem* fs, size_t inode_index);

// create new inode with parent_inode and file.
// without checking parent node. Only set value
size_t create_inode(struct Filesystem* fs, size_t parent_inode_index, bool is_file);

// get sector on the last layer by indirect level
size_t get_sector_count_by_indirect_level(size_t indirect_level);

// calculate memory size of the last layer by indirect level
size_t calc_size_with_indirect_level(size_t indirect_level) {
    return get_sector_count_by_indirect_level(indirect_level) * SECTOR_DATA_SIZE;
}

// calculate pow
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

// calculate indirect level for indobe by of it size
size_t needed_inode_indirect_level(size_t size) {
    int i = 0;

    size_t cur_size = calc_size_with_indirect_level(i);
    while (cur_size < size) {
        ++i;
        cur_size = calc_size_with_indirect_level(i);
    }

    return i;
}

// return count of sectors on last layer by indirect level
size_t get_sector_count_by_indirect_level(size_t indirect_level) {
    return pow(SECTOR_INDIRECT_COUNT, indirect_level);
}

// get pointer to sector in current indirect level
size_t get_pointer_in_cur_indirect_level(size_t indirect_level, size_t full_indirect_level, size_t dest_index) {
    size_t step_size = get_sector_count_by_indirect_level(full_indirect_level - indirect_level - 1);

    return dest_index / step_size;
}

// get global sector index by it index in inode
size_t get_sector_index_in_inode(struct Filesystem* fs, size_t inode_index, size_t sector_index_in_inode) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);
    int i;

    struct Sector* sector_pointer = fs->inode_system[inode_index].sector_pointer;
    size_t sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);

    for (i = 1; i < indirect_level; ++i) {
        size_t index = get_pointer_in_cur_indirect_level(i, indirect_level, sector_index_in_inode);

        sector_index_in_inode -= get_sector_count_by_indirect_level(indirect_level - i - 1) * index;

        sector_pointer = get_pointer_in_sector_by_index(fs, sector_index, index);
        sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);
    }

    return sector_index;
}

// copy one inode to another
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

// delete inode and all sectors
void delete_inode(struct Filesystem* fs, size_t inode_index) {
    clear_inode(fs, inode_index);

    remove_on_map(fs->inode_map, inode_index);
}

size_t get_real_sector_count_by_indirect_level(size_t indirect_level) {
    size_t sum = 0;
    int i;

    for (i = 0; i <= indirect_level; ++i) {
        sum += get_sector_count_by_indirect_level(i);
    }

    return sum;
}

// get real size of inode
size_t get_inode_real_size(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;

    size_t indirect_level = needed_inode_indirect_level(inode_size);

    return get_real_sector_count_by_indirect_level(indirect_level) * SECTOR_SIZE;
}

// get count of not empty sectors in last layer of inode
// with half empty sector
size_t get_filled_sectors(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;

    size_t result = inode_size / SECTOR_DATA_SIZE;
    if (inode_size % SECTOR_DATA_SIZE > 0) {
        ++result;
    }

    return result;
}

// create new inode with indirect level. IMPORTANT needed indirect level after this will work bad
size_t create_inode_with_indirect_level(struct Filesystem* fs, size_t parent_inode_index, bool is_file, size_t indirect_level) {
    size_t inode_index = create_inode(fs, parent_inode_index, is_file);
    int i;

    for (i = 0; i < indirect_level; ++i) {
        size_t sector_count = get_sector_count_by_indirect_level(i);

        int j;
        for (j = 0; j < sector_count; ++j) {
            size_t sector_index = get_sector_index_in_inode(fs, inode_index, j);

            make_sector_indirect(fs, sector_index);
        }
    }

    return inode_index;
}

// add indirect level to inode
size_t add_indirect_level_to_inode(struct Filesystem* fs, size_t inode_index) {
    size_t cur_indirect_level = needed_indode_indirect_level(fs, inode_index);

    struct Inode* inode = fs->inode_system[inode_index];

    size_t new_inode_index = create_inode_with_indirect_level(fs, inode->parent_inode_index, inode->is_file, cur_indirect_level + 1);

    copy_inode(fs, new_inode_index, inode_index);
    delete_inode(fs, inode_index);

    return new_inode_index;
}

// decrement indirect level
void decrement_indirect_level_in_inode(struct Filesystem* fs, size_t inode_index) {
    size_t size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(size);

    fs->inode_system[inode_index].size = caclc_size_with_indirect_level(indirect_level - 1);
}

//
void remove_all_sectors_in_inode(struct Filesystem* fs, size_t inode_index) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);
    int i;

    while (indirect_level > 0) {
        size_t sector_count = get_sector_count_by_indirect_level(indirect_level);

        for (i = 0; i < sector_count; ++i) {
            size_t sector_index = get_sector_index_in_inode(fs, inode_index, i);

            remove_sector(fs, sector_index);
        }

        decrement_indirect_level_in_inode(fs, inode_index);
        inode_size = fs->inode_system[inode_index].size;
        indirect_level = needed_inode_indirect_level(inode_size);
    }
}

// create new inode
size_t create_inode(struct Filesystem* fs, size_t parent_inode_index, bool is_file) {
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

// append data to the end of inode
void append_data_to_inode(struct Filesystem* fs, size_t inode_index, char* data, size_t input_data_size) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t new_size = inode_size + input_data_size;

    if (needed_inode_indirect_level(new_size) != needed_inode_indirect_level(inode_size)) {
        add_indirect_level_to_inode(inode_index);
    }

    size_t cur_sector_index_in_inode = get_filled_sectors(fs, inode_index);

    while (input_data_size > 0) {
        size_t sector_index = get_sector_index_in_inode(fs, cur_sector_index_in_inode);

        append_to_sector(fs, sector_index, data, input_data_size);

        input_data_size -= SECTOR_DATA_SIZE;
        if (input_data_size > 0) {
            data += SECTOR_DATA_SIZE;
        }

        ++cur_sector_index_in_inode;
    }

    fs->inode_system[inode_index].size = new_size;
}

// clear all data in inode
void clear_inode(struct Filesystem* fs, size_t inode_index) {
    remove_all_sectors_in_inode(fs, inode_index);
    
    fs->inode_system[inode_index].size = 0;
}

//  print inode data
void print_inode(struct Filesystem* fs, size_t inode_index) {
    int i;
    size_t filled_sectors = get_filled_sectors(fs, inode_index);

    for (i = 0; i < filled_sectors; ++i) {
        size_t sector_index = get_sector_index_in_inode(fs, inode_index, i);

        print_sector(fs, sector_index);
    }
}

// print all metadata in inode
void print_metadata(struct Filesystem* fs, size_t inode_index) {
    struct Inode* inode = (fs->inode_system + inode_index);

    printf("is_file: %d, name: %s, size: %d, real_size: %d\n", inode->is_file, inode->name, inode->size, get_inode_real_size(fs, inode_index));
}
