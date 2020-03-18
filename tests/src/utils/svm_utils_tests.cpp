/**
 * @file svm_utils_tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Unit test source file of qvariable
 *
 * @version 1.0
 * @date 2020-03-17
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 *
 */

#include "francor_mbed/utils/utils.h"
#include "francor_mbed/utils/svm_utils.h"
#include <math.h>
#include <cmath>

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

TEST_GROUP(SVMPWM)
{
  void setup() {}
  void teardown() {}
};

void calcCCRValues(uint16_t& ccr1, uint16_t& ccr2, uint16_t& ccr3,
                   const double angle_deg, const uint16_t ccr_max = 1000u,
                   const double mod = 1.0)
{
  

  // Calculate sector angle
  const unsigned int sector = static_cast<unsigned int>(floor(angle_deg / 60.0));
  const double sec_angle = angle_deg - static_cast<double>(sector) * 60.0;

  const double angle_rad = sec_angle * (M_PI / 180.0);

  const double ccra = sin((M_PI / 3.0) - angle_rad) * static_cast<double>(ccr_max) * mod;
  const double ccrb = sin(angle_rad) * static_cast<double>(ccr_max) * mod;

  const uint16_t ccra_u = static_cast<uint16_t>(round(ccra));
  const uint16_t ccrb_u = static_cast<uint16_t>(round(ccrb));
  const uint16_t ccr0_u = (ccr_max - ccra_u - ccrb_u) >> 1u;

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

TEST(SVMPWM, LUTValuePrecision)
{
  constexpr uint16_t NUM_VALUES = (1 << 8u);
  constexpr uint16_t CCR_MAX = 1000;

  constexpr SVMPWMLUT<NUM_VALUES, CCR_MAX> lut = {};

  double angle = (M_PI / 3.0);
  printf("\n");
  for(auto idx = 0u; idx < NUM_VALUES; idx++)
  { 
    // Calculate ccr value with high prec
    const double ccr_high_prec = static_cast<double>(CCR_MAX) * sin(angle);

    // Round ccr values
    const uint16_t ccr_calc = static_cast<uint16_t>(round(ccr_high_prec));

    CHECK_EQUAL(ccr_calc, lut[idx]);

    // Update angle
    angle -= (60.0 / static_cast<double>(NUM_VALUES)) * (M_PI / 180.0);
  }
}

/**
 * Test if all values (num angles, mask, etc.) are calculated correctly at
 * compile time.
 */
TEST(SVMPWM, ConstructorMaskDefault)
{
  SVMPWM<7, 1000> pwm0;
  SVMPWM<8, 1000> pwm1;
  SVMPWM<9, 1000> pwm2;
  SVMPWM<10, 1000> pwm3;

  CHECK_EQUAL(128, pwm0._num_angles);
  CHECK_EQUAL(256, pwm1._num_angles);
  CHECK_EQUAL(512, pwm2._num_angles);
  CHECK_EQUAL(1024, pwm3._num_angles);

  CHECK_EQUAL(7, pwm0._sec_bit_pos);
  CHECK_EQUAL(8, pwm1._sec_bit_pos);
  CHECK_EQUAL(9, pwm2._sec_bit_pos);
  CHECK_EQUAL(10, pwm3._sec_bit_pos);

  CHECK_EQUAL(768, pwm0._angle_max);
  CHECK_EQUAL(1536, pwm1._angle_max);
  CHECK_EQUAL(3072, pwm2._angle_max);
  CHECK_EQUAL(6144, pwm3._angle_max);

  CHECK_EQUAL(0x7F, pwm0._sec_angle_mask);
  CHECK_EQUAL(0xFF, pwm1._sec_angle_mask);
  CHECK_EQUAL(0x1FF, pwm2._sec_angle_mask);
  CHECK_EQUAL(0x3FF, pwm3._sec_angle_mask);

  CHECK_EQUAL(0, pwm0._angle);
  CHECK_EQUAL(0u, pwm0._actv_sec);
  CHECK_EQUAL(0u, pwm0._sec_angle);

  CHECK_EQUAL(0u, pwm0._ccr_chn1);
  CHECK_EQUAL(0u, pwm0._ccr_chn2);
  CHECK_EQUAL(0u, pwm0._ccr_chn3);
}

/**
 * Test if overflow works correctly
 */
TEST(SVMPWM, AngleOverflowQ8)
{
  SVMPWM<8, 1000> svm;

  // Set elec angle to nearly max value
  svm._angle = svm._angle_max - 2;

  // Check if overflow occurs correctly
  svm.move(1);
  CHECK_EQUAL(1535, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(255, svm._sec_angle);

  svm.move(1);
  CHECK_EQUAL(0, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(0, svm._sec_angle);

  // Check if ovverflow occurs correctly with high delta
  svm._angle = svm._angle_max - 12;

  svm.move(1);
  CHECK_EQUAL(1525, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(245, svm._sec_angle);

  svm.move(20);
  CHECK_EQUAL(9, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(9, svm._sec_angle);
}

/**
 * Test if overflow works correctly
 */
TEST(SVMPWM, AngleOverflowQ10)
{
  SVMPWM<10, 1000> svm;

  // Set elec angle to nearly max value
  svm._angle = svm._angle_max - 2;

  // Check if overflow occurs correctly
  svm.move(1);
  CHECK_EQUAL(6143, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(1023, svm._sec_angle);

  svm.move(1);
  CHECK_EQUAL(0, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(0, svm._sec_angle);

  // Check if ovverflow occurs correctly with high delta
  svm._angle = svm._angle_max - 12;

  svm.move(1);
  CHECK_EQUAL(6133, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(1013, svm._sec_angle);

  svm.move(20);
  CHECK_EQUAL(9, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(9, svm._sec_angle);
}

/**
 * Test if underflow works correctly
 */
TEST(SVMPWM, AngleUnderflowQ8)
{
  SVMPWM<8, 1000> svm;

  // Set elec angle to nearly max value
  svm._angle = 1;

  // Check if overflow occurs correctly
  svm.move(-1);
  CHECK_EQUAL(0, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(0, svm._sec_angle);

  svm.move(-1);
  CHECK_EQUAL(1535, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(255, svm._sec_angle);

  // Check if ovverflow occurs correctly with high delta
  svm._angle = 10;

  svm.move(-1);
  CHECK_EQUAL(9, svm._angle);
  CHECK_EQUAL(0, svm._actv_sec);
  CHECK_EQUAL(9, svm._sec_angle);

  svm.move(-20);
  CHECK_EQUAL(1525, svm._angle);
  CHECK_EQUAL(5, svm._actv_sec);
  CHECK_EQUAL(245, svm._sec_angle);
}

TEST(SVMPWM, CCRCalculation)
{
  constexpr uint16_t PREC = 8;
  constexpr uint16_t CCR = 1000u;
  constexpr uint16_t NUM = (1u << PREC);

  SVMPWM<8, 1000> svm;

  const double delta_angle = (60.0 / static_cast<double>(NUM));
  double angle = 0.0;

  for(auto idx = 0u; idx < (NUM * 7); idx++)
  {
    uint16_t exp_ccr1 = 0, exp_ccr2 = 0, exp_ccr3 = 0;
    calcCCRValues(exp_ccr1, exp_ccr2, exp_ccr3, angle, 1000, 1.0);

    svm.updateCCRValues();

    /*printf("[%i] Angle: %f (SVM-Angle: %i %i %i) | EXP: %i %i %i | SVM: %i %i %i\n", idx, angle, svm._angle, 
           svm._sec_angle, svm._actv_sec, 
           static_cast<int>(exp_ccr1), static_cast<int>(exp_ccr2), static_cast<int>(exp_ccr3), 
           static_cast<int>(svm.getCCRChn1()), static_cast<int>(svm.getCCRChn2()), 
           static_cast<int>(svm.getCCRChn3()));*/

    CHECK_TRUE_TEXT(abs(static_cast<int>(exp_ccr1) - svm.getCCRChn1()) < 5, "CCR1 out of boundaries!");

    angle += delta_angle;
    if(angle < 0.0) {angle += 360.0;}
    else if(angle > 360.0) {angle -= 360.0;}

    svm.move(1);
  }
}

TEST(SVMPWM, CCRCalculationNeg)
{
  constexpr uint16_t PREC = 8;
  constexpr uint16_t CCR = 1000u;
  constexpr uint16_t NUM = (1u << PREC);

  SVMPWM<8, 1000> svm;

  const double delta_angle = (60.0 / static_cast<double>(NUM));
  double angle = 0.0;
  
  for(auto idx = 0u; idx < (NUM * 7); idx++)
  {
    uint16_t exp_ccr1 = 0, exp_ccr2 = 0, exp_ccr3 = 0;
    calcCCRValues(exp_ccr1, exp_ccr2, exp_ccr3, angle, 1000, 1.0);

    svm.updateCCRValues();

    /*printf("[%i] Angle: %f (SVM-Angle: %i %i %i) | EXP: %i %i %i | SVM: %i %i %i\n", idx, angle, svm._angle, 
           svm._sec_angle, svm._actv_sec, 
           static_cast<int>(exp_ccr1), static_cast<int>(exp_ccr2), static_cast<int>(exp_ccr3), 
           static_cast<int>(svm.getCCRChn1()), static_cast<int>(svm.getCCRChn2()), 
           static_cast<int>(svm.getCCRChn3()));*/

    CHECK_TRUE_TEXT(abs(static_cast<int>(exp_ccr1) - svm.getCCRChn1()) < 5, "CCR1 out of boundaries!");

    angle -= delta_angle;
    if(angle < 0.0) {angle += 360.0;}
    else if(angle > 360.0) {angle -= 360.0;}

    svm.move(-1);
  }
}
 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/