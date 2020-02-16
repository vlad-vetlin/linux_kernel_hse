#ifndef FILESYSTEM_SYSTEM_H
#define FILESYSTEM_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "classes.h"
#include "configs.h"
#include "sector_map.h"
#include "inode.h"
#include "sector.h"

unsigned char read_char_data(void* file) {
    unsigned char result;

    fscanf(file, "%c", &result);

    return result;
}

void save_char_data(void* file, unsigned char data) {
    fprintf(file, "%c", data);
}

void read_sector(struct Filesystem* fs, size_t sector_index, void* file) {
    fscanf(file, "%ld ", &fs->sectors[sector_index].size);

    if (fs->sectors[sector_index].size > 0) {
        fgets(fs->sectors[sector_index].data, fs->sectors[sector_index].size + 1, file);
    }
}

void save_sector_to_file(struct Filesystem* fs, void* file, struct Sector* sector) {
    fprintf(file, " %ld ", sector->size);

    if (sector->size > 0) {
        int i;

        for (i = 0; i < sector->size; ++i) {
            fprintf(file, "%c", sector->data[i]);
        }
    }
}

void read_inode(struct Filesystem* fs, size_t inode_index, void* file) {
    struct Inode* inode = get_inode_pointer(fs, inode_index);

    size_t inode_index_from_mem;
    fscanf(file, "%ld", &inode_index_from_mem);

    if (inode_index_from_mem == (size_t)(-1)) {
        inode->parent_inode = NULL;
    } else {
        inode->parent_inode = get_inode_pointer(fs, inode_index_from_mem);
    }

    size_t name_size;
    fscanf(file, "%hd", &inode->is_file);
    fscanf(file, "%ld", &inode->size);
    fscanf(file, "%ld ", &name_size);

    inode->name = (char*)malloc(name_size);

    fgets(inode->name, name_size + 1, file);

    size_t sector_index;
    fscanf(file, " %ld", &sector_index);
    inode->sector_pointer = get_pointer_to_sector(fs, sector_index);
}

void save_inode_to_file(struct Filesystem* fs, void* file, struct Inode* inode) {
    if (inode->parent_inode == NULL) {
        fprintf(file, " %ld", (size_t)-1);
    } else {
        fprintf(file, " %ld", get_inode_index(fs, inode->parent_inode));
    }

    fprintf(file, " %hhd", inode->is_file);
    fprintf(file, " %ld", inode->size);
    fprintf(file, " %ld ", strlen(inode->name));
    fprintf(file, "%s", inode->name);

    fprintf(file, " %ld", get_index_to_sector_by_pointer(fs, inode->sector_pointer));
}

void init_filesystem(struct Filesystem* fs) {
    int i;

    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
    fs->sector_map = (unsigned char*)malloc(SECTOR_MAP_SIZE);
    fs->inode_map = (unsigned char*)malloc(INODE_MAP_SIZE);
    fs->inode_system = (struct Inode*)malloc(INODE_FULL_SIZE);
    fs->sectors = (struct Sector*)malloc(SECTOR_FULL_SIZE);

    fs->fin = fopen("filesystem", "rb");

    if (fs->fin != NULL) {

        for (i = 0; i < SECTOR_MAP_SIZE; ++i) {
            fs->sector_map[i] = read_char_data(fs->fin);
        }

        for (i = 0; i < INODE_MAP_SIZE; ++i) {
            fs->inode_map[i] = read_char_data(fs->fin);
        }

        for (i = 0; i < SECTOR_COUNT; ++i) {
            if (check_bit_by_index(fs->sector_map, i)) {
                printf("1");
                read_sector(fs, i, fs->fin);
            } else {
                printf("0");
            }
        }

        printf("\n");

        for (i = 0; i < INODE_COUNT; ++i) {
            if (check_bit_by_index(fs->inode_map, i)) {
                printf("1");
                read_inode(fs, i, fs->fin);
            } else {
                printf("0");
            }
        }

        printf("\n");

        printf("%ld\n", get_number_of_first_empty(fs->sector_map, SECTOR_MAP_SIZE));
        printf("%ld\n", get_number_of_first_empty(fs->inode_map, INODE_MAP_SIZE));

        fclose(fs->fin);
    } else {
        size_t root_inode = create_inode_with_name(fs, NULL, 0, "test");
    }


}

unsigned char* get_sector_map(struct Filesystem* fs) {
    return fs->sector_map;
}

struct Sector* get_sectors(struct Filesystem* fs) {
    return fs->sectors;
}

unsigned char* get_inode_map(struct Filesystem* fs) {
    return fs->inode_map;
}

struct Inode* get_inode_system(struct Filesystem* fs) {
    return fs->inode_system;
}

void save_filesystem(struct Filesystem* fs) {
    int i;

    fs->fout = fopen("filesystem", "wb");

    for (i = 0; i < SECTOR_MAP_SIZE; ++i) {
        save_char_data(fs->fout, fs->sector_map[i]);
    }

    for (i = 0; i < INODE_MAP_SIZE; ++i) {
        save_char_data(fs->fout, fs->inode_map[i]);
    }

    for (i = 0; i < SECTOR_COUNT; ++i) {
        if (check_bit_by_index(fs->sector_map, i)) {
            save_sector_to_file(fs, fs->fout, &fs->sectors[i]);
        }
    }

    for (i = 0; i < INODE_COUNT; ++i) {
        if (check_bit_by_index(fs->inode_map, i)) {
            save_inode_to_file(fs, fs->fout, &fs->inode_system[i]);
        }
    }

    fclose(fs->fout);
}

void free_filesystem(struct Filesystem* fs) {
    int i;

    save_filesystem(fs);

    for (i = 0; i < INODE_MAP_SIZE; ++i) {
        if (check_bit_by_index(fs->inode_map, i)) {
            free(fs->inode_system[i].name);
        }
    }

    free(fs->sector_map);
    free(fs->inode_map);
    free(fs->inode_system);
    free(fs->sectors);
}

size_t find_index_of_root_inode(struct Filesystem* fs) {
    int i;
    for (i = 0; i < INODE_COUNT; ++i) {
        if (check_bit_by_index(fs->inode_map, i) && inode_is_root(fs, i)) {
            return i;
        }
    }
}

#endif //FILESYSTEM_SYSTEM_H