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
 * @file svm_lut_tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Unit tests for space vector modulation lookup table
 *
 * @version 1.0
 * @date 2020-03-19
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg - BSD 3-Clause License
 *
 */

#include "francor_mbed/utils/utils.h"
#include "francor_mbed/utils/svm_lut.h"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/MemoryLeakDetectorMallocMacros.h>
#include <CppUTest/MemoryLeakDetectorNewMacros.h>

using namespace francor_mbed;

/*--------------------------------------------------------------------------------*/
/** @addtogroup francor_mbed
  * @{
  */

/*--------------------------------------------------------------------------------*/
/** @addtogroup Tests
  * @{
  */

/*--------------------------------------------------------------------------------*/
/** @addtogroup SVMLUT
  * @{
  */

TEST_GROUP(SVMLUT)
{
  void setup() {}
  void teardown() {}
};

/**
 * Test if tables are compiling and number of entries is set correctly
 */
TEST(SVMLUT, Constructor)
{
  constexpr SVMLUTROM<8u, 1000u>  svm_lut_rom1 = {};
  constexpr SVMLUTROM<9u, 1000u>  svm_lut_rom2 = {};

  CHECK_EQUAL(257u, svm_lut_rom1.getNumEntries());
  CHECK_EQUAL(513u, svm_lut_rom2.getNumEntries());
}

/**
 * Test if values in table with 8 bit precision are valid
 */
TEST(SVMLUT, ValuePrecision8BInc)
{
  constexpr uint16_t BitPrecision = 8u;
  constexpr uint16_t CCRMax = 1000u;

  constexpr SVMLUTROM<BitPrecision, CCRMax> lut = {};

  const double angle_step_rad = (60.0 / static_cast<double>(lut.getNumEntries() - 1)) 
                                    * (M_PI / 180.0);
  
  double angle_rad = (M_PI / 3.0);
  for(auto idx = 0u; idx < lut.getNumEntries(); idx++)
  {
    const double exp_entry = round(static_cast<double>(CCRMax) * sin(angle_rad));

    CHECK_EQUAL(static_cast<uint16_t>(exp_entry), lut[idx]);

    angle_rad -= angle_step_rad;
  }
}

/**
 * Test if values in table with 12 bit precision are valid
 */
TEST(SVMLUT, ValuePrecision12BInc)
{
  constexpr uint16_t BitPrecision = 12u;
  constexpr uint16_t CCRMax = 1000u;

  constexpr SVMLUTROM<BitPrecision, CCRMax> lut = {};

  const double angle_step_rad = (60.0 / static_cast<double>(lut.getNumEntries() - 1)) 
                                    * (M_PI / 180.0);
  
  double angle_rad = (M_PI / 3.0);
  for(auto idx = 0u; idx < lut.getNumEntries(); idx++)
  {
    const double exp_entry = round(static_cast<double>(CCRMax) * sin(angle_rad));

    CHECK_EQUAL(static_cast<uint16_t>(exp_entry), lut[idx]);

    angle_rad -= angle_step_rad;
  }
}

 /**
  * @}
  */ // SVMLUT
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/