#include "dirbox.h"

Status FillDirBox(char *dpath, DirBox *dbox)
{
    DEBUG_LOG(3, "FillDirBox : %s", dpath);

    DIR *dir = opendir(dpath);
    if (NULL != dir)
    {
        struct dirent *entry = NULL;
        while (NULL != (entry = readdir(dir)))
        {
            char fpath[FILENAME_MAX];
            strcat(strcat(strcpy(fpath, dpath), PATHSEPS), entry->d_name);
            struct stat statbuf;
            lstat(fpath, &statbuf);

            if(S_ISDIR(statbuf.st_mode))
            {
                if(0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name))
                    continue;

                DEBUG_VAR(2, "%s", fpath);

                DirBox *subDir;
                if (NULL != (subDir = (DirBox *)malloc(sizeof(DirBox))))
                {
                    InitDirBox(subDir, entry->d_name);
                    FillDirBox(fpath, subDir);

                    AddChild(dbox, subDir);
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

                FileBox *infile = NULL;
                if (NULL != (infile = (FileBox *)malloc(sizeof(FileBox))))
                {
                    InitFileBox(infile, entry->d_name, &statbuf);
                    DEBUG_LOG(5, "InitFileBox OK", "");
                    AddBaby(dbox, infile);
                    DEBUG_LOG(5, "AddBaby OK", "");
                }
                else
                {
                    fprintf(stderr, "malloc failed.\n");
                    return OVERFLOW;
                }
            }
        }

        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Cannot open directory: %s\n", dpath);
    }

    return OK;
}

Status InitDirBox(DirBox *dbox, const char *dname)
{
    if (dbox == NULL)
        return ERROR;

    strcpy(dbox->name, dname);
    dbox->child_count = 0;
    dbox->child = NULL;
    dbox->baby_count = 0;
    dbox->baby = NULL;

    dbox->front = NULL;
    dbox->rear = NULL;

    return OK;
}

Status AddChild(DirBox *dbox, DirBox *dchild)
{
    if (dbox == NULL)
        return ERROR;

    (dbox->child_count)++;
    if (NULL != dbox->child)
    {
        dbox = dbox->child;
        while (NULL != dbox->rear)
        {
            dbox = dbox->rear;
        }
        dbox->rear = dchild;
        dchild->front = dbox;
    }
    else
    {
        dbox->child = dchild;
    }

    return OK;
}

Status AddBaby(DirBox *dbox, FileBox *fbaby)
{
    if (dbox == NULL)
        return ERROR;

    (dbox->baby_count)++;
    if (NULL != dbox->baby)
    {
        FileBox *fbox = dbox->baby;

        while (NULL != fbox->rear)
            fbox = fbox->rear;

        fbox->rear = fbaby;
        fbaby->front = fbox;
    }
    else
    {
        dbox->baby = fbaby;
    }

    return OK;
}

Status ForEachChild(DirBox *dbox, char *parent, int depth, pDealWithChild *dwc)
{
    DEBUG_VAR(3, "%s", parent);
    DirBox *childbox = dbox->child;

    char fpath[FILENAME_MAX];
    strcpy(fpath, parent);
    if (0 != strcmp(parent, "")) strcat(fpath, PATHSEPS);

    while (NULL != childbox)
    {
        char fcpath[FILENAME_MAX];
        strcpy(fcpath, fpath);
        strcat(fcpath, childbox->name);

        ForEachChild(childbox, fcpath, depth + 1, dwc);
        childbox = childbox->rear;
    }

    dwc(dbox, fpath, depth);
}
