#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include "cykernel.h"

typedef void (*FP)(void);

TCB tcb_table[MAX_TASK];
TCB *ready_queue[MAX_PRIORITY];
TCB *wait_queue;

jmp_buf context_table[MAX_TASK];

void disable_signal(void)
{
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);
}

void enable_signal(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigprocmask(SIG_SETMASK, &set, NULL);
}


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
TCB *next_task;
int in_scheduler;

void scheduler(void)
{
    int i;

    next_task = NULL;
    for(i = 0; i < MAX_PRIORITY; i++) {
        if(ready_queue[i] != NULL) {
            next_task = ready_queue[i];
            break;
        }
    }

    if(!in_scheduler) {
        in_scheduler = 1;
        while(next_task == NULL){
            sigset_t set;
            sigemptyset(&set);
            sigsuspend(&set);
        }
        in_scheduler = 0;

        if(current_task == NULL) {
            current_task = next_task;
            longjmp(next_task->context, 1);
        }
        else if(next_task != current_task) {
            TCB *temp = current_task;
            current_task = next_task;
            dispatch(temp->context, current_task->context);
        }
    }
} 


void ini_task(void)
{
    usermain();
}

Type_Create_Task initial_create_task = {
    .task = ini_task,
    .task_priority = 5
};

const struct itimerval systim_run = {
    { 0, TIMER_PERIOD*1000 },
    { 0, TIMER_PERIOD*1000 }
};

void hdr_systim(int signalno)
{
    TCB *tcb, *next;

    BEGIN_CRITICAL_SECTION

    tcb = wait_queue;
    while(tcb != NULL) {
        next = (TCB*)(tcb->next);

        if(tcb->waittime > TIMER_PERIOD) {
            tcb->waittime -= TIMER_PERIOD;
        }
        else {
            queue_remove_entry(&wait_queue, tcb);
            current_task->status = TASK_STATUS_READY;
            current_task->waitfact = WAITFACT_NONE;
            current_task->waittime = 0;
            queue_add_entry(&ready_queue[tcb->task_priority - 1], tcb);
        }

        tcb = next;
    }
    scheduler();

    END_CRITICAL_SECTION
}

void start_systim(void)
{
    signal(SIGALRM, hdr_systim);
    setitimer(ITIMER_REAL, &systim_run, NULL);
}

int main()
{
    make_stack(-1);
    start_systim();
    cy_create_task(&initial_create_task);
    return 0;
}


/*
    API
*/

ID cy_create_task(Type_Create_Task *pk_create_task)
{
    uint32_t i;
    ID return_code;

    BEGIN_CRITICAL_SECTION

    for(i = 0; i < MAX_TASK; i++) {
        if(tcb_table[i].status == TASK_STATUS_NONE)
        {
            tcb_table[i].taskid = (ID)(i+1);
            tcb_table[i].status = TASK_STATUS_READY;
            tcb_table[i].task = pk_create_task->task;
            tcb_table[i].task_priority = pk_create_task->task_priority;
            break;
        }
    }
    if(i < MAX_TASK) {
        queue_add_entry(&ready_queue[pk_create_task->task_priority - 1], &tcb_table[i]);
        return_code = (ID)(i + 1);
    }
    else {
        return_code = (ID)E_LIMIT_ERROR;
    }
    scheduler();

    END_CRITICAL_SECTION
    return return_code;
}

ERROR cy_delay_task(RELATIVE_TIME delaytime)
{
    BEGIN_CRITICAL_SECTION

    queue_remove_entry(&ready_queue[current_task->task_priority - 1], current_task);
    current_task->status = TASK_STATUS_WAIT;
    current_task->waitfact = WAITFACT_DELAY;
    current_task->waittime = delaytime;
    queue_add_entry(&wait_queue, current_task);
    scheduler();

    END_CRITICAL_SECTION
    return E_OK;
}