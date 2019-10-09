/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card.h
//-----------------------------------------------------------------------------
// Description: Interface header for HP smart card library.
//
//*****************************************************************************

#ifndef _LIB_HP_SMART_CARD_H_
#define _LIB_HP_SMART_CARD_H_

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "hp_types.h"
#include "hw_smart_card_private.h"
#include "hp_smart_card_types.h"
#include "hp_smart_card_config.h"

//-----------------------------------------------------------------------------
// HP smart card library result types
//-----------------------------------------------------------------------------

// NOTE: ResultToString array must match this enum.
typedef enum
{
    HP_SMART_CARD_OK                    = 0,        // (No error)
    HP_SMART_CARD_NOT_INITIALIZED       = 1,        // Need to init smart card device before read/write
    HP_SMART_CARD_ERROR                 = 2,        // Unknown error
    HP_SMART_CARD_DEVICE_ABSENT         = 3,        // Requested smart card device did not respond
    HP_SMART_CARD_WRITE_ERROR           = 4,        // Protocol error on write operation
    HP_SMART_CARD_READ_ERROR            = 5,        // Protocol error on read operation
    HP_SMART_CARD_DATA_NOT_VALID        = 6,        // Chip works OK, but has bad parity
    HP_SMART_CARD_BAD_SIZE_FOR_TAG      = 7,        // Size specified for the tag did not match
    HP_SMART_CARD_TAG_NOT_FOUND         = 8,        // Specified tag not found on smart card
    HP_SMART_CARD_DUPLICATE_TAG         = 9,        // Two identical tags found on smart card
    HP_SMART_CARD_BAD_CHIP_ID           = 10,       // Incorrect chip identifier
    HP_SMART_CARD_INVALID_CHIP_TAG      = 11,       // Incorrect chip tag
    HP_SMART_CARD_INVALID_FAMILY_ID     = 12,       // Incorrect family ID
    HP_SMART_CARD_ILLEGAL_ACCESS        = 13,       // Illegal memory access
    HP_SMART_CARD_CACHE_NOT_FLUSHED     = 14,       // Cache contents are out of synch with part
    HP_SMART_CARD_BAD_TOKEN             = 15,
    HP_SMART_CARD_BAD_WRITE_PROT        = 16,
    HP_SMART_CARD_INVALID_DEVICE_ID     = 17,       // Invalid device id
    // New results for Library 2.5
    HP_SMART_CARD_LIB_NOT_INITIALIZED   = 18,       // Need to initialize smart card library
} HP_SMART_CARD_result_t;

typedef enum
{
    HP_DBG_LEVEL_ERROR      = 0x00,                             // Errors. Always enabled
    HP_DBG_LEVEL_IFC_TRACE  = 0x01,                             // Interface API traces
    HP_DBG_LEVEL_AUTH_INFO  = 0x02,                             // Info on authentication
    HP_DBG_LEVEL_HSCC_INFO  = 0x04,                             // Info on Host Smart card  --- Not Used
    HP_DBG_LEVEL_HSCC_TRACE = 0x08,                             // host smart card chip trace
    HP_DBG_LEVEL_HSC_TRACE  = 0x10,                             // host smart card trace
    HP_DBG_LEVEL_SCP_TRACE  = 0x20,                             // smart card proto trace
    HP_DBG_LEVEL_SC_TRACE   = 0x40,                             // smart card trace
    HP_DBG_LEVEL_CUSTOMER   = 0x80,                             // customer trace

    HP_DEBUG_LEVEL_DEFAULT  = HP_DBG_LEVEL_ERROR,
} HP_DEBUG_level_t;


//-----------------------------------------------------------------------------
// Library Function Prototypes
//-----------------------------------------------------------------------------

//*****************************************************************************
// LIB_HP_SMART_CARD_get_version -
//-----------------------------------------------------------------------------
// Description - This function returns smart card library version.
//
// Inputs: none
//
// Return: Smart card library version as NULL terminated ASCII string ("Major.Minor")
//         Example: "1.0"
//                                                              hp_smart_card.c
//*****************************************************************************
extern const char* LIB_HP_SMART_CARD_get_version(void);

//*****************************************************************************
// LIB_HP_SMART_CARD_init -
//-----------------------------------------------------------------------------
// Description - Initialize HP smart card library. This function should be
//               called at startup and before invoking any other smart card API.
//               Low level I2C and GPIO libraries should initialized before
//               invoking this API.
//
// Inputs: none
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_init(void);

//*****************************************************************************
// LIB_HP_SMART_CARD_shutdown -
//-----------------------------------------------------------------------------
// Description - Shutdown HP smart card library. Flushes smart card cache,
//               cleanup.
//
// Inputs: none
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_shutdown(void);

//*****************************************************************************
// LIB_HP_SMART_CARD_device_init -
//-----------------------------------------------------------------------------
// Description - Intitializes the HP Component smart card device. Device to initialize is
//               identified by the device_id. This function initializes cache,
//               verifies data integrity (parity). This function should be
//               called to initialize each smart card device.
//
// Inputs: device_id of the device to initialize.
//
// Return: HP_SMART_CARD_result_t code.
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_device_id_t device_id);

//*****************************************************************************
// LIB_HP_SMART_CARD_device_present -
//-----------------------------------------------------------------------------
// Description - This API can be called to detect the presence of HP component
//               smart card device. This can be useful in troubleshooting.
//
// Inputs: device_id - device id of smart card to check for
//
// Return: HP_SMART_CARD_OK or HP_SMART_CARD_ABSENT, or specific smart card error.
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_device_id_t device_id);

//*****************************************************************************
// LIB_HP_SMART_CARD_device_close -
//-----------------------------------------------------------------------------
// Description - Flushes smart card cache and invalidates device.
//               Requires new device_present and device_init calls.
//
// Inputs: device_id - device id of smart card to close
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_close(HP_SMART_CARD_device_id_t device_id);

//*****************************************************************************
// LIB_HP_SMART_CARD_flush -
//-----------------------------------------------------------------------------
// Description - Flushes all smart card device caches (if needed).
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_flush(void);

//*****************************************************************************
// LIB_HP_SMART_CARD_read_field -
//-----------------------------------------------------------------------------
// Description - Read a smart card field identified by "field" from the device
//               identified by device_id.
//
// Inputs: device_id - device id of the smart card to read.
//         field     - which field to read
//         size      - size of data type pointed by "dst".
//         dst       - value read from the smart card field will be filled
//                     in this buffer.
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read_field(HP_SMART_CARD_device_id_t device_id,
                                                           HP_SMART_CARD_field_t field,
                                                           uint8_t size,
                                                           void                      *dst);

//*****************************************************************************
// LIB_HP_SMART_CARD_read_string -
//-----------------------------------------------------------------------------
// Description - Read a smart card string identified by "field" from the device
//               identified by device_id.
//
// Inputs: device_id   - device id of the smart card to read.
//         first_field - first field of the string
//         length      - length of string (number of consecutive fields)
//         dst         - value read from the smart card field will be filled
//                       in this buffer.
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read_string(HP_SMART_CARD_device_id_t device_id,
                                                            HP_SMART_CARD_field_t first_field,
                                                            uint8_t length,
                                                            void                      *dst);

//*****************************************************************************
// LIB_HP_SMART_CARD_write_field -
//-----------------------------------------------------------------------------
// Description - Write a smart card field identified by "field" from the device
//               identified by device_id
//
// Inputs: device_id - device id of the smart card to read.
//         field     - which field to write
//         size      - size of dst
//         src       - value read from this buffer will be written to smart card field.
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_write_field(HP_SMART_CARD_device_id_t device_id,
                                                            HP_SMART_CARD_field_t field,
                                                            uint8_t size,
                                                            void                      *src);

//*****************************************************************************
// LIB_HP_SMART_CARD_write_string -
//-----------------------------------------------------------------------------
// Description - Write a smart card field identified by "field" from the device
//               identified by device_id
//
// Inputs: device_id   - device id of the smart card to read.
//         first_field - first field of the string
//         length      - length of string (number of consecutive fields)
//         src         - value read from this buffer will be written to smart card string.
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_write_string(HP_SMART_CARD_device_id_t device_id,
                                                             HP_SMART_CARD_field_t first_field,
                                                             uint8_t length,
                                                             void                      *src);

//*****************************************************************************
// LIB_HP_SMART_CARD_read -
//-----------------------------------------------------------------------------
// Description - Read raw data from smart card starting from smart card address "start",
//               reads "size" number of bytes.
//
// Inputs: device_id - device id of the smart card to read.
//         start     - start address of the smart card to read from.
//         size      - Number of bytes to read. Valid range 0 - 254.
//         dst       - value read from the smart card field will be filled
//                     in this buffer.
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read(HP_SMART_CARD_device_id_t device_id,
                                                     uint8_t start,
                                                     uint8_t size,
                                                     void                      *dst);

//*****************************************************************************
// LIB_HP_SMART_CARD_change_partition -
//-----------------------------------------------------------------------------
// Description - Changes a partition type for a smart card device.
//               The partition needs to be of type C_MODE_RW2RO
//               And the new value needs to be C_MODE_RO
//
// Inputs: device_id - device id of the smart card device
//         id        - partition identifier
//         new_access_mode (C_MODE_RO is the only value that will work)
//
// Return: HP_SMART_CARD_result_t
//                                                              hp_smart_card.c
//*****************************************************************************
extern HP_SMART_CARD_result_t LIB_HP_SMART_CARD_change_RW_partition_to_RO(HP_SMART_CARD_device_id_t device_id,
                                                                          uint8_t id);

//*****************************************************************************
// LIB_HP_SMART_CARD_register_cache_monitor_callback
//-----------------------------------------------------------------------------
// Description - Register for a callback that will be invoked if smart card
//               library encounters an error during cache flush.
//
//
// Inputs: flush_error_callback_func - Function pointer that would be invoked
//                                     when cache flush fails.
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_register_cache_monitor_callback(void (*callback_func)(HP_SMART_CARD_device_id_t,
                                                                                    HP_SMART_CARD_result_t));

//                                                              hp_smart_card.c
extern void CACHE_MONITOR_impl(HP_SMART_CARD_device_id_t device_id, HP_SMART_CARD_result_t result);

//*****************************************************************************
// LIB_HP_SMART_CARD_set_log_mask -
//-----------------------------------------------------------------------------
// Description - This function can be used to control the debug messages that will
//               appear in debug log. This is to avoid flooding debug log with
//               log messages. Every debug message has an area level associated with it.
//               Debug message will be logged only if the corresponding
//               mask is set. By default only the debug messages at HP_DBG_LEVEL_ERROR
//               are enabled.
//
//               /* Following line causes all the messages which are having
//                  level equal to HP_DBG_MASK_IFC_TRACE OR HP_DBG_MASK_ERROR
//                  appear in log. */
//               LIB_HP_SMART_CARD_set_log_mask (HP_DBG_LEVEL_IFC_TRACE);
//
// Inputs: level - area level of the message to be enabled.
//
// Return: None
//                                                              hp_debug.log.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_set_log_mask(HP_DEBUG_level_t category);

//*****************************************************************************
// LIB_HP_SMART_CARD_clear_log_mask -
//-----------------------------------------------------------------------------
// Description - This function can be used to disable any debug levels except
//               debug messages at level HP_DBG_MASK_ERROR.
//
//               /* Following line disables the debug messages at level
//                  HP_DBG_MASK_IFC_TRACE. */
//               LIB_HP_SMART_CARD_clear_log_mask (HP_DBG_LEVEL_IFC_TRACE);
//
// Inputs: mask - area level of the message to be disabled
//
// Return: None
//                                                              hp_debug.log.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_clear_log_mask(HP_DEBUG_level_t category);

//*****************************************************************************
// LIB_HP_SMART_CARD_set_log_depth -
//-----------------------------------------------------------------------------
// Description - This function is an alternate way to control the debug messages
//               that will appear in debug log. Each message also has a numeric
//               depth associated with the message.  Higher numbers will log more
//               detailed information.
//
//               A message will be logged if it matches the log MASK or if it
//               is at the logging depth (or lower).
//
//               0 (default) -> log top level errors
//               1 -> add logging of entry/exit of LIB calls and messages
//               2 -> add logging of low level errors
//               3 -> add logging of entry/exit of primary routines and messages
//               4 -> add logging of data buffers
//               5 -> add logging of I2C/GPIO calls if enabled in customer code
//               255 -> log everything
// Inputs: depth - depth of the messages to be enabled.
//
// Return: None
//                                                              hp_debug.log.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_set_log_depth(uint8_t depth);

//*****************************************************************************
// LIB_HP_SMART_CARD_register_assert_callback -
//-----------------------------------------------------------------------------
// Description - Register a callback for assert notification. HP_ASSERT() macro
//               is used inside smart card library to detect assertion failures.
//               HP_ASSERT() logs the file name and line number of the failed
//               assertion. If a callback is registered using this API, then the
//               callback will be invoked to notify the assert event.
//               Only one callback can be registered at a time. If this function
//               is called multiple times, only the last callback will be notified.
//
//
// Inputs: assert_callback_func - Function pointer to assert handler.
//
// Return: None
//                                                              hp_assert.c
//*****************************************************************************
extern void LIB_HP_SMART_CARD_register_assert_callback(void (*assert_callback_func)(const char *error_str));

//*****************************************************************************
// LIB_HP_SMART_CARD_result_string -
//-----------------------------------------------------------------------------
// Description - Convert HP_SMART_CARD_result_t to string.
//
// Inputs: none
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern const char* LIB_HP_SMART_CARD_result_string(int result);

//*****************************************************************************
// LIB_HP_SMART_CARD_last_status -
//-----------------------------------------------------------------------------
// Description - Get detailed status from the last LIB_HP_SMART_CARD call.
//
// Inputs: none
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern HW_SMART_CARD_status_t LIB_HP_SMART_CARD_last_status(void);

//*****************************************************************************
// LIB_HP_SMART_CARD_status_string -
//-----------------------------------------------------------------------------
// Description - Convert HW_SMART_CARD_status_t to string.
//
// Inputs: none
//
// Return: none
//                                                              hp_smart_card.c
//*****************************************************************************
extern const char* LIB_HP_SMART_CARD_status_string(int status);
#endif /* _LIB_HP_SMART_CARD_H_ */
