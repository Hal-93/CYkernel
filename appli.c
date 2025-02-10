#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include "cykernel.h"

void task_1(void)
{
    while(1) {
        printf("task_1\n");
        cy_delay_task(500);
        scheduler();
    }
}

void task_2(void)
{
    while(1) {
        printf("task_2\n");
        cy_delay_task(500);
        scheduler();
    }
}

void task_3(void)
{
    while(1) {
        printf("task_3\n");
        cy_delay_task(500);
        scheduler();
    }
}

void task_4(void)
{
    while(1) {
        printf("task_4\n");
        cy_delay_task(500);
        scheduler();
    }
}

Type_Create_Task createtask_1 = {
    .task = task_1,
    .task_priority = 3
};
Type_Create_Task createtask_2 = {
    .task = task_2,
    .task_priority = 2
};
Type_Create_Task createtask_3 = {
    .task = task_3,
    .task_priority = 3
};
Type_Create_Task createtask_4 = {
    .task = task_4,
    .task_priority = 4
};

void usermain(void)
{
    cy_create_task(&createtask_1);
    cy_create_task(&createtask_2);
    cy_create_task(&createtask_3);
    cy_create_task(&createtask_4);
    
    while(1){
        printf("usermain\n");
        cy_delay_task(500);
        scheduler();
    }
}