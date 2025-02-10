#include <stdio.h>
#include "cykernel.h"

void queue_add_entry(TCB **queue, TCB *tcb)
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

void queue_remove_top(TCB **queue)
{
    TCB	*top;

    if(*queue == NULL) return;

    top = *queue;
    *queue = top->next;
    if(*queue != NULL) {
        (*queue)->pre = top->pre;
    }
}

void queue_remove_entry(TCB **queue, TCB *tcb)
{
    if(*queue == tcb) {    
        queue_remove_top(queue);
    } else {
        (tcb->pre)->next = tcb->next;
        if(tcb->next != NULL) {
            (tcb->next)->pre = tcb->pre;
        } else {
            (*queue)->pre = tcb->pre;
        }
    }
}
