/*
    sc_i2c_adapter.h

    @Function
        Declaration of basic operations on i2c device
        Implementation of basic operations on i2c a device

    @Create
        2019.9.20

    @Auther
        H.M. Wang

    @Copyrights
        Copyrights 2019 - 2029, Allrights reserved.

    @Loacation
        Beijing
*/

#ifndef _SC_I2C_ADAPTER_H_
#define _SC_I2C_ADAPTER_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RESULT_OK = 0,                                      /* Success */
    RESULT_FAILED = 1,                                  /* Failure */
    RESULT_FAILED_INVALID_GROUPID = 10000,              /* Invalid group id */
    RESULT_FAILED_INVALID_ADDRESS = 10001,              /* Invalid address */
    RESULT_FAILED_DATA_EMPTY      = 10002,              /* Empty data */
    RESULT_FAILED_CANNOT_OPENFILE = 10003,              /* Opening file failed */
    RESULT_FAILED_WRITE_FILE_FAILED = 10004             /* Writee file failed */
} SC_I2C_ADAPTER_RESULT;

//-----------------------------------------------------------------------------
// Function Prototypes

/*
    i2c_adapter_open_device

    @Description
        This function initializes HP smart card I2C library. Will be
        called by smart card library to initialize I2C library.
    @Params
        group_id: i2c device group id
        device_address: i2c device address

    @Return
        0: Success
        1：Failed
*/
SC_I2C_ADAPTER_RESULT i2c_adapter_open_device(
        int group_id,
        int device_address);

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
// Return: None
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_read(HP_SMART_CARD_device_id_t device_id,
                                                  uint8_t addr,
                                                  uint8_t                  *data,
                                                  size_t num_bytes_to_read);

//*****************************************************************************
// HP_SMART_CARD_i2c_write -
//-----------------------------------------------------------------------------
// Description - This function reads data from the requested I2C device.
//
// Inputs: device_id - smart card device identifier
//         addr  - internal register address to read.
//         data  - pointer to buffer that is contains data to write.
//         num_bytes_to_write - Number of bytes to write.
//
// Return: None
//
//*****************************************************************************
HP_SMART_CARD_i2c_result_t HP_SMART_CARD_i2c_write(HP_SMART_CARD_device_id_t device_id,
                                                   uint8_t addr,
                                                   uint8_t                   *data,
                                                   size_t num_bytes_to_write);
#ifdef __cplusplus
}
#endif

#endif /*  _SC_I2C_ADAPTER_H_ */
