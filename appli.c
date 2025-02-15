#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include "cykernel.h"

ID taskid_1, taskid_2;

void task_1(void)
{
    while(1) {
        printf("task_1\n");
        cy_delay_task(500);
        cy_wakeup_task(taskid_2);
        cy_sleep_task(TIMEOUT_FOREVER);
    }
}

void task_2(void)
{
    while(1) {
        printf("task_2\n");
        cy_delay_task(500);
        cy_wakeup_task(taskid_1);
        cy_sleep_task(TIMEOUT_FOREVER);
    }
}

Type_Create_Task createtask_1 = {
    .task = task_1,
    .task_priority = 1
};
Type_Create_Task createtask_2 = {
    .task = task_2,
    .task_priority = 2
};


void usermain(void)
{
    taskid_1 = cy_create_task(&createtask_1);
    taskid_2 = cy_create_task(&createtask_2);
    
    while(1){
        printf("usermain\n");
		cy_sleep_task(TIMEOUT_FOREVER);
    }
}