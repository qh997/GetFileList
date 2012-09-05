#ifndef _DIRBOX_H
#define _DIRBOX_H

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"
#include "filebox.h"

typedef struct dirBox
{
    char name[FILENAME_MAX];
    struct stat *dstat;

    int child_count;
    struct dirBox *child;

    int baby_count;
    FileBox *baby;

    struct dirBox *front;
    struct dirBox *rear;
} DirBox;

typedef void (pDealWithChild)(DirBox *, char *, int);

Status FillDirBox(char *dpath, DirBox *dbox);

Status InitDirBox(DirBox *dbox, const char *dname, struct stat *dirstat);
Status AddChild(DirBox *dbox, DirBox *dchild);
Status AddBaby(DirBox *dbox, FileBox *fbaby);
int DirSizeOf(DirBox *dbox);
int DirDateOf(DirBox *dbox);
Status ForEachChild(DirBox *dbox, char *parent, int depth, pDealWithChild *dwc);

#endif //_DIRBOX_H
