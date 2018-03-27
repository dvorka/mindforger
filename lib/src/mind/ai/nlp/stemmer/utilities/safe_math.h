/**@addtogroup Mathematics
   @brief Math and statistics classes.
   @date 2004-2015
   @copyright Oleander Software, Ltd.
   @author Oleander Software, Ltd.
   @details This program is free software; you can redistribute it and/or modify
    it under the terms of the BSD License.
* @{*/

#ifndef __SAFE_MATH_H__
#define __SAFE_MATH_H__

#include <cmath>
#include <cassert>
#include <functional>

//DIVISION OPERATIONS
//-------------------

///@brief Modulus operation that checks for modulus by zero or into zero (returns zero for those situations).
///@param dividend The dividend (i.e., the value being divided).
///@param divisor The divisor (i.e., the value dividing by).
///@returns The remainder of the modulus operation, or zero if one of the values was invalid.
template<typename T>
inline T safe_modulus(const T dividend, const T divisor)
    {
    if (dividend == 0 || divisor == 0)
        { return 0; }
    return dividend%divisor;
    }

///@brief Division operation that checks for division by zero or into zero (returns zero for those situations).
///@param dividend The dividend (i.e., the value being divided).
///@param divisor The divisor (i.e., the value dividing by).
///@returns The quotient of the division operation, or zero if one of the values was invalid.
///@note If the template type has floating point precision, then the result will retain its precision.
template<typename T>
inline T safe_divide(const T dividend, const T divisor)
    {
    if (dividend == 0 || divisor == 0)
        { return 0; }
    return dividend/static_cast<T>(divisor);
    }

//DOUBLE OPERATIONS
//-----------------

/**@brief Compares two double values (given the specified precision).
   @param actual The value being reviewed.
   @param expected The expected value to compare against.
   @param delta The tolerance of how different the values can be. The larger the delta, the
   higher precision used in the comparison.
   @returns True if the value matches the expected value.*/
inline bool compare_doubles(const double actual, const double expected, const double delta = 1e-6)
    {
    assert(delta >= 0 && "delta value should be positive when comparing doubles");
    return (std::fabs(actual-expected) <= std::fabs(delta));
    }

/**@brief Compares two double values for less than (given the specified precision).
   @param left The value being reviewed.
   @param right The other value to compare against.
   @param delta The tolerance of how different the values can be. The larger the delta, the
   higher precision used in the comparison.
   @returns True if the value is less than the other value.*/
inline bool compare_doubles_less(const double left, const double right, const double delta = 1e-6)
    {
    assert(delta >= 0 && "delta value should be positive when comparing doubles");
    return std::fabs(left-right) > std::fabs(delta) && (left < right);
    }

/**@brief Compares two double values for less than or equal to (given the specified precision).
   @param left The value being reviewed.
   @param right The other value to compare against.
   @param delta The tolerance of how different the values can be. The larger the delta, the
   higher precision used in the comparison.
   @returns True if the value is less than or equal to the other value.*/
inline bool compare_doubles_less_or_equal(const double left, const double right, const double delta = 1e-6)
    {
    assert(delta >= 0 && "delta value should be positive when comparing doubles");
    return compare_doubles_less(left,right,delta) || compare_doubles(left,right,delta);
    }

/**@brief Compares two double values for greater than (given the specified precision).
   @param left The value being reviewed.
   @param right The other value to compare against.
   @param delta The tolerance of how different the values can be. The larger the delta, the
   higher precision used in the comparison.
   @returns True if the value is greater than the other value.*/
inline bool compare_doubles_greater(const double left, const double right, const double delta = 1e-6)
    {
    assert(delta >= 0 && "delta value should be positive when comparing doubles");
    return std::fabs(left-right) > std::fabs(delta) && (left > right);
    }

///@brief "less" interface for double values.
class double_less : public std::binary_function<double, double, bool>
    {
public:
    inline bool operator()(const double& left, const double& right) const
        { return compare_doubles_less(left,right); }
    };

//INTEGER OPERATIONS
//------------------

///@brief Converts an integral type to a boolean. Compilers complain about directly assigning
///an int to a bool (casting doesn't help either), so this works around that.
///@param intVal The integer value to convert to a boolean.
///@returns The boolean equivalent of the integer.
template<typename T>
inline bool int_to_bool(const T intVal)
    { return (intVal != 0); }

#endif //__SAFE_MATH_H__
