/*BSD 3-Clause License

Copyright (c) 2019, Franconian Open Robotics
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
 * @file STMHardwareTests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 * 
 * @brief Tests for STM32 hardware representation for rosserial
 * 
 * @version 0.1
 * @date 2019-05-04
 * 
 * @copyright Copyright (c) 2019 Franconian Open Robotics
 * 
 */

/* Includes ----------------------------------------------------------------------*/
#include <CppUTest/TestHarness.h>
#include <cstring>
#include "STMHardware.h"
/* -------------------------------------------------------------------------------*/

constexpr uint32_t ROSSERIAL_DEFAULT_BAUD = 57600u;

extern UART_HandleTypeDef huart2;

TEST_GROUP(STMHardware)
{
  void setup()
  {
    // Set baudrate to default rosserial
    huart2.Init.BaudRate  = 57600u;
  }

  void teardown()
  {

  }

  void checkValueReset()
  {
    CHECK(huart2.Instance == _hardware._serial.Instance);
    CHECK(ros::STM_HW_DEF_BAUD == _hardware._baud);
    CHECK(0u == _hardware._tx_size);
    CHECK(0u == _hardware._rx_read_pos);
    CHECK(0u == _hardware._rx_size);
    
    for(auto idx = 0u; idx < ros::STM_HW_BUF_SIZE; idx++)
    {
      CHECK(0u == _hardware._tx_buffer[idx]);
      CHECK(0u == _hardware._rx_buffer[idx]);
    }
  }

  ros::STMHardware  _hardware;
};

TEST(STMHardware, Constructor)
{
  checkValueReset();
}

TEST(STMHardware, Init)
{
  _hardware.init();

  CHECK(ros::STM_HW_DEF_BAUD == _hardware._baud);
}

TEST(STMHardware, InitBaud115200)
{
 huart2.Init.BaudRate = 115200u;
 _hardware.init();

 CHECK(115200u == _hardware._baud); 
}

TEST(STMHardware, InitResetValue)
{
  // Set values
  _hardware._baud = 1234;
  _hardware._tx_buffer[0] = 2;
  _hardware._rx_buffer[0] = 3;
  _hardware._rx_read_pos = 5;
  _hardware._rx_size = 45;

  _hardware.init();

  checkValueReset();
}

TEST(STMHardware, ReadNoData)
{
  _hardware.init();

  CHECK(0 == _hardware._rx_size);
  CHECK(-1 == _hardware.read());
}

TEST(STMHardware, ReadData)
{
  _hardware.init();

  const char      msg[]     = "Hello World!";
  const uint16_t  msg_size  = strlen(msg);

  // Copy data to rx buffer
  _hardware._rx_size = msg_size;
  memcpy(_hardware._rx_buffer, msg, msg_size);

  // Read data
  for(auto idx = 0u; idx < msg_size; idx++)
  {
    CHECK(idx == _hardware._rx_read_pos);
    CHECK(msg[idx] == _hardware.read());
  }

  CHECK(0 == _hardware._rx_size);
}