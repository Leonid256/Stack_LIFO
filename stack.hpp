#ifndef STACK_HPP
#define STACK_HPP

typedef double stack_elem_t;

//----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "colours.hpp"

//----------------------------------------------------------------------------
#define CANARY_PROTECT_ON

//----------------------------------------------------------------------------
struct stack_lifo_t
{
    #ifdef CANARY_PROTECT_ON
        stack_elem_t l_canary;
        stack_elem_t* data;
    #endif
    stack_elem_t* real_data;
    size_t size;
    size_t capacity;
    #ifdef CANARY_PROTECT_ON
        stack_elem_t r_canary;
    #endif
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
    STK_EMPTY_ERR           = 11,
    STK_STRUCT_CANARY_ERR   = 12
};

//----------------------------------------------------------------------------
stk_error_codes_t stack_init(stack_lifo_t* stk, size_t capacity, int line);
stk_error_codes_t increase_stack_init(stack_elem_t* data, size_t capacity);
stk_error_codes_t stack_push(stack_lifo_t* stk, stack_elem_t value, int line);
stk_error_codes_t stack_pop(stack_lifo_t* stk, stack_elem_t* value, int line);
stk_error_codes_t stack_verify(stack_lifo_t* stk);
stk_error_codes_t stack_init_verify(stack_lifo_t* stk);
stk_error_codes_t stack_dump(stack_lifo_t* stk, const char* name, const char* mode, int line);
stk_error_codes_t stack_destroy(stack_lifo_t* stk, int line);
bool is_equal(stack_elem_t a, stack_elem_t b);

//----------------------------------------------------------------------------
#define POISON_POINTER NULL


#define stack_init_default(stk, capacity) stack_init(stk, capacity, __LINE__)
#define stack_push_default(stk, value) stack_push(stk, value, __LINE__)
#define stack_pop_default(stk, value) stack_pop(stk, value, __LINE__)
#define stack_dump_default(stk, name, mode) stack_dump(stk, name, mode, __LINE__)
#define stack_destroy_default(stk) stack_destroy(stk, __LINE__)
//----------------------------------------------------------------------------
const size_t START_CAPACITY = 5;
const int INCREASE_DATA = 2;
const stack_elem_t POISON_ELEM = 0xEDAEDADEDA;
const stack_elem_t STRUCT_CANARY_VALUE = 0xDEADBEEFDEAD;
const stack_elem_t CANARY_VALUE = 0xDEADBEEF;
const char* STACK_DUMP_FILE = "stack_dump.log";
const double EPCILON = 0.001;

//----------------------------------------------------------------------------
#endif
