#include "classes.h"
#include "configs.h"
#include "inode.h"
#include "sector.h"
#include "sector_map.h"
#include "system.h"
#include "session.h"
#include "commands.h"

int main() {
    struct Filesystem fs;
    struct Session sess;


    init_filesystem(&fs);

    size_t root_inode = create_inode_with_name(&fs, NULL, 0, "test");
//    printf("\n***\n");
//    create_inode_with_name(fs, get_inode_pointer(fs, root_inode), 0, "kek");
//    printf("\n***\n");
//    create_inode_with_name(fs, get_inode_pointer(fs, root_inode), 0, "lol");
//    printf("\n***\n");
//    create_inode_with_name(fs, get_inode_pointer(fs, root_inode), 0, "zu");
//    printf("\n***\n");

    set_root_file_as_cur(&fs, &sess);

    while (1) {
        command_parse(&fs, &sess);

        printf("\n_____________________________________\n");
    }

    free_filesystem(&fs);
}
