#include "outqueue.h"

static int queuenode_cmp(QueueNode *L, QueueNode *R);

Status InitQueueList(QueueList *Q)
{
    if (Q == NULL)
        return ERROR;

    Q->mainstr = NULL;
    Q->front = NULL;
    Q->rear = NULL;
    Q->cmptype = CMPT_NUL;
    Q->cmpkey.lkey = NULL;

    return OK;
}

Status CreatQueueNode(QueueNode *Q, char *mstr, CmpType ct, CmpKey ck)
{
    if (OK != InitQueueList(Q))
        return ERROR;

    Q->mainstr = (char *)malloc(strlen(mstr) + 1);
    if (NULL == Q->mainstr)
        return OVERFLOW;

    strcpy(Q->mainstr, mstr);

    Q->cmptype = ct;

    if (CMPT_INT == Q->cmptype)
    {
        Q->cmpkey.lkey = (long *)malloc(sizeof(long));
        *(Q->cmpkey.lkey) = *(ck.lkey);
    }
    else if (CMPT_STR == Q->cmptype)
    {
        Q->cmpkey.skey = (char *)malloc(strlen(ck.skey) + 1);
        strcpy(Q->cmpkey.skey, ck.skey);
    }
}

Status AddQueueNode(QueueList *Q, QueueNode *QN)
{
    if (NULL == Q || NULL == QN)
        return ERROR;

    while (NULL != Q->rear && 0 <= queuenode_cmp(QN, Q->rear))
        Q = Q->rear;
    QueueList *qr = Q->rear;
    QN->front = Q;
    QN->rear = qr;
    if (NULL != QN->rear)
        QN->rear->front = QN;

    Q->rear = QN;
}

static int queuenode_cmp(QueueNode *L, QueueNode *R)
{
    if (L->cmptype == CMPT_INT)
        return *(L->cmpkey.lkey) == *(R->cmpkey.lkey) ? 0
             : *(L->cmpkey.lkey) > *(R->cmpkey.lkey)  ? 1 : -1;
    else if (L->cmptype == CMPT_STR)
        return strcmp(L->cmpkey.skey, R->cmpkey.skey);
    else
        return 1;
}
