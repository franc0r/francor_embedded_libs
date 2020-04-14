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
 * @file svm_handler_tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Unit tests for space vector modulation handler
 *
 * @version 1.0
 * @date 2020-03-20
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg - BSD 3-Clause License
 *
 */

#include "francor_mbed/utils/svm_handler.h"

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
/** @addtogroup SVMHandler
  * @{
  */

TEST_GROUP(SVMHandler)
{
  void setup() {}
  void teardown() {}
};

/**
 * @brief This function calculates the ccr values with high precision to
 *        be able to compare it with the ones from the lookup table
 * 
 * @param ccr1 Reference to CCR1 value (output)
 * @param ccr2 Reference to CCR2 value (output)
 * @param ccr3 Reference to CCR3 value (output)
 * @param angle_deg Electrical angle in degree
 * @param ccr_max Maximum CCR value
 * @param mod Modulation factor
 */
void calcExpCCRValues(uint16_t& ccr1, uint16_t& ccr2, uint16_t& ccr3,
                      const double angle_deg, const uint16_t ccr_max = 1000u,
                      const double mod = 1.0)
{
  // Calculate sector angle
  const unsigned int sector = static_cast<unsigned int>(floor(angle_deg) / 60.0);
  const double sec_angle = angle_deg - static_cast<double>(sector) * 60.0;

  const double angle_rad = sec_angle * (M_PI / 180.0);

  const double ccra = sin((M_PI / 3.0) - angle_rad) * static_cast<double>(ccr_max) * mod;
  const double ccrb = sin(angle_rad) * static_cast<double>(ccr_max) * mod;

  const uint16_t ccra_u = static_cast<uint16_t>(round(ccra));
  const uint16_t ccrb_u = static_cast<uint16_t>(round(ccrb));
  const uint16_t ccr0_u = (ccr_max - ccra_u - ccrb_u + 1) >> 1u;

  switch(sector % 6)
  {
    case 0:
    {
      ccr1 = ccr0_u;
      ccr2 = ccr0_u + ccrb_u;
      ccr3 = ccr0_u + ccra_u + ccrb_u;
    }break;

    case 1:
    {
      ccr1 = ccr0_u;
      ccr2 = ccr0_u + ccra_u + ccrb_u;
      ccr3 = ccr0_u + ccra_u;
    }break;

    case 2:
    {
      ccr1 = ccr0_u + ccrb_u;
      ccr2 = ccr0_u + ccra_u + ccrb_u;
      ccr3 = ccr0_u;
    }break;

    case 3:
    {
      ccr1 = ccr0_u + ccra_u + ccrb_u;
      ccr2 = ccr0_u + ccra_u;
      ccr3 = ccr0_u;
    }break;

    case 4:
    {
      ccr1 = ccr0_u + ccra_u + ccrb_u;
      ccr2 = ccr0_u;
      ccr3 = ccr0_u + ccrb_u;
    }break;

    case 5:
    {
      ccr1 = ccr0_u + ccra_u;
      ccr2 = ccr0_u;
      ccr3 = ccr0_u + ccra_u + ccrb_u;
    }break;
  }

}

/**
 * Test if class can be build and values are resetted to 0
 */
TEST(SVMHandler, Constructor)
{
  SVMHandler<8, 1000> svm;

  CHECK_EQUAL(0.234375, svm.getAnglePrecision());
  CHECK_EQUAL(0, svm.getElecAngle());
  CHECK_EQUAL(0, svm.getSectorAngle());
  CHECK_EQUAL(0, svm.getSector());
  CHECK_EQUAL(0, svm.getCCRChn1());
  CHECK_EQUAL(0, svm.getCCRChn2());
  CHECK_EQUAL(0, svm.getCCRChn3());
}

/**
 * Test if the move function calculates the angles
 * correctly and if the overflow behavior is valid
 */
TEST(SVMHandler, MoveAngleBitPosP3)
{
  constexpr uint16_t BitPrecision = 3u;
  constexpr uint16_t CCRMax = 1000;
  constexpr uint32_t N = (1 << BitPrecision) * 16;

  SVMHandler<BitPrecision, CCRMax> svm;

  double angle = 0.0;
  for(auto idx = 0; idx < N; idx++)
  {
    const uint8_t   sector        = static_cast<uint8_t>(floor(angle / 60.0));

    CHECK_EQUAL(sector, svm.getSector());

    //printf("Angle: %f: Elec-Angle: %i Sec-Angle: %i Sector: %i\n",
    //       static_cast<double>(idx) * svm.getAnglePrecision(),
    //       svm.getElecAngle(), svm.getSectorAngle(), svm.getSector());

    svm.move(1);
    angle += svm.getAnglePrecision();

    if(angle >= 360.0)  angle -= 360.0;
    else if(angle < 0.0) angle += 360.0;
  }
}

/**
 * Test if the move function calculates the angles
 * correctly and if the overflow behavior is valid
 */
TEST(SVMHandler, MoveAngleBitNegP3)
{
  constexpr uint16_t BitPrecision = 12u;
  constexpr uint16_t CCRMax = 1000;
  constexpr uint32_t N = (1 << BitPrecision) * 16;

  SVMHandler<BitPrecision, CCRMax> svm;

  double angle = 0.0;
  for(auto idx = 0; idx < N; idx++)
  {
    const uint8_t   sector        = static_cast<uint8_t>(floor(angle / 60.0));

    CHECK_EQUAL(sector, svm.getSector());

    //printf("Angle: %f: Elec-Angle: %i Sec-Angle: %i Sector: %i\n",
    //       static_cast<double>(idx) * svm.getAnglePrecision(),
    //       svm.getElecAngle(), svm.getSectorAngle(), svm.getSector());

    svm.move(-1);
    angle -= svm.getAnglePrecision();

    if(angle >= 360.0)  angle -= 360.0;
    else if(angle < 0.0) angle += 360.0;
  }
}


/**
 * Test if the values for the ccr registers are calculated correctly
 * for every angle while driving in positive direction
 */
TEST(SVMHandler, CCRCalculation)
{
  constexpr uint16_t PREC = 3;
  constexpr uint16_t CCR = 1000u;
  constexpr uint16_t NUM = (1u << PREC);

  SVMHandler<PREC, CCR> svm;

  const double delta_angle = (60.0 / static_cast<double>(NUM));
  double angle = 0.0;

  for(auto idx = 0u; idx < (NUM * 7); idx++)
  {
    uint16_t exp_ccr1 = 0, exp_ccr2 = 0, exp_ccr3 = 0;
    calcExpCCRValues(exp_ccr1, exp_ccr2, exp_ccr3, angle, CCR, 1.0);

    svm.update(fxp(1.0));

    CHECK_EQUAL(exp_ccr1, svm.getCCRChn1());
    CHECK_EQUAL(exp_ccr2, svm.getCCRChn2());
    CHECK_EQUAL(exp_ccr3, svm.getCCRChn3());

    /*printf("[%i]: Angle: %.3f", angle);
    printf(" | IDX: %i SEC-IDX: %i SEC: %i ", svm.getAngle(), svm.getSecAngle(), svm.getActvSec());
    printf(" | EXP: %i %i %i", exp_ccr1, exp_ccr2, exp_ccr3);

    printf("\n");*/

    angle += delta_angle;
    if(angle < 0.0) {angle += 360.0;}
    else if(angle > 360.0) {angle -= 360.0;}

    svm.move(1);
  }
}

/**
 * Test if the values for the ccr registers are calculated correctly
 * for every angle while driving in negative direction
 */
TEST(SVMHandler, CCRCalculationNeg)
{
  constexpr uint16_t PREC = 4;
  constexpr uint16_t CCR = 1000u;
  constexpr uint16_t NUM = (1u << PREC);

  SVMHandler<PREC, CCR> svm;

  const double delta_angle = (60.0 / static_cast<double>(NUM));
  double angle = 0.0;
  
  for(auto idx = 0u; idx < (NUM * 7); idx++)
  {
    uint16_t exp_ccr1 = 0, exp_ccr2 = 0, exp_ccr3 = 0;
    calcExpCCRValues(exp_ccr1, exp_ccr2, exp_ccr3, angle, 1000, 1.0);

    svm.update(fxp(1.0));

    CHECK_EQUAL(exp_ccr1, svm.getCCRChn1());
    CHECK_EQUAL(exp_ccr2, svm.getCCRChn2());
    CHECK_EQUAL(exp_ccr3, svm.getCCRChn3());

    angle -= delta_angle;
    if(angle < 0.0) {angle += 360.0;}
    else if(angle > 360.0) {angle -= 360.0;}

    svm.move(-1);
  }

  constexpr SVMLUTROM<8, 1000> rom_lut;
}

 /**
  * @}
  */ // SVMHandler
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/