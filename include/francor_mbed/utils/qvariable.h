/*
BSD 3-Clause License

Copyright (c) 2020, Franconian Open Robotics
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file qvariable.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Fixed-Point variable class header
 *
 * @version 1.0
 * @date 2020-03-13
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg - BSD 3-Clause License
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
template<typename DataType = std::int32_t, std::size_t NumFracBits = 14>
class QVariable
{
public:

  /** \brief Default constructor */
  QVariable() {}

  /**
   * @brief Construct a new q-variable with raw value
   * 
   * @param v Value of variable in raw fixed point representation
   */
  QVariable(const DataType v) :
    _raw_value(v) {}

  /**
   * @brief Construct a new q-variabel from different scaled q-variable raw values
   * 
   * @param v Raw value of variable
   * @param num_frac_bits Number of fractional bits of variable
   */
  QVariable(const DataType v, const std::size_t num_frac_bits)
  {
    if(_num_frac_bits >= num_frac_bits) 
    {
      _raw_value = static_cast<DataType>(v) << (_num_frac_bits - num_frac_bits);
    }
    else
    {
      _raw_value = static_cast<DataType>(v) >> (num_frac_bits - _num_frac_bits);
    }
  }

  /**
   * @brief Construct a new q-variable with floating point value
   * 
   * @param v Value of variable in floating point value
   */
  QVariable(const double v) :
    _raw_value(static_cast<DataType>(v / _precision)) {}

  /**
   * @brief Set the raw fixed point value
   * 
   * @param v Raw fixed point value to set
   */
  void setRawValue(const DataType v)
  {
    _raw_value = v;
  }

  /**
   * @brief Converts another fixed point value to this representation
   * 
   * @tparam T QVariable class
   * @param v Variable
   */
  template<typename T>
  void convert(const T& v)
  {
    if(_num_frac_bits >= v.getNumFracBits())
    {
      _raw_value = static_cast<DataType>(v.getRawValue()) << (_num_frac_bits - v.getNumFracBits());
    }
    else
    {
      _raw_value = static_cast<DataType>(v.getRawValue()) >> (v.getNumFracBits() - _num_frac_bits);
    }
  }

  /* Get and typecast operators */

  /**
   * @brief Get the number of fractional bits 
   * 
   * @return const std::size_t Number of fractional bits
   */
  static const std::size_t getNumFracBits(void) {return _num_frac_bits;}

  /**
   * @brief Get the precision of the fixed point value
   * 
   * @return const double Precision as floating point value
   */
  static const double getPrecision(void) {return _precision;}

  /**
   * @brief Get the raw fixed point value of the variable
   * 
   * @return const DataType Raw value of variable
   */
  const DataType getRawValue(void) const {return _raw_value;}

  /** \brief Operator: Get decimal value (explicit cast) */
  explicit operator DataType() const {
    const QVariable bias(0.5);
    return (_raw_value + bias._raw_value) >> _num_frac_bits;
   }

  /** \brief Operator: Get floating point value float */
  operator const float() const {return static_cast<float>(_raw_value) 
                                         * static_cast<float>(_precision);}

  /** \brief Operator: Get floating point value double */
  operator const double() const {return static_cast<double>(_raw_value) * _precision;}

  /** \brief Operator: Set fxp value from another fxp variable */
  QVariable& operator = (QVariable const v)
  {
    _raw_value = v._raw_value;
    return *this;
  }

  /* Arithmetic operators between FXP variables */

  QVariable operator + (QVariable const& rhs) const
  {
    return QVariable(static_cast<DataType>(this->_raw_value + rhs._raw_value));
  } 

  QVariable operator - (QVariable const& rhs) const
  {
    return QVariable(static_cast<DataType>(this->_raw_value - rhs._raw_value));
  }

  QVariable operator * (QVariable const& rhs) const
  {
    return QVariable(static_cast<DataType>((this->_raw_value * rhs._raw_value) >> NumFracBits));
  }

  QVariable operator / (QVariable const& rhs) const
  {
    return QVariable(static_cast<DataType>((this->_raw_value << NumFracBits) / rhs._raw_value));
  }

  QVariable operator % (QVariable const& rhs) const
  {
    return QVariable(static_cast<DataType>(this->_raw_value % rhs._raw_value));
  }

  /* Compare operators */

  bool operator == (QVariable const& rhs) const
  {
    return (this->_raw_value == rhs._raw_value);
  }

  bool operator != (QVariable const& rhs) const
  {
    return (this->_raw_value != rhs._raw_value);
  }

  bool operator >= (QVariable const& rhs) const
  {
    return (this->_raw_value >= rhs._raw_value);
  }

  bool operator <= (QVariable const& rhs) const
  {
    return (this->_raw_value <= rhs._raw_value);
  }

  bool operator > (QVariable const& rhs) const
  {
    return (this->_raw_value > rhs._raw_value);
  }

  bool operator < (QVariable const& rhs) const
  {
    return (this->_raw_value < rhs._raw_value);
  }
  

#ifndef BUILD_TESTS
private:
#endif

  /** \brief Raw representation of fixed point value */
  DataType  _raw_value  = 0;

  /** \brief Calculated accuracy of fixed point value */
  static constexpr double _precision = (1.0 / static_cast<double>(1 << NumFracBits));

  /** \brief Store number of fractional bits */
  static constexpr std::size_t _num_frac_bits = NumFracBits;
};


/**
 * @brief Default fixed point data type
 */
typedef QVariable<int32_t, 10u> fxp;

 /**
  * @}
  */ // Utils
/*--------------------------------------------------------------------------------*/

}; /* francor_mbed */

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/
