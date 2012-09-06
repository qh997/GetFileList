#ifndef __FILEBOX_H__
#define __FILEBOX_H__

#include <sys/stat.h>

#include "common.h"

typedef struct st_filebox
{
    char name[FILENAME_MAX];

    struct stat *info;

    struct st_filebox *front;
    struct st_filebox *rear;
} ST_FileBox;

Status init_filebox(ST_FileBox *filebox, char *filename, struct stat *filestat);
mode_t get_filebox_mode(ST_FileBox *filebox);
off_t get_filebox_size(ST_FileBox *filebox);
time_t get_filebox_date(ST_FileBox *filebox);

#endif //__FILEBOX_H__
