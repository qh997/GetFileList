#ifndef _OUTQUEUE_H
#define _OUTQUEUE_H

#include "common.h"

typedef enum compareType
{
    CMPT_NUL = 1,
    CMPT_INT,
    CMPT_STR,
} CmpType;

typedef union compareKey
{
    long *lkey;
    char *skey;
} CmpKey;

typedef struct queueNode
{
    char *mainstr;
    CmpType cmptype;
    CmpKey cmpkey;

    struct queueNode *front;
    struct queueNode *rear;
} QueueNode, QueueList;

Status InitQueueList(QueueList *Q);
Status CreatQueueNode(QueueNode *Q, char *mstr, CmpType ct, CmpKey ck);
Status AddQueueNode(QueueList *Q, QueueNode *QN);

#endif //_OUTQUEUE_H
