#include "dirbox.h"

Status fill_dirbox(ST_DirBox *dirbox, char *dirpath)
{
    DEBUG_LOG(3, "fill_dirbox : %s", dirpath);

    DIR *dir_handle = opendir(dirpath);
    if (NULL != dir_handle)
    {
        struct dirent *entry = NULL;
        while (NULL != (entry = readdir(dir_handle)))
        {
            char entry_path[FILENAME_MAX];
            strcat(strcat(strcpy(entry_path, dirpath), PATHSEPS), entry->d_name);
            struct stat entry_stat;
            lstat(entry_path, &entry_stat);

            if(S_ISDIR(entry_stat.st_mode))
            {
                if(0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name))
                    continue;

                DEBUG_VAR(2, "%s", entry_path);

                ST_DirBox *child_dirbox;
                if (NULL != (child_dirbox = (ST_DirBox *)malloc(sizeof(ST_DirBox))))
                {
                    init_dirbox(child_dirbox, entry->d_name, &entry_stat);
                    fill_dirbox(child_dirbox, entry_path);

                    add_dirbox_child(dirbox, child_dirbox);
                }
                else
                {
                    fprintf(stderr, "malloc failed.\n");
                    return OVERFLOW;
                }
            }
            else
            {
                DEBUG_LOG(2, "find file: %s", entry->d_name);

                ST_FileBox *infile = NULL;
                if (NULL != (infile = (ST_FileBox *)malloc(sizeof(ST_FileBox))))
                {
                    init_filebox(infile, entry->d_name, &entry_stat);
                    DEBUG_LOG(5, "init_filebox OK", "");
                    add_dirbox_baby(dirbox, infile);
                    DEBUG_LOG(5, "add_dirbox_baby OK", "");
                }
                else
                {
                    fprintf(stderr, "malloc failed.\n");
                    return OVERFLOW;
                }
            }
        }

        closedir(dir_handle);
    }
    else
    {
        fprintf(stderr, "Cannot open directory: %s\n", dirpath);
    }

    return OK;
}

Status init_dirbox(ST_DirBox *dirbox, const char *dirname, struct stat *dirstat)
{
    if (dirbox == NULL)
        return ERROR;

    strcpy(dirbox->name, dirname);

    if (NULL != dirstat)
    {
        dirbox->info = (struct stat *)malloc(sizeof(struct stat));
        memcpy(dirbox->info, dirstat, sizeof(struct stat));
    }
    else
        dirbox->info = NULL;

    dirbox->child_count = 0;
    dirbox->child = NULL;
    dirbox->baby_count = 0;
    dirbox->baby = NULL;

    dirbox->front = NULL;
    dirbox->rear = NULL;

    return OK;
}

Status add_dirbox_child(ST_DirBox *dirbox, ST_DirBox *child_box)
{
    if (dirbox == NULL)
        return ERROR;

    (dirbox->child_count)++;
    if (NULL != dirbox->child)
    {
        dirbox = dirbox->child;
        while (NULL != dirbox->rear)
        {
            dirbox = dirbox->rear;
        }
        dirbox->rear = child_box;
        child_box->front = dirbox;
    }
    else
    {
        dirbox->child = child_box;
    }

    return OK;
}

Status add_dirbox_baby(ST_DirBox *dirbox, ST_FileBox *filebox)
{
    if (dirbox == NULL)
        return ERROR;

    (dirbox->baby_count)++;
    if (NULL != dirbox->baby)
    {
        ST_FileBox *dirbox_baby = dirbox->baby;

        while (NULL != dirbox_baby->rear)
            dirbox_baby = dirbox_baby->rear;

        dirbox_baby->rear = filebox;
        filebox->front = dirbox_baby;
    }
    else
    {
        dirbox->baby = filebox;
    }

    return OK;
}

off_t get_dirbox_size(ST_DirBox *dirbox)
{
    return dirbox->info->st_size;
}

time_t get_dirbox_date(ST_DirBox *dirbox)
{
    return dirbox->info->st_mtime;
}

void for_each_child(ST_DirBox *dirbox, char *current_path, int current_depth, FN_ChildBehavior *action)
{
    DEBUG_VAR(3, "%s", current_path);
    ST_DirBox *children = dirbox->child;

    if (0 != strcmp(current_path, ""))
        strcat(current_path, PATHSEPS);

    while (NULL != children)
    {
        char child_path[FILENAME_MAX];
        strcpy(child_path, current_path);
        strcat(child_path, children->name);

        for_each_child(children, child_path, current_depth + 1, action);
        children = children->rear;
    }

    action(dirbox, current_path, current_depth);
}
