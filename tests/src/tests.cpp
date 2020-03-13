/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief Main source file of tests
 *
 * @version 1.0
 * @date 2020-03-13
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 *
 */

#include <iostream>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/MemoryLeakDetectorMallocMacros.h>
#include <CppUTest/MemoryLeakDetectorNewMacros.h>


int main(int argc, char* argv[])
{
  std::cout << "-----------------------------------------------------------------" << std::endl;
  std::cout << "Unit Tests of FRANCOR e.V. MBED Library" << std::endl;
  std::cout << "-----------------------------------------------------------------" << std::endl;
  const int result = RUN_ALL_TESTS(argc, argv);
  std::cout << "-----------------------------------------------------------------" << std::endl;
  return result;
}