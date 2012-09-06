#include "filebox.h"

Status init_filebox(ST_FileBox *filebox, char *filename, struct stat *filestat)
{
    if (NULL == filebox)
        return ERROR;

    strcpy(filebox->name, filename);
    filebox->info = (struct stat *)malloc(sizeof(struct stat));
    memcpy(filebox->info, filestat, sizeof(struct stat));

    filebox->front = NULL;
    filebox->rear = NULL;

    return OK;
}

mode_t get_filebox_mode(ST_FileBox *filebox)
{
    return filebox->info->st_mode;
}

off_t get_filebox_size(ST_FileBox *filebox)
{
    return filebox->info->st_size;
}

time_t get_filebox_date(ST_FileBox *filebox)
{
    return filebox->info->st_mtime;
}
