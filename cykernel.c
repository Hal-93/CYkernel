#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>

#define MAX_TASK 2
#define TASK_STACK_SIZE 1024

void task_1(void);
void task_2(void);
typedef void (*FP)(void);
FP fp_table[] = {
    task_1,
    task_2,
};

jmp_buf context_table[MAX_TASK];

#define DISPATCH(from, to)((setjmp(from)==0)?longjmp(to, 1):0)

void task_1(void)
{
    while(1) {
        printf("task 1\n");
        sleep(1);
        DISPATCH(context_table[0], context_table[1]);
    }
}

void task_2(void)
{
    while(1) {
        printf("task 2\n");
        sleep(1);
        DISPATCH(context_table[1], context_table[0]);
    }
}

void make_stack(int n);

void init_context(int n)
{
    if(n < MAX_TASK) {
        if(setjmp(context_table[n]) == 0) {
            make_stack(n);
        }
        else {
            fp_table[n]();
        }
    }
}

void make_stack(int n)
{
    char stack[TASK_STACK_SIZE];
    init_context(++n);
}

int main()
{
    make_stack(-1);
    longjmp(context_table[0],1);
    return 0;
}