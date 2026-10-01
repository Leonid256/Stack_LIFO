#include "stack.hpp"

//----------------------------------------------------------------------------
int main()
{
    stack_lifo_t stk1 = {};
    stk_error_codes_t err = STK_NO_ERROR;
    err = stack_init(&stk1, START_CAPACITY);
    stack_dump(&stk1, STACK_DUMP_FILE, "w");

    stack_push(&stk1, 10);
    stack_push(&stk1, 20);
    stack_push(&stk1, 30);
    stack_push(&stk1, 40);
    stack_push(&stk1, 50);
    stack_push(&stk1, 60);
    stack_push(&stk1, 70);

    double x = 0;

    err = stack_pop(&stk1, &x);
    printf("x1 = %lg\n", x);

    err = stack_pop(&stk1, &x);
    printf("x2 = %lg\n", x);

    err = stack_pop(&stk1, &x);
    printf("x3 = %lg\n", x);

    err = stack_pop(&stk1, &x);
    printf("x4 = %lg\n", x);

    stack_push(&stk1, 80);

    err = stack_pop(&stk1, &x);
    printf("x5 = %lg\n", x);

    stack_dump(&stk1, STACK_DUMP_FILE, "a");
    stack_destroy(&stk1);

    return 0;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_init(stack_lifo_t* stk, size_t capacity)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_init_verify(stk)) != 0)
    {
        printf(RED "Stack_init enter error in %s:%d\n" CRESET, __FILE__, __LINE__);
        return err;
    }

    stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t));
    if (raw_data == NULL)
    {
        printf(RED "Memory allocation error in %s:%d\n" CRESET, __FILE__, __LINE__);
        return STK_MEMORY_ERROR;
    }
    raw_data[0] = CANARY_VALUE;
    raw_data[capacity + 1] = CANARY_VALUE;

    stk -> data = raw_data + 1;
    stk -> size = 0;
    stk -> capacity = capacity;

    for (size_t i = 0; i < capacity; i++)
    {
        (stk -> data)[i] = POISON_ELEM;
    }

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_push(stack_lifo_t* stk, stack_elem_t value)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != STK_NO_ERROR)
    {
        printf(RED "Stack_push enter error in %s:%d\n" CRESET, __FILE__, __LINE__);
        printf(YEL "err = %d\n" CRESET, err);

        return err;
    }

    if ((stk -> size) >= (stk -> capacity))
    {
        stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data - 1, ((stk -> capacity) * INCREASE_DATA + 2) * sizeof(stack_elem_t));
        if (temp == NULL)
        {
            printf(RED "Stack increase error in %s:%d\n" CRESET, __FILE__, __LINE__);
            return STK_INCREASE_ERR;
        }

        temp[stk -> capacity * INCREASE_DATA + 1] = CANARY_VALUE;
        stk -> data = temp + 1;

        stk -> data[stk -> capacity] = POISON_ELEM;
        (stk -> capacity) *= INCREASE_DATA;

        for (size_t i = stk -> capacity / INCREASE_DATA + 1; i < stk -> capacity; i++)
        {
            stk -> data[i] = POISON_ELEM;
        }
    }

    (stk -> data)[stk -> size++] = value;

    return stack_verify(stk);
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_pop(stack_lifo_t* stk, stack_elem_t* value)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != STK_NO_ERROR)
    {
        printf(RED "Stack_pop enter error in %s:%d\n" CRESET, __FILE__, __LINE__);
        printf(YEL "err = %d\n" CRESET, err);

        return err;
    }

    if (stk -> size > 0)
    {
        *value = (stk -> data)[--(stk -> size)];
        (stk -> data)[(stk -> size)] = POISON_ELEM;
    }
    else if (stk -> size == 0)
    {
        *value = POISON_ELEM;
        printf(RED "Error in %s:%d: try to pop elem, but stack is empty. Value = POISON_ELEM value\n" CRESET, __FILE__, __LINE__);
        return STK_EMPTY_ERR;
    }
    else
    {
        printf(RED "Error in %s:%d: trying to Pop negative(<0) index\n" CRESET, __FILE__, __LINE__);
    }

    if ((stk -> capacity) >= (stk -> size) * INCREASE_DATA * 2 && (stk -> capacity) > START_CAPACITY)
    {
        stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data - 1, ((stk -> capacity) / INCREASE_DATA + 2) * sizeof(stack_elem_t));
        if (temp == NULL)
        {
            printf(RED "Stack decrease error in %s:%d\n" CRESET, __FILE__, __LINE__);
            return STK_DECREASE_ERR;
        }

        (stk -> capacity) /= INCREASE_DATA;
        temp[stk -> capacity + 1] = CANARY_VALUE;
        stk -> data = temp + 1;
        
    }

    return stack_verify(stk);
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_verify(stack_lifo_t* stk)
{
    if (stk == NULL)                     return STK_PTR_NULL;
    if (stk -> data == NULL)             return STK_DATA_PTR_NULL;
    if ((ssize_t)stk -> size < 0)        return STK_ELEM_ERROR;
    if (stk -> capacity <= 0)            return STK_ELEM_ERROR;
    if (stk -> size > stk -> capacity)  return STK_ELEM_ERROR;
    if (stk -> data[-1] != CANARY_VALUE || stk -> data[stk -> capacity] != CANARY_VALUE) return STK_CANARY_ERR;

    for (size_t i = 0; i < stk -> size; i++)
    {
        if (stk -> data[i] == POISON_ELEM)
            return STK_DATA_POISON_VALUE;
    }

    for (size_t i = stk -> size; i < stk -> capacity; i++)
    {
        if (stk -> data[i] != POISON_ELEM)
            return STK_DATA_POISON_VALUE;
    }

    return STK_NO_ERROR;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_init_verify(stack_lifo_t* stk)
{
    stk_error_codes_t err = STK_NO_ERROR;

    if (stk == NULL)          err = STK_PTR_NULL;
    if (stk -> data != 0)     err = STK_INIT_ERR;
    if (stk -> size != 0)     err = STK_INIT_ERR;
    if (stk -> capacity != 0) err = STK_INIT_ERR;

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_dump(stack_lifo_t* stk, const char* name, const char* mode)
{
    stk_error_codes_t err = STK_NO_ERROR;

    FILE* file = fopen(name, mode);
    if (file == NULL)
    {
        printf(RED "Stack dump error in %s:%d\n" CRESET, __FILE__, __LINE__);
        err = STK_DUMP_ERR;
        return err;
    }

    fprintf(file, "<------------\n");
    fprintf(file, "capacity = %zu\n", stk -> capacity);
    fprintf(file, "size = %zu\n", stk -> size);
    fprintf(file, "data [%p]\n", stk -> data);

    fprintf(file, "    {\n");
    for (size_t i = 0; i < (stk -> capacity); i++)
    {
        if (stk -> data[i] == POISON_ELEM)
        {
            fprintf(file, "     [%zu] = %lg(POISON_ELEM)\n", i, stk -> data[i]);
        }
        else if (stk -> data[i] == CANARY_VALUE)
        {
            fprintf(file, "     [%zu] = %lg(CANARY_VALUE)\n", i, stk -> data[i]);
        }
        else
        {
            fprintf(file, "     *[%zu] = %lg\n", i, stk -> data[i]);
        }
        
    }
    fprintf(file, "    }\n");
    fprintf(file, "------------>\n");
    fprintf(file, "\n");

    fclose(file);

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_destroy(stack_lifo_t* stk)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != 0)
    {
        printf(RED "Stack_destroy enter error in %s:%d\n" CRESET, __FILE__, __LINE__);

        return err;
    }

    free(stk -> data - 1);
    *(stk -> data) = POISON_POINTER;

    return err;
}
