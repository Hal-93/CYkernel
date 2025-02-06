#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include "cykernel.h"

typedef void (*FP)(void);

TCB tcb_table[MAX_TASK];

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
    uint32_t next;

    if(current_task != NULL){
        next = current_task->taskid;
        if(next>=MAX_TASK) next = 0;
    } 
    else {
        next = 0;
    }
    while(tcb_table[next].status != TASK_STATUS_READY) {
        if(++next >= MAX_TASK) next = 0;
    }
    if(current_task == NULL){
        current_task = &tcb_table[next];
        longjmp(tcb_table[next].context, 1);
    }
    else if(next != current_task->taskid-1)
    {
        TCB *temp = current_task;
        current_task = &tcb_table[next];
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
        if(tcb_table[i].status == TASK_STATUS_NONE){
            tcb_table[i].taskid = (ID)(i+1);
            tcb_table[i].status = TASK_STATUS_READY;
            tcb_table[i].task = pk_create_task->task;
            break;
        }
    }
    return (i < MAX_TASK)?(ID)(i+1):(ID)E_LIMIT_ERROR;
}

