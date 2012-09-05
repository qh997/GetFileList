#include "filebox.h"

Status InitFileBox(FileBox *fbox, char *filename, struct stat *filestat)
{
    if (NULL == fbox)
        return ERROR;

    strcpy(fbox->fname, filename);
    fbox->fstat = (struct stat *)malloc(sizeof(struct stat));
    memcpy(fbox->fstat, filestat, sizeof(struct stat));

    fbox->front = NULL;
    fbox->rear = NULL;

    return OK;
}

int FileSizeOf(FileBox *fbox)
{
    return fbox->fstat->st_size;
}

int FileDateOf(FileBox *fbox)
{
    return fbox->fstat->st_mtime;
}
