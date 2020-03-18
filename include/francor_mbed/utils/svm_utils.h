/**
 * @file svm_utils.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Utils for generation of values necessary for space vector modulation
 *
 * @version 1.0
 * @date 2020-03-17
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 *
 */

#pragma once

#include <francor_mbed/utils/utils.h>
#include <francor_mbed/utils/qvariable.h>
#include <cmath>

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
 * @brief Space Vector Modulation PWM Lookup Table
 * 
 * @tparam 60u Number of values in lookup table
 * 
 * # Overview
 * 
 * This table generates all necessary duty cycle values for SVM pwm generation.
 * The PWM duty cycle is controlled via the CCR register values, which are stored
 * in this lookup table. The ccr values are calculated for angles between [0; 60] degree
 * following this formula:
 * 
 * ```
 * _ccr_lut[0:N-1] = ccr_limit * sin(60° - (60°/N) * [0:N-1]);
 * ```
 * The precision of the table can be controlled via the template parameter N.
 * At default for every degree one ccr value is stored (N = 60).
 * 
 * # Usage
 * 
 * Use this function only as constexpr class to generate a lookup table at compile
 * time!
 * 
 * ```
 * constexpr SVMPWMLUT<60> lut;
 * 
 * const uint16_t ccr_at_33deg = lut[33];
 * ```
 * 
 * 
 */
template <uint16_t NumValues = 60u, uint16_t CCRMax = 1000u>
class SVMPWMLUT
{
public:
  
  /**
   * @brief Constexpr constructor for lookup table
   * 
   * @param ccr_limit Maximum value the CCR register can reach
   */
  constexpr SVMPWMLUT(void) {
    // Calculate angle step per value in rad
    const double angle_inc = (60.0 / static_cast<double>(NumValues)) * (M_PI / 180.0);
    const double ccr_limit_d = static_cast<double>(CCRMax);

    // Generate lookup table
    for(auto idx = 0u; idx < NumValues; idx++) {

      const double angle_rad = (M_PI / 3.0) - angle_inc * static_cast<double>(idx);
      const double ccr_d = round(ccr_limit_d * sin(angle_rad));

      _ccr_lut[idx] = static_cast<uint16_t>(ccr_d);
    }
  }

  /**
   * @brief Access operator to get value from LUT
   * 
   * @param idx Index or angle of value (Take care value is < N)
   * @return constexpr uint16_t CCR value in lookup table
   */
  inline const uint16_t operator[] (const uint16_t idx) const { return _ccr_lut[idx]; }

private:

  uint16_t  _ccr_lut[NumValues] = {0};  //!< Array containing all data (lookup array)
};

/**
 * @brief SVPWM Support Class
 * 
 * @tparam AnglePrec Defines the precision of the lookup table
 * @tparam CCRMax Maximum value of CCR register
 * 
 * # Overview
 * 
 * This class calculates the weights of the different PWM channels
 */
template<uint16_t AnglePrec = 8u, uint16_t CCRMax = 1000u>
class SVMPWM
{
public:

  SVMPWM(void)
  {

  }

  /**
   * @brief Move electrical angle by delta value
   * 
   * @param delta value to add to angle
   */
  void move(const int16_t delta) 
  {
    _angle +=  delta;

    if(_angle >= _angle_max) {
      _angle -= _angle_max;
    }
    else if(_angle < 0) {
      _angle += _angle_max;
    }

    _sec_angle = static_cast<uint16_t>(_angle & _sec_angle_mask);
    _actv_sec  = static_cast<uint8_t>(_angle >> _sec_bit_pos);
  }

  void updateCCRValues(void)
  {
    const uint16_t ccr_a = _lut[_sec_angle];
    const uint16_t ccr_b = _lut[_num_angles - _sec_angle - 1];
    const uint16_t ccr_ab = ccr_a + ccr_b;

    uint16_t ccr_0 = 0u;
    if(ccr_ab < CCRMax) {
      ccr_0 = (CCRMax - ccr_ab) >> 1u;
    }


    switch(_actv_sec)
    {
      case 0:
      {
        _ccr_chn1 = ccr_0;
        _ccr_chn2 = ccr_0 + ccr_b;
        _ccr_chn3 = ccr_0 + ccr_a + ccr_b;
      }break;

      case 1:
      {
        _ccr_chn1 = ccr_0;
        _ccr_chn2 = ccr_0 + ccr_a + ccr_b;
        _ccr_chn3 = ccr_0 + ccr_a;
      }break;

      case 2:
      {
        _ccr_chn1 = ccr_0 + ccr_b;
        _ccr_chn2 = ccr_0 + ccr_a + ccr_b;
        _ccr_chn3 = ccr_0;
      }break;

      case 3:
      {
        _ccr_chn1 = ccr_0 + ccr_a + ccr_b;
        _ccr_chn2 = ccr_0 + ccr_a;
        _ccr_chn3 = ccr_0;
      }break;

      case 4:
      {
        _ccr_chn1 = ccr_0 + ccr_a + ccr_b;
        _ccr_chn2 = ccr_0;
        _ccr_chn3 = ccr_0 + ccr_b;
      }break;

      case 5:
      {
        _ccr_chn1 = ccr_0 + ccr_a;
        _ccr_chn2 = ccr_0;
        _ccr_chn3 = ccr_0 + ccr_a + ccr_b;
      }break;
    }
  }

  /* Getters */
  const int16_t   getAngle(void) const {return _angle;}
  const uint8_t   getActvSec(void) const {return _actv_sec;}
  const uint16_t  getSecAngle(void) const {return _sec_angle;}

  const uint16_t getCCRChn1(void) const {return _ccr_chn1;}
  const uint16_t getCCRChn2(void) const {return _ccr_chn2;}
  const uint16_t getCCRChn3(void) const {return _ccr_chn3;}

#ifndef BUILD_TESTS
private:
#endif

  /** \brief Number of angles in LUT */
  static constexpr uint16_t _num_angles = (1u << AnglePrec);

  /** \brief Position of sector angle in electrical angle */
  static constexpr uint8_t _sec_bit_pos = AnglePrec;

  /** \brief Mask to get sectur angle */
  static constexpr int16_t _sec_angle_mask = _num_angles - 1;

  /** \brief Maximum value of electrical angle */
  static constexpr int16_t _angle_max = _num_angles + (5u << AnglePrec);

  /** \brief Lookuptable generated at compile time */
  static constexpr inline SVMPWMLUT<_num_angles, CCRMax> _lut = {};

  int16_t   _angle = 0;       //!< Active electrical angle
  uint8_t   _actv_sec = 0u;   //!< Active sector
  uint16_t  _sec_angle = 0u;  //!< Angle in sector

  uint16_t _ccr_chn1 = 0u; //!< CCR value of channel 1
  uint16_t _ccr_chn2 = 0u; //!< CCR value of channel 2
  uint16_t _ccr_chn3 = 0u; //!< CCR value of channel 3
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