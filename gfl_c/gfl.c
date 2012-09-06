#include "gfl.h"

static ST_QueueList *_s_result_list = NULL;

int main(int argc, char **argv)
{
    if (ERROR == parse_args(argc, argv))
        return 1;
    debug_args();

    char **dir = get_dirpaths();

    bool frist_g = true;
    while (0 != strcmp("\0", *dir))
    {
        if (getopt_group())
        {
            if (!frist_g)
                printf("\n");
            printf("%s\n", *dir);
            frist_g = false;
        }

        ST_DirBox *dirBox = (ST_DirBox *)malloc(sizeof(ST_DirBox));
        init_dirbox(dirBox, *dir, NULL);
        fill_dirbox(dirBox, *dir++);

        _s_result_list = (ST_QueueList *)malloc(sizeof(ST_QueueList));
        init_queuelist(_s_result_list);

        if (getopt_output(OOP_ABS))
            for_each_child(dirBox, dirBox->name, 1, fill_result_list);
        else
            for_each_child(dirBox, "", 1, fill_result_list);

        if (getopt_reverse())
        {
            ST_QueueList *rstlist = _s_result_list->rear;
            while (NULL != rstlist->rear)
                rstlist = rstlist->rear;

            while (NULL != rstlist->front->front)
            {
                printf("%s\n", rstlist->name);
                rstlist = rstlist->front;
            }
        }
        else
        {
            ST_QueueList *rstlist = _s_result_list->rear;
            while (NULL != rstlist)
            {
                printf("%s\n", rstlist->name);
                rstlist = rstlist->rear;
            }
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

void fill_result_list(ST_DirBox *dbox, char *dpath, int dep)
{
    if (0 < getopt_depth() && getopt_depth() < dep)
        return;

    if (getopt_filetype(OFT_DIR))
    {
        ST_DirBox *child = dbox->child;
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

            ST_QueueNode *qn = (ST_QueueNode *)malloc(sizeof(ST_QueueNode));
            if (NULL == qn)
                return;

            EN_CmpKey cmpkey;
            if (getopt_sortby(OSB_PATH) || getopt_sortby(OSB_FILE))
            {
                char fpath[FILENAME_MAX];
                strcat(strcpy(fpath, dpath), child->name);
                cmpkey.skey = getopt_sortby(OSB_PATH) ? fpath : child->name;
                creat_queuenode(qn, mstr, CMPT_STR, cmpkey);
            }
            else if (getopt_sortby(OSB_SIZE) || getopt_sortby(OSB_DATE))
            {
                long ckey = getopt_sortby(OSB_SIZE) ? get_dirbox_size(child) : get_dirbox_date(child);
                cmpkey.lkey = &ckey;
                creat_queuenode(qn, mstr, CMPT_INT, cmpkey);
            }
            else
            {
                creat_queuenode(qn, mstr, CMPT_NUL, cmpkey);
            }

            add_queuelist_node(_s_result_list, qn);

            free(mstr);
            
            child = child->rear;
        }
    }

    ST_FileBox *baby = dbox->baby;
    while (NULL != baby)
    {
        if ((getopt_filetype(OFT_REG) && S_ISREG(get_filebox_mode(baby)))
             || (getopt_filetype(OFT_SYM) && S_ISLNK(get_filebox_mode(baby))))
        {
            char *mstr = NULL;
            if (getopt_output(OOP_REL) || getopt_output(OOP_ABS))
            {
                mstr = (char *)malloc(strlen(dpath) + strlen(baby->name) + 2);
                strcpy(mstr, dpath);
            }
            else
            {
                mstr = (char *)malloc(strlen(baby->name) + 2);
                strcpy(mstr, "");
            }
            strcat(mstr, baby->name);

            ST_QueueNode *qn = (ST_QueueNode *)malloc(sizeof(ST_QueueNode));
            if (NULL == qn)
                return;

            EN_CmpKey cmpkey;
            if (getopt_sortby(OSB_PATH) || getopt_sortby(OSB_FILE))
            {
                char fpath[FILENAME_MAX];
                strcat(strcpy(fpath, dpath), baby->name);
                cmpkey.skey = getopt_sortby(OSB_PATH) ? fpath : baby->name;
                creat_queuenode(qn, mstr, CMPT_STR, cmpkey);
            }
            else if (getopt_sortby(OSB_SIZE) || getopt_sortby(OSB_DATE))
            {
                long ckey = getopt_sortby(OSB_SIZE) ? get_filebox_size(baby) : get_filebox_date(baby);
                cmpkey.lkey = &ckey;
                creat_queuenode(qn, mstr, CMPT_INT, cmpkey);
            }
            else
            {
                creat_queuenode(qn, mstr, CMPT_NUL, cmpkey);
            }

            add_queuelist_node(_s_result_list, qn);

            free(mstr);
        }
        
        baby = baby->rear;
    }
}
