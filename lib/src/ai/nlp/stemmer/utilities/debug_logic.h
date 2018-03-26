/**@addtogroup Debugging
   @brief Functions used for debugging.
   @date 2008-2015
   @copyright Oleander Software, Ltd.
   @author Oleander Software, Ltd.
   @details This program is free software; you can redistribute it and/or modify
    it under the terms of the BSD License.
* @{*/

#ifndef __DEBUG_LOGIC_H__
#define __DEBUG_LOGIC_H__

#include <iostream>
#include <fstream>
#include <iterator>

/**
\def __DEBUG_FUNCTION_NAME__
    Expands to the name of the current function. Will not be defined if
    compiler does not have a native function name macro.
\def CASSERT(expression)
    Validates that an expression is true at compile time. If the expression is false
    then compilation will fail.
\def NON_UNIT_TEST_ASSERT(expression)
    If unit test symbol (__UNITTEST) is defined then does nothing; otherwise asserts.
    This is useful for suppressing asserts when unit testing.
\def DUMP_TO_FILE(stream,file)
    Prints data stream to a specified file.*/
/** @} */

#ifdef __PRETTY_FUNCTION__
    #define __DEBUG_FUNCTION_NAME__ __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
    #define __DEBUG_FUNCTION_NAME__ __FUNCTION__
#elif defined(__func__)
    #define __DEBUG_FUNCTION_NAME__ __func__
#elif defined(__FUNCSIG__)
    #define __DEBUG_FUNCTION_NAME__ __FUNCSIG__
#elif defined(__FUNCDNAME__)
    #define __DEBUG_FUNCTION_NAME__ __FUNCDNAME__
#endif

//----------------------------------------------------------------------
#if !defined (NDEBUG) || defined ENABLE_LOGIC_CHECKS || defined DEBUG_ENABLE_ALL
    #define CASSERT(x) typedef char __C_ASSERT__[(x) ? 1 : -1]
#else
    #define CASSERT(x) ((void)0)
#endif

//----------------------------------------------------------------------
#ifdef __UNITTEST
    #define NON_UNIT_TEST_ASSERT(x) ((void)0)
#else
    #define NON_UNIT_TEST_ASSERT(x) assert(x)
#endif

//----------------------------------------------------------------------
#if !defined (NDEBUG) || defined DEBUG_ENABLE_ALL
    #define DUMP_TO_FILE(x,file) __debug::__dump_to_file((x), (file))
#else
    #define DUMP_TO_FILE(x,file) ((void)0)
#endif

//----------------------------------------------------------------------
namespace __debug
    {
    //debug helpers
    inline void __dump_to_file(const wchar_t* begin, const std::string& file_path)
        {
        if (begin == NULL)
            { return; }
        std::wofstream f(file_path.c_str());
        f.write(begin, static_cast<std::streamsize>(std::wcslen(begin)));
        }
    }

#endif //__DEBUG_LOGIC_H__
