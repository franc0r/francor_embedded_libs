/**
 * @file qvariable_tests.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Unit test source file of qvariable
 *
 * @version 1.0
 * @date 2020-03-13
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 *
 */

#include "francor_mbed/utils/utils.h"
#include "francor_mbed/utils/qvariable.h"

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

 TEST_GROUP(QVariable)
 {
   void setup()
   {

   }

   void teardown()
   {

   }
 };

const bool QVariableCheckResult(const double result, const double expected_result,
                               const double precision) 
{
  return (abs(expected_result - result) <= precision);
}

/**
 * Test if the constructor initializes the variables correctly
 */
 TEST(QVariable, ConstructorDefault)
 {
   QVariable<uint32_t, 8>  var1;
   QVariable<uint32_t, 4>  var2;

   CHECK_EQUAL(0u, var1._raw_value);
   CHECK_EQUAL(0u, var2._raw_value);

   CHECK_EQUAL(0.00390625, var1._precision);
   CHECK_EQUAL(0.0625, var2._precision);
 }

/**
 * Test if the constructor initializes the variable correctly
 */
TEST(QVariable, ConstructorDouble)
{
  QVariable<uint32_t, 8>  var1(2.134);

  CHECK_EQUAL(546u, var1._raw_value);
}

/**
 * Test if float is returned correctly
 */
TEST(QVariable, GetFloatValue)
{
  QVariable<int32_t, 8>  var1(-2.134);

  const float val = var1;
  CHECK_EQUAL(-2.1328125f, val);
}

/**
 * Test if double is returned correctly
 */
TEST(QVariable, GetDoubleValue)
{
  QVariable<uint32_t, 8>  var1(2.134);

  const double val = var1;
  CHECK_EQUAL(2.1328125, val);
}

/**
 * Test if number of fractional is returned correctly
 */
TEST(QVariable, GetNumFracBits)
{
  QVariable<uint32_t, 8>  var1(2.134);
  QVariable<uint32_t, 6>  var2(5.134);
  QVariable<uint32_t, 2>  var3(2.134);

  CHECK_EQUAL(8u, var1.getNumFracBits());
  CHECK_EQUAL(6u, var2.getNumFracBits());
  CHECK_EQUAL(2u, var3.getNumFracBits());
}

/**
 * Test if addition of two fxp values works correctly
 */
TEST(QVariable, AddTwoFXPValues1)
{
  QVariable<int32_t, 8> var1(2.56), var2(2.44);

  QVariable<int32_t, 8> res = var1 + var2;

  CHECK_EQUAL(655, var1.getRawValue());
  CHECK_EQUAL(624, var2.getRawValue());

  CHECK_EQUAL(1279, res.getRawValue());
  
  CHECK_EQUAL(true, QVariableCheckResult(double(res), 5.0, res.getPrecision()));
}

/**
 * Test if addition of two fxp values works correctly
 */
TEST(QVariable, AddTwoFXPValues2)
{
  QVariable<int32_t, 8> var1(2.56), var2(-12.56);

  QVariable<int32_t, 8> res = var1 + var2;

  CHECK_EQUAL(655, var1.getRawValue());
  CHECK_EQUAL(-3215, var2.getRawValue());

  CHECK_EQUAL(-2560, res.getRawValue());
  
  CHECK_EQUAL(true, QVariableCheckResult(double(res), -10.0, res.getPrecision()));
}

/**
 * Test if substraction of two fxp values works correctly
 */
TEST(QVariable, SubsTwoFXPValues1)
{
  QVariable<int32_t, 8> var1(152.985), var2(2.985);

  QVariable<int32_t, 8> res = var1 - var2;

  CHECK_EQUAL(39164, var1.getRawValue());
  CHECK_EQUAL(764, var2.getRawValue());

  CHECK_EQUAL(38400, res.getRawValue());
  
  CHECK_EQUAL(true, QVariableCheckResult(double(res), 150.0, res.getPrecision()));
}

/**
 * Test if substraction of two fxp values works correctly
 */
TEST(QVariable, SubsTwoFXPValues2)
{
  QVariable<int32_t, 8> var1(152.985), var2(-152.015);

  QVariable<int32_t, 8> res = var1 - var2;

  CHECK_EQUAL(39164, var1.getRawValue());
  CHECK_EQUAL(-38915, var2.getRawValue());

  CHECK_EQUAL(78079, res.getRawValue());
  
  CHECK_EQUAL(true, QVariableCheckResult(double(res), 305.0, res.getPrecision()));
}

/**
 * Test if multiplication of two fxp values works correctly
 */
TEST(QVariable, MultiTwoFXPValues1)
{
  QVariable<int32_t, 8> var1(152.56), var2(2.0);

  QVariable<int32_t, 8> res = var1 * var2;

  CHECK_EQUAL(39055, var1.getRawValue());
  CHECK_EQUAL(512, var2.getRawValue());

  CHECK_EQUAL(78110, res.getRawValue());
  CHECK_EQUAL(true, QVariableCheckResult(double(res), 305.12, res.getPrecision()));
}

/**
 * Test if multiplication of two fxp values works correctly
 */
TEST(QVariable, MultiTwoFXPValues2)
{
  QVariable<int32_t, 8> var1(152.56), var2(-2.0);

  QVariable<int32_t, 8> res = var1 * var2;

  CHECK_EQUAL(39055, var1.getRawValue());
  CHECK_EQUAL(-512, var2.getRawValue());

  CHECK_EQUAL(-78110, res.getRawValue());
  CHECK_EQUAL(true, QVariableCheckResult(double(res), -305.12, res.getPrecision()));
}

/**
 * Test if division of two fxp values works correctly
 */
TEST(QVariable, DivideTwoFXPValues1)
{
  QVariable<int32_t, 8> var1(223.5), var2(0.1);

  QVariable<int32_t, 8> res = var1 / var2;

  CHECK_EQUAL(57216, var1.getRawValue());
  CHECK_EQUAL(25, var2.getRawValue());

  CHECK_EQUAL(585891, res.getRawValue());
  
  // Result is 2288.64 due to round error in var2(0.1) = 0.097656
  CHECK_EQUAL(true, QVariableCheckResult(double(res), 2288.64, res.getPrecision()));
}

/**
 * Test if division of two fxp values works correctly
 */
TEST(QVariable, DivideTwoFXPValues2)
{
  QVariable<int32_t, 8> var1(223.5), var2(-0.1);

  QVariable<int32_t, 8> res = var1 / var2;

  CHECK_EQUAL(57216, var1.getRawValue());
  CHECK_EQUAL(-25, var2.getRawValue());

  CHECK_EQUAL(-585891, res.getRawValue());
  
  // Result is 2288.64 due to round error in var2(0.1) = -0.097656
  CHECK_EQUAL(true, QVariableCheckResult(double(res), -2288.64, res.getPrecision()));
}

 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/