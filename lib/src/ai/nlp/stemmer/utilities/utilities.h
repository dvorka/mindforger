/**@addtogroup Utilities
@brief Utility classes.
@date 2003-2015
@copyright Oleander Software, Ltd.
@author Oleander Software, Ltd.
@details This program is free software; you can redistribute it and/or modify
it under the terms of the BSD License.
* @{*/

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <algorithm>
#include <functional>
#include <cmath>
#include <cassert>

///@returns The item count of an array.
///@note Do not call this on an empty array.
///Also, this is meant for arrays of intrinsic types only.
#define size_of_array(x) (sizeof(x)/sizeof(x[0]))

/**Range checks a given value and truncates it if it is too high or low.
   @param start The start of the valid range.
   @param end The end of the valid range.
   @param value The value to be range checked.
   @returns The value if within the valid range. If it was too large, then the end of the range
   is returned. If too low, then the start of the range is returned.*/
template<typename T>
inline T within_range(const T start, const T end, const T value)
    {
    return ( (value >= start) && (value <= end) ) ? value :
            (value < start) ? start :
            (value > end) ? end : /*never reaches this branch*/ value;
    }

///@returns True if a value is within a given range.
template<typename T>
inline bool is_within(const T value, const T first, const T second)
    {
    assert(first <= second);
    return (value >= first && value <= second);
    }

///Determines if a value is within a given range.
template<typename T>
class within : public std::unary_function<T, bool>
    {
public:
    /**Constructor.
    @param range_begin The beginning of the valid range.
    @param range_end The end of the valid range.*/
    within(T range_begin, T range_end)
        : m_range_begin(range_begin), m_range_end(range_end)
        {}
    /**@returns True if \ca value is within the valid range of values.
    @param value The value to review.*/
    inline bool operator()(T value) const
        { return is_within(value, m_range_begin, m_range_end); }
private:
    T m_range_begin;
    T m_range_end;
    };

///pair interface that compares on the first item
template<typename T1, typename T2>
class comparable_first_pair : public std::pair<T1,T2>
    {
public:
    comparable_first_pair() : std::pair<T1,T2>() {}
    comparable_first_pair(const T1& t1, const T2& t2) : std::pair<T1,T2>(t1,t2) {}
    bool operator<(const comparable_first_pair<T1,T2>& that) const
        { return std::pair<T1,T2>::first < that.first; }
    bool operator==(const comparable_first_pair<T1,T2>& that) const
        { return std::pair<T1,T2>::first == that.first; }
    };

///class that remembers its original value from construction.
template <typename T>
class backup_variable
    {
public:
    backup_variable(const T& value) : m_originalValue(value), m_value(value)
        {}
    void operator=(const T& value)
        { m_value = value; }
    bool operator==(const T& value) const
        { return m_value == value; }
    bool operator<(const T& value) const
        { return m_value < value; }
    bool operator<=(const T& value) const
        { return m_value <= value; }
    bool operator>(const T& value) const
        { return m_value > value; }
    bool operator>=(const T& value) const
        { return m_value >= value; }
    void operator+(const T& value)
        { m_value + value; }
    void operator+=(const T& value)
        { m_value += value; }
    void operator-(const T& value)
        { m_value - value; }
    void operator-=(const T& value)
        { m_value -= value; }
    operator const T() const
        { return m_value; }
    T* operator&()
        { return &m_value; }
    const T& get_value() const
        { return m_value; }
    T& get_value()
        { return m_value; }
    bool has_changed() const
        { return m_value != m_originalValue; }
private:
    T m_originalValue;
    T m_value;
    };

///Determines if a given value is either of two other given values
template<typename T>
inline bool is_either(const T value, const T first, const T second)
    {
    return (value == first || value == second);
    }

///Determines if a given value is neither of two other given values
template<typename T>
inline bool is_neither(const T value, const T first, const T second)
    {
    assert(first != second);
    return (value != first && value != second);
    }

/**calls a member function of elements in a container for each
element in another container*/
template<typename inT, typename outT, typename member_extract_functorT>
inline outT copy_member(inT begin, inT end, outT dest, member_extract_functorT get_value)
    {
    for (; begin != end; ++dest, ++begin)
        *dest = get_value(*begin);
    return (dest);
    }

///Copies a member value between objects based on specified criteria
template<typename inT, typename outT,
         typename _Pr,
         typename member_extract_functorT>
inline outT copy_member_if(inT begin, inT end, outT dest,
                           _Pr meets_criteria,
                           member_extract_functorT get_value)
    {
    for (; begin != end; ++begin)
        {
        if (meets_criteria(*begin))
            {
            *dest = get_value(*begin);
            ++dest;
            }
        }
    return (dest);
    }

#endif //__UTILITIES_H__
