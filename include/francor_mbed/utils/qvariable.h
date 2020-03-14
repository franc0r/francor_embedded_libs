/**
 * @file qvariable.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Fixed-Point variable class header
 *
 * @version 1.0
 * @date 2020-03-13
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 *
 */

#pragma once

#include <francor_mbed/utils/utils.h>

namespace francor_mbed
{

/*--------------------------------------------------------------------------------*/
/** @addtogroup francor_mbed
  * @{
  */

/*--------------------------------------------------------------------------------*/
/** @addtogroup Utils
  * @{
  */

/**
 * @brief QVariable - Fixed point variable representation
 * 
 * @tparam DataType DataType of the variable e.g. uint8_t, int8_t, ...
 * @tparam NumFracBits Number of fractional bits
 * 
 * # Overview
 * 
 * The QVariable class represents fixed point values and supports basic mathematical
 * computations.
 * 
 * You can specify a fixed point number by the amount of fractional bits defined via
 * the template parameter NumFracBits. If you want e.g. to specify a variable with 4 fractional
 * bits its a Q4 variable. That means that you have
 * 
 * ´´´
 * Precision = 1 / (2 ^ NumFracBits) = 2 ^ -NumFracBits
 * Precision = 1 / (2 ^ 4) = 2 ^ - 4 = 0.0625
 * ´´´
 * 
 * In that case every decimal number in the fixed point variable represents a 0.0625 step.
 * If you have for example a raw decimal value of 121 it represents 7.5625 as real value.
 * You can find an explanation of FXP number representation on Wikipedia 
 * https://en.wikipedia.org/wiki/Q_(number_format)
 * 
 * 
 */
template<typename DataType = std::uint32_t, std::size_t NumFracBits = 14>
class QVariable
{
public:

  /** \brief Default constructor */
  QVariable() {}

  /** \brief Create Q-value from floating point value */
  QVariable(const double v) :
    _raw_value(static_cast<DataType>(v / _accuracy)) {}

  /* Get and typecast operators */

  /**
   * @brief Get the number of fractional bits 
   * 
   * @return const std::size_t Number of fractional bits
   */
  static const std::size_t getNumFracBits(void) {return NumFracBits;}

  /** \brief Operator: Get raw value */
  operator const DataType() const {return _raw_value;}

  /** \brief Operator: Get floating point value */
  operator const double() const {return static_cast<double>(_raw_value) * _accuracy;}

  /* Arithmetic operators */
  
  QVariable operator + (const DataType rhs) const {
    QVariable res;
    res._raw_value = this->_raw_value + rhs;
    return res;
  }

  QVariable& operator += (const DataType rhs) {
    this->_raw_value += rhs;
    return *this;
  }

  QVariable operator - (const DataType rhs) const {
    QVariable res;
    res._raw_value = this->_raw_value - rhs;
    return res;
  }

  QVariable& operator -= (const DataType rhs) {
    this->_raw_value -= rhs;
    return *this;
  }

  QVariable operator * (const DataType rhs) const {
    QVariable res;
    res._raw_value = (this->_raw_value * rhs) >> NumFracBits;
    return res;
  }

  QVariable& operator *= (const DataType rhs) {
    this->_raw_value *= rhs;
    this->_raw_value = (this->_raw_value >> NumFracBits);
    return *this;
  }

  QVariable operator / (const DataType rhs) const {
    QVariable res;
    res._raw_value = (this->_raw_value << NumFracBits) / rhs;
    return res;
  }

  QVariable& operator /= (const DataType rhs) {
    this->_raw_value = this->_raw_value << NumFracBits;
    this->_raw_value /= rhs;
    return *this;
  }


  

#ifndef BUILD_TESTS
private:
#endif

  /** \brief Raw representation of fixed point value */
  DataType  _raw_value  = 0;

  /** \brief Calculated accuracy of fixed point value */
  static constexpr double _accuracy = (1.0 / static_cast<double>(1 << NumFracBits));
};

 /**
  * @}
  */ // Utils
/*--------------------------------------------------------------------------------*/

}; /* francor_mbed */

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/