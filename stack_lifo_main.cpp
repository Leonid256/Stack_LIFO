typedef double StackElem_t;

//----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//----------------------------------------------------------------------------
struct stack_lifo
{
    StackElem_t* data;
    size_t size;
    size_t capacity;
};
//----------------------------------------------------------------------------
int Stack_Init(stack_lifo* stk, size_t capacity);
int Stack_Push(stack_lifo* stk, StackElem_t elem);
int Stack_Pop(stack_lifo* stk, StackElem_t* value);
int Stack_Error(stack_lifo* stk);
int Stack_Init_Error(stack_lifo* stk);

//----------------------------------------------------------------------------
const size_t START_CAPACITY = 5;
const int INCREASE_DATA = 2;

//----------------------------------------------------------------------------
int main()
{
    stack_lifo stk1 = {};
    int err = Stack_Init(&stk1, START_CAPACITY);
    if (err) printf("Init stack error\n");

    Stack_Push(&stk1, 10);
    Stack_Push(&stk1, 20);
    Stack_Push(&stk1, 30);

    double x = 0;
    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    
    printf("x = %lg\n", x);

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Init(stack_lifo* stk, size_t capacity)
{
    assert(Stack_Init_Error(stk) == 0);

    stk -> data = (StackElem_t*)calloc(capacity, sizeof(StackElem_t));
    stk -> size = 0;
    stk -> capacity = capacity;

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Push(stack_lifo* stk, StackElem_t value)
{
    assert(Stack_Error(stk) == 0);

    (stk -> data)[stk -> size++] = value;

    if ((stk -> size) >= (stk -> capacity))
    {
        StackElem_t* temp = (StackElem_t*)realloc(stk -> data, (stk -> capacity) * sizeof(StackElem_t) * INCREASE_DATA);
        if (temp != NULL)
        {
            stk -> data = temp;
            (stk -> capacity) *= 2;
        }
        else
        {
            printf("Stack increase error\n");
            return 7;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Pop(stack_lifo* stk, StackElem_t* value)
{
    assert(Stack_Error(stk) == 0);

    *value = (stk -> data)[--(stk -> size)];

    if ((stk -> capacity) >= (stk -> size) * INCREASE_DATA * 2)
    {
        StackElem_t* temp = (StackElem_t*)realloc(stk -> data, (stk -> capacity) / INCREASE_DATA * sizeof(StackElem_t));
        if (temp != NULL)
        {
            stk -> data = temp;
            (stk -> capacity) /= INCREASE_DATA;
        }
        else
        {
            printf("Stack decrease error\n");
            return 6;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Error(stack_lifo* stk)
{
    int error = 0;

    if (stk == NULL)                     error = 1;
    if (stk -> data == NULL)             error = 2;
    if (stk -> capacity == 0)            error = 3;
    if (stk -> size >= stk -> capacity)  error = 4;

    return error;
}

//----------------------------------------------------------------------------
int Stack_Init_Error(stack_lifo* stk)
{
    int error = 0;

    if (stk == NULL)          error = 1;
    if (stk -> data != 0)     error = 8;
    if (stk -> size != 0)     error = 8;
    if (stk -> capacity != 0) error = 8;

    return error;
}