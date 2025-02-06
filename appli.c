#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include "cykernel.h"

void task_1(void)
{
    while(1) {
        printf("task_1\n");
        sleep(1);
        scheduler();
    }
}

void task_2(void)
{
    while(1) {
        printf("task_2\n");
        sleep(1);
        scheduler();
    }
}

void task_3(void)
{
    while(1) {
        printf("task_3\n");
        sleep(1);
        scheduler();
    }
}

void task_4(void)
{
    while(1) {
        printf("task_4\n");
        sleep(1);
        scheduler();
    }
}

Type_Create_Task createtask_1 = {
    .task = task_1
};
Type_Create_Task createtask_2 = {
    .task = task_2
};
Type_Create_Task createtask_3 = {
    .task = task_3
};
Type_Create_Task createtask_4 = {
    .task = task_4
};

void usermain(void)
{
    cy_create_task(&createtask_1);
    cy_create_task(&createtask_2);
    cy_create_task(&createtask_3);
    cy_create_task(&createtask_4);
    
    while(1){
        printf("usermain\n");
        sleep(1);
        scheduler();
    }
}