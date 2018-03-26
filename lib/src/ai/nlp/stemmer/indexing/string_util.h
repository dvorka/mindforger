/**
\date 2003-2015
\copyright Oleander Software, Ltd.
\author Oleander Software, Ltd.
\details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
*/

#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cwctype>
#include <cctype>
#include <cstring>
#include <cwchar>
#include <cstddef>
#include <string>
#include <functional>
#include <cassert>
#include <clocale>
#include <stdexcept>
#include "common_lang_constants.h"
#include "../utilities/safe_math.h"
#include "../utilities/utilities.h"
#include "../utilities/debug_logic.h"

namespace string_util
    {
    ///lowercases any Western European alphabetic characters
    inline wchar_t tolower_western(const wchar_t c)
        {
        return ((c >= L'A') && (c <= L'Z')) ||
            ((c >= 0xC0) && (c <= 0xD6)) ||
            ((c >= 0xD8) && (c <= 0xDE))
                ? (c + 32) : c;
        }

    ///ANSI C decorators
    ///strtol
    inline double strtol(const char* str, char** strend, int radix)
        { return std::strtol(str, strend, radix); }
    inline double strtol(const wchar_t* str, wchar_t** strend, int radix)
        { return std::wcstol(str, strend, radix); }
    ///strtod
    inline double strtod(const char* str, char** strend)
        { return std::strtod(str, strend); }
    inline double strtod(const wchar_t* str, wchar_t** strend)
        { return std::wcstod(str, strend); }
    ///atoi
    inline int atoi(const char* str)
        {
        if (str == NULL)
            { return 0; }
        return std::atoi(str);
        }
    inline int atoi(const wchar_t* str)
        {
        if (str == NULL)
            { return 0; }
        wchar_t* dummy = NULL;
        return static_cast<int>(std::wcstol(str, &dummy, 10));
        }
    ///atol
    inline long atol(const char* str)
        {
        if (str == NULL)
            { return 0; }
        return std::atol(str);
        }
    inline long atol(const wchar_t* str)
        {
        if (str == NULL)
            { return 0; }
        wchar_t* dummy = NULL;
        return std::wcstol(str, &dummy, 10);
        }
    ///tolower
    inline int tolower(char c)
        {
        return std::tolower(static_cast<unsigned char>(c));
        }
    inline wchar_t tolower(wchar_t c)
        {
        return std::towlower(c);
        }
    ///toupper
    inline int toupper(char c)
        {
        return std::toupper(static_cast<unsigned char>(c));
        }
    inline wchar_t toupper(wchar_t c)
        {
        return std::towupper(c);
        }
    ///memset
    template<typename T>
    inline T* memset(T* dest, int c, size_t count)
        {
        return static_cast<T*>(std::memset(dest, c, count));
        }
    //partial specialized versions of memset
    inline char* memset(char* dest, int c, size_t count)
        {
        return static_cast<char*>(std::memset(dest, c, count));
        }
    inline wchar_t* memset(wchar_t* dest, int c, size_t count)
        {
        return std::wmemset(dest, static_cast<wchar_t>(c), count);
        }
    ///strchr
    inline const char* strchr(const char* s, int ch)
        {
        return std::strchr(s, ch);
        }
    inline const wchar_t* strchr(const wchar_t* s, wchar_t ch)
        {
        return std::wcschr(s, ch);
        }
    ///strstr
    inline const char* strstr(const char* s1, const char* s2)
        {
        return std::strstr(s1, s2);
        }
    inline const wchar_t* strstr(const wchar_t* s1, const wchar_t* s2)
        {
        return std::wcsstr(s1, s2);
        }
    ///strcspn
    inline size_t strcspn(const char* string1,const char* string2)
        {
        return std::strcspn(string1, string2);
        }
    inline size_t strcspn(const wchar_t* string1,const wchar_t* string2)
        {
        return std::wcscspn(string1, string2);
        }
    ///strncat
    inline char* strncat(char* strDest, const char* strSource, size_t count)
        {
        return std::strncat(strDest, strSource, count);
        }
    inline wchar_t* strncat(wchar_t* strDest, const wchar_t* strSource, size_t count)
        {
        return std::wcsncat(strDest, strSource, count);
        }
    ///wctomb
    inline int wctomb(wchar_t* s, wchar_t wc)
        {
        s[0] = wc;
        return -1;
        }
    inline int wctomb(char* s, wchar_t wc)
        {
        return std::wctomb(s, wc);
        }
    //strlen
    inline size_t strlen(const char* text) { return std::strlen(text); }
    inline size_t strlen(const wchar_t* text)
        { return std::wcslen(text); }
    ///strcmp
    inline int strcmp(const char* string1, const char* string2)
        {
        return std::strcmp(string1, string2);
        }
    inline int strcmp(const wchar_t* string1, const wchar_t* string2)
        {
        return std::wcscmp(string1, string2);
        }
    ///strncmp
    inline int strncmp(const char* string1, const char* string2, size_t count)
        {
        return std::strncmp(string1, string2, count);
        }
    inline int strncmp(const wchar_t* string1, const wchar_t* string2, size_t count)
        {
        return std::wcsncmp(string1, string2, count);
        }
    ///strncpy
    inline char* strncpy(char* strDest, const char* strSource, size_t count)
        {
        return std::strncpy(strDest, strSource, count);
        }
    inline wchar_t* strncpy(wchar_t* strDest, const wchar_t* strSource, size_t count)
        {
        return std::wcsncpy(strDest, strSource, count);
        }

    ///functions not available in ANSI C
    /*Converts an integer value into a string.
      @param value The integer to convert.
      @param out The character buffer to write the integer as a string into. This can be either a char* or wchar_t* buffer.
      @param length The length of the output buffer (in character count).
      @returns 0 on success, -1 on failure. Will fail if the buffer is either invalid
      or not large enough to hold the converted value.*/
    template<typename charT>
    inline int itoa(long value, charT* out, const size_t length)
        {
        if (length == 0 || out == NULL)
            { return -1; }
        //space for a negative sign if we need it
        const size_t signPos = value < 0 ? 1 : 0;
        size_t i = 0;
        if (value == 0)
            { out[i++] = 0x30/*zero*/; }
        else if (value < 0)
            {
            out[i++] = 0x2D/*minus sign*/;
            value = -value;
            }
        while (value > 0 && i+1 < length)
            {
            out[i++] = 0x30 + value%10;
            value /= 10;
            }
        /*Not enough space in the buffer or NULL terminator? Clear out the data that we copied
        into it and return failure.*/
        if (i+1 == length && value > 0)
            {
            std::memset(out,0, length*sizeof(charT));
            return -1;
            }
        out[i] = 0;
        std::reverse<charT*>(out+signPos, out+i);
        return 0;
        }
    
    /**Determines whether a character is a space, tab, or newline. Also includes double-width and no break spaces.
    @param ch The letter to be analyzed.*/
    template<typename T>
    inline bool is_space(const T ch)
        {
        return (ch == 0x20) ?
            true : (ch == 0x0D) ?
            true : (ch == 0x0A) ?
            true : (ch == 0x09) ?
            true : (ch == 0xA0/*no break space*/) ?
            true : (ch == 0x3000/*Japanese Ideographic Space*/) ?
            true : (ch == 0x200B/*Zero-width space*/) ?
            true : false;
        }

    /**Determines whether a character is a hexademical digit (0-9,A-F,a-f).
    @param ch The letter to be analyzed.*/
    template<typename T>
    inline bool is_hex_digit(const T ch)
        {
        return (iswdigit(wchar_t(ch)) ||
            ((ch >= 0x61/*'a'*/ && ch <= 0x66/*'f'*/) ||
            (ch >= 0x41/*'A'*/ && ch <= 0x46/*'F'*/)) );
        }

    /**Converts string in hex format to int. Default figures out how much of the string
        is a valid hex string, but passing a value to the second parameter overrides this
        and allows you to indicate how much of the string to try to convert.
        @param hexStr The string to convert.
        @length How much of the string to analyze. The value -1 (the default) will tell the function
        to read until there are no more valid hexadecimal digits.
        @returns The value of the string as an integer.*/
    template<typename T>
    inline int axtoi(const T* hexStr, size_t length = -1)
        {
        if (hexStr == NULL || *hexStr == 0 || length == 0)
            { return 0; }
        //skip leading 0x
        if (hexStr[0] == 0x30/*0*/ &&
            length != 1 &&
            is_either<T>(hexStr[1], 0x78/*x*/, 0x58/*X*/))
            {
            hexStr += 2;
            //if they specified a length to read then take into account the 0x that we just skipped over
            if (length != static_cast<size_t>(-1) && length >= 2)
                {
                length -= 2;
                if (length == 0)//just a 0x string, then we're done
                    { return 0; }
                }
            }
        if (length == static_cast<size_t>(-1))
            {
            const T* currentPos = hexStr;
            do
                {
                if (currentPos[0] == 0 || !string_util::is_hex_digit(currentPos[0]))
                    { break; }
                }
            while (currentPos++);

            length = currentPos-hexStr;
            //if no valid hex digits then fail and return zero
            if (length == 0)
                { return 0; }
            }
        size_t strPos = 0;
        int intValue = 0;
        //storage for converted values
        int* digits = new int[length+1]; std::auto_ptr<int> digitsDeleter(digits);
        std::memset(digits, 0, sizeof(int)*(length+1));
        while (strPos < length)
            {
            if (hexStr[strPos] == 0)
                break;
            //0-9
            if (is_within<T>(hexStr[strPos], common_lang_constants::NUMBER_0, common_lang_constants::NUMBER_9) )
                digits[strPos] = (hexStr[strPos]&0x0F);
            //A-F
            else if (is_within<T>(hexStr[strPos], common_lang_constants::LOWER_A, common_lang_constants::LOWER_F) ||
                     is_within<T>(hexStr[strPos], common_lang_constants::UPPER_A, common_lang_constants::UPPER_F) )
                digits[strPos] = (hexStr[strPos]&0x0F)+9;
            else
                { break; }
            ++strPos;
            }
        const size_t count = strPos;
        size_t digitPos = strPos - 1;
        strPos = 0;
        while (strPos < count)
            {
            //shift OR the bits into return value.
            intValue = intValue | (digits[strPos] << (digitPos << 2));
            --digitPos;
            ++strPos;
            }

        return intValue;
        }

    /**@returns The number of characters in the string pointed to by str, not including the
        terminating '\0' character, but at most maxlen. In doing this, strnlen looks only at
        the first maxlen characters at str and never beyond str+maxlen. This function should be used
        for input that may not be NULL terminated.
       @param str The string to review.
       @param maxlen The maximum length of the string to scan.
       @returns The valid length of the string or maxlen, whichever is shorter.*/
    template<typename T>
    inline size_t strnlen(const T* str, const size_t maxlen)
        {
        if (!str || maxlen == 0)
            { return 0; }
        size_t i;
        for (i = 0; i < maxlen && str[i]; ++i);
        return i;
        }

    ///search for substring in string (case-insensitive)
    template<typename T>
    inline const T* stristr(const T* string, const T* strSearch)
        {
        if (!string || !strSearch || *strSearch == 0)
            { return NULL; }
        while (*string)
            {
            //compare the characters one at a time
            size_t i = 0;
            for (i = 0; strSearch[i] != 0; ++i)
                {
                if (string[i] == 0)
                    { return NULL; }
                if (string_util::tolower(strSearch[i]) != string_util::tolower(string[i]) )
                    {
                    ++string;
                    break;
                    }
                }
            //if the substring loop completed then the substring was found
            if (strSearch[i] == 0)
                { return string; }
            }
        return NULL;
        }

    /**Searches for substring in a larger string (case-insensitively), limiting the search
    to a specified number of characters.*/
    template<typename T>
    inline const T* strnistr(const T* string, const T* strSearch, const size_t string_len)
        {
        if (!string || !strSearch || string_len == 0 || *strSearch == 0)
            { return NULL; }
        for (size_t i = 0; i < string_len; ++i)
            {
            //compare the characters one at a time
            size_t j = 0;
            for (j = 0; strSearch[j] != 0; ++j)
                {
                if ((i+j) >= string_len || string[i+j] == 0)
                    { return NULL; }
                if (string_util::tolower(strSearch[j]) != string_util::tolower(string[i+j]) )
                    { break; }
                }
            //if the substring loop completed then the substring was found
            if (strSearch[j] == 0)
                { return (string+i); }
            }
        return NULL;
        }

    /**Search string in reverse for substring.
    "offset" is how far we are in the source string already and how far
    to go back.*/
    template<typename T>
    inline const T* strrstr(const T* string,
                    const T* search,
                    size_t offset)
        {
        if (!string || !search)
            { return NULL; }
        const size_t len = string_util::strlen(search);
        if (len > offset) return NULL;
        string -= len;
        offset -= len;
        bool fnd = false;
        while (!fnd && offset > 0)
            {
            fnd = true;
            for (size_t i=0; i < len; ++i)
                {
                if (string[i] != search[i])
                    {
                    fnd = false;
                    break;
                    }
                }
            if (fnd) return string;
            --string;
            --offset;
            }
        return NULL;
        }

    ///Case-insensitive comparison by character count.
    template<typename T>
    inline int strnicmp(const T* first, const T* last, size_t count)
        {
        //first check if either of the strings are NULL
        if (!first && !last)
            { return 0; }
        else if (!first && last)
            { return -1; }
        else if (first && !last)
            { return 1; }

        int f,l;
        int result = 0;

        if (count)
            {
            do
                {
                f = string_util::tolower(*(first++) );
                l = string_util::tolower(*(last++) );
                } while ( (--count) && f && (f == l) );
            result = static_cast<int>(f - l);
            }
        return result;
        }

    ///Case-insensitive comparison.
    template<typename T>
    inline int stricmp(const T* first, const T* last)
        {
        //first check if either of the strings are NULL
        if (!first && !last)
            { return 0; }
        else if (!first && last)
            { return -1; }
        else if (first && !last)
            { return 1; }

        int f,l;
        do
            {
            f = string_util::tolower(*(first++) );
            l = string_util::tolower(*(last++) );
            } while (f && (f == l) );

        return static_cast<int>(f - l);
        }

    /**Natural order comparison functions.
    Compare, recognizing numeric strings.*/
    template<typename T>
    inline int strnatordcmp(const T* first_string, const T* second_string, bool case_insensitive = false)
        {
        //first check if either of the strings are NULL
        if (!first_string && !second_string)
            { return 0; }
        else if (!first_string && second_string)
            { return -1; }
        else if (first_string && !second_string)
            { return 1; }

        size_t first_string_index = 0, second_string_index = 0;
        T ch1, ch2;
 
        while (true)
            {
            ch1 = first_string[first_string_index];
            ch2 = second_string[second_string_index];

            //skip leading spaces
            while (is_space(ch1))
                { ch1 = first_string[++first_string_index]; }

            while (is_space(ch2))
                { ch2 = second_string[++second_string_index]; }

            //process run of digits
            if (iswdigit(ch1) && iswdigit(ch2))
                {
                T *firstEnd, *secondEnd;
                double firstDouble = string_util::strtod(first_string+first_string_index, &firstEnd);
                double secondDouble = string_util::strtod(second_string+second_string_index, &secondEnd);

                if (firstDouble < secondDouble)
                    { return -1; }
                else if (firstDouble > secondDouble)
                    { return 1; }
                else //numbers are equal
                    {
                    //if this was the end of both strings then they are equal
                    if (*firstEnd == 0 && *secondEnd == 0)
                        { return 0; }
                    /*the first string is done, but there is more to the second string
                    after the number, so first is smaller*/
                    else if (*firstEnd == 0)
                        { return -1; }
                    /*the second string is done, but there is more to the first string
                    after the number, so first is bigger*/
                    else if (*secondEnd == 0)
                        { return 1; }
                    //there is more to both of them, so move the counter and move on
                    else
                        {
                        first_string_index = (firstEnd - first_string);
                        second_string_index = (secondEnd - second_string);
                        continue;
                        }
                    }
                }

            //if we are at the end of the strings then they are the same
            if (ch1 == 0 && ch2 == 0)
                { return 0; }

            if (case_insensitive)
                {
                ch1 = string_util::tolower(ch1);
                ch2 = string_util::tolower(ch2);
                }

            if (ch1 < ch2)
                { return -1; }
            else if (ch1 > ch2)
                { return 1; }

            ++first_string_index;
            ++second_string_index;
            }
        }

    ///Compare, recognizing numeric strings and ignoring case.
    template<typename T>
    inline int strnatordncasecmp(const T* a, const T* b)
        {
        return strnatordcmp(a, b, true);
        }

    ///Indicates whether a larger strings ends with the specified suffix. Lengths are provided
    ///by the caller for efficiency. This function is case sensitive.
    template<typename T>
    inline bool has_suffix(const T* text, const size_t text_length, const T* suffix, const size_t suffix_length)
        {
        if (text == NULL || suffix == NULL || text_length <= suffix_length)
            { return false; }
        return (string_util::strnicmp(text+(text_length-suffix_length), suffix, suffix_length) == 0);
        }

    /**Searches for a matching tag, skipping an extra open/close pairs of symbols in between.
    @param openSymbol The opening symbol.
    @param closeSymbol The closing symbol that we are looking for.
    @param fail_on_overlapping_open_symbol Whether it should immediately return failure if an open
    symbol is found before a matching close symbol.
    @returns A pointer to where the closing tag is, or NULL if one can't be found.*/
    template<typename T>
    inline const T* find_matching_close_tag(const T* string, const T openSymbol, const T closeSymbol,
                                            const bool fail_on_overlapping_open_symbol = false)
        {
        if (!string)
            { return NULL; }
        long open_stack = 0;
        while (*string)
            {
            if (string[0] == openSymbol)
                {
                if (fail_on_overlapping_open_symbol)
                    { return NULL; }
                ++open_stack;
                }
            else if (string[0] == closeSymbol)
                {
                if (open_stack == 0)
                    { return string; }
                --open_stack;
                }
            ++string;
            }
        return NULL;
        }

    ///Searches for a matching tag, skipping an extra open/close pairs of symbols in between.
    template<typename T>
    inline const T* find_matching_close_tag(const T* string, const T* openSymbol,
                                            const T* closeSymbol)
        {
        if (!string || !openSymbol || !closeSymbol)
            { return NULL; }
        const size_t openSymbolLength = string_util::strlen(openSymbol);
        const size_t closeSymbolLength = string_util::strlen(closeSymbol);
        if (openSymbolLength == 0 || closeSymbolLength == 0)
            { return NULL; }
        long open_stack = 0;
        const T openSymbolFirstCharacter = openSymbol[0];
        const T closeSymbolFirstCharacter = closeSymbol[0];
        while (*string)
            {
            //to prevent unnecessary calls to strncmp, we check the current character first
            if (string[0] == openSymbolFirstCharacter &&
                string_util::strncmp(string,openSymbol,openSymbolLength) == 0)
                {
                ++open_stack;
                string += openSymbolLength;
                continue;
                }
            else if (string[0] == closeSymbolFirstCharacter &&
                     string_util::strncmp(string,closeSymbol,closeSymbolLength) == 0)
                {
                if (open_stack == 0)
                    { return string; }
                --open_stack;
                }
            ++string;
            }
        return NULL;
        }

    /**Searches for a single character in a string for n number of characters.
        Size argument should be less than or equal to the length of the string being searched.
        @param string The string to search in.
        @param ch The character to search for.
        @param numberOfCharacters The number of characters to search through in the string.
        @returns A pointer in the string where the character was found, or NULL if not found.*/
    template<typename T>
    inline const T* strnchr(const T* string, const T ch, size_t numberOfCharacters)
        {
        if (!string)
            { return NULL; }
        size_t i = 0;
        for (i = 0; i < numberOfCharacters; ++i)
            {
            /*if string being searched is shorter
            than the size argument then return failure (NULL).*/
            if (string[i] == 0)
                {
                NON_UNIT_TEST_ASSERT(0 && "strnchr: string shorter than size parameter.");
                return NULL;
                }
            if (string[i] == ch)
                { return string+i; }
            }
        return NULL;
        }

    /**Searches for a single character from a sequence in a string and
    return a pointer if found.*/
    template<typename T>
    inline const T* strcspn_pointer(const T* string1, const T* string2, const size_t string2Length)
        {
        if (!string1 || !string2 || string2Length == 0)
            { return NULL; }
        assert((string_util::strlen(string2) == string2Length) && "Invalid length passed to strcspn_pointer().");
        size_t i = 0;
        while (string1)
            {
            if (string1[0] == 0)
                { return NULL; }
            for (i = 0; i < string2Length; ++i)
                {
                if (string1[0] == string2[i])
                    { return string1; }
                }
            ++string1;
            }
        return NULL;
        }

    /**Searches for a single character from a sequence in a string for n number of characters.
        @param stringToSearch The string to search.
        @param stringToSearchLength The length of the string being searched.
        @param searchString The sequence of characters to search for.
        @param searchStringLength The length of the sequence string.
        @returns The index into the string that the character was found. Returns the length of the string if not found.*/
    template<typename T>
    inline size_t strncspn(const T* stringToSearch, const size_t stringToSearchLength,
        const T* searchString, const size_t searchStringLength)
        {
        if (!stringToSearch || !searchString || stringToSearchLength == 0 || searchStringLength == 0)
            { return stringToSearchLength; }
        assert((string_util::strlen(searchString) == searchStringLength) && "Invalid length passed to strncspn().");
        size_t i = 0, j = 0;
        for (i = 0; i < stringToSearchLength; ++i)
            {
            /*If string being searched is shorter than the size argument then fail.
            Note that we should pass back the length of the string that the caller passed in
            so that they know that it failed, not the actual position that we stopped on which would
            be a NULL termintor.*/
            if (stringToSearch[i] == 0)
                {
                NON_UNIT_TEST_ASSERT(0 && "strncspn: string shorter than size parameter.");
                return stringToSearchLength;
                }
            for (j = 0; j < searchStringLength; ++j)
                {
                if (stringToSearch[i] == searchString[j])
                    { return i; }
                }
            }
        return i;
        }

    /**Searches for a single character not from a sequence in a string in reverse.
        @param string The string to search in.
        @param search The sequence of characters to skip.
        @param offset Where to begin the search. If -1, then the reverse search will begin at the end of the string.
        @returns The position of where the last non-matching character is at, or -1 if it can't be found.*/
    template<typename T>
    inline size_t find_last_not_of(const T* string, const T* search,
                    size_t offset = std::basic_string<T>::npos)
        {
        if (!string || !search)
            { return std::basic_string<T>::npos; }
        if (offset == std::basic_string<T>::npos)
            { offset = string_util::strlen(string)-1; }

        size_t i=0;
        while (offset != std::basic_string<T>::npos)
            {
            for (i = 0; search[i] != 0; ++i)
                {
                if (string[offset] == search[i])
                    { break; }
                }
            //If the whole search string wasn't scanned, then there was a match. Move on.
            if (search[i] != 0)
                {
                if (offset == 0)
                    { return std::basic_string<T>::npos; }
                --offset;
                }
            else
                { return offset; }
            }
        return std::basic_string<T>::npos;
        }

    /**Searches for the last instance of a character in a string in reverse.
        @param string The string to search.
        @param ch The character to search for.
        @param offset The offset in the string to begin the search from. The default (-1) will begin the search at the end of the string.
        @returns The offset of the found character, or -1 if not found.*/
    template<typename T>
    inline size_t find_last_of(const T* string,
                    const T ch,
                    size_t offset = -1)
        {
        if (!string)
            { return static_cast<size_t>(-1); }
        if (offset == static_cast<size_t>(-1))
            { offset = string_util::strlen(string)-1; }

        while (offset != static_cast<size_t>(-1))
            {
            if (string[offset] == ch)
                { return offset; }
            --offset;
            }
        return static_cast<size_t>(-1);
        }

    /*Searches for the first occurrence that is not a character from a sequence in
        a string for n number of characters and returns zero-based index if found.
        @param stringToSearch The string to search.
        @param stringToSearchLength The length of the string being searched.
        @param searchString The sequence of characters to perform a reverse match with.
        @param searchStringLength The length of the search character sequence.
        @returns The index into the string that the character was not found, or 
        the length of the string if nothing was found that couldn't match the search string.*/
    template<typename T>
    inline size_t find_first_not_of(const T* stringToSearch, const size_t stringToSearchLength,
                                    const T* searchString, const size_t searchStringLength)
        {
        if (!stringToSearch || !searchString || stringToSearchLength == 0 || searchStringLength == 0)
            { return stringToSearchLength; }
        NON_UNIT_TEST_ASSERT((string_util::strlen(searchString) == searchStringLength) && "Invalid length passed to find_first_not_of().");
        size_t i = 0, j = 0;
        for (i = 0; i < stringToSearchLength; ++i)
            {
            /*If string being searched is shorter
            than the size argument then return string length to indicate failure to find anything.
            Although this isn't really the index that we stopped in, this will tell the caller that
            the search failed.*/
            if (stringToSearch[i] == 0)
                {
                NON_UNIT_TEST_ASSERT(0 && "find_first_not_of: string shorter than size parameter.");
                return stringToSearchLength;
                }
            for (j = 0; j < searchStringLength; ++j)
                {
                //stop if a character matching anything in the sequence
                if (stringToSearch[i] == searchString[j])
                    { break; }
                }
            //if we went through the whole sequence then nothing matched
            if (j == searchStringLength)
                { return i; }
            }
        return i;
        }

    //utility classes
    //equal functors
    template<typename T>
    class equal_string_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::stricmp(a_, b_) == 0);
            }
        };

    template<typename T>
    class equal_basic_string_i_compare
        : public std::binary_function<T, T, bool>
        {
    public:
        inline bool operator()(const T& a_, const T& b_) const
            {
            return (string_util::stricmp(a_.c_str(), b_.c_str()) == 0);
            }
        };

    template<typename T>
    class equal_string_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strcmp(a_, b_) == 0);
            }
        };

    //less functors
    template<typename T>
    class less_string_n_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        less_string_n_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strncmp(a_, b_, m_comparison_size) < 0);
            }
    private:
        size_t m_comparison_size;
        };

    template<typename T>
    class less_string_ni_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        less_string_ni_compare(size_t comparison_size) : m_comparison_size(comparison_size) {}
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strnicmp(a_, b_, m_comparison_size) < 0);
            }
    private:
        size_t m_comparison_size;
        };

    template<typename T>
    class less_string_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::stricmp(a_, b_) < 0);
            }
        };

    template<typename T>
    class less_string_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strcmp(a_, b_) < 0);
            }
        };

    template<typename T>
    class less_basic_string_compare
        : public std::binary_function<T, T, bool>
        {
    public:
        inline bool operator()(const T& a_, const T& b_) const
            {
            return (string_util::strcmp(a_.c_str(), b_.c_str()) < 0);
            }
        };

    template<typename T>
    class less_string_natural_order_i_compare
        : public std::binary_function<const T*, const T*, bool>
        {
    public:
        inline bool operator()(const T* a_, const T* b_) const
            {
            return (string_util::strnatordncasecmp(a_, b_) < 0);
            }
        };

    ///trims whitespace from around a string
    template<typename char_typeT>
    class string_trim
        {
    public:
        const char_typeT* operator()(const char_typeT* value,
                                     size_t length = std::basic_string<char_typeT>::npos)
            {
            m_trimmed_string_length = 0;
            if (value == NULL)
                { return NULL; }
            if (length == 0)
                { return value; }
            if (length == std::basic_string<char_typeT>::npos)
                { length = string_util::strlen(value); }
            const char_typeT* start = value;
            //end is last valid character in the string, not the one after it
            const char_typeT* end = value+(length-1);
            while (start && (start <= end))
                {
                if (is_space(start[0]))
                    { ++start; }
                else
                    { break; }
                }
            while (end > start)
                {
                if (is_space(end[0]))
                    { --end; }
                else
                    { break; }
                }
            //if start overran end then this string was all spaces.
            m_trimmed_string_length = (start > end) ? 0 : (end-start)+1;
            return start;
            }
        size_t get_trimmed_string_length() const
            { return m_trimmed_string_length; }
    private:
        size_t m_trimmed_string_length;
        };

    /** \addtogroup StringOperations
    * Classes for string operations.
    * @{*/
    /**
    @class string_tokenize
    @brief Tokenizes a string using a set of delimiters.
    @date 2010
    */
    /** @} */
    template<typename T>
    class string_tokenize
        {
    public:
        ///Constructor which takes the string to parse and the delimiters to use.
        ///@param val The string to parse.
        ///@param delim The set of delimiters to separate the string.
        string_tokenize(const T& val, const T& delim) :
            m_value(val), m_start(NULL), m_next_delim(NULL), m_delim(delim), m_has_more_tokens(true)
            {
            m_start = m_value.c_str();
            m_next_delim = string_util::strcspn_pointer(m_start, m_delim.c_str(), m_delim.length());
            }
        ///@returns Whether or not there are more tokens in the string.
        inline bool has_more_tokens() const
            { return m_has_more_tokens; }
        ///@returns Whether or not there are more delimiters in the string.
        ///This is useful for seeing if there are any delimiters at all when first loading the string.
        inline bool has_more_delimiters() const
            { return (m_next_delim != NULL); }
        ///@returns The next token from the original string as a string object
        ///Note that empty tokens can be returned if there is proceeding or trailing
        ///delimiters in the string, or if there are repeated delimiters next to each other.
        inline T get_next_token()
            {
            if (m_next_delim)
                {
                const wchar_t* current_start = m_start;
                const wchar_t* current_next_delim = m_next_delim;
                //move the pointers to the next token
                m_start = ++m_next_delim;
                m_next_delim = string_util::strcspn_pointer(m_start, m_delim.c_str(), m_delim.length());
                return T(current_start, current_next_delim-current_start);
                }
            //no more delims means that we are on the last token
            else if (m_start)
                {
                m_has_more_tokens = false;
                const wchar_t* current_start = m_start;
                m_start = NULL;
                return T(current_start);
                }
            //if called when there are no more tokens, then return an empty string
            else
                {
                m_has_more_tokens = false;
                return T();
                }
            }
    private:
        string_tokenize() {}
        string_tokenize(const string_tokenize&) {}

        T m_value;
        const wchar_t* m_start;
        const wchar_t* m_next_delim;
        T m_delim;
        bool m_has_more_tokens;
        };

    ///Removes all whitespace from a string
    template<typename T>
    inline T remove_all_whitespace(const T& text)
        {
        T tempText = text;
        for (typename T::size_type i = 0; i < tempText.length(); /*in loop*/)
            {
            if (tempText[i] == 10 || tempText[i] == 13 || tempText[i] == 9)
                { tempText.erase(i,1); }
            else
                { ++i; }
            }
        return tempText;
        }

    ///Removes all instances of a character from a string.
    template<typename Tchar_type, typename T>
    inline void remove_all(T& text, Tchar_type char_to_replace)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(char_to_replace, start);
            if (start == T::npos)
                { return; }
            text.erase(start,1);
            }
        }

    ///helper functions for stemmers
    template<typename Tchar_type, typename T>
    inline void replace_all(T& text, Tchar_type text_to_replace, Tchar_type replacement_text)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text[start++] = replacement_text;
            }
        }

    template<typename T, typename Tchar_type>
    inline void replace_all(T& text, const Tchar_type* text_to_replace, const Tchar_type* replacement_text)
        {
        size_t start = 0;
        const size_t textToReplaceLength = string_util::strlen(text_to_replace);
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, textToReplaceLength, replacement_text);
            }
        }

    template<typename T>
    inline void replace_all(T& text, const T& text_to_replace, const T& replacement_text)
        {
        size_t start = 0;
        while (start != T::npos)
            {
            start = text.find(text_to_replace, start);
            if (start == T::npos)
                {
                return;
                }
            text.replace(start, text_to_replace.length(), replacement_text);
            }
        }

    /**Strips extraneous spaces/tabs/carriage returns from a block of text so
    that there isn't more than one space consecutively.*/
    template<typename string_typeT>
    size_t remove_extra_spaces(string_typeT& Text)
        {
        size_t numberOfSpacesRemoved = 0;

        if (!Text.length() )
            { return 0; }
        bool alreadyHasSpace = true;
        //make sure that there is only a space between each word
        for (unsigned int i = 0; i < Text.length(); ++i)
            {
            //if this is the first space found after the current
            //word then it's OK--just leave it
            if (is_space(static_cast<wchar_t>(Text[i])) && !alreadyHasSpace)
                { alreadyHasSpace = true; }
            //this is extra space right after another--get rid of it
            else if (is_space(static_cast<wchar_t>(Text[i])) && alreadyHasSpace)
                {
                //make sure it isn't a Windows \r\n
                if (i && !(Text[i-1] == 13 && Text[i] == 10))
                    {
                    ++numberOfSpacesRemoved;
                    Text.erase(i--,1);
                    }
                }
            //we are starting another word--reset
            else if (!is_space(static_cast<wchar_t>(Text[i])))
                { alreadyHasSpace = false; }
            }

        return numberOfSpacesRemoved;
        }

    /**Removes blank lines from a block of text
    @param Text The text to have blank lines removed from.
    @return Number of characters (not lines) removed from the block.*/
    template<typename string_typeT>
    size_t remove_blank_lines(string_typeT& Text)
        {
        size_t numberOfLinesRemoved = 0;

        if (!Text.length() )
            { return 0; }
        bool alreadyHasNewLine = true;
        //make sure that there is only a space between each word
        for (unsigned int i = 0; i < Text.length(); ++i)
            {
            //if this is the first space found after the current
            //word then it's OK--just leave it
            if (is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13) && !alreadyHasNewLine)
                { alreadyHasNewLine = true; }
            //this is extra space right after another--get rid of it
            else if (is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13) && alreadyHasNewLine)
                {
                //make sure it isn't a Windows \r\n
                if (i && !(Text[i-1] == 13 && Text[i] == 10))
                    {
                    ++numberOfLinesRemoved;
                    Text.erase(i--,1);
                    }
                }
            //we are starting another word--reset
            else if (!is_either<wchar_t>(static_cast<wchar_t>(Text[i]), 10, 13))
                { alreadyHasNewLine = false; }
            }

        return numberOfLinesRemoved;
        }

    /**Converts strings to double values, but also takes into account ranges (returning the average). For example, a string
    like "5-8" will return 6.5. Hyphens and colons are seen as range separators.*/
    template<typename Tchar_type>
    inline double strtod_ex(const Tchar_type* nptr, Tchar_type** endptr)
        {
        if (nptr == NULL)
            {
            *endptr = NULL;
            return 0.0f;
            }
        wchar_t sepStr[3] = { 0x2D, 0x3A, 0 };
        const Tchar_type* separator = string_util::strcspn_pointer<Tchar_type>(nptr, sepStr, 2);
        //if there is no hyphen or there is one but it is at the end then just call strtod
        if (separator == NULL || *(separator+1) == 0)
            { return string_util::strtod(nptr, endptr); }
        else
            {
            const double d1 = string_util::strtod(nptr, endptr);
            const double d2 = string_util::strtod(++separator, endptr);
            return safe_divide<double>(d1+d2, 2);
            }
        }

    template<typename Tchar_type>
    inline bool is_one_of(const Tchar_type character, const Tchar_type* char_string)
        {
        while(*char_string)
            {
            if (character == char_string[0])
                {
                return true;
                }
            ++char_string;
            }
        return false;
        }
    }

#endif //__STRING_UTIL_H__
