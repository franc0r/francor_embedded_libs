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
 * @tparam Q Fixed point position
 */
template<typename DataType = std::uint32_t, std::size_t Q = 14>
class QVariable
{
public:

  /** \brief Default constructor */
  QVariable() {}

  /** \brief Create Q-value from floating point value */
  QVariable(const double v) :
    _raw_value(static_cast<DataType>(v / _accuracy)) {}

  /* Get and typecast operators */

  /** \brief Operator: Get raw value */
  operator const DataType() const {return _raw_value;}

  /** \brief Operator: Get floating point value */
  operator const double() const {return static_cast<double>(_raw_value) * _accuracy;}

  /* Arithmetic operators */
  
  QVariable operator + (const DataType rhs) {
    QVariable res;
    res._raw_value = this->_raw_value + rhs;
    return res;
  }

  QVariable& operator += (const DataType rhs) {
    this->_raw_value += rhs;
    return *this;
  }

  QVariable operator - (const DataType rhs) {
    QVariable res;
    res._raw_value = this->_raw_value - rhs;
    return res;
  }

  QVariable& operator -= (const DataType rhs) {
    this->_raw_value -= rhs;
    return *this;
  }

  QVariable operator * (const DataType rhs) {
    QVariable res;
    res._raw_value = (this->_raw_value * rhs) >> Q;
    return res;
  }

  QVariable& operator *= (const DataType rhs) {
    this->_raw_value *= rhs;
    this->_raw_value = (this->_raw_value >> Q);
    return *this;
  }


  

#ifndef BUILD_TESTS
private:
#endif

  /** \brief Raw representation of fixed point value */
  DataType  _raw_value  = 0;

  /** \brief Calculated accuracy of fixed point value */
  static constexpr double _accuracy = (1.0 / static_cast<double>(1 << Q));
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