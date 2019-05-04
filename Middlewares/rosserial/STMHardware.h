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
 * @file STMHardware.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 * 
 * @brief STM32 hardware representation for rosserial
 * 
 * @version 0.1
 * @date 2019-05-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef ROS_STM32_HARDWARE_H_
#define ROS_STM32_HARDWARE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------------*/

#include <stdint.h>

#if defined(STM32F4)
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_uart.h"
#else
  #error "Please specify STM hardware type e.g. STM32F3 or STM32F4"
#endif

/* -------------------------------------------------------------------------------*/

namespace ros
{

/* Hardware Configuration --------------------------------------------------------*/
constexpr uint16_t  STM_HW_BUF_SIZE = 512u; //!< Size of tx/rx buffer

extern UART_HandleTypeDef huart2; //!< Standard serial interface of nucleo boards
/* -------------------------------------------------------------------------------*/

/**
 * @brief Class representing STM32 device as rosserial hardware
 */
class STMHardware
{
  public:

    /**
     * @brief Construct a new STMHardware object
     * 
     * @param serial Reference to serial interface
     */
    STMHardware(void) :
    _serial(huart2),
    _baud(57600)
    {

    }

    /**
     * @brief Initialize hardware interface
     */
    void init()
    {

    }

    /**
     * @brief Read data from serial interface
     * 
     * @return int Returns received character or -1 if
     *             buffer is empty.
     */
    int read()
    {
      return -1;
    }

    /**
     * @brief Write data via serial interface
     * 
     * @param data Pointer to array containing data
     * @param size Size of data to send
     */
    void write(uint8_t* data, const uint16_t size)
    {

    }

  protected:

    UART_HandleTypeDef&   _serial; //!< Serial interface
    uint32_t              _baud;   //!< Baudrate

    uint8_t     _tx_buffer[STM_HW_BUF_SIZE];  //!< Data to transmit
    uint16_t    _tx_size;                     //!< Size of data to transmit

    uint8_t     _rx_buffer[STM_HW_BUF_SIZE];  //!< Received data
    uint16_t    _rx_read_pos;                 //!< Current read position in buffer
    uint16_t    _rx_size;                     //!< Amount of data in buffer
};


}; /* namespace ros */

#ifdef __cplusplus
};
#endif

#endif /* ROS_STM32_HARDWARE_H_*/