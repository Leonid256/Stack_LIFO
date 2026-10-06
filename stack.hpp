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
#include "stk_verify.hpp"

//----------------------------------------------------------------------------
extern unsigned long long error;
extern FILE* file;

//----------------------------------------------------------------------------
const size_t START_CAPACITY = 5;
const int INCREASE_DATA = 2;
const stack_elem_t POISON_ELEM = 0xEDAEDADEDA;
const stack_elem_t STRUCT_CANARY_VALUE = 0xDEADBEEFDEAD;
const stack_elem_t CANARY_VALUE = 0xDEADBEEF;
extern const char* STACK_DUMP_FILE;
const double EPCILON = 0.001;

//----------------------------------------------------------------------------
#define CANARY_PROTECT_ON
#define STK_VERIFY_ON

//----------------------------------------------------------------------------
struct stack_lifo_t
{
    #ifdef CANARY_PROTECT_ON
        stack_elem_t l_canary;
        stack_elem_t* buffer;
    #endif
    stack_elem_t* data;
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
    STK_ELEM_ERROR          = 4,
    STK_MEMORY_ERROR        = 8,
    STK_DATA_POISON_VALUE   = 16,
    STK_DECREASE_ERR        = 32,
    STK_INCREASE_ERR        = 64,
    STK_INIT_ERR            = 128,
    STK_DUMP_ERR            = 256,
    STK_CANARY_ERR          = 512,
    STK_EMPTY_ERR           = 1024,
    STK_STRUCT_CANARY_ERR   = 2048,
    STK_FILE_ERROR          = 4096
};

enum stack_status
{
    SUCCESS = 0,
    PIZDETS = 67
};

//----------------------------------------------------------------------------
stack_status stack_init(stack_lifo_t* stk, size_t capacity, int line);
stack_status stack_push(stack_lifo_t* stk, stack_elem_t value, int line);
stack_status stack_pop(stack_lifo_t* stk, stack_elem_t* value, int line);
stack_status stack_verify(stack_lifo_t* stk);
stack_status stack_init_verify(stack_lifo_t* stk);
stack_status stack_dump(stack_lifo_t* stk, int line);
stack_status stack_destroy(stack_lifo_t* stk, int line);
bool is_equal(stack_elem_t a, stack_elem_t b);
stack_status file_init(const char* name, const char* mode, int line);
stack_status stack_errors_print(int line);
void print_stack_status(stack_status stk_status);

//----------------------------------------------------------------------------
#define POISON_POINTER NULL
#define SPEC_TYPEDEF "%lg"

#define STACK_INIT(stk, capacity) stack_init(stk, capacity, __LINE__)
#define STACK_PUSH(stk, value) stack_push(stk, value, __LINE__)
#define STACK_POP(stk, value) stack_pop(stk, value, __LINE__)
#define STACK_DUMP(stk) stack_dump(stk, __LINE__)
#define STACK_DESTROY(stk) stack_destroy(stk, __LINE__)
#define FILE_INIT(name, mode) file_init(name, mode, __LINE__)

#define ERROR_PRINT(err) if ((error & err) == err) fprintf(file, "\t%s\n", #err);

//----------------------------------------------------------------------------
#endif
