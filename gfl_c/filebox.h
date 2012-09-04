#ifndef _FILEBOX_H
#define _FILEBOX_H

#include <sys/stat.h>

#include "common.h"

typedef struct fileBox
{
    char fname[FILENAME_MAX];

    struct stat *fstat;

    struct fileBox *front;
    struct fileBox *rear;
} FileBox;

Status InitFileBox(FileBox *fbox, char *filename, struct stat *filestat);

#endif //_FILEBOX_H
