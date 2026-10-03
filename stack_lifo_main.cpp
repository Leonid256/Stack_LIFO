#include "stack.hpp"

//TODO: many errors possibility
//TODO: on debug mode
//TODO: errors to file
//----------------------------------------------------------------------------
int main()
{
    stack_lifo_t stk1 = {};
    stk_error_codes_t err = STK_NO_ERROR;
    err = stack_init_default(&stk1, START_CAPACITY);
    err = stack_dump_default(&stk1, STACK_DUMP_FILE, "w");

    stack_push_default(&stk1, 10);
    stack_push_default(&stk1, 20);
    stack_push_default(&stk1, 30);
    stack_push_default(&stk1, 40);
    stack_push_default(&stk1, 50);
    stack_push_default(&stk1, 60);
    stack_push_default(&stk1, 70);

    double x = 0;

    stack_pop_default(&stk1, &x);
    printf("x1 = %lg\n", x);

    stack_pop_default(&stk1, &x);
    printf("x2 = %lg\n", x);

    stack_pop_default(&stk1, &x);
    printf("x3 = %lg\n", x);

    stack_pop_default(&stk1, &x);
    printf("x4 = %lg\n", x);

    stack_push_default(&stk1, 80);

    stack_pop_default(&stk1, &x);
    printf("x5 = %lg\n", x);

    err = stack_dump_default(&stk1, STACK_DUMP_FILE, "a");
    err = stack_destroy_default(&stk1);

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_init(stack_lifo_t* stk, size_t capacity, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_init_verify(stk)) != 0)
    {
        printf(RED "Stack_init enter error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
        return err;
    }

    #ifdef CANARY_PROTECT_ON
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t));
    #else
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity, sizeof(stack_elem_t));
    #endif

    if (raw_data == NULL)
    {
        printf(RED "Memory allocation error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
        return STK_MEMORY_ERROR;
    }
    else
    {
        #ifdef CANARY_PROTECT_ON
            stk -> data = POISON_POINTER;
        #endif
        stk -> real_data = POISON_POINTER;
    }
    
    #ifdef CANARY_PROTECT_ON
        raw_data[0] = CANARY_VALUE;
        raw_data[capacity + 1] = CANARY_VALUE;
        stk -> l_canary = STRUCT_CANARY_VALUE;
        stk -> data = raw_data;
        stk -> real_data = raw_data + 1;
    #else
        stk -> real_data = raw_data;
    #endif

    stk -> size = 0;
    stk -> capacity = capacity;
    #ifdef CANARY_PROTECT_ON
        stk -> r_canary = STRUCT_CANARY_VALUE;
    #endif

    for (size_t i = 0; i < capacity; i++)
    {
        (stk -> real_data)[i] = POISON_ELEM;
    }

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_push(stack_lifo_t* stk, stack_elem_t value, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != STK_NO_ERROR)
    {
        printf(RED "Stack_push enter error in (%s)%s:%d ; " CRESET, __func__, __FILE__, line);
        printf(YEL "err = %d\n" CRESET, err);
        if (err == STK_DATA_PTR_NULL)
            printf(BLU "\tPossibly forgotten to assign a new pointer after realloc\n" CRESET);

        return err;
    }

    if ((stk -> size) >= (stk -> capacity))
    {
        #ifdef CANARY_PROTECT_ON
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data, ((stk -> capacity) * INCREASE_DATA + 2) * sizeof(stack_elem_t));
        #else
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> real_data, ((stk -> capacity) * INCREASE_DATA) * sizeof(stack_elem_t));
        #endif

        if (temp == NULL)
        {
            printf(RED "Stack increase error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
            return STK_INCREASE_ERR;
        }
        else
        {
            #ifdef CANARY_PROTECT_ON
                stk -> data = POISON_POINTER;
            #endif
            stk -> real_data = POISON_POINTER;
        }

        #ifdef CANARY_PROTECT_ON
            temp[stk -> capacity * INCREASE_DATA + 1] = CANARY_VALUE;
            stk -> real_data = temp + 1;
            stk -> data = temp;
        #else
            stk -> real_data = temp;
        #endif

        stk -> real_data[stk -> capacity] = POISON_ELEM;
        (stk -> capacity) *= INCREASE_DATA;

        for (size_t i = stk -> capacity / INCREASE_DATA + 1; i < stk -> capacity; i++)
        {
            stk -> real_data[i] = POISON_ELEM;
        }
    }

    (stk -> real_data)[stk -> size++] = value;

    return stack_verify(stk);
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_pop(stack_lifo_t* stk, stack_elem_t* value, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != STK_NO_ERROR)
    {
        printf(RED "Stack_pop enter error in (%s)%s:%d ; " CRESET, __func__, __FILE__, line);
        printf(YEL "err = %d\n" CRESET, err);
        if (err == STK_DATA_PTR_NULL)
            printf(BLU "\tPossibly forgotten to assign a new pointer after realloc\n" CRESET);

        return err;
    }

    if (stk -> size > 0)
    {
        *value = (stk -> real_data)[--(stk -> size)];
        (stk -> real_data)[(stk -> size)] = POISON_ELEM;
    }
    else if (stk -> size == 0)
    {
        *value = POISON_ELEM;
        printf(RED "Error in (%s)%s:%d: try to pop elem, but stack is empty. Value = POISON_ELEM value\n" CRESET, __func__, __FILE__, line);
        return STK_EMPTY_ERR;
    }
    else
    {
        printf(RED "Error in (%s)%s:%d: trying to Pop negative(<0) index\n" CRESET, __func__, __FILE__, line);
    }

    if ((stk -> capacity) >= (stk -> size) * INCREASE_DATA * 2 && (stk -> capacity) > START_CAPACITY)
    {
        #ifdef CANARY_PROTECT_ON
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data, ((stk -> capacity) / INCREASE_DATA + 2) * sizeof(stack_elem_t));
        #else
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> real_data, ((stk -> capacity) / INCREASE_DATA) * sizeof(stack_elem_t));
        #endif

        if (temp == NULL)
        {
            printf(RED "Stack decrease error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
            return STK_DECREASE_ERR;
        }
        else
        {
            #ifdef CANARY_PROTECT_ON
                stk -> data = POISON_POINTER;
            #endif
            stk -> real_data = POISON_POINTER;
        }

        (stk -> capacity) /= INCREASE_DATA;
        temp[stk -> capacity + 1] = CANARY_VALUE;
        #ifdef CANARY_PROTECT_ON
            stk -> real_data = temp + 1;
            stk -> data = temp;
        #else
            stk -> real_data = temp;
        #endif
        
    }

    return stack_verify(stk);
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_verify(stack_lifo_t* stk)
{
    if (stk == NULL)                     return STK_PTR_NULL;

    #ifdef CANARY_PROTECT_ON
        if (stk -> data == NULL)         return STK_DATA_PTR_NULL;
    #endif
    if (stk -> real_data == NULL)        return STK_DATA_PTR_NULL;
    if ((ssize_t)stk -> size < 0)        return STK_ELEM_ERROR;
    if (stk -> capacity <= 0)            return STK_ELEM_ERROR;
    if (stk -> size > stk -> capacity)   return STK_ELEM_ERROR;
    #ifdef CANARY_PROTECT_ON
        if (!is_equal(stk -> l_canary, STRUCT_CANARY_VALUE) || !is_equal(stk -> r_canary, STRUCT_CANARY_VALUE)) return STK_STRUCT_CANARY_ERR;
        if (!is_equal(stk -> real_data[-1], CANARY_VALUE) || !is_equal(stk -> real_data[stk -> capacity], CANARY_VALUE)) return STK_CANARY_ERR;
    #endif

    for (size_t i = 0; i < stk -> size; i++)
    {
        if (is_equal(stk -> real_data[i], POISON_ELEM))
            return STK_DATA_POISON_VALUE;
    }

    for (size_t i = stk -> size; i < stk -> capacity; i++)
    {
        if (!is_equal(stk -> real_data[i], POISON_ELEM))
            return STK_DATA_POISON_VALUE;
    }

    return STK_NO_ERROR;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_init_verify(stack_lifo_t* stk)
{
    stk_error_codes_t err = STK_NO_ERROR;

    if (stk == NULL)            err = STK_PTR_NULL;
    #ifdef CANARY_PROTECT_ON
        if (stk -> data != 0)   err = STK_INIT_ERR;
    #endif
    if (stk -> real_data != 0)  err = STK_INIT_ERR;
    if (stk -> size != 0)       err = STK_INIT_ERR;
    if (stk -> capacity != 0)   err = STK_INIT_ERR;
    #ifdef CANARY_PROTECT_ON
        if (!is_equal(stk -> l_canary, 0))   err = STK_INIT_ERR;
        if (!is_equal(stk -> r_canary, 0))   err = STK_INIT_ERR;
    #endif

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_dump(stack_lifo_t* stk, const char* name, const char* mode, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;

    if ((err = stack_verify(stk)) != 0)
    {
        printf(RED "Stack_dump enter error in (%s)%s:%d ; " CRESET, __func__, __FILE__, line);
        printf(YEL "err = %d\n" CRESET, err);
        if (err == STK_DATA_PTR_NULL)
            printf(BLU "\tPossibly forgotten to assign a new pointer after realloc\n" CRESET);
        
        return err;
    }

    FILE* file = fopen(name, mode);
    if (file == NULL)
    {
        printf(RED "Stack dump error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
        err = STK_DUMP_ERR;
        return err;
    }

    fprintf(file, "<------------\n");
    fprintf(file, "capacity = %zu\n", stk -> capacity);
    fprintf(file, "size = %zu\n", stk -> size);
    fprintf(file, "data [%p]\n", stk -> real_data);

    fprintf(file, "    {\n");
    for (size_t i = 0; i < (stk -> capacity); i++)
    {
        if (is_equal(stk -> real_data[i], POISON_ELEM))
        {
            fprintf(file, "     [%zu] = %lg(POISON_ELEM)\n", i, stk -> real_data[i]);
        }
        #ifdef CANARY_PROTECT_ON
            else if (is_equal(stk -> real_data[i], CANARY_VALUE))
            {
                fprintf(file, "     [%zu] = %lg(CANARY_VALUE)\n", i, stk -> real_data[i]);
            }
        #endif
        else
        {
            fprintf(file, "     *[%zu] = %lg\n", i, stk -> real_data[i]);
        }
        
    }
    fprintf(file, "    }\n");
    fprintf(file, "------------>\n");
    fprintf(file, "\n");

    fclose(file);

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_destroy(stack_lifo_t* stk, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != 0)
    {
        printf(RED "Stack_destroy enter error in (%s)%s:%d ; " CRESET, __func__, __FILE__, line);
        printf(YEL "err = %d\n" CRESET, err);
        if (err == STK_DATA_PTR_NULL)
            printf(BLU "\tPossibly forgotten to assign a new pointer after realloc\n" CRESET);
        
        return err;
    }

    #ifdef CANARY_PROTECT_ON
        free(stk -> data);
        stk -> data = POISON_POINTER;
    #else
        free(stk -> real_data);
    #endif

    stk -> real_data = POISON_POINTER;

    return err;
}

//----------------------------------------------------------------------------
bool is_equal(stack_elem_t a, stack_elem_t b)
{
    if (abs(a - b) < EPCILON)
        return true;
    else
        return false;
}
