#include "gfl.h"

static QueueList *QList = NULL;

int main(int argc, char **argv)
{
    if (ERROR == option_parse(argc, argv))
        return 1;
    debug_args();

    char **dir = get_dirpaths();

    while (0 != strcmp("\0", *dir))
    {
        DirBox *dirBox = (DirBox *)malloc(sizeof(DirBox));
        InitDirBox(dirBox, *dir, NULL);
        FillDirBox(*dir++, dirBox);

        QList = (QueueList *)malloc(sizeof(QueueList));
        InitQueueList(QList);

        if (getopt_output(OOP_ABS))
            ForEachChild(dirBox, dirBox->name, 1, MakeQueueList);
        else
            ForEachChild(dirBox, "", 1, MakeQueueList);

        QueueList *rstlist = QList->rear;
        while (NULL != rstlist)
        {
            printf("%s\n", rstlist->mainstr);
            rstlist = rstlist->rear;
        }
    }

    return 0;
}

void debug_args()
{
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_REG));
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_DIR));
    DEBUG_VAR(1, "%d", getopt_filetype(OFT_SYM));
    DEBUG_VAR(1, "%d", getopt_reverse());
    DEBUG_VAR(1, "%d", getopt_depth());
    DEBUG_VAR(1, "%d", getopt_group());

    char **dir = get_dirpaths();
    while (0 != strcmp("\0", *dir))
    {
        DEBUG_VAR(1, "%s", *dir);
        dir++;
    }
}

void MakeQueueList(DirBox *dbox, char *dpath, int dep)
{
    if (0 < getopt_depth() && getopt_depth() < dep)
        return;

    if (getopt_filetype(OFT_DIR))
    {
        DirBox *child = dbox->child;
        while (NULL != child)
        {
            char *mstr = NULL;
            if (getopt_output(OOP_REL) || getopt_output(OOP_ABS))
            {
                mstr = (char *)malloc(strlen(dpath) + strlen(child->name) + 2);
                strcpy(mstr, dpath);
            }
            else
            {
                mstr = (char *)malloc(strlen(child->name) + 2);
                strcpy(mstr, "");
            }
            strcat(strcat(mstr, child->name), PATHSEPS);

            QueueNode *qn = (QueueNode *)malloc(sizeof(QueueNode));
            if (NULL == qn)
                return;

            CmpKey cmpkey;
            if (getopt_sortby(OSB_PATH) || getopt_sortby(OSB_FILE))
            {
                char fpath[FILENAME_MAX];
                strcat(strcpy(fpath, dpath), child->name);
                cmpkey.skey = getopt_sortby(OSB_PATH) ? fpath : child->name;
                CreatQueueNode(qn, mstr, CMPT_STR, cmpkey);
            }
            else if (getopt_sortby(OSB_SIZE) || getopt_sortby(OSB_DATE))
            {
                long ckey = getopt_sortby(OSB_SIZE) ? DirSizeOf(child) : DirDateOf(child);
                cmpkey.lkey = &ckey;
                CreatQueueNode(qn, mstr, CMPT_INT, cmpkey);
            }
            else
            {
                CreatQueueNode(qn, mstr, CMPT_NUL, cmpkey);
            }

            AddQueueNode(QList, qn);

            free(mstr);
            
            child = child->rear;
        }
    }

    FileBox *baby = dbox->baby;
    while (NULL != baby)
    {
        bool shown = FALSE;
        if (getopt_filetype(OFT_REG) && S_ISREG(baby->fstat->st_mode))
            shown = TRUE;
        else if (getopt_filetype(OFT_SYM) && S_ISLNK(baby->fstat->st_mode))
            shown = TRUE;

        if (shown)
        {
            char *mstr = NULL;
            if (getopt_output(OOP_REL) || getopt_output(OOP_ABS))
            {
                mstr = (char *)malloc(strlen(dpath) + strlen(baby->fname) + 2);
                strcpy(mstr, dpath);
            }
            else
            {
                mstr = (char *)malloc(strlen(baby->fname) + 2);
                strcpy(mstr, "");
            }
            strcat(mstr, baby->fname);

            QueueNode *qn = (QueueNode *)malloc(sizeof(QueueNode));
            if (NULL == qn)
                return;

            CmpKey cmpkey;
            if (getopt_sortby(OSB_PATH) || getopt_sortby(OSB_FILE))
            {
                char fpath[FILENAME_MAX];
                strcat(strcpy(fpath, dpath), baby->fname);
                cmpkey.skey = getopt_sortby(OSB_PATH) ? fpath : baby->fname;
                CreatQueueNode(qn, mstr, CMPT_STR, cmpkey);
            }
            else if (getopt_sortby(OSB_SIZE) || getopt_sortby(OSB_DATE))
            {
                long ckey = getopt_sortby(OSB_SIZE) ? FileSizeOf(baby) : FileDateOf(baby);
                cmpkey.lkey = &ckey;
                CreatQueueNode(qn, mstr, CMPT_INT, cmpkey);
            }
            else
            {
                CreatQueueNode(qn, mstr, CMPT_NUL, cmpkey);
            }

            AddQueueNode(QList, qn);

            free(mstr);
        }
        
        baby = baby->rear;
    }
}
