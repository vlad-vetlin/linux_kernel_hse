#include "system.h"
#include "sector_map.h"
#include <stdio.h>

int main() {
    struct Filesystem *fs = malloc(sizeof(fs));
    int i;

    init_filesystem(fs);

    for (i = 0; i < 100; ++i) {
        printf("%ld\n", get_number_of_first_empty_sector(fs));
        reserve_first_sector_on_map(fs);
    }

    free_filesystem(fs);
    free(fs);
}
