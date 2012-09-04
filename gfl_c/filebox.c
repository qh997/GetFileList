#include "filebox.h"

Status InitFileBox(FileBox *fbox, char *filename, struct stat *filestat)
{
    if (NULL == fbox)
        return ERROR;

    strcpy(fbox->fname, filename);
    fbox->fstat = filestat;

    fbox->front = NULL;
    fbox->rear = NULL;

    return OK;
}