#include "stack.hpp"

unsigned long long error = 0;
FILE* file = NULL;

//TODO: many errors possibility
//TODO: on debug mode
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
        file_init_default(STACK_DUMP_FILE,  "w");
        if (file == NULL)
        {
            printf(RED "ERROR: the file is not opened\n" CRESET);
            return STK_DUMP_ERR;
        }
    }

    stack_status stk_status = SUCCESS;
    stack_lifo_t stk1 = {};
    stk_error_codes_t err = STK_NO_ERROR;
    err = stack_init_default(&stk1, START_CAPACITY);
    err = stack_dump_default(&stk1);

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

    err = stack_dump_default(&stk1);
    err = stack_destroy_default(&stk1);

    return err;
}

//----------------------------------------------------------------------------
stack_status stack_init(stack_lifo_t* stk, size_t capacity, int line)
{
    stack_status err = SUCCESS;
    if ((err = stack_init_verify(stk)) != SUCCESS)
    {
        fprintf(file, "Stack_init enter error in (%s)%s:%d\n", __func__, __FILE__, line);
        return err;
    }

    #ifdef CANARY_PROTECT_ON
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t));
    #else
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity, sizeof(stack_elem_t));
    #endif

    if (raw_data == NULL)
    {
        fprintf(file, "Memory allocation error in (%s)%s:%d\n", __func__, __FILE__, line);
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
        fprintf(file, "Stack_push enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
        fprintf(file, "err = %d\n", err);
        if (err == STK_DATA_PTR_NULL)
            fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");

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
            fprintf(file, "Stack increase error in (%s)%s:%d\n", __func__, __FILE__, line);
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
        fprintf(file, "Stack_pop enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
        fprintf(file, "err = %d\n", err);
        if (err == STK_DATA_PTR_NULL)
            fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");

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
        fprintf(file, "Error in (%s)%s:%d: try to pop elem, but stack is empty. Value = POISON_ELEM value\n", __func__, __FILE__, line);
        return STK_EMPTY_ERR;
    }
    else
    {
        fprintf(file, "Error in (%s)%s:%d: trying to Pop negative(<0) index\n", __func__, __FILE__, line);
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
            fprintf(file, "Stack decrease error in (%s)%s:%d\n", __func__, __FILE__, line);
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
stack_status stack_verify(stack_lifo_t* stk)
{
    if (stk == NULL)
    {
        error |= STK_PTR_NULL;
        return PIZDETS;
    }

    #ifdef CANARY_PROTECT_ON
        if (stk -> data == NULL)         
        {
            error |= STK_DATA_PTR_NULL;
            return PIZDETS;
        }
    #endif
    if (stk -> real_data == NULL)        
    {
        error |= STK_DATA_PTR_NULL;
        return PIZDETS;
    }
    if ((ssize_t)stk -> size < 0)        
    {
        error |= STK_ELEM_ERROR;
        return PIZDETS;
    }
    if (stk -> capacity <= 0)            
    {
        error |= STK_ELEM_ERROR;
        return PIZDETS;
    }
    if (stk -> size > stk -> capacity)   
    {
        error |= STK_ELEM_ERROR;
        return PIZDETS;
    }
    #ifdef CANARY_PROTECT_ON
        if (!is_equal(stk -> l_canary, STRUCT_CANARY_VALUE) || !is_equal(stk -> r_canary, STRUCT_CANARY_VALUE)) 
        {
            error |= STK_STRUCT_CANARY_ERR;
            return PIZDETS;
        }
        if (!is_equal(stk -> real_data[-1], CANARY_VALUE) || !is_equal(stk -> real_data[stk -> capacity], CANARY_VALUE)) 
        {
            error |= STK_CANARY_ERR;
            return PIZDETS;
        }
    #endif

    for (size_t i = 0; i < stk -> size; i++)
    {
        if (is_equal(stk -> real_data[i], POISON_ELEM))
        {
            error |= STK_DATA_POISON_VALUE;
            return PIZDETS;
        }
    }

    for (size_t i = stk -> size; i < stk -> capacity; i++)
    {
        if (!is_equal(stk -> real_data[i], POISON_ELEM))
        {
            error |= STK_DATA_POISON_VALUE;
            return PIZDETS;
        }
    }

    return SUCCESS;
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
stk_error_codes_t stack_dump(stack_lifo_t* stk, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;

    if ((err = stack_verify(stk)) != 0)
    {
        fprintf(file, "Stack_dump enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
        fprintf(file, "err = %d\n", err);
        if (err == STK_DATA_PTR_NULL)
            fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");
        
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

    return err;
}

//----------------------------------------------------------------------------
stk_error_codes_t stack_destroy(stack_lifo_t* stk, int line)
{
    stk_error_codes_t err = STK_NO_ERROR;
    if ((err = stack_verify(stk)) != 0)
    {
        fprintf(file, "Stack_destroy enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
        fprintf(file, "err = %d\n", err);
        if (err == STK_DATA_PTR_NULL)
            fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");
        
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

//----------------------------------------------------------------------------
stk_error_codes_t file_init(const char* name, const char* mode, int line)
{
    file = fopen(name, mode);

    if (file == NULL)
    {
        printf(RED "Stack dump error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
        return STK_DUMP_ERR;
    }

    return STK_NO_ERROR;
}
