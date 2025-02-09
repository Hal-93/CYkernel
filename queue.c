#include <stdio.h>
#include "cykernel.h"

void tqueue_add_entry(TCB **queue, TCB *tcb)
{
    TCB *queue_end;

    if(*queue == NULL)
    {
        *queue = tcb;
        tcb->pre = tcb;
    }
    else
    {
        queue_end = (*queue)->pre;
        queue_end->next = tcb;
        tcb->pre = queue_end;
        (*queue)->pre = tcb;
    }
    tcb->next = NULL;
}