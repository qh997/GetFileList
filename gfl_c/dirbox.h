#ifndef _DIRBOX_H
#define _DIRBOX_H

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"

typedef struct dirBox
{
    char dir_name[FILENAME_MAX];

    int dir_count;
    struct dirBox *child;

    struct dirBox *next;
} DirBox;

void traversal_dir(char *dir_path);

#endif //_DIRBOX_H
