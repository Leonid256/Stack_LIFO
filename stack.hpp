typedef double stack_elem_t;

//----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------
struct stack_lifo_t
{
    stack_elem_t* data;
    size_t size;
    size_t capacity;
};

enum stk_error_codes_t
{
    STK_NO_ERROR            = 0,
    STK_PTR_NULL            = 1,
    STK_DATA_PTR_NULL       = 2,
    STK_ELEM_ERROR          = 3,
    STK_MEMORY_ERROR        = 4,
    STK_DATA_POISON_VALUE   = 5,
    STK_DECREASE_ERR        = 6,
    STK_INCREASE_ERR        = 7,
    STK_INIT_ERR            = 8,
    STK_DUMP_ERR            = 9,
    STK_CANARY_ERR          = 10,
    STK_EMPTY_ERR           = 11
};

//----------------------------------------------------------------------------
stk_error_codes_t stack_init(stack_lifo_t* stk, size_t capacity);
stk_error_codes_t increase_stack_init(stack_elem_t* data, size_t capacity);
stk_error_codes_t stack_push(stack_lifo_t* stk, stack_elem_t elem);
stk_error_codes_t stack_pop(stack_lifo_t* stk, stack_elem_t* value);
stk_error_codes_t stack_verify(stack_lifo_t* stk);
stk_error_codes_t stack_init_verify(stack_lifo_t* stk);
stk_error_codes_t stack_dump(stack_lifo_t* stk, const char* name, const char* mode);
stk_error_codes_t stack_destroy(stack_lifo_t* stk);

//----------------------------------------------------------------------------
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

#define CRESET "\e[0m"
//----------------------------------------------------------------------------
const size_t START_CAPACITY = 5;
const int INCREASE_DATA = 2;
const stack_elem_t POISON_ELEM = 0xEDAEDADEDA;
const stack_elem_t CANARY_VALUE = 0xDEADBEEF;
const stack_elem_t POISON_POINTER = NULL;
const char* STACK_DUMP_FILE = "stack_dump.txt";
