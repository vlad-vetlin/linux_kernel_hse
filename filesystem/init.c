#include "system.h"

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

    set_root_file_as_cur(&fs, &sess);

    while (1) {
        command_parse(&fs, &sess);

        printf("\n_____________________________________\n");
    }

    free_filesystem(&fs);
}
