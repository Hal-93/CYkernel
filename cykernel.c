#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include "cykernel.h"

typedef void (*FP)(void);

TCB tcb_table[MAX_TASK];
TCB *ready_queue;

jmp_buf context_table[MAX_TASK];

void dispatch(jmp_buf from, jmp_buf to){
    if(setjmp(from) == 0)
    {
        longjmp(to, 1);
    }
}

void make_stack(int n);

void init_context(int n)
{
    if(n < MAX_TASK) {
        if(setjmp(tcb_table[n].context) == 0) {
            make_stack(n);
        }
        else {
            tcb_table[n].task();
        }
    }
}

void make_stack(int n)
{
    char stack[TASK_STACK_SIZE];
    init_context(++n);
}


TCB *current_task;

void scheduler(void)
{
    TCB *next_tcb;

    if(current_task != NULL) {
        next_tcb = current_task->next;
        if(next_tcb == NULL)
        {
            next_tcb = ready_queue;
        }
    }
    else {
        while(ready_queue == NULL);
        next_tcb = ready_queue;
    }

    if(current_task == NULL) {
        current_task = next_tcb;
        longjmp(next_tcb->context, 1);
    }
    else if(next_tcb != current_task) {
        TCB *temp = current_task;
        current_task = next_tcb;
        dispatch(temp->context, current_task->context);
    }
} 


void ini_task(void)
{
    usermain();
}

Type_Create_Task initial_create_task = {
    .task = ini_task
};

int main()
{
    make_stack(-1);
    cy_create_task(&initial_create_task);
    scheduler();
    return 0;
}


/*
    API
*/

ID cy_create_task(Type_Create_Task *pk_create_task)
{
    uint32_t i;

    for(i = 0; i < MAX_TASK; i++) {
        if(tcb_table[i].status == TASK_STATUS_NONE)
        {
            tcb_table[i].taskid = (ID)(i+1);
            tcb_table[i].status = TASK_STATUS_READY;
            tcb_table[i].task = pk_create_task->task;
            break;
        }
    }
    if(i < MAX_TASK) {
        tqueue_add_entry(&ready_queue, &tcb_table[i]);
        return (ID)(i+1);
    }
    else {
        return (ID)E_LIMIT_ERROR;
    }
    return (i < MAX_TASK)?(ID)(i+1):(ID)E_LIMIT_ERROR;
}
