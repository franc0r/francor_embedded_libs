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
 * @file svm_handler.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Space Vector Modulation Handler Class
 *
 * @version 1.0
 * @date 2020-03-20
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg - BSD 3-Clause License
 *
 */

#pragma once

#include <francor_mbed/utils/utils.h>
#include <francor_mbed/utils/svm_lut.h>
#include <francor_mbed/utils/qvariable.h>

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
 * @brief Space Vector Modulation Handler for driving BLDC/EC drives
 * 
 * @tparam BitPrecision Bit precision of the lookup table
 * @tparam CCRMax Maximum value to set in the campture compare register 
 * 
 * # Overview
 * 
 * Space vector modulation is necessary if you want to drive an BLDC drive with
 * high efficiency and smoother control. It is a special modulation principle to
 * apply a desired voltage to a three phased electric bldc drive. You can get a
 * very detailed explanation of the principles and calculations behind space vector
 * modulation at the following webpage. I would recommend to read this page, because
 * otherwise its hard to understand the following explanations.
 * 
 * https://www.switchcraft.org/learning/2017/3/15/space-vector-pwm-intro
 * 
 * To sum up: A BLDC motor is driven by a magnetic field generated by commutation.
 * The magnetic field is generated by driving the six MOSFETS and the position or angle 
 * of the electric field is specified. At optimum, the electric or magnetic field runs 90° ahead 
 * of the rotor (mechanical angle) to generate the maximum torque.
 * 
 * This class helps generating the electrical angle via SVM (space vector modulation). 
 * You can specify a desired electrical angle and the class will calculate the capture compare register
 * values to generate the desired angle in the drive.
 * 
 * To make it possible to calculate the values even on small microcontrollers, a few tricks are used.
 * First, the class uses a lookup table, which is already provided by the class @ref SVMLUTRAM. 
 * This saves the calculation of sine values.
 * 
 * Second, to save the current angle, an integer variable is used to take advantage of the 
 * overflow property.
 * In SVM there are eight switching combinations to generate the electric field, 
 * where two switching states produce no current flow 
 * (all high transistors on or all low transistors on). The additional six switching states divide 
 * the 360° circle into six sectors. For example, if you want to generate the electrical angle 73°, 
 * you are in the second sector at 13°.
 * 
 * ```
 * // Calculation for 73° electrical angle
 * Angle-Sector = 73° mod 60° = 13 °
 * Sector = floor(73° / 60°) = 1
 * 
 * // Calculation for 236° electrical angle
 * Angle-Sector = 236° mod 60° = 56 °
 * Sector = floor(73° / 60°) = 3
 * ```
 * 
 * To come back to the trick with the integer overflow. Lets say we will set a bit precision
 * of the electrical angle of 8 bits for every sector. This means we have a resolution of
 * 0.234°
 * 
 * ```
 * Angle-Resolution = 60° / 2^BitPrecision = 60° / 2^8 = 0.234°
 * ```
 * 
 * If we now want to move the electrical angle we will just increment or decrement
 * one value:
 * 
 * ```
 * Move forward: elec_angle ++;
 * Move backward: elec_angle --;
 * ```
 * 
 * In our case this will step the electrical angle by 0.234° for every increment.
 * If we now want to get the active sector, but want to save the computation time
 * for the modolus we can perform the following trick:
 * 
 * ```
 * // Current electrical angle is 384 = 384 * 0.234° = 89.856°
 * uint16_t elec_angle = 384
 * 
 * // First 8 bit save the 60° angle information
 * uint16_t elec_angle_sector = elec_angle & 0xFF = 128
 * 
 * // 3 bits after the angle store the sector information
 * uint8_t  elec_actv_sector  = (elec_angle >> 8) & 0x07 = 1
 * 
 * ```
 * 
 * If we use this feature it means we can just increment the electrical angle
 * and the sector angle and active sector can easily be extracted. Due to the
 * relation to bit precision the variable just overflows and starts again at the angle
 * 0° if the limit is reached.
 * 
 * The rotation or switching order by running in positive direction is as followed.
 * The coding is as followed UVW = 001 (U - low, V - low and W - high):
 * 
 * -  0 0 1
 * -  0 1 1
 * -  0 1 0
 * -  1 1 0
 * -  1 0 0
 * -  1 0 1
 * 
 * Information: All functions are developed for a center aligned PWM mode. The advantage is
 * that you reduce the switching times of the MOSFETs.
 * Take care to config your PWM generator in Center-Aligned Mode! Means that the timer counter
 * will count up and down. The timer starts counting down when the reload value has been reached.
 * https://microchipdeveloper.com/pwr3101:pwm-edge-center-aligned-modes
 * 
 * # Usage
 * 
 * The usage of the SVM handler is pretty simple. The following code shows a example implementation.
 * This functions have been tested with STMicroelectronics hardware. You can use these functions for
 * example on the dev boards:STSPIN3201, B-G431B-ESC or IHMXX.
 * 
 * 
 * ```
 * // Global instance of the svm handler with 8 bit precision
 * // and a maximum duty cycle value of 1000 (means 1000 = 100 % duty cycle)
 * SVMHandler<8u, 1000> g_svm_handler;
 * 
 * // This function is for example called every millisecond by a task scheduler or
 * // another function
 * void updateSVMTask(void)
 * {
 *  // Move electrical angle by 1 increment
 *  // 1 inc = 0.234 deg -> v = 234 deg/sec (when task is called every ms)
 *  g_svm_handler.move(1);
 * 
 *  // Calculate CCR values
 *  g_svm_handler.update();
 * }
 * 
 * // Interrupt function called when period of timer is elapsed
 * void TimerPeriodElapsedInterrupt(Timer* htim)
 * {
 *  // Update CCR values
 *  htim->Instance->CCR1 = g_svm_handler.getCCRChn1();
 *  htim->Instance->CCR1 = g_svm_handler.getCCRChn2();
 *  htim->Instance->CCR1 = g_svm_handler.getCCRChn3();
 * }
 * 
 * ```
 * 
 * 
 */
template<uint16_t BitPrecision = 8u, uint16_t CCRMax = 1000u>
class SVMHandler
{
public:

  /**
   * @brief Construct a new handler instance
   */
  SVMHandler()  {}

  /**
   * @brief Set the electrical angle to the desired value
   * 
   * @param elec_angle [0; 2^8 * 6]
   */
  void setElecAngle(const int32_t elec_angle)
  {
    _elec_angle = elec_angle;
    limitElecAngle();
  }

  /**
   * @brief Moves electrical angle by the desired increments
   * 
   * Moves the electrical angle by the desired increments with
   * a resolution of 1 inc = 60.0 / 2^BitPrecision
   * 
   * @param delta Value to add to electrical angle
   */
  void move(const int16_t delta)
  {
    _elec_angle += static_cast<int32_t>(delta);
    limitElecAngle();
  }

  /**
   * @brief Updates the handler
   * 
   * Calculates the necessarry duty cycles for all three channels to generate the desired
   * electrical angle
   * Note: A modulation factor of 1.0 equal (srt(3)/2) modulation. That means that the maximum
   * voltage at m = 1.0 is Vmax * (sqrt(3)/2).
   * 
   * @param m Modulation factor [0.0; 1.0]
   */
  void update(const fxp& m = fxp(1.0))
  {
    // Convert values from lookup table to fixed point representation
    fxp fxp_ccr_a(static_cast<int32_t>(_lut.getScaleA(_sector_angle)), 0u);
    fxp fxp_ccr_b(static_cast<int32_t>(_lut.getScaleB(_sector_angle)), 0u);

    // Scale ccr values with modulation factor
    fxp_ccr_a = fxp_ccr_a * m;
    fxp_ccr_b = fxp_ccr_b * m;

    // Convert back to normal representation
    const uint16_t ccr_a = static_cast<uint16_t>(static_cast<int32_t>(fxp_ccr_a));
    const uint16_t ccr_b = static_cast<uint16_t>(static_cast<int32_t>(fxp_ccr_b));

    // Calculate CCR0 value
    int32_t ccr_0 = static_cast<int32_t>(CCRMax) - static_cast<int>(ccr_a) - static_cast<int>(ccr_b);

    // Check if ccr 0 is smaller than 0 (limit value)
    if(ccr_0 < 0) {
      ccr_0 = 0;
    }
    else {
      // Due to center PWM generation divide value by 2
      ccr_0 = ((ccr_0 + 1) >> 1u);
    }

    calculateCCRChnValues(ccr_a, ccr_b, ccr_0);
  }

  /** Getters **/
  const double    getAnglePrecision() const {return _angle_precision;}

  const int32_t   getElecAngle() const {return _elec_angle;}
  const uint16_t  getSectorAngle() const {return _sector_angle;}
  const uint8_t   getSector() const {return _sector;}

  const uint16_t  getCCRChn1() const {return _ccr_chn1;}
  const uint16_t  getCCRChn2() const {return _ccr_chn2;}
  const uint16_t  getCCRChn3() const {return _ccr_chn3;}

private:

  /**
   * @brief Limitates the electrical angle and calculates sector and sector angle
   */
  void limitElecAngle(void)
  {
    // Calculate maximum allowed value for electrical angle
    constexpr int32_t elec_angle_max = static_cast<int32_t>((1u << BitPrecision) * 6);

    // Limit angle
    if(_elec_angle >= elec_angle_max) {
      _elec_angle -= elec_angle_max;
    }
    else if(_elec_angle < 0) {
      _elec_angle += elec_angle_max;
    }

    // extract sector angle and sector
    _sector_angle = static_cast<uint16_t>(_elec_angle & ((1u << BitPrecision) - 1));
    _sector = static_cast<uint8_t>((_elec_angle >> BitPrecision) & 0x07);
  }

    /**
   * @brief Assigns the calculated ccr weights to the correct ccr registers depending on
   *        the sector
   * 
   * @param ccr_a CCR channel A
   * @param ccr_a CCR channel B
   * @param ccr_a CCR channel 0
   */
  inline void calculateCCRChnValues(const uint16_t ccr_a, 
                                    const uint16_t ccr_b, 
                                    const uint16_t ccr_0)
  {
    switch(_sector)
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

  /** \brief Precision of one angle increment */
  static constexpr double _angle_precision = (60.0 / static_cast<double>(1u << BitPrecision));

  /** \brief Lookup table for ccr values from 0 to 60° */
  const SVMLUTRAM<BitPrecision, CCRMax> _lut;

  /** \brief Electrical angle in decimal representation (1 inc = 60.0° / (2^BitPrecision)) */
  int32_t _elec_angle = 0;

  /** \brief Active sector in the SVM circle [0;5] */
  uint8_t _sector = 0u; 

  /** \brief Angle relative in sector, resolution is the same as at electrical angle */
  uint16_t _sector_angle = 0u;

  uint16_t _ccr_chn1 = 0u;  //!< Capture Compare Register value for PWM channel 1
  uint16_t _ccr_chn2 = 0u;  //!< Capture Compare Register value for PWM channel 2
  uint16_t _ccr_chn3 = 0u;  //!< Capture Compare Register value for PWM channel 3
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