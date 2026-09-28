typedef double StackElem_t;

//----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//----------------------------------------------------------------------------
struct stack_lifo
{
    StackElem_t* data;
    int size;
    size_t capacity;
};
//----------------------------------------------------------------------------
int Stack_Init(stack_lifo* stk, size_t capacity);
int Stack_Push(stack_lifo* stk, StackElem_t elem);
int Stack_Pop(stack_lifo* stk, StackElem_t* value);
int Stack_Error(stack_lifo* stk);
int Stack_Init_Error(stack_lifo* stk);
void Stack_Info(stack_lifo* stk);

//----------------------------------------------------------------------------
const size_t START_CAPACITY = 5;
const int INCREASE_DATA = 2;
const StackElem_t POISON = 0xEDA;

//----------------------------------------------------------------------------
int main()
{
    stack_lifo stk1 = {};
    int err = Stack_Init(&stk1, START_CAPACITY);
    if (err) printf("Init stack error\n");
    Stack_Info(&stk1);

    Stack_Push(&stk1, 10);
    Stack_Push(&stk1, 20);
    Stack_Push(&stk1, 30);

    double x = 0;
    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    printf("x1 = %lg\n", x);

    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    printf("x2 = %lg\n", x);

    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    printf("x3 = %lg\n", x);

    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    printf("x4 = %lg\n", x);
    
    Stack_Push(&stk1, 40);

    err = Stack_Pop(&stk1, &x);
    if (err) printf("stack Pop error\n");
    printf("x5 = %lg\n", x);

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Init(stack_lifo* stk, size_t capacity)
{
    if (Stack_Init_Error(stk) != 0)
    {
        printf("Stack_Init enter error\n");
        Stack_Info(stk);
    }

    stk -> data = (StackElem_t*)calloc(capacity, sizeof(StackElem_t));
    stk -> size = 0;
    stk -> capacity = capacity;

    for (size_t i = 0; i < (stk -> capacity); i++)
    {
        (stk -> data)[i] = POISON;
    }

    return 0;
}

//----------------------------------------------------------------------------
int Stack_Push(stack_lifo* stk, StackElem_t value)
{
    if (Stack_Error(stk) != 0)
    {
        printf("Stack_Push enter error\n");
        Stack_Info(stk);
    }

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
    if (Stack_Error(stk) != 0)
    {
        printf("Stack_Pop enter error\n");
        Stack_Info(stk);
    }

    if (stk -> size > 0)
    {
        *value = (stk -> data)[--(stk -> size)];
        (stk -> data)[(stk -> size)] = POISON;
    }
    else if (stk -> size == 0)
    {
        *value = (stk -> data)[0];
        printf("Error: try to pop elem, but stack is empty. Value = POISON value\n");
    }
    else
    {
        printf("Error: trying to Pop negative(<0) index\n");
        Stack_Info(stk);
    }

    if ((stk -> capacity) >= (stk -> size) * INCREASE_DATA * 2 && (stk -> capacity) > START_CAPACITY)
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
    if (stk -> size != 0)
    {
        if (stk -> data[stk -> size - 1] == POISON)
            return 5;
    }

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

//----------------------------------------------------------------------------
void Stack_Info(stack_lifo* stk)
{
    printf("<------------\n");
    printf("capacity = %zu\n", stk -> capacity);
    printf("size = %d\n", stk -> size);
    printf("data [%p]\n", stk -> data);

    printf("    {\n");
    for (size_t i = 0; i < (stk -> capacity); i++)
    {
        if (stk -> data[i] != POISON)
        {
            printf("     *[%zu] = %lg\n", i, stk -> data[i]);
        }
        else
        {
            printf("     [%zu] = %lg(POISON)\n", i, stk -> data[i]);
        }
    }
    printf("    }\n");
    printf("------------>\n");
}