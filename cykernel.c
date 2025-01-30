#include <stdio.h>
#include <setjmp.h>

jmp_buf context;

void func_2(void)
{
    printf("start func_2\n");
    longjmp(context, 1);
    printf("end func2\n");
}

void func_1(void)
{
    printf("start func_1\n");
    func_2();
    printf("end func1\n");
}

int main()
{
    printf("start main\n");

    if(setjmp(context) == 0)
    {
        func_1();
    }

    printf("end main\n");

    return 0;
}