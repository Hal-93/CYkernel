#include <stdio.h>
#include "cykernel.h"

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

ERROR cy_sleep_task(TIMEOUT timeout)
{
    ERROR return_code = E_OK;

    BEGIN_CRITICAL_SECTION

    if(current_task->wakeup_count > 0) {
        current_task->wakeup_count--;
    }
    else {
        queue_remove_entry(&ready_queue[current_task->task_priority - 1], current_task);
        current_task->status = TASK_STATUS_WAIT;
        current_task->waitfact = WAITFACT_SLEEP;
        current_task->waittime = (RELATIVE_TIME)timeout;
        queue_add_entry(&wait_queue, current_task);
        scheduler();
    }

    END_CRITICAL_SECTION
    return return_code;
}

ERROR cy_wakeup_task(ID taskid)
{
    ERROR return_code = E_OK;
    TCB *tcb;

    if(!(taskid > 0 && taskid < MAX_TASK)) {
        return E_INVALID_ID;
    }

    BEGIN_CRITICAL_SECTION

    tcb = &tcb_table[taskid - 1];
    if(tcb->status == TASK_STATUS_NONE || tcb->status == TASK_STATUS_DORMANT) {
        return_code = E_OBJECT_DOES_NOT_EXIST;
    } 
    else if (tcb->status == TASK_STATUS_WAIT && tcb->waitfact == WAITFACT_SLEEP) {
        queue_remove_entry(&wait_queue, tcb);
        tcb->status = TASK_STATUS_READY;
        tcb->waitfact = WAITFACT_NONE;
        tcb->waittime = 0;
        queue_add_entry(&ready_queue[tcb->task_priority - 1], tcb);
        scheduler();
    }
    else {
        tcb->wakeup_count++;
    }

    END_CRITICAL_SECTION
    return return_code;
}