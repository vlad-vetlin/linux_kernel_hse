#ifndef FILESYSTEM_COMMANDS_H
#define FILESYSTEM_COMMANDS_H

#include <stdio.h>

#include "string.h"
#include "classes.h"
#include "ls.h"
#include "make.h"
#include "change_dir.h"

char* get_filename() {
    char filename[20];

    scanf("%s", filename);

    size_t len = strlen(filename) + 1;
    char* filename_in_memory = malloc(len);

    int i;
    for (i = 0; i < len - 1; ++i) {
        filename_in_memory[i] = filename[i];
    }

    filename_in_memory[len - 1] = '\0';

    return filename_in_memory;
}

void command_parse(struct Filesystem* fs, struct Session* sess) {
    char command_name[10];

    scanf("%s", command_name);

    if (strcmp(command_name, "ls") == 0) {
        ls(fs, sess);
    } else if (strcmp(command_name, "mk") == 0) {
        short is_file;

        char* filename = get_filename();

        scanf("%hd", &is_file);

        make(fs, sess, filename, is_file);
    } else if (strcmp(command_name, "cd") == 0) {

        char* filename = get_filename();

        change_dir(fs, sess, filename);
    }
}

#endif //FILESYSTEM_COMMANDS_H
