#include "classes.h"
#include "configs.h"
#include "inode.h"
#include "sector.h"
#include "sector_map.h"
#include "system.h"

int main() {
    struct Filesystem *fs = malloc(sizeof(fs));

    init_filesystem(fs);

    size_t inode_index = create_inode_with_name(fs, NULL, 0, "test");

    int i;
    for (i = 0; i < 10; ++i) {
        inode_index = append_data_to_inode(fs, inode_index, "tttttttttttt", 12);
        printf("\n");
        print_metadata(fs, inode_index);

        printf("\n");
        print_inode(fs, inode_index);
        printf("\n");
    }



    free_filesystem(fs);
    free(fs);
}
