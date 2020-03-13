/**
 * @file utils.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@posteo.de)
 *
 * @brief General Defines and Utilities for Franconian Embedded Projects
 *
 * @version 1.0
 * @date 2019-11-18
 *
 * @copyright Copyright (c) 2020 Francor e.V. Nuremberg
 * 
 */

#pragma once

#include <cstdint>
#include <limits>
#include <array>
#include <math.h>
#include <stdbool.h>


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
 * @brief Enumeration of results returned by all standard francor embedded functions
 */
enum Result : uint16_t
{
  RES_OK    = 0U,   //!< OK
  RES_ERROR,        //!< General Error
  RES_INVLD_PARAMS, //!< Parameters set not valid
  RES_PTR_ERROR,    //!< Error due to invaid pointer in parameters/variables
  RES_OUT_OF_MEM    //!< No space left in memory (array full etc.)
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