#include "system.h"
#include "sector_map.h"
#include <stdio.h>
#include "sector.h"

int main() {
    struct Filesystem *fs = malloc(sizeof(fs));

    init_filesystem(fs);

    size_t sector = make_sector(fs);

    update_sector(fs, sector, "test\ntest\n\0");
    
    print_sector(fs, sector);

    update_sector(fs, sector, "kek\n\0");

    print_sector(fs, sector);

    size_t sector_2 = make_sector(fs);

    update_sector(fs, sector_2, "zu\n\0");

    print_sector(fs, sector);
    print_sector(fs, sector_2);

    free_filesystem(fs);
    free(fs);
}
