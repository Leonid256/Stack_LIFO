#ifndef STK_VERIFY_HPP
#define STK_VERIFY_HPP

#ifdef STK_VEFIFY_ON
    #define STK_VERIFY(...) __VA_ARGS__
#else
    #define STK_VERIFY(...)
#endif

#endif