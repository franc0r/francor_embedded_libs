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
 * @file svm_lut.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Lookup tables for space vector modulation
 *
 * @version 1.0
 * @date 2020-03-17
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
 * @brief Space Vector Modulation Lookup Table for CCR Registers in ROM
 * 
 * @tparam BitPrecision Bit precision of the table. The table will store 2^BitPrecision + 1 entries
 * @tparam CCRMax Maximum value to set in the campture compare register 
 * 
 * # Overview
 * 
 * If you want to perform space vector modulation to control brushless dc drives
 * you need to calculate trigonometric functions, especially sinus, at a very high
 * frequency and mostly with no FPU (floating point unit) support. 
 * To take care of this problem, this constexpr class calculates the necessary values
 * of the campture compare registers at compile time and stores the values into the ROM area.
 * 
 * Space vector modulation has to provide sinus values between [0;60] degrees to scale the
 * duty cycles correctly. In general you scale between two vectors:
 * 
 * ```
 * ccr_scale_a = ccr_max * (2.0 / sqrt(3.0)) * m * sin(60° - electrical_angle);
 * ccr_scale_b = ccr_max * (2.0 / sqrt(3.0)) * m * sin(electrical_angle);
 * ccr_scale_0 = (ccr_max - ccr_scale_a - ccr_scale_b) / 2.0;
 * 
 * ccr_max: Maximum CCR value
 * m: Modulation factor [0;  (sqrt(3.0) / 2.0)]
 * ```
 * 
 * As seen in the calculation above you can store the ccr scaling values in a lookup table
 * with the following formula. (Example with a precision of 2 bits = 4 values).
 * To avoid using the sqrt(3.0) / 2.0 limitation we remove the (2.0 / sqrt(3.0)) part.
 * This means that a modulation value m = 1 equals (sqrt(3.0) / 2.0) in the above equation.
 * ```
 * N = 4
 * ccr_max = 1000
 * angle_step = 60.0 / 4.0 = 15 deg/inc
 * 
 * lut_value[0:N+1] = round(ccr_max * sin(60 - angle_step * [0:N+1]))
 * 
 * lut_value[0] = 866
 * lut_value[1] = 707
 * lut_value[2] = 500
 * lut_value[3] = 258
 * lut_value[4] = 0
 * ```
 * 
 * You recognize that we need one additional value in the list to be able to lookup both scales in one table:
 * 
 * ```
 * lut_value[0] = 866
 * lut_value[1] = 707
 * lut_value[2] = 500
 * lut_value[3] = 258
 * lut_value[4] = 0
 * 
 * // Get values for angle 0°
 * ccr_scale_a[0°] = lut_value[0]     = 866
 * ccr_scale_b[0°] = lut_value[N - 0] = 0
 * 
 * // Get values for angle 15°
 * ccr_scale_a[15°] = lut_value[1]     = 707
 * ccr_scale_b[15]  = lut_value[N - 1] = 258
 * 
 * 
 * ```
 * 
 * If you want to get more information about SVM modulation and the calculations behind that
 * please have a look at these webpages:
 * https://www.switchcraft.org/learning/2017/3/15/space-vector-pwm-intro
 * https://www.st.com/content/ccc/resource/technical/document/application_note/a3/96/99/cd/64/2b/49/5e/CD00055518.pdf/files/CD00055518.pdf/jcr:content/translations/en.CD00055518.pdf
 * 
 * # Usage
 * 
 * The usage of the class is pretty straight forward:
 * 
 * ```
 * // In this example a lookup table is stored into ROM
 * // with 8 bit precision (= 257 entries)
 * constexpr SVMLUTROM<8u, 1000> lut_rom = {};
 * 
 * // Access data - WARNING: Read from ROM is slow! Use SVMLUTRAM for
 * // high speed applications!
 * // In this example the data is converted to double data type to be able to 
 * // set a smaller modulation value. Remember that m = 1.0 is equal to (srt(3)/2)!
 * const double m = 0.1;
 * const double ccr_scale_a = static_cast<double>(lut_rom[5]) * m;
 * const double ccr_scale_b = static_cast<double>(lut_rom[lut_rom.getNumEntries() - 1 - 5]) * m;
 * 
 * ```
 * 
 * WARNING: Read data from ROM is very slow even in MCUs! Use SVMLUTRAM for fast access.
 * 
 */
template<uint16_t BitPrecision = 8u, uint16_t CCRMax = 1000u>
class SVMLUTROM
{
public:

  /**
   * @brief Constructor create lookup table for ROM
   */
  constexpr SVMLUTROM() 
  {
    const double angle_step_rad = (60.0 / static_cast<double>(_num_entries - 1))
                                  * (M_PI / 180.0);
    const double ccr_max = static_cast<double>(CCRMax);

    for(auto idx = 0u; idx < _num_entries; idx++) 
    {
      const double angle_rad = (M_PI / 3.0) - angle_step_rad
                                * static_cast<double>(idx);
      
      const double ccr_entry = round(ccr_max * sin(angle_rad));

      _entry_list[idx] = ccr_entry;
    }
  }

  /**
   * @brief Access operator to read lookup table entries
   * 
   * @param idx Index of entry directly related to angle
   * @return const uint16_t Value stored in lookup table at desired index/angle
   */
  inline const uint16_t operator[] (const uint16_t idx) const {return _entry_list[idx];}

  /**
   * @brief Get the number of entries in the lookup table
   * 
   * @return const uint16_t Number of entries
   */
  const uint16_t getNumEntries() const {return _num_entries;}

private:

  /** \brief Number of entries in ROM table */
  static const uint16_t _num_entries = (1u << BitPrecision) + 1;

  /** \brief Lookup table */
  uint16_t _entry_list[_num_entries] = {0};
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