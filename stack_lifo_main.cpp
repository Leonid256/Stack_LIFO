#include "stack.hpp"

//----------------------------------------------------------------------------
const char* STACK_DUMP_FILE = "stack_dump.log";
unsigned long long error = 0;
FILE* file = NULL;

//----------------------------------------------------------------------------
int main()
{
    if (file != NULL)
    {
        printf(RED "ERROR: Attempt to open opened file\n" CRESET);
        return STK_DUMP_ERR;
    }
    else
    {
        FILE_INIT(STACK_DUMP_FILE,  "w");
        if (file == NULL)
        {
            printf(RED "ERROR: the file is not opened\n" CRESET);
            return STK_DUMP_ERR;
        }
    }

    stack_status stk_status = SUCCESS;
    stack_lifo_t stk1 = {};
    stk_status = STACK_INIT(&stk1, START_CAPACITY);
    stk_status = STACK_DUMP(&stk1);
    print_stack_status(stk_status);

    STACK_PUSH(&stk1, 10);
    STACK_PUSH(&stk1, 20);
    STACK_PUSH(&stk1, 30);
    STACK_PUSH(&stk1, 40);
    STACK_PUSH(&stk1, 50);
    STACK_PUSH(&stk1, 60);
    STACK_PUSH(&stk1, 70);

    double x = 0;

    STACK_POP(&stk1, &x);
    printf("x1 = %lg\n", x);

    STACK_POP(&stk1, &x);
    printf("x2 = %lg\n", x);

    STACK_POP(&stk1, &x);
    printf("x3 = %lg\n", x);

    STACK_POP(&stk1, &x);
    printf("x4 = %lg\n", x);

    STACK_PUSH(&stk1, 80);

    STACK_POP(&stk1, &x);
    printf("x5 = %lg\n", x);


    memset(&stk1, -1, sizeof(stack_lifo_t));    //TODO: error here
    stack_push(&stk1, 8, __LINE__);             //TODO: error here

    stk_status = STACK_DUMP(&stk1);
    stk_status = STACK_DESTROY(&stk1);

    print_stack_status(stk_status);

    return stk_status;
}
