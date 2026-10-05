#include "stack.hpp"

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
        file_init_default(STACK_DUMP_FILE,  "w");
        if (file == NULL)
        {
            printf(RED "ERROR: the file is not opened\n" CRESET);
            return STK_DUMP_ERR;
        }
    }

    stack_status stk_status = SUCCESS;
    stack_lifo_t stk1 = {};
    stk_status = stack_init_default(&stk1, START_CAPACITY);
    stk_status = stack_dump_default(&stk1);
    print_stack_status(stk_status);

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

    stk_status = stack_dump_default(&stk1);
    stk_status = stack_destroy_default(&stk1);

    print_stack_status(stk_status);

    return stk_status;
}

//----------------------------------------------------------------------------
stack_status stack_init(stack_lifo_t* stk, size_t capacity, int line)
{
    stack_status err = SUCCESS;

    #ifdef STK_VERIFY_ON
        if ((err = stack_init_verify(stk)) != SUCCESS)
        {
            fprintf(file, "Stack_init enter error in (%s)%s:%d\n", __func__, __FILE__, line);
            return err;
        }
    #endif

    #ifdef CANARY_PROTECT_ON
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity + 2, sizeof(stack_elem_t));
    #else
        stack_elem_t* raw_data = (stack_elem_t*)calloc(capacity, sizeof(stack_elem_t));
    #endif

    if (raw_data == NULL)
    {
        fprintf(file, "Memory allocation error in (%s)%s:%d\n", __func__, __FILE__, line);
        error |= STK_MEMORY_ERROR;
        return PIZDETS;
    }
    else
    {
        #ifdef CANARY_PROTECT_ON
            stk -> buffer = POISON_POINTER;
        #endif
        stk -> data = POISON_POINTER;
    }
    
    #ifdef CANARY_PROTECT_ON
        raw_data[0] = CANARY_VALUE;
        raw_data[capacity + 1] = CANARY_VALUE;
        stk -> l_canary = STRUCT_CANARY_VALUE;
        stk -> buffer = raw_data;
        stk -> data = raw_data + 1;
    #else
        stk -> data = raw_data;
    #endif

    stk -> size = 0;
    stk -> capacity = capacity;
    #ifdef CANARY_PROTECT_ON
        stk -> r_canary = STRUCT_CANARY_VALUE;
    #endif

    for (size_t i = 0; i < capacity; i++)
    {
        (stk -> data)[i] = POISON_ELEM;
    }

    fprintf(file, "Stack init success\n");

    return err;
}

//----------------------------------------------------------------------------
stack_status stack_push(stack_lifo_t* stk, stack_elem_t value, int line)
{
    stack_status err = SUCCESS;

    #ifdef STK_VERIFY_ON
        if ((err = stack_verify(stk)) != SUCCESS)
        {
            fprintf(file, "Stack_push enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
            if ((error & STK_DATA_PTR_NULL) == STK_DATA_PTR_NULL)
                fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");

            return err;
        }
    #endif

    if ((stk -> size) >= (stk -> capacity))
    {
        #ifdef CANARY_PROTECT_ON
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> buffer, ((stk -> capacity) * INCREASE_DATA + 2) * sizeof(stack_elem_t));
        #else
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data, ((stk -> capacity) * INCREASE_DATA) * sizeof(stack_elem_t));
        #endif

        if (temp == NULL)
        {
            fprintf(file, "Stack increase error in (%s)%s:%d\n", __func__, __FILE__, line);
            error |= STK_INCREASE_ERR;
            return PIZDETS;
        }
        else
        {
            #ifdef CANARY_PROTECT_ON
                stk -> buffer = POISON_POINTER;
            #endif
            stk -> data = POISON_POINTER;
        }

        #ifdef CANARY_PROTECT_ON
            temp[stk -> capacity * INCREASE_DATA + 1] = CANARY_VALUE;
            stk -> data = temp + 1;
            //stk -> buffer = temp;     //TODO: TODO: TODO: TODO: TODO:mistake here
        #else
            stk -> data = temp;
        #endif

        stk -> data[stk -> capacity] = POISON_ELEM;
        (stk -> capacity) *= INCREASE_DATA;

        for (size_t i = stk -> capacity / INCREASE_DATA + 1; i < stk -> capacity; i++)
        {
            stk -> data[i] = POISON_ELEM;
        }
    }

    (stk -> data)[stk -> size++] = value;

    fprintf(file, "Stack push success. Value = " SPEC_TYPEDEF "\n", value);

    #ifdef STK_VERIFY_ON
        return stack_verify(stk);
    #else
        return err;
    #endif
}

//----------------------------------------------------------------------------
stack_status stack_pop(stack_lifo_t* stk, stack_elem_t* value, int line)
{
    stack_status err = SUCCESS;

    #ifdef STK_VERIFY_ON
        if ((err = stack_verify(stk)) != SUCCESS)
        {
            fprintf(file, "Stack_pop enter error in (%s)%s:%d ; ", __func__, __FILE__, line);
            if ((error & STK_DATA_PTR_NULL) == STK_DATA_PTR_NULL)
                fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");

            return err;
        }
    #endif

    if (stk -> size > 0)
    {
        *value = (stk -> data)[--(stk -> size)];
        (stk -> data)[(stk -> size)] = POISON_ELEM;
    }
    else if (stk -> size == 0)
    {
        *value = POISON_ELEM;
        fprintf(file, "Error in (%s)%s:%d: try to pop elem, but stack is empty. Value = POISON_ELEM value\n", __func__, __FILE__, line);
        error |= STK_EMPTY_ERR;
        return PIZDETS;
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
            stack_elem_t* temp = (stack_elem_t*)realloc(stk -> data, ((stk -> capacity) / INCREASE_DATA) * sizeof(stack_elem_t));
        #endif

        if (temp == NULL)
        {
            fprintf(file, "Stack decrease error in (%s)%s:%d\n", __func__, __FILE__, line);
            error |= STK_DECREASE_ERR;
            return PIZDETS;
        }
        else
        {
            #ifdef CANARY_PROTECT_ON
                stk -> buffer = POISON_POINTER;
            #endif
            stk -> data = POISON_POINTER;
        }

        (stk -> capacity) /= INCREASE_DATA;
        temp[stk -> capacity + 1] = CANARY_VALUE;
        #ifdef CANARY_PROTECT_ON
            stk -> data = temp + 1;
            stk -> buffer = temp;
        #else
            stk -> data = temp;
        #endif
        
    }

    fprintf(file, "Stack pop success. Value = " SPEC_TYPEDEF "\n", *value);

    #ifdef STK_VERIFY_ON
        return stack_verify(stk);
    #else 
        return err;
    #endif
}

//----------------------------------------------------------------------------
stack_status stack_verify(stack_lifo_t* stk)
{
    stack_status err = SUCCESS;

    if (stk == NULL)
    {
        error |= STK_PTR_NULL;
        return PIZDETS;
    }

    #ifdef CANARY_PROTECT_ON
        if (stk -> buffer == NULL)         
        {
            error |= STK_DATA_PTR_NULL;
            return PIZDETS;
        }
    #endif
    if (stk -> data == NULL)        
    {
        error |= STK_DATA_PTR_NULL;
        return PIZDETS;
    }
    if ((ssize_t)stk -> size < 0)        
    {
        error |= STK_ELEM_ERROR;
        err = PIZDETS;
    }
    if (stk -> capacity <= 0)            
    {
        error |= STK_ELEM_ERROR;
        err = PIZDETS;
    }
    if (stk -> size > stk -> capacity)   
    {
        error |= STK_ELEM_ERROR;
        err = PIZDETS;
    }
    #ifdef CANARY_PROTECT_ON
        if (!is_equal(stk -> l_canary, STRUCT_CANARY_VALUE) || !is_equal(stk -> r_canary, STRUCT_CANARY_VALUE)) 
        {
            error |= STK_STRUCT_CANARY_ERR;
            err = PIZDETS;
        }
        if (!is_equal(stk -> data[-1], CANARY_VALUE) || !is_equal(stk -> data[stk -> capacity], CANARY_VALUE)) 
        {
            error |= STK_CANARY_ERR;
            err = PIZDETS;
        }
    #endif

    for (size_t i = 0; i < stk -> size; i++)
    {
        if (is_equal(stk -> data[i], POISON_ELEM))
        {
            error |= STK_DATA_POISON_VALUE;
            err = PIZDETS;
        }
    }

    for (size_t i = stk -> size; i < stk -> capacity; i++)
    {
        if (!is_equal(stk -> data[i], POISON_ELEM))
        {
            error |= STK_DATA_POISON_VALUE;
            err = PIZDETS;
        }
    }

    return err;
}

//----------------------------------------------------------------------------
stack_status stack_init_verify(stack_lifo_t* stk)
{
    stack_status err = SUCCESS;

    if (stk == NULL)            
    {
        error |= STK_PTR_NULL;
        return PIZDETS;
    }
    #ifdef CANARY_PROTECT_ON
        if (stk -> buffer != 0)   
        {
            error |= STK_INIT_ERR;
            err = PIZDETS;
        }
    #endif
    if (stk -> data != 0)  
    {
        error |= STK_INIT_ERR;
        err = PIZDETS;
    }
    if (stk -> size != 0)       
    {
        error |= STK_INIT_ERR;
        err = PIZDETS;
    }
    if (stk -> capacity != 0)   
    {
        error |= STK_INIT_ERR;
        err = PIZDETS;
    }
    #ifdef CANARY_PROTECT_ON
        if (!is_equal(stk -> l_canary, 0))   
        {
            error |= STK_INIT_ERR;
            err = PIZDETS;
        }
        if (!is_equal(stk -> r_canary, 0))   
        {
            error |= STK_INIT_ERR;
            err = PIZDETS;
        }
    #endif

    return err;
}

//----------------------------------------------------------------------------
stack_status stack_dump(stack_lifo_t* stk, int line)
{
    stack_status err = SUCCESS;

    #ifdef STK_VERIFY_ON
        if ((err = stack_verify(stk)) != SUCCESS)
        {
            fprintf(file, "Stack_dump enter error in (%s)%s:%d ; \n", __func__, __FILE__, line);
            stack_errors_print(line);

            if ((error & STK_DATA_PTR_NULL) == STK_DATA_PTR_NULL)
                fprintf(file, "\tPossibly forgotten to assign a new pointer after realloc\n");
            
            return err;
        }
    #endif

    stack_errors_print(line);

    fprintf(file, "<------------\n");
    fprintf(file, "capacity = %zu\n", stk -> capacity);
    fprintf(file, "size = %zu\n", stk -> size);
    fprintf(file, "data [%p]\n", stk -> data);

    fprintf(file, "    {\n");
    for (size_t i = 0; i < (stk -> capacity); i++)
    {
        if (is_equal(stk -> data[i], POISON_ELEM))
        {
            fprintf(file, "     [%zu] = %lg(POISON_ELEM)\n", i, stk -> data[i]);
        }
        #ifdef CANARY_PROTECT_ON
            else if (is_equal(stk -> data[i], CANARY_VALUE))
            {
                fprintf(file, "     [%zu] = %lg(CANARY_VALUE)\n", i, stk -> data[i]);
            }
        #endif
        else
        {
            fprintf(file, "     *[%zu] = %lg\n", i, stk -> data[i]);
        }
        
    }
    fprintf(file, "    }\n");
    fprintf(file, "------------>\n");
    fprintf(file, "\n");

    fprintf(file, "Stack dump success\n");

    return err;
}

//----------------------------------------------------------------------------
stack_status stack_destroy(stack_lifo_t* stk, int line)
{
    stack_status err = SUCCESS;

    #ifdef STK_VERIFY_ON
        if ((err = stack_verify(stk)) != SUCCESS)
        {
            fprintf(file, "Stack_destroy enter error in (%s)%s:%d ; \n", __func__, __FILE__, line);

            stack_errors_print(line);

            return err;
        }
    #endif

    #ifdef CANARY_PROTECT_ON
        free(stk -> buffer);
        stk -> buffer = POISON_POINTER;
    #else
        free(stk -> data);
    #endif

    stk -> data = POISON_POINTER;

    fprintf(file, "Stack destroy success\n");

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
stack_status file_init(const char* name, const char* mode, int line)
{
    file = fopen(name, mode);

    if (file == NULL)
    {
        printf(RED "Stack dump error in (%s)%s:%d\n" CRESET, __func__, __FILE__, line);
        error |= STK_DUMP_ERR;
        return PIZDETS;
    }

    return PIZDETS;
}

//----------------------------------------------------------------------------
stack_status stack_errors_print(int line)
{
    fprintf(file, "#############\n");
    fprintf(file, "\tErrors in stack at (%s)%s:%d ; \n", __func__, __FILE__, line);
    if (error == STK_NO_ERROR)
    {
        fprintf(file, "\tNo errors in stack\n");
        fprintf(file, "#############\n");
        return SUCCESS;
    }

    if ((error & STK_PTR_NULL) == STK_PTR_NULL)                       fprintf(file, "\tSTK_PTR_NULL\n");
    if ((error & STK_DATA_PTR_NULL) == STK_DATA_PTR_NULL)             fprintf(file, "\tSTK_DATA_PTR_NULL\n");
    if ((error & STK_ELEM_ERROR) == STK_ELEM_ERROR)                   fprintf(file, "\tSTK_ELEM_ERROR\n");
    if ((error & STK_MEMORY_ERROR) == STK_MEMORY_ERROR)               fprintf(file, "\tSTK_MEMORY_ERROR\n");
    if ((error & STK_DATA_POISON_VALUE) == STK_DATA_POISON_VALUE)     fprintf(file, "\tSTK_DATA_POISON_VALUE\n");
    if ((error & STK_DECREASE_ERR) == STK_DECREASE_ERR)               fprintf(file, "\tSTK_DECREASE_ERR\n");
    if ((error & STK_INCREASE_ERR) == STK_INCREASE_ERR)               fprintf(file, "\tSTK_INCREASE_ERR\n");
    if ((error & STK_INIT_ERR) == STK_INIT_ERR)                       fprintf(file, "\tSTK_INIT_ERR\n");
    if ((error & STK_DUMP_ERR) == STK_DUMP_ERR)                       fprintf(file, "\tSTK_DUMP_ERR\n");
    if ((error & STK_CANARY_ERR) == STK_CANARY_ERR)                   fprintf(file, "\tSTK_CANARY_ERR\n");
    if ((error & STK_EMPTY_ERR) == STK_EMPTY_ERR)                     fprintf(file, "\tSTK_EMPTY_ERR\n");
    if ((error & STK_STRUCT_CANARY_ERR) == STK_STRUCT_CANARY_ERR)     fprintf(file, "\tSTK_STRUCT_CANARY_ERR\n");
    if ((error & STK_FILE_ERROR) == STK_FILE_ERROR)                   fprintf(file, "\tSTK_FILE_ERROR\n");

    fprintf(file, "#############\n");

    return SUCCESS;
}

//----------------------------------------------------------------------------
void print_stack_status(stack_status stk_status)
{
    if (stk_status == SUCCESS)
        printf(GRN "Stack status: %u\n" CRESET, stk_status);
    else
        printf(YEL "Stack status: %u\n" CRESET, stk_status);

    fprintf(file, "Stack status print success\n");
}
