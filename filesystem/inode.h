#ifndef FILESYSTEM_INODE_H
#define FILESYSTEM_INODE_H

#include "classes.h"
#include "sector.h"
#include "configs.h"
#include "sector_map.h"

// remove inode from mapped and delete all sectors
void clear_inode(struct Filesystem* fs, size_t inode_index);

// create new inode with parent_inode and file.
// without checking parent node. Only set value
size_t create_inode(struct Filesystem* fs, struct Inode* parent_inode, short is_file);

void print_metadata(struct Filesystem* fs, size_t inode_index);

// get sector on the last layer by indirect level
size_t get_sector_count_by_indirect_level(size_t indirect_level);

// calculate memory size of the last layer by indirect level
size_t calc_size_with_indirect_level(size_t indirect_level) {
    return get_sector_count_by_indirect_level(indirect_level) * SECTOR_DATA_SIZE;
}

// calculate pow
size_t fast_pow(size_t value, size_t p) {
    if (p == 0) {
        return 1;
    } else if (p == 1) {
        return value;
    } else {
        size_t k = fast_pow(value, p / 2);

        return k * k * fast_pow(value, p % 2);
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
    return fast_pow(SECTOR_INDIRECT_COUNT, indirect_level);
}

// get pointer to sector in current indirect level
size_t get_pointer_in_cur_indirect_level(size_t indirect_level, size_t full_indirect_level, size_t dest_index) {
    size_t step_size = get_sector_count_by_indirect_level(full_indirect_level - indirect_level);

    return dest_index / step_size;
}

size_t get_sector_index_in_inode_by_indirect_level(struct Filesystem* fs, size_t inode_index, size_t indirect_level, size_t sector_index_in_inode) {
    int i;

    struct Sector* sector_pointer = fs->inode_system[inode_index].sector_pointer;
    size_t sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);

    for (i = 1; i <= indirect_level; ++i) {
        size_t index = get_pointer_in_cur_indirect_level(i, indirect_level, sector_index_in_inode);

        size_t step_size = get_sector_count_by_indirect_level(indirect_level - i);

        sector_index_in_inode -= step_size * index;

        sector_pointer = (struct Sector*)get_pointer_in_sector_by_index(fs, sector_index, index);
        sector_index = get_index_to_sector_by_pointer(fs, sector_pointer);
    }

    return sector_index;
}

// get global sector index by it index in inode
size_t get_sector_index_in_inode(struct Filesystem* fs, size_t inode_index, size_t sector_index_in_inode) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(inode_size);

    get_sector_index_in_inode_by_indirect_level(fs, inode_index, indirect_level, sector_index_in_inode);
}

// copy one inode to another
void copy_inode(struct Filesystem* fs, size_t to_inode_index, size_t from_inode_index, size_t to_indirect_level) {
    int i;

    size_t from_inode_size = fs->inode_system[from_inode_index].size;
    size_t from_indirect_level = needed_inode_indirect_level(from_inode_size);
    size_t from_inode_sector_count = get_sector_count_by_indirect_level(from_indirect_level);

    for (i = 0; i < from_inode_sector_count; ++i) {
        size_t from_sector_index = get_sector_index_in_inode(fs, from_inode_index, i);
        size_t to_sector_index = get_sector_index_in_inode_by_indirect_level(fs, to_inode_index, to_indirect_level, i);

        copy_sector(fs, to_sector_index, from_sector_index);
    }

    fs->inode_system[to_inode_index].size = from_inode_size;
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

    return get_real_sector_count_by_indirect_level(indirect_level) * SECTOR_SIZE + INODE_SIZE;
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
size_t create_inode_with_indirect_level(struct Filesystem* fs, struct Inode* parent_inode, short is_file, size_t indirect_level) {
    size_t inode_index = create_inode(fs, parent_inode, is_file);

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
    struct Inode inode = fs->inode_system[inode_index];

    size_t cur_indirect_level = needed_inode_indirect_level(inode.size);

    size_t new_inode_index = create_inode_with_indirect_level(fs, inode.parent_inode, inode.is_file, cur_indirect_level + 1);

    copy_inode(fs, new_inode_index, inode_index, cur_indirect_level + 1);
    delete_inode(fs, inode_index);

    return new_inode_index;
}

// decrement indirect level
void decrement_indirect_level_in_inode(struct Filesystem* fs, size_t inode_index) {
    size_t size = fs->inode_system[inode_index].size;
    size_t indirect_level = needed_inode_indirect_level(size);

    fs->inode_system[inode_index].size = calc_size_with_indirect_level(indirect_level - 1);
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
size_t create_inode(struct Filesystem* fs, struct Inode* parent_inode, short is_file) {
    size_t inode_index = get_number_of_first_empty(fs->inode_map, INODE_COUNT);

    reserve_on_map(fs->inode_map, inode_index);

    fs->inode_system[inode_index].name = "";
    fs->inode_system[inode_index].is_file = is_file;
    fs->inode_system[inode_index].size = 0;
    fs->inode_system[inode_index].parent_inode = parent_inode;

    size_t sector_index = make_sector(fs);

    fs->inode_system[inode_index].sector_pointer = get_pointer_to_sector(fs, sector_index);

    return inode_index;
}

// append data to the end of inode
size_t append_data_to_inode(struct Filesystem* fs, size_t inode_index, char* data, size_t input_data_size) {
    size_t inode_size = fs->inode_system[inode_index].size;
    size_t new_size = inode_size + input_data_size;

    size_t new_indirect_level = needed_inode_indirect_level(new_size);

    size_t new_inode_index = inode_index;

    if (new_indirect_level != needed_inode_indirect_level(inode_size)) {
        new_inode_index = add_indirect_level_to_inode(fs, inode_index);
    }

    size_t cur_sector_index_in_inode = get_filled_sectors(fs, new_inode_index) - 1;

    while (input_data_size > 0) {
        size_t sector_index = get_sector_index_in_inode_by_indirect_level(fs, new_inode_index, new_indirect_level, cur_sector_index_in_inode);

        size_t appended_size = append_to_sector(fs, sector_index, data, input_data_size);

        if (input_data_size <= appended_size) {
            break;
        }

        input_data_size -= appended_size;
        data += appended_size;

        ++cur_sector_index_in_inode;
    }

    fs->inode_system[new_inode_index].size = new_size;

    return new_inode_index;
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

struct Inode* get_inode_pointer(struct Filesystem* fs, size_t inode_index) {
    return &(fs->inode_system[inode_index]);
}

// print all metadata in inode
void print_metadata(struct Filesystem* fs, size_t inode_index) {
    struct Inode* inode = get_inode_pointer(fs, inode_index);

    printf("is_file: %d, name: %s, size: %ld, real_size: %ld\n", inode->is_file, inode->name, inode->size, get_inode_real_size(fs, inode_index));
}

size_t get_inode_index(struct Filesystem* fs, struct Inode* inode_pointer) {
    return (size_t) ((inode_pointer - fs->inode_system));
}

void set_inode_name(struct Filesystem* fs, size_t inode_index, char* data) {
    fs->inode_system[inode_index].name = data;
}

size_t create_inode_with_name(struct Filesystem* fs, struct Inode* parent_inode, short is_file, char* name) {
    size_t inode_index = create_inode(fs, parent_inode, is_file);

    set_inode_name(fs, inode_index, name);

    return inode_index;
}

short inode_is_root(struct Filesystem* fs, size_t index) {
    return fs->inode_system[index].parent_inode == NULL;
}


#endif //FILESYSTEM_INODE_H
