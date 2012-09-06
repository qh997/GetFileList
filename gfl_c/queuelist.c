#include "queuelist.h"

static int _compare_queuenode(ST_QueueNode *left_node, ST_QueueNode *right_node);

Status init_queuelist(ST_QueueList *qlist)
{
    if (qlist == NULL)
        return ERROR;

    qlist->name = NULL;
    qlist->front = NULL;
    qlist->rear = NULL;
    qlist->cmptype = CMPT_NUL;
    qlist->cmpkey.lkey = NULL;

    return OK;
}

Status creat_queuenode(ST_QueueNode *qnode, char *node_name, EN_CmpType cmp_type, EN_CmpKey cmp_key)
{
    if (OK != init_queuelist(qnode))
        return ERROR;

    qnode->name = (char *)malloc(strlen(node_name) + 1);
    if (NULL == qnode->name)
        return OVERFLOW;

    strcpy(qnode->name, node_name);

    qnode->cmptype = cmp_type;

    if (CMPT_INT == qnode->cmptype)
    {
        qnode->cmpkey.lkey = (long *)malloc(sizeof(long));
        *(qnode->cmpkey.lkey) = *(cmp_key.lkey);
    }
    else if (CMPT_STR == qnode->cmptype)
    {
        qnode->cmpkey.skey = (char *)malloc(strlen(cmp_key.skey) + 1);
        strcpy(qnode->cmpkey.skey, cmp_key.skey);
    }

    return OK;
}

Status add_queuelist_node(ST_QueueList *qlist, ST_QueueNode *qnode)
{
    if (NULL == qlist || NULL == qnode)
        return ERROR;

    while (NULL != qlist->rear && 0 <= _compare_queuenode(qnode, qlist->rear))
        qlist = qlist->rear;

    ST_QueueList *qr = qlist->rear;
    qnode->front = qlist;
    qnode->rear = qr;
    if (NULL != qnode->rear)
        qnode->rear->front = qnode;

    qlist->rear = qnode;

    return OK;
}

static int _compare_queuenode(ST_QueueNode *left_node, ST_QueueNode *right_node)
{
    if (left_node->cmptype == CMPT_INT)
        return *(left_node->cmpkey.lkey) == *(right_node->cmpkey.lkey) ? 0
             : *(left_node->cmpkey.lkey) > *(right_node->cmpkey.lkey)  ? 1 : -1;
    else if (left_node->cmptype == CMPT_STR)
        return strcmp(left_node->cmpkey.skey, right_node->cmpkey.skey);
    else
        return 1;
}
