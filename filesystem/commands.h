#ifndef FILESYSTEM_COMMANDS_H
#define FILESYSTEM_COMMANDS_H

#include <stdio.h>

#include "string.h"
#include "classes.h"
#include "ls.h"
#include "make.h"
#include "change_dir.h"
#include "command_utils.h"
#include "echo.h"
#include "cat.h"
#include "rm.h"

void command_parse(struct Filesystem* fs, struct Session* sess) {
    char command_name[10];

    scanf("%s", command_name);

    if (strcmp(command_name, "ls") == 0) {
        ls(fs, sess);
    } else if (strcmp(command_name, "mk") == 0) {
        short is_file;

        char* filename = get_string();

        scanf("%hd", &is_file);

        make(fs, sess, filename, is_file);
    } else if (strcmp(command_name, "cd") == 0) {
        char* filename = get_string();

        change_dir(fs, sess, filename);
    } else if (strcmp(command_name, "echo") == 0) {
        char* filename = get_string();
        char data[100];

        size_t len;
        scanf("%ld ", &len);

        if (len > 100) {
            printf("len is too big. Max is 100\n");
            return;
        }

        fgets(data, len + 1, stdin);

        echo(fs, sess, filename, data);
    } else if (strcmp(command_name, "cat") == 0) {
        char* filename = get_string();

        cat(fs, sess, filename);
    } else if (strcmp(command_name, "rm") == 0) {
        char* filename = get_string();

        remove_command(fs, sess, filename);
    }
}

#endif //FILESYSTEM_COMMANDS_H
