/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card_i2c_ifc.h
//-----------------------------------------------------------------------------
// Description: This is the I2C driver interface expected by smart card library.
//
//*****************************************************************************

#ifndef _HP_SMART_CARD_I2C_IFC_H_
#define _HP_SMART_CARD_I2C_IFC_H_

#include <stddef.h>

#include "hp_types.h"
#include "hp_smart_card_config.h"

typedef enum
{
    HP_SMART_CARD_I2C_SUCCESS,                          /* Success */
    HP_SMART_CARD_I2C_FAILED,                           /* Failure */
} HP_SMART_CARD_i2c_result_t;

//-----------------------------------------------------------------------------
// Library Function Prototypes
//-----------------------------------------------------------------------------

//*****************************************************************************
// HP_SMART_CARD_i2c_init -
//-----------------------------------------------------------------------------
// Description - This function initializes HP smart card I2C library. Will be
//               called by smart card library to initialize I2C library.
//
// Inputs: None
//
// Return: None
//
//*****************************************************************************
void HP_SMART_CARD_i2c_init(void);

//*****************************************************************************
// HP_SMART_CARD_i2c_read -
//-----------------------------------------------------------------------------
// Description - This function reads data from the requested I2C device.
//
// Inputs: device_id - smart card device identifier
//         addr  - internal register address to read
//         data  - pointer to buffer that is used to return the data read.
//         num_bytes_to_read - Number of bytes to read.
//
// Return:
//  HP_SMART_CARD_I2C_SUCCESS: success
//  HP_SMART_CARD_I2C_FAILED: failed
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_read(HP_SMART_CARD_device_id_t device_id,
                                                  uint8_t addr,
                                                  uint8_t *data,
                                                  size_t num_bytes_to_read);

//*****************************************************************************
// HP_SMART_CARD_i2c_read -
//-----------------------------------------------------------------------------
// Description - This function reads data from the requested I2C device.
//
// Inputs: i2c_addr - smart card i2c address
//         addr  - internal register address to read
//         data  - pointer to buffer that is used to return the data read.
//         num_bytes_to_read - Number of bytes to read.
//
//  HP_SMART_CARD_I2C_SUCCESS: success
//  HP_SMART_CARD_I2C_FAILED: failed
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_read_direct(uint8_t i2c_addr,
                                                         uint8_t addr,
                                                         uint8_t *data,
                                                         size_t num_bytes_to_read);

//*****************************************************************************
// HP_SMART_CARD_i2c_write -
//-----------------------------------------------------------------------------
// Description - This function writes data to the requested I2C device.
//
// Inputs: device_id - smart card device identifier
//         addr  - internal register address to read.
//         data  - pointer to buffer that is contains data to write.
//         num_bytes_to_write - Number of bytes to write.
//
//  HP_SMART_CARD_I2C_SUCCESS: success
//  HP_SMART_CARD_I2C_FAILED: failed
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_write(HP_SMART_CARD_device_id_t device_id,
                                                   uint8_t addr,
                                                   uint8_t                   *data,
                                                   size_t num_bytes_to_write);

//*****************************************************************************
// HP_SMART_CARD_i2c_write_direct -
//-----------------------------------------------------------------------------
// Description - This function writes data to the requested I2C device.
//
// Inputs: i2c_addr - smart card i2c address
//         addr  - internal register address to read.
//         data  - pointer to buffer that is contains data to write.
//         num_bytes_to_write - Number of bytes to write.
//
//  HP_SMART_CARD_I2C_SUCCESS: success
//  HP_SMART_CARD_I2C_FAILED: failed
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_write_direct(uint8_t i2c_addr,
                                                          uint8_t addr,
                                                          uint8_t *data,
                                                          size_t num_bytes_to_write);

#endif /*  _HP_SMART_CARD_I2C_IFC_H_ */
