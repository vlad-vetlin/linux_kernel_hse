// size 8 + 1 + 8 = 19
struct Inode {
    size_t parent_inode_index;
    bool is_file;
    size_t size;

    char* name;

    size_t sector_index;
};

void clear_inode(struct Filesystem* fs, size_t inode_index);

size_t needed_inode_indirect_level(struct Filesystem* fs, size_t size) {}

size_t get_sector_count_by_indirect_level(size_t indirect_level) {}

size_t get_sector_index_in_inode(struct Filesystem* fs, size_t inode_index, size_t sector_index_in_inode) {}

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

size_t create_inode_with_indirect_level(struct Filesystem* fs, size_t indirect_level) {}

size_t add_indirect_level_to_inode(struct Filesystem* fs, size_t inode_index) {
    size_t cur_indirect_level = needed_indode_indirect_level(fs, inode_index);

    size_t new_inode_index = create_inode_with_indirect_level(cur_indirect_level + 1);

    copy_inode(fs, new_inode_index, inode_index);
    delete_inode(fs, inode_index);

    return new_inode_index;
}

void decrease_indirect_level_from_inode(struct Filesystem* fs, size_t inode_index) {}

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

void create_indode(struct Filesystem* fs, size_t parent_inode_index, bool is_file) {
    size_t inode_index = get_number_of_first_empty(fs->inode_map, INODE_CONT);

    reserve_on_map(fs->inode_map, inode_index);

    fs->inode_system[inode_index].name = "";
    fs->inode_system[inode_index].is_file = is_file;
    fs->inode_system[inode_index].size = 0;
    fs->inode_system[inode_index].parent_inode = parent_inode;

    size_t sector_index = make_sector(fs);

    fs->inode_system[inode_index].sector_index = sector_index;
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
