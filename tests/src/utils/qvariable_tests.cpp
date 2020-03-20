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
 * @file qvariable_tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Unit test source file of qvariable
 *
 * @version 1.0
 * @date 2020-03-13
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg - BSD 3-Clause License
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
 * Test if decimal value is returned correctly
 */
TEST(QVariable, GetDecimalValue)
{
  QVariable<int32_t, 15>  var(-30.45);

  const int32_t dec = static_cast<int32_t>(var);

  CHECK_EQUAL(-30, dec);
}

/**
 * Test if float is returned correctly
 */
TEST(QVariable, GetFloatValue)
{
  QVariable<int32_t, 8>  var1(-2.134);

  const float val = static_cast<float>(var1);
  CHECK_EQUAL(-2.1328125f, val);
}

/**
 * Test if double is returned correctly
 */
TEST(QVariable, GetDoubleValue)
{
  QVariable<uint32_t, 8>  var1(2.134);

  const double val = static_cast<double>(var1);
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
 * Test if conversion from one fxp precision to another value works
 */
TEST(QVariable, ConvertFXPValues)
{
  QVariable<uint32_t, 8>  var1(2.134);
  QVariable<uint32_t, 6>  var2(5.125);

  CHECK_EQUAL(546, var1.getRawValue());
  CHECK_EQUAL(328, var2.getRawValue());

  var1.convert(var2);
  CHECK_EQUAL(1312, var1.getRawValue());
  CHECK_EQUAL(5.125, static_cast<double>(var1));
}

/**
 * Test if assignment works
 */
TEST(QVariable, AssignFXPValue)
{
  QVariable<uint32_t, 8>  var1(2.134);
  QVariable<uint32_t, 8>  var2(5.125);

  CHECK_EQUAL(546, var1.getRawValue());
  CHECK_EQUAL(1312, var2.getRawValue());

  var1 = var2;

  CHECK_EQUAL(1312, var1.getRawValue());
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
 * Test if operator % works
 */
TEST(QVariable, ModulusOperator)
{
  QVariable<int8_t, 2> val1(20.5), val2(10.0);

  QVariable<int8_t, 2> res = val1 % val2;

  CHECK_EQUAL(2, res.getRawValue());
}

/**
 * Test if comparison operator == works correctly
 */
TEST(QVariable, CompareOperatorEqual)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44);

  CHECK_EQUAL(true, (var1 == var2));
  CHECK_EQUAL(false, (var1 == var3));
}

/**
 * Test if comparison operator != works correctly
 */
TEST(QVariable, CompareOperatorUnEqual)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44);

  CHECK_EQUAL(true, (var1 != var3));
  CHECK_EQUAL(false, (var1 != var2));
}

/**
 * Test if comparison operator >= works correctly
 */
TEST(QVariable, CompareOperatorGreaterEqual)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44), var4(0.1);

  CHECK_EQUAL(true, (var1 >= var2));
  CHECK_EQUAL(true, (var1 >= var4));
  CHECK_EQUAL(false, (var1 >= var3));
}

/**
 * Test if comparison operator <= works correctly
 */
TEST(QVariable, CompareOperatorSmallerEqual)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44), var4(0.1);

  CHECK_EQUAL(true, (var1 <= var2));
  CHECK_EQUAL(true, (var4 <= var3));
  CHECK_EQUAL(false, (var3 <= var1));
}

/**
 * Test if comparison operator > works correctly
 */
TEST(QVariable, CompareOperatorGreater)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44), var4(0.1);

  CHECK_EQUAL(true, (var3 > var2));
  CHECK_EQUAL(true, (var1 > var4));
  CHECK_EQUAL(false, (var4 > var1));
}

/**
 * Test if comparison operator < works correctly
 */
TEST(QVariable, CompareOperatorSmaller)
{
  QVariable<int32_t, 14> var1(1234.34), var2(1234.34), var3(1234.44), var4(0.1);

  CHECK_EQUAL(true, (var1 < var3));
  CHECK_EQUAL(true, (var4 < var2));
  CHECK_EQUAL(false, (var1 < var4));
}

 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/