#ifndef __OUTQUEUE_H__
#define __OUTQUEUE_H__

#include "common.h"

typedef enum
{
    CMPT_NUL = 1,
    CMPT_INT,
    CMPT_STR,
} EN_CmpType;

typedef union
{
    long *lkey;
    char *skey;
} EN_CmpKey;

typedef struct st_queuenode
{
    char *name;
    EN_CmpType cmptype;
    EN_CmpKey cmpkey;

    struct st_queuenode *front;
    struct st_queuenode *rear;
} ST_QueueNode, ST_QueueList;

Status init_queuelist(ST_QueueList *qnode);
Status creat_queuenode(ST_QueueNode *qnode, char *node_name, EN_CmpType cmp_type, EN_CmpKey cmp_key);
Status add_queuelist_node(ST_QueueList *qlist, ST_QueueNode *qnode);

#endif //__OUTQUEUE_H__
