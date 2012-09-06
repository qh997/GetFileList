#ifndef __DIRBOX_H__
#define __DIRBOX_H__

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"
#include "filebox.h"

typedef struct st_dirbox
{
    char name[FILENAME_MAX];
    struct stat *info;

    int child_count;
    struct st_dirbox *child;

    int baby_count;
    ST_FileBox *baby;

    struct st_dirbox *front;
    struct st_dirbox *rear;
} ST_DirBox;

typedef void FN_ChildBehavior(ST_DirBox *, char *, int);

Status fill_dirbox(ST_DirBox *dirbox, char *dirpath);
Status init_dirbox(ST_DirBox *dirbox, const char *dirname, struct stat *dirstat);
Status add_dirbox_child(ST_DirBox *parent_box, ST_DirBox *child_box);
Status add_dirbox_baby(ST_DirBox *dbox, ST_FileBox *fbaby);
off_t get_dirbox_size(ST_DirBox *dirbox);
time_t get_dirbox_date(ST_DirBox *dirbox);
void for_each_child(ST_DirBox *dirbox, char *current_path, int current_depth, FN_ChildBehavior *action);

#endif //__DIRBOX_H__
