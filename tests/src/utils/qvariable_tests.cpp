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

/**
 * Test if the constructor initializes the variables correctly
 */
 TEST(QVariable, ConstructorDefault)
 {
   QVariable<uint32_t, 8>  var1;
   QVariable<uint32_t, 4>  var2;

   CHECK_EQUAL(0u, var1._raw_value);
   CHECK_EQUAL(0u, var2._raw_value);

   CHECK_EQUAL(0.00390625, var1._accuracy);
   CHECK_EQUAL(0.0625, var2._accuracy);
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
 * Test if raw value is returned correctly
 */
TEST(QVariable, GetRawValue)
{
  QVariable<uint32_t, 8>  var1(2.134);

  const uint32_t raw_val = var1;
  CHECK_EQUAL(546u, raw_val);
}

/**
 * Test if floating point value is returned correctly
 */
TEST(QVariable, GetFPValue)
{
  QVariable<uint32_t, 8>  var1(2.134);

  const double val = var1;
  CHECK_EQUAL(2.1328125, val);
}

/**
 * Test if addition of two values by + works correctly
 */
TEST(QVariable, AddTwoQVals)
{
  QVariable<uint32_t, 4>  var1(2.5), var2(12.7);

  CHECK_EQUAL(40u, static_cast<uint32_t>(var1));
  CHECK_EQUAL(203u, static_cast<uint32_t>(var2));

  QVariable<uint32_t, 4> res = var1 + var2;

  CHECK_EQUAL(243, static_cast<uint32_t>(res));
  CHECK_EQUAL(15.1875, static_cast<double>(res));
}

/**
 * Test if addition of two values by += works correctly
 */
TEST(QVariable, AddTwoQValsByAddEq)
{
  QVariable<uint32_t, 4>  var1(2.5), var2(12.7);

  CHECK_EQUAL(40u, static_cast<uint32_t>(var1));
  CHECK_EQUAL(203u, static_cast<uint32_t>(var2));

  var1 += var2;

  CHECK_EQUAL(243, static_cast<uint32_t>(var1));
  CHECK_EQUAL(15.1875, static_cast<double>(var1));
}

/**
 * Test if addition of raw value by + works correctly
 */
TEST(QVariable, AddTwoQValsRaw)
{
  QVariable<uint32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<uint32_t>(var1));

  QVariable<uint32_t, 4> res = var1 + 203u;

  CHECK_EQUAL(243, static_cast<uint32_t>(res));
  CHECK_EQUAL(15.1875, static_cast<double>(res));
}

/**
 * Test if addition of raw value by += works correctly
 */
TEST(QVariable, AddTwoQValsRawByAddEq)
{
  QVariable<uint32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<uint32_t>(var1));

  var1 += 203u;

  CHECK_EQUAL(243, static_cast<uint32_t>(var1));
  CHECK_EQUAL(15.1875, static_cast<double>(var1));
}


/**
 * Test if substraction of two values by + works correctly
 */
TEST(QVariable, SubstTwoQVals)
{
  QVariable<int32_t, 4>  var1(2.5), var2(12.7);

  CHECK_EQUAL(40, static_cast<int32_t>(var1));
  CHECK_EQUAL(203, static_cast<int32_t>(var2));

  QVariable<int32_t, 4> res = var1 - var2;

  CHECK_EQUAL(-163, static_cast<int32_t>(res));
  CHECK_EQUAL(-10.1875, static_cast<double>(res));
}

/**
 * Test if substraction of two values by -= works correctly
 */
TEST(QVariable, SubstTwoQValsBySubstEq)
{
  QVariable<int32_t, 4>  var1(2.5), var2(12.7);

  CHECK_EQUAL(40, static_cast<int32_t>(var1));
  CHECK_EQUAL(203, static_cast<int32_t>(var2));

  var1 -= var2;

  CHECK_EQUAL(-163, static_cast<int32_t>(var1));
  CHECK_EQUAL(-10.1875, static_cast<double>(var1));
}

/**
 * Test if substraction of raw value by - works correctly
 */
TEST(QVariable, SubstTwoQValsRaw)
{
  QVariable<int32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<int32_t>(var1));

  QVariable<int32_t, 4> res = var1 - 203;

  CHECK_EQUAL(-163, static_cast<int32_t>(res));
  CHECK_EQUAL(-10.1875, static_cast<double>(res));
}

/**
 * Test if substraction of raw value by -= works correctly
 */
TEST(QVariable, SubstTwoQValsRawBySubstEq)
{
  QVariable<int32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<int32_t>(var1));

  var1 -= 203u;

  CHECK_EQUAL(-163, static_cast<int32_t>(var1));
  CHECK_EQUAL(-10.1875, static_cast<double>(var1));
}

/**
 * Test if multiplication of two values by * works correctly
 */
TEST(QVariable, MultiTwoQVals)
{
  QVariable<int32_t, 4>  var1(2.5), var2(-12.7);

  CHECK_EQUAL(40, static_cast<int32_t>(var1));
  CHECK_EQUAL(-203, static_cast<int32_t>(var2));

  QVariable<int32_t, 4> res = var1 * var2;

  CHECK_EQUAL(-508, static_cast<int32_t>(res));
  CHECK_EQUAL(-31.75, static_cast<double>(res));
}

/**
 * Test if multiplication of two values by *= works correctly
 */
TEST(QVariable, MultiTwoQValsByMultiEq)
{
  QVariable<int32_t, 4>  var1(2.5), var2(12.7);

  CHECK_EQUAL(40, static_cast<int32_t>(var1));
  CHECK_EQUAL(203, static_cast<int32_t>(var2));

  var1 *= var2;

  CHECK_EQUAL(507, static_cast<int32_t>(var1));
  CHECK_EQUAL(31.6875, static_cast<double>(var1));
}

/**
 * Test if multiplication of raw value by * works correctly
 */
TEST(QVariable, MultiTwoQValsRaw)
{
  QVariable<int32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<int32_t>(var1));

  QVariable<int32_t, 4> res = var1 * -203;

  CHECK_EQUAL(-508, static_cast<int32_t>(res));
  CHECK_EQUAL(-31.75, static_cast<double>(res));
}

/**
 * Test if multiplication of raw value by *= works correctly
 */
TEST(QVariable, MultiTwoQValsRawByMultiEq)
{
  QVariable<int32_t, 4>  var1(2.5);

  CHECK_EQUAL(40u, static_cast<int32_t>(var1));

  var1 *= 203u;

  CHECK_EQUAL(507, static_cast<int32_t>(var1));
  CHECK_EQUAL(31.6875, static_cast<double>(var1));
}

 /**
  * @}
  */ // Tests
/*--------------------------------------------------------------------------------*/

 /**
  * @}
  */ // francor_mbed
/*--------------------------------------------------------------------------------*/