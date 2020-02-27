/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// hp_smart_card.c
//-----------------------------------------------------------------------------
// Description: This file is broken into several distinct layers...
//
//*****************************************************************************

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#define INCLUDE_SMART_CARD_HEADER_FIELD_DATA
#include "hp_smart_card_types.h"
#undef INCLUDE_SMART_CARD_HEADER_FIELD_DATA

#define INCLUDE_HP_SMART_CARD_LAYOUT_DATA
#include "hp_smart_card_layout.h"
#undef INCLUDE_HP_SMART_CARD_LAYOUT_DATA

#include "hp_types.h"
#include "hp_assert.h"
#include "hp_debug_log_internal.h"
#include "hp_os_service.h"
#include "carnac_types.h"
#include "hp_generic_macros.h"
#include "hp_smart_card_version.h"

#include "hw_smart_card.h"
#include "hw_smart_card_private.h"

#include "hp_smart_card_config.h"
#include "hp_host_smart_card_ifc.h"
#include "hp_host_smart_card.h"
#include "../hp_host_smart_card/hp_host_smart_card.h"
#include "../hp_host_smart_card/hp_host_smart_card_ifc.h"

#include <stdio.h>

#define DBG_SMART_CARD_ID           "SC"

#define NUM_SMART_CARD_DEVICES      HP_SMART_CARD_DEVICE_NUM_DEVICES
//-----------------------------------------------------------------------------
// Type Definitions (enums)
//-----------------------------------------------------------------------------
typedef enum
{
    SMART_CARD_MODE_NONE    = 0x00,                                         // none of below
    SMART_CARD_MODE_CACHE   = 0x01,                                         // use cache for read/write   - cache layer
    SMART_CARD_MODE_TRANS   = 0x02,                                         // use transactions for write - trans layer
    SMART_CARD_MODE_ALL     = 0x03,                                         // all of the above
    SMART_CARD_MODE_DEFAULT = 0x03,
} SMART_CARD_mode_t;

typedef enum
{
    ACU_DBG_EXCEPTION   = 5,                                                    /* Error reports     */
    ACU_DBG_INFO        = 10,                                                   /* Information       */
    ACU_DBG_TRACE       = 15,                                                   /* Rudimentery trace */
    ACU_DBG_TRACE2      = 20,                                                   /* Detailed trace    */
} SMART_CARD_debug_level_t;

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
// *************************** WARNING ****************************************
// Only one thread should be writing to all component devices, since the code
// can only support a single waiting write (there is no way to target
// specific callers when signalling completion).
// *************************** WARNING ****************************************

typedef struct
{
    int     size;
    uint8_t data[MAX_SMART_CARD_SIZE];
} cache_info_t;

typedef HP_SMART_CARD_device_id_t   HP_SMART_CARD_id_t;

//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------

static HP_MUTEX_ID              _pub_cache_lock[NUM_SMART_CARD_DEVICES];
static HP_MUTEX_ID              _smart_card_mutex;
static int                      _last_flush_device = -1;
static uint8_t                  smart_card_shadow[NUM_SMART_CARD_DEVICES][MAX_SMART_CARD_SIZE];
static cache_info_t             pub_cache[NUM_SMART_CARD_DEVICES];
static cache_info_t             priv_cache;
static void                     (*_cache_monitor_callback_func)(HP_SMART_CARD_device_id_t, HP_SMART_CARD_result_t) = NULL;

static HW_SMART_CARD_status_t   _last_status;

static unsigned char            _initialized = 0;
static HP_thread_t              _thread;


//-----------------------------------------------------------------------------
// Status to string translation table
//-----------------------------------------------------------------------------

// NOTE: This table must match values in HP_SMART_CARD_result_t(hp_smart_card.h) used as index.
static const char* ResultToString[] =
{
    "HP_SMART_CARD_OK",
    "HP_SMART_CARD_NOT_INITIALIZED",
    "HP_SMART_CARD_ERROR",
    "HP_SMART_CARD_DEVICE_ABSENT",
    "HP_SMART_CARD_WRITE_ERROR",
    "HP_SMART_CARD_READ_ERROR",
    "HP_SMART_CARD_DATA_NOT_VALID",
    "HP_SMART_CARD_BAD_SIZE_FOR_TAG",
    "HP_SMART_CARD_TAG_NOT_FOUND",
    "HP_SMART_CARD_DUPLICATE_TAG",
    "HP_SMART_CARD_BAD_CHIP_ID",
    "HP_SMART_CARD_INVALID_CHIP_TAG",
    "HP_SMART_CARD_INVALID_FAMILY_ID",
    "HP_SMART_CARD_ILLEGAL_ACCESS",
    "HP_SMART_CARD_CACHE_NOT_FLUSHED",
    "HP_SMART_CARD_BAD_TOKEN",
    "HP_SMART_CARD_BAD_WRITE_PROT",
    "HP_SMART_CARD_INVALID_DEVICE_ID",
    // New results for Library 2.5
    "HP_SMART_CARD_LIB_NOT_INITIALIZED",
};

// NOTE: This table must match values in HW_SMART_CARD_status_t(hw_smart_card_private.h) used as index.
static const char* StatusToString[] =
{
    "",                                                 // show success as an empty string, since also used for "no additional status"
    "HW_SMART_CARD_not_auth_e",
    "HW_SMART_CARD_failed_e",
    "HW_SMART_CARD_illegal_access_e",
    "HW_SMART_CARD_memory_error_e",
    "HW_SMART_CARD_invalid_param_e",
    "HW_SMART_CARD_invalid_change_e",
    "HW_SMART_CARD_invalid_crc_e",
    /* protocol failure */
    "HW_SMART_CARD_proto_failure_e",
    "HW_SMART_CARD_rsp_len_too_long_e",
    "HW_SMART_CARD_rsp_len_mismatch_e",
    /* transport failed */
    "HW_SMART_CARD_xpt_not_set_failure_e",
    "HW_SMART_CARD_xpt_cmd_failed_e",
    "HW_SMART_CARD_xpt_rsp_failed_e",
    "HW_SMART_CARD_xpt_no_device_e",
    /* some auxilary library failed */
    "HW_SMART_CARD_aux_failure_e",
    /* mac could not be verified */
    "HW_SMART_CARD_mac_verification_failure_e",
    "HW_SMART_CARD_high_security_mac_rejected_e",
    /* device id revoked */
    "HW_SMART_CARD_device_revoked_id_e",
    /* device id changed unexpectedly */
    "HW_SMART_CARD_unexpected_device_change_e",
    /* unknown failure */
    "HW_SMART_CARD_unknown_failure_e",

    /* retry - for stats purposes */
    "HW_SMART_CARD_retry_e",

    /* null pointer error (to help caller) */
    "HW_SMART_CARD_null_pointer_e",
    /* get acumen type returns unknow device type*/
    "HW_SMART_CARD_invalid_device_type_e",

    /* not forward compatible */
    "HW_SMART_CARD_nfc_key_e",
    /* not compatible */
    "HW_SMART_CARD_incompatible_key_e",
};


//-----------------------------------------------------------------------------
// Defines/Macros
//-----------------------------------------------------------------------------

#define SMART_CARD_MAX_NUM_OF_TAGS    16


static HP_SMART_CARD_result_t LIB_HP_SMART_CARD_raw_read_field(HP_SMART_CARD_device_id_t device_id,
                                                               HP_SMART_CARD_field_t field,
                                                               int size,
                                                               void            *dst);

//-----------------------------------------------------------------------------
// Tag Layer Function Prototypes
//-----------------------------------------------------------------------------
static void            smart_card_tag_init(void);

static HP_SMART_CARD_result_t smart_card_tag_device_init(HP_SMART_CARD_id_t device,
                                                         int device_size);

static HP_SMART_CARD_result_t smart_card_tag_set_header(HP_SMART_CARD_id_t device,
                                                        int size,
                                                        int num_fields,
                                                        const HP_SMART_CARD_field_desc_t *field_desc);

static HP_SMART_CARD_result_t smart_card_tag_set_tag_desc(HP_SMART_CARD_id_t device,
                                                          HP_SMART_CARD_tag_t tag_id,
                                                          int size,
                                                          int num_fields,
                                                          int last_field,
                                                          const HP_SMART_CARD_field_desc_t *field_desc,
                                                          HP_SMART_CARD_part_mode_t mode);

static HP_SMART_CARD_result_t smart_card_tag_read(HP_SMART_CARD_id_t device,
                                                  HP_SMART_CARD_field_t field,
                                                  int size,
                                                  void           *dst,
                                                  SMART_CARD_mode_t mode);

static HP_SMART_CARD_result_t smart_card_string_read(HP_SMART_CARD_id_t device,
                                                     HP_SMART_CARD_field_t first_field,
                                                     int length,
                                                     void           *dst,
                                                     SMART_CARD_mode_t mode);

static HP_SMART_CARD_result_t smart_card_tag_write(HP_SMART_CARD_id_t device,
                                                   HP_SMART_CARD_field_t field,
                                                   int size,
                                                   void           *src,
                                                   SMART_CARD_mode_t mode);

static HP_SMART_CARD_result_t smart_card_string_write(HP_SMART_CARD_id_t device,
                                                      HP_SMART_CARD_field_t first_field,
                                                      int length,
                                                      void           *src,
                                                      SMART_CARD_mode_t mode);


//-----------------------------------------------------------------------------
// cache layer function prototypes
//-----------------------------------------------------------------------------
static void            smart_card_cache_init(void);

static HP_SMART_CARD_result_t smart_card_cache_device_init(HP_SMART_CARD_id_t device,
                                                           int size);

static HP_SMART_CARD_result_t smart_card_cache_write(HP_SMART_CARD_id_t device,
                                                     int addr,
                                                     int size,
                                                     void          *src,
                                                     SMART_CARD_mode_t mode);

static HP_SMART_CARD_result_t smart_card_cache_read(HP_SMART_CARD_id_t device,
                                                    int addr,
                                                    int size,
                                                    void          *dst,
                                                    SMART_CARD_mode_t mode,
                                                    HP_SMART_CARD_bool_t cache_flushing);

static HP_SMART_CARD_result_t flush_pending_for_threading_mode(void);

static HP_SMART_CARD_result_t _flush_all_pending(void);

static HP_SMART_CARD_result_t _flush_device(int device);

static HP_SMART_CARD_result_t smart_card_cache_flush(HP_SMART_CARD_id_t device);


//-----------------------------------------------------------------------------
// transaction layer function prototypes
//-----------------------------------------------------------------------------
static void            smart_card_trans_init(void);

static HP_SMART_CARD_result_t smart_card_trans_device_init(HP_SMART_CARD_id_t device,
                                                           int parity_off,
                                                           HP_SMART_CARD_bool_t update_shadow);

static HP_SMART_CARD_result_t smart_card_trans_write(HP_SMART_CARD_id_t device,
                                                     int addr,
                                                     int size,
                                                     uint8_t          *src,
                                                     SMART_CARD_mode_t mode,
                                                     HP_SMART_CARD_bool_t cache_flushing);

static HP_SMART_CARD_result_t smart_card_trans_read(HP_SMART_CARD_id_t device,
                                                    int addr,
                                                    int size,
                                                    void          *dst,
                                                    SMART_CARD_mode_t mode,
                                                    HP_SMART_CARD_bool_t cache_flushing);


//-----------------------------------------------------------------------------
// raw layer function prototypes
//-----------------------------------------------------------------------------
static void              smart_card_raw_init(void);

static HP_SMART_CARD_result_t smart_card_raw_present(HP_SMART_CARD_id_t device);

static HP_SMART_CARD_result_t smart_card_raw_write(HP_SMART_CARD_id_t device,
                                                   int addr,
                                                   int size,
                                                   void          *src,
                                                   SMART_CARD_mode_t mode,
                                                   HP_SMART_CARD_bool_t smart_card_shadow_locked,
                                                   HP_SMART_CARD_bool_t cache_flushing);

static HP_SMART_CARD_result_t smart_card_raw_read(HP_SMART_CARD_id_t device,
                                                  int addr,
                                                  int size,
                                                  void          *dst,
                                                  SMART_CARD_mode_t mode,
                                                  HP_SMART_CARD_bool_t cache_flushing);

//*****************************************************************************
//*****************************************************************************
//                              SMART CARD DEVICE LAYER
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Local types
//-----------------------------------------------------------------------------
typedef enum
{
    HP_SMART_CARD_STATE_UNINITIALIZED,
    HP_SMART_CARD_STATE_INITIALIZED,
} HP_SMART_CARD_state_t;

typedef enum
{
    _DEVICE_STATE_UNINITIALIZED,
    _DEVICE_STATE_PRESENT,
    _DEVICE_STATE_MISSING,
} _device_state_t;

typedef struct
{
    uint8_t num_bytes;
    uint8_t start_byte;
    uint8_t shift;
    uint8_t mask_index;
} field_info_t;

typedef struct
{
    uint8_t                     start_addr;
    uint8_t                     end_addr;
    HP_SMART_CARD_bool_t        dirty;
    HP_SMART_CARD_part_mode_t   access_mode;
} tag_info_t;

typedef struct
{
    uint8_t         header_size;
    uint8_t         device_size;
    field_info_t    field_info[HP_SMART_CARD_NUM_FIELD_TYPES];
} template_info_t;

#define bswap_32(x)    ((((x) & 0xFF000000) >> 24) | \
                        (((x) & 0x00FF0000) >> 8) |  \
                        (((x) & 0x0000FF00) << 8) |  \
                        (((x) & 0x000000FF) << 24))

static tag_info_t   pub_tag_info[NUM_SMART_CARD_DEVICES][SMART_CARD_MAX_NUM_OF_TAGS];
static tag_info_t   priv_tag_info[SMART_CARD_MAX_NUM_OF_TAGS];

//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------
static const HP_SMART_CARD_device_info_t *_find_device_info_for(HP_SMART_CARD_id_t device);

static HP_SMART_CARD_result_t _set_tag_desc(HP_SMART_CARD_id_t device);


static HP_SMART_CARD_result_t _xlat_hw_result(HW_SMART_CARD_status_t hw_result,
                                              HP_SMART_CARD_result_t def_val);

static HP_SMART_CARD_result_t _check_lib_init();
static HP_SMART_CARD_result_t _check_lib_and_device_id(HP_SMART_CARD_id_t device);
static HP_SMART_CARD_result_t _check_lib_and_device_init(HP_SMART_CARD_id_t device);

static HP_SMART_CARD_bool_t _device_config_ok(int device);

static void *_cache_monitor(void *arg);

//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------
static HP_SMART_CARD_state_t    smart_card_state [NUM_SMART_CARD_DEVICES];

static template_info_t          template_info[NUM_SMART_CARD_DEVICES];

static uint32_t                 mask[33] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000F,
    0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
    0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
    0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
    0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
    0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
    0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
    0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};


//*****************************************************************************
// _check_lib_init -
//-----------------------------------------------------------------------------
// Description - returns HP_SMART_CARD_OK if the library has been initialized;
//               otherwise, returns an error code.
//*****************************************************************************
static HP_SMART_CARD_result_t _check_lib_init()
{
    if (1 != _initialized)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 0,
                        "ERROR: Library has not been initialized\n");
        return HP_SMART_CARD_LIB_NOT_INITIALIZED;
    }
    return HP_SMART_CARD_OK;
}


//*****************************************************************************
// _check_lib_and_device_id -
//-----------------------------------------------------------------------------
// Description - returns HP_SMART_CARD_OK if the library has been initialized
//               and device ID is valid; otherwise, returns an error code.
//*****************************************************************************
static HP_SMART_CARD_result_t _check_lib_and_device_id(HP_SMART_CARD_id_t device)
{
    HP_SMART_CARD_result_t result = _check_lib_init();
    if (HP_SMART_CARD_OK != result)
        return result;

    // check for invalid device
    if (device >= HP_SMART_CARD_DEVICE_NUM_DEVICES)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 0,
                        "ERROR: Invalid device ID (%d)\n", device);
        return HP_SMART_CARD_INVALID_DEVICE_ID;
    }
    return HP_SMART_CARD_OK;
}


//*****************************************************************************
// _check_lib_and_device_init -
//-----------------------------------------------------------------------------
// Description - returns HP_SMART_CARD_OK if the library and device have been
//               initialized; otherwise, returns an error code.
//*****************************************************************************
static HP_SMART_CARD_result_t _check_lib_and_device_init(HP_SMART_CARD_id_t device)
{
    HP_SMART_CARD_result_t result = _check_lib_and_device_id(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    // call more detailed check
    if (_device_config_ok(device))
    {
        return HP_SMART_CARD_OK;
    }
    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_ERROR, 0,
                    "ERROR: Device %d has not been initialized\n", device);
    return HP_SMART_CARD_NOT_INITIALIZED;
}


//*****************************************************************************
// _device_config_ok -
//-----------------------------------------------------------------------------
// Description - returns TRUE if the configuration is OK for accessing this
//               device
//
// Inputs: device
//
// Return: TRUE if OK to access this device, FALSE otherwise
//
//*****************************************************************************
static HP_SMART_CARD_bool_t _device_config_ok(int device)
{
    if (HP_SMART_CARD_STATE_INITIALIZED == smart_card_state[device])
        return HP_SMART_CARD_TRUE;
    return HP_SMART_CARD_FALSE;
}

//*****************************************************************************
// LIB_HP_SMART_CARD_get_version -
//-----------------------------------------------------------------------------
// Description - This function returns smart card library version.
//
// Inputs: none
//
// Return: Smart card library version as NULL terminated ASCII string ("Major.Minor")
//
//*****************************************************************************
const char* LIB_HP_SMART_CARD_get_version(void)
{
    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "LIB_HP_SMART_CARD_get_version(): Version = %s\n", HP_SMART_CARD_LIB_VERSION);

    return HP_SMART_CARD_LIB_VERSION;
}

//*****************************************************************************
// LIB_HP_SMART_CARD_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
void LIB_HP_SMART_CARD_init(void)
{
    int i;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_init():\n");

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    /* Display library revision here */
    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Smart Card library version: %s\n", LIB_HP_SMART_CARD_get_version());

    /* If this function is called multiple times without a corresponding shutdown,
       ignore the call.
     */
    if (1 == _initialized)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID,
                        HP_DBG_LEVEL_IFC_TRACE, 1,
                        "Exiting LIB_HP_SMART_CARD_init(): Called multiple times\n");
        return;
    }
    else
    {
        _initialized = 1;
    }

    /* Initialise host smart card */
    hp_host_smart_card_init();

    /* Initialize HW layer */
    hw_smart_card_init();

    /* Initialize the lower levels appropriately */
    smart_card_raw_init();
    smart_card_trans_init();
    smart_card_cache_init();
    smart_card_tag_init();

    /* Now initialize the local data for this layer */
    for (i = 0; i < NUM_SMART_CARD_DEVICES; i++)
    {
        smart_card_state[i] = HP_SMART_CARD_STATE_UNINITIALIZED;
    }

#ifndef SINGLE_THREADED
    /* Create cache monitor thread */
    HP_thread_create(&_thread, _cache_monitor, NULL);
#endif

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_init():\n");
}

//*****************************************************************************
// LIB_HP_SMART_CARD_shutdown -
//-----------------------------------------------------------------------------
// Description - Shutdown HP smart card library. Flushes smart card cache,
//               cleanup.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
void LIB_HP_SMART_CARD_shutdown(void)
{
    int32_t i;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_shutdown():\n");

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    if (1 == _initialized)
    {
        /* Mark all the devices as uninitialized */
        for (i = 0; i < NUM_SMART_CARD_DEVICES; i++)
        {
            smart_card_state[i] = HP_SMART_CARD_STATE_UNINITIALIZED;
        }

        /* flush any pending data */
        flush_pending_for_threading_mode();

        // cancel the cache monitor thread
        HP_thread_cancel(_thread);
    }

    // allow LIB_HP_SMART_CARD_init to execute
    _initialized = 0;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_shutdown()\n");
}


//*****************************************************************************
// LIB_HP_SMART_CARD_device_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_device_id_t device_id)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    int                     parity_offset;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_device_init(): device_is = %d\n", device_id);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_id(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    // Calculate the parity offset.  The parity block is at the end of the part
    parity_offset = MAX_SMART_CARD_SIZE - PARITY_BLOCK_SIZE;

    // Initialize the smart card layers
    if (result == HP_SMART_CARD_OK)
    {
        // Read the entire part into the smart card "shadow" memory.
        // Calculate and verify the parity of the device being initialized
        result = smart_card_trans_device_init(device, parity_offset, TRUE);
    }

    if (result == HP_SMART_CARD_OK)
    {
        // Read the entire part into the smart card "cache" memory
        result = smart_card_cache_device_init(device, parity_offset);
    }

    if (result == HP_SMART_CARD_OK)
    {
        // Zero initialize the tag data structure
        result = smart_card_tag_device_init(device, parity_offset);
    }

    // Now setup the smart card tag descriptions for the device (allow field access).
    // This is for both the header and the tag fields.
    // Also validates the device (chip tag and family id).
    if (result == HP_SMART_CARD_OK)
    {
        result = _set_tag_desc(device);
    }

    /* Set state information for this smart card */
    if (result == HP_SMART_CARD_OK)
    {
        smart_card_state[device] = HP_SMART_CARD_STATE_INITIALIZED;
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 1,
                        "Device %d initialized successfully\n", device, 0);
    }
    else
    {
        smart_card_state[device] = HP_SMART_CARD_STATE_UNINITIALIZED;
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_ERROR, 0,
                        "ERROR - device %d init failure\n", device, 0);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_device_init(): result = %d\n", result);

    return(result);
}

//*****************************************************************************
// LIB_HP_SMART_CARD_change_partition -
//-----------------------------------------------------------------------------
// Description - Changes a partition type for a smart card device
//               The partition needs to be of type C_MODE_RW2RO
//               And the new value needs to be C_MODE_RO
//
// Inputs: color ([PT_YELLOW|PT_BLACK|PT_CYAN|...])
//         id    (0 - N)
//         access_mode (C_MODE_RO is the only value that will work)
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_change_RW_partition_to_RO(HP_SMART_CARD_device_id_t device_id,
                                                                   unsigned char id)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result  = HP_SMART_CARD_OK;
    HW_SMART_CARD_status_t  status  = HW_SMART_CARD_failed_e;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_change_partition(): device_id = %d, partition_id = %d\n",
                    device_id, id);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (_device_config_ok(device))
    {
        if (result == HP_SMART_CARD_OK)
        {
            status = HW_SMART_CARD_change_partition(&hw_smart_card_device[device], id, C_MODE_RO);
        }
    }
    else
    {
        return(HP_SMART_CARD_NOT_INITIALIZED);
    }

    result = _xlat_hw_result(status, HP_SMART_CARD_ERROR);
    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_change_partition(): result = %d\n",
                    result);
    return(result);
}



//*****************************************************************************
// LIB_HP_SMART_CARD_device_present -
//-----------------------------------------------------------------------------
// Description - Abreviated smart card validation to determine if a smart card
//               device is present.
//
// Inputs: device - id of smart card to check for
//
// Return: HP_SMART_CARD_OK or HP_SMART_CARD_ABSENT, or specific smart card error.
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_device_id_t device_id)
{
    HP_SMART_CARD_id_t          device;
    HP_SMART_CARD_result_t      result = HP_SMART_CARD_OK;

    HP_HOST_SMART_CARD_return_t hsc_result = HP_HOST_SMART_CARD_ok_e;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_device_present(): device_id = %d\n",
                    device_id);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    /* If the request is to check the presence of host smart card */
    if (HP_SMART_CARD_DEVICE_HOST == device)
    {
        hsc_result = HP_HOST_SMART_CARD_echo((uint8_t *) "Hello", sizeof("Hello"));
        if (HP_HOST_SMART_CARD_ok_e == hsc_result)
        {
            result = HP_SMART_CARD_OK;
        }
        else
        {
            result = HP_SMART_CARD_DEVICE_ABSENT;
        }
    }
    else
    {
        /* If we get initialize request on an invalid device */
        if (device >= HP_SMART_CARD_DEVICE_NUM_DEVICES)
        {
            result = HP_SMART_CARD_INVALID_DEVICE_ID;
        }

        if (result == HP_SMART_CARD_OK)
        {
            result =
                (smart_card_raw_present(device) == HP_SMART_CARD_OK) ?
                HP_SMART_CARD_OK :
                HP_SMART_CARD_DEVICE_ABSENT;
        }
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_device_present(): result = %d\n",
                    result);
    return(result);
}


//*****************************************************************************
// LIB_HP_SMART_CARD_device_close -
//-----------------------------------------------------------------------------
// Description - Flushes smart card cache and invalidates device.
//               Requires new device_present and device_init calls.
//
// Inputs: device_id - device id of smart card to close
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_device_close(HP_SMART_CARD_device_id_t device_id)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_device_close(): device_id = %d\n", device_id);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_id(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    /* flush any pending device data */
    flush_pending_for_threading_mode();

    // Mark device as uninitialized
    smart_card_state[device] = HP_SMART_CARD_STATE_UNINITIALIZED;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_device_close(): result = %d\n", result);

    return(result);
}


//*****************************************************************************
// LIB_HP_SMART_CARD_flush -
//-----------------------------------------------------------------------------
// Description - Flushes all smart card device caches that need flushed.
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_flush()
{
    HP_SMART_CARD_result_t result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_flush()\n");

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    // flush any pending deviced
    result = flush_pending_for_threading_mode();

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Exiting LIB_HP_SMART_CARD_flush()\n", result);

    return(result);
}

HP_SMART_CARD_result_t flush_pending_for_threading_mode()
{
    HP_SMART_CARD_result_t result = HP_SMART_CARD_OK;

#ifndef SINGLE_THREADED
    // MULTI THREADED - trigger flush (should not be necessary, but) and wait for completion
    flushq_will_wait_for_flush_complete();
    flushq_trigger_flush();
    result = flushq_wait_for_flush_complete(-1);
#else
    // SINGLE THREADED - flush device
    result = _flush_all_pending();
#endif
    return(result);
}



//*****************************************************************************
// LIB_HP_SMART_CARD_read_field -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read_field(HP_SMART_CARD_device_id_t device_id,
                                                    HP_SMART_CARD_field_t field,
                                                    unsigned char size,
                                                    void                      *dst)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_read_field(): device_id = %d, field = %d\n",
                    device_id, field);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (NULL == dst)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        if (_device_config_ok(device))
        {
            result = smart_card_tag_read(device, field, size, dst, SMART_CARD_MODE_DEFAULT);
        }
        else
        {
            result = HP_SMART_CARD_NOT_INITIALIZED;
        }
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_read_field(): result = %d\n",
                    result);

    return(result);
}


//*****************************************************************************
// LIB_HP_SMART_CARD_read_string -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read_string(HP_SMART_CARD_device_id_t device_id,
                                                     HP_SMART_CARD_field_t field,
                                                     unsigned char size,
                                                     void                      *dst)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_read_string(): device_id = %d, field = %d\n",
                    device_id, field);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (NULL == dst)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        result = smart_card_string_read(device, field, size, dst, SMART_CARD_MODE_DEFAULT);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_read_string(): result = %d\n",
                    result);

    return(result);
}


//*****************************************************************************
// LIB_HP_SMART_CARD_read_field -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_raw_read_field(HP_SMART_CARD_device_id_t device_id,
                                                        HP_SMART_CARD_field_t field,
                                                        int size,
                                                        void            *dst)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_raw_read_field(): device_id = %d, field = %d\n",
                    device_id, field);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    /* If we get initialize request on an invalid device */
    if (device >= HP_SMART_CARD_DEVICE_NUM_DEVICES)
    {
        result = HP_SMART_CARD_INVALID_DEVICE_ID;
    }

    if (NULL == dst)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        result = smart_card_tag_read(device, field, size, dst, SMART_CARD_MODE_NONE);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_raw_read_field(): result = %d\n",
                    result);

    return(result);
}


//*****************************************************************************
// LIB_HP_SMART_CARD_write_field -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_write_field(HP_SMART_CARD_device_id_t device_id,
                                                     HP_SMART_CARD_field_t field,
                                                     unsigned char size,
                                                     void                      *src)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_write_field(): device_id = %d, field = %d\n",
                    device_id, field);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (NULL == src)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        result = smart_card_tag_write(device, field, size, src, SMART_CARD_MODE_DEFAULT);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_write_field(): result = %d\n",
                    result);

    return(result);
}

//*****************************************************************************
// LIB_HP_SMART_CARD_write_string -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_write_string(HP_SMART_CARD_device_id_t device_id,
                                                      HP_SMART_CARD_field_t field,
                                                      unsigned char size,
                                                      void                      *src)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_write_string(): device_id = %d, field = %d\n",
                    device_id, field);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (NULL == src)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        result = smart_card_string_write(device, field, size, src, SMART_CARD_MODE_DEFAULT);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_write_string(): result = %d\n",
                    result);

    return(result);
}

//*****************************************************************************
// LIB_HP_SMART_CARD_read -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HP_SMART_CARD_result_t LIB_HP_SMART_CARD_read(HP_SMART_CARD_device_id_t device_id,
                                              unsigned char start,
                                              unsigned char size,
                                              void                      *dst)
{
    HP_SMART_CARD_id_t      device;
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Entering LIB_HP_SMART_CARD_read(): device_id = %d, start = %d\n",
                    device_id, start);

    _last_status = HW_SMART_CARD_success_e;                                                 // clear Last Status

    device = device_id;

    // check library and device init (errors will be logged)
    result = _check_lib_and_device_init(device);
    if (HP_SMART_CARD_OK != result)
        return result;

    if (NULL == dst)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;
    if (0 == size)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;
    if (254 < size)
        result = HP_SMART_CARD_ILLEGAL_ACCESS;

    if (result == HP_SMART_CARD_OK)
    {
        result = smart_card_cache_read(device, start, size, dst, SMART_CARD_MODE_DEFAULT, FALSE);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, (result ? 0 : 1),
                    "Exiting LIB_HP_SMART_CARD_read(): result = %d\n",
                    result);

    return(result);
}

//*****************************************************************************
// LIB_HP_SMART_CARD_register_cache_monitor_callback -
//-----------------------------------------------------------------------------
// Description - Register for cache monitor error callback.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
void LIB_HP_SMART_CARD_register_cache_monitor_callback(void (*callback_func)(HP_SMART_CARD_device_id_t,
                                                                             HP_SMART_CARD_result_t))
{
    _last_status = HW_SMART_CARD_success_e;                     // clear Last Status
    // OK to set callback to NULL
    _cache_monitor_callback_func = callback_func;
}

void CACHE_MONITOR_impl(HP_SMART_CARD_device_id_t device_id, HP_SMART_CARD_result_t result)
{
    /* If callback is registered, invoke callback */
    if (NULL != _cache_monitor_callback_func)
    {
        (*_cache_monitor_callback_func)(device_id, result);
    }
}

//*****************************************************************************
// LIB_HP_SMART_CARD_result_string -
//-----------------------------------------------------------------------------
// Description - Convert HP_SMART_CARD_result_t to string.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
const char* LIB_HP_SMART_CARD_result_string(int result)
{
    int count = sizeof(ResultToString) / sizeof(ResultToString[0]);
    if (result < 0 || result >= count)
        return "";
    return ResultToString[result];
}

//*****************************************************************************
// LIB_HP_SMART_CARD_last_status -
//-----------------------------------------------------------------------------
// Description - Get detailed status from the last LIB_HP_SMART_CARD call.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
HW_SMART_CARD_status_t LIB_HP_SMART_CARD_last_status(void)
{
    return _last_status;
}

//*****************************************************************************
// LIB_HP_SMART_CARD_status_string -
//-----------------------------------------------------------------------------
// Description - Convert HW_SMART_CARD_status_t to string.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
const char* LIB_HP_SMART_CARD_status_string(int status)
{
    int count = sizeof(StatusToString) / sizeof(StatusToString[0]);
    if (status < 0 || status >= count)
        return "";
    return StatusToString[status];
}

//*****************************************************************************
// _set_tag_desc -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t _set_tag_desc(HP_SMART_CARD_id_t device)
{
    HP_SMART_CARD_result_t              result;
    const HP_SMART_CARD_tag_desc_t      *tag_fields;
    int                                 i;
    int                                 nfields;
    HP_SMART_CARD_tag_t                 tag_id;
    HP_SMART_CARD_part_mode_t           mode;
    const HP_SMART_CARD_device_info_t   *device_info_p = NULL;
    int                                 last_field;

    // Set the smart card header format for the specified device
    result = smart_card_tag_set_header(device,
                                       N_HP_SMART_CARD_HEADER_BYTES,
                                       N_HP_SMART_CARD_HEADER_FIELDS,
                                       hp_smart_card_header_fields);
    if (result != HP_SMART_CARD_OK)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_ERROR, 0,
                        "_set_tag_desc: device %d  result = %d\n", device, result);
    }

    // Read chip tag and family ID and get pointer to device info
    device_info_p = _find_device_info_for(device);
    if (NULL == device_info_p)
    {
        return HP_SMART_CARD_BAD_CHIP_ID;
    }

    // Set the non-header tagged fields
    nfields     = device_info_p->num_tag_fields;
    tag_fields  = device_info_p->tag;
    last_field  = device_info_p->last_field;

    for (i = 0; (i < nfields) && (result == HP_SMART_CARD_OK); i++)
    {
        tag_id  = tag_fields[i].tag_id;
        mode    = device_info_p->tag[tag_id - 1].access_mode;

        result = smart_card_tag_set_tag_desc(device,
                                             tag_id,
                                             tag_fields[i].size,
                                             tag_fields[i].num_fields,
                                             last_field,
                                             tag_fields[i].field_desc,
                                             mode);
    }

    if (result != HP_SMART_CARD_OK)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_ERROR, 0,
                        "_set_tag_desc: device %d, tag %d, result %d\n", device, tag_fields[i].tag_id, result);
    }

    // Copy tag boundaries (start and end addr) to priv_tag_info
    memcpy(&priv_tag_info, &pub_tag_info[device], sizeof(pub_tag_info[device]));

    return(result);
}

//*****************************************************************************
// _find_device_info_for -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static const HP_SMART_CARD_device_info_t *_find_device_info_for(HP_SMART_CARD_id_t device)
{
    uint8_t chip_tag    = 0;
    uint8_t family_id   = 0;
    int     i;

    // Read chip tag
    if (smart_card_tag_read(device,
                            HP_SMART_CARD_CHIP_TAG,
                            sizeof(chip_tag),
                            &chip_tag,
                            SMART_CARD_MODE_DEFAULT) != HP_SMART_CARD_OK)
        return NULL;

    // Read family ID
    if (smart_card_tag_read(device,
                            HP_SMART_CARD_FAMILY_ID,
                            sizeof(family_id),
                            &family_id,
                            SMART_CARD_MODE_DEFAULT) != HP_SMART_CARD_OK)
        return NULL;

    // Validate chip tag and family ID
    for (i = 0; i < NUM_HP_SMART_CARD_DEVICES; i++)
    {
        if (hp_smart_card_device[i].chip_tag == chip_tag &&
            hp_smart_card_device[i].family_id == family_id)
            return(&(hp_smart_card_device[i]));
    }
    return NULL;
}

//*****************************************************************************
//*****************************************************************************
//                              SMART CARD TAG LAYER
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//
// The TAG Layer (mode options: ...).
// Provides access to the smart card parts based on tagged fields....
//
// Modes:
//   SMART_CARD_MODE_...  -
//
// Returned values:
//   HP_SMART_CARD_OK        - operation was successful.
//   HP_SMART_CARD_ERROR     - unknown error performing operation.
//
// Function prototypes:
//
//    smart_card_tag_init  (...
//    smart_card_tag_write (...
//    smart_card_tag_read  (...
//
//*****************************************************************************



//*****************************************************************************
// smart_card_tag_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static void smart_card_tag_init(void)
{
    // verify that the tag field arrays are large enough
    HP_ASSERT(HP_SMART_CARD_INK_LAST_FIELD < HP_SMART_CARD_NUM_FIELD_TYPES);
#ifdef INCLUDE_HP_SMART_CARD_SUPPLY
    HP_ASSERT(HPSCS_LAST_FIELD < HP_SMART_CARD_NUM_FIELD_TYPES);
#endif
}

//*****************************************************************************
// smart_card_tag_device_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_tag_device_init(HP_SMART_CARD_id_t device,
                                                         int device_size)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    HP_SMART_CARD_field_t   field;
    int                     tag_id;


    template_info[device].device_size   = (uint8_t) device_size;
    template_info[device].header_size   = 0;

    for (field = HP_SMART_CARD_NOT_A_FIELD; field < HP_SMART_CARD_NUM_FIELD_TYPES; field++)
    {
        // Make sure the field is setup to an uninitialized state
        template_info[device].field_info[field].num_bytes = 0;
    }
    for (tag_id = 0; tag_id < SMART_CARD_MAX_NUM_OF_TAGS; tag_id++)
    {
        // Zero initialize tag information
        pub_tag_info[device][tag_id].start_addr     = 0;
        pub_tag_info[device][tag_id].end_addr       = 0;
        pub_tag_info[device][tag_id].dirty          = FALSE;
        pub_tag_info[device][tag_id].access_mode    = C_MODE_RO;
    }
    return(result);
}


//*****************************************************************************
// smart_card_tag_set_header -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_tag_set_header(HP_SMART_CARD_id_t device,
                                                        int hsize,
                                                        int num_fields,
                                                        const HP_SMART_CARD_field_desc_t *field_desc)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    field_info_t            *field;
    int                     i;
    int                     start;                                                                                                                                                                                                                                      // start bit
    int                     size;                                                                                                                                                                                                                                       // number of bits


    // Setup the header size - this tells us where to start looking for tags.
    template_info[device].header_size   = (uint8_t) hsize;
    pub_tag_info[device][0].start_addr  = 0;
    pub_tag_info[device][0].end_addr    = (uint8_t) hsize - 1;


    // Initalize the fields descriptions for the header
    for (i = 0; i < num_fields; i++, field_desc++)
    {
        field   = &template_info[device].field_info[field_desc->field_id];
        start   = field_desc->start_bit;
        size    = field_desc->size;

        field->start_byte   = field_desc->start_byte;
        field->mask_index   = field_desc->size;

        // Calculate the shift value
        if (size > 32)
            field->shift = 0;
        else
            field->shift = (uint8_t) (25 + start - size);

        // Calculate the number of bytes are used to contain the field value
        if (size > 0)
        {
            field->num_bytes    = 1;
            size                -= (start + 1);
        }
        while (size > 0)
        {
            field->num_bytes++;
            size -= 8;
        }
    }
    return(result);
}


//*****************************************************************************
// smart_card_tag_set_tag_desc -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_tag_set_tag_desc(HP_SMART_CARD_id_t device,
                                                          HP_SMART_CARD_tag_t tag_id,
                                                          int expected_size,
                                                          int num_fields,
                                                          int last_field,
                                                          const HP_SMART_CARD_field_desc_t *field_desc,
                                                          HP_SMART_CARD_part_mode_t mode)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    int                     field_id;
    field_info_t            *field;
    uint8_t                 bvalue;                                                                                                                                                                                     // data read from smart card
    uint8_t                 tag;                                                                                                                                                                                        // tag read from smart card
    uint8_t                 count   = 0;                                                                                                                                                                                // data byte count from smart card
    uint8_t                 found   = FALSE;
    int                     i;
    int                     start;                                                                                                                                                                                                                                      // start bit
    int                     size;                                                                                                                                                                                                                                       // number of bits
    int                     offset;                                                                                                                                                                                                                                     // offset from the start of smart card


    // Make sure that the header fields and the device have been initialized
    if ((template_info[device].header_size == 0) || (template_info[device].device_size == 0))
    {
        result = HP_SMART_CARD_NOT_INITIALIZED;
        return(result);
    }

    // Look for the tag_id - if not found return an error.
    // If found then save the offset of the tag and add to the
    // start_byte of each field.
    offset = template_info[device].header_size;
    while ((offset < template_info[device].device_size) && !found)
    {
        result = smart_card_cache_read(device, offset, 1, &bvalue,
                                       SMART_CARD_MODE_CACHE, FALSE);
        if (result != HP_SMART_CARD_OK)
            break;
        tag     = bvalue >> 4;
        count   = (bvalue & 0x0f) * 4;
        if (tag == tag_id)
        {
            found = TRUE;
        }
        else
        {
            offset += count + 1;
        }
    }

    // Set the result if the tag is not found
    if (!found)
    {
        result = HP_SMART_CARD_TAG_NOT_FOUND;
    }

    // Record tag information
    pub_tag_info[device][tag_id].start_addr     = (uint8_t) offset;
    pub_tag_info[device][tag_id].end_addr       = (uint8_t) offset + count;
    pub_tag_info[device][tag_id].access_mode    = mode;

    // Initialize the fields for the tag
    if (result == HP_SMART_CARD_OK)
    {
        for (i = 0; i < num_fields; i++, field_desc++)
        {
            field_id = field_desc->field_id;
            HP_ASSERT(field_id < last_field);
            field   = &template_info[device].field_info[field_id];
            start   = field_desc->start_bit;
            size    = field_desc->size;

            field->start_byte   = field_desc->start_byte + (uint8_t) offset;
            field->mask_index   = field_desc->size;

            // Calculate the shift value
            if (size > 32)
                field->shift = (start + 1) % 8;
            else
                field->shift = (uint8_t) (25 + start - size);

            // Calculate the number of bytes are used
            // to contain the field value
            if (size > 0)
            {
                field->num_bytes    = 1;
                size                -= (start + 1);
            }
            while (size > 0)
            {
                field->num_bytes++;
                size -= 8;
            }
        }
    }
    return(result);
}


//*****************************************************************************
// smart_card_tag_read -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_tag_read(HP_SMART_CARD_id_t device,
                                                  HP_SMART_CARD_field_t field,
                                                  int size,                                                                                                                                                                                                                                                                                                 //legal values 1,2,4,...
                                                  void *dst,
                                                  SMART_CARD_mode_t mode)
{
    HP_SMART_CARD_result_t  result  = HP_SMART_CARD_OK;
    uint32_t                value   = 0;
    field_info_t            *pfield = &template_info[device].field_info[field];
    uint8_t                 buf[64];


    // If no size then just return
    if (size == 0)
        return(result);

    // If field has not been initialized then return an error.
    if (pfield->num_bytes == 0)
        result = HP_SMART_CARD_NOT_INITIALIZED;

    // If the size is 3 bytes or greater than 4 bytes then assume a character
    // array there is no need to modify what is read from the smart card.  Another
    // assumption here is that character arrays in smart card will start on
    // a byte boundary.  REVISIT: make sure these assumptions are valid.
    if ((result == HP_SMART_CARD_OK) && ((size > 4) || (size == 3)))
    {
        HP_ASSERT(size < 63);

        if (0 == pfield->shift)
        {
            /* Data starts from byte boundry */
            result = smart_card_cache_read(device,
                                           pfield->start_byte,
                                           size,
                                           dst,
                                           mode,
                                           FALSE);
        }
        else
        {
            /* Clear the buffer */
            memset(buf, 0, 64);

            result = smart_card_cache_read(device,
                                           pfield->start_byte,
                                           pfield->num_bytes,
                                           buf,
                                           mode,
                                           FALSE);

            shift_right(buf, pfield->num_bytes, pfield->shift);

            /* Copy the data to requested buffer */
            memcpy(dst, buf + 1, size);
        }
    }

    // If the size is <= 4 (but not 3) then read the number of bytes in the
    // field info structure into "value".  Shift and mask "value" and copy
    // into "dst" based on the size.
    else if ((result == HP_SMART_CARD_OK) && (size <= 4))
    {
        result = smart_card_cache_read(device,
                                       pfield->start_byte,
                                       pfield->num_bytes,
                                       &value,
                                       mode,
                                       FALSE);
        if (result == HP_SMART_CARD_OK)
        {
            if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
            {
                value = bswap_32(value);                                                                                                        // make little endian conversion
            }

            value   >>= pfield->shift;
            value   &= mask[pfield->mask_index];
            switch (size)
            {
            case 1:
                *(uint8_t *) dst = (uint8_t) value;
                break;
            case 2:
                *(uint16_t *) dst = (uint16_t) value;
                break;
            case 4:
                *(uint32_t *) dst = (uint32_t) value;
                break;
            }
        }
    }

    return(result);
}


//*****************************************************************************
// smart_card_string_read -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_string_read(HP_SMART_CARD_id_t device,
                                                     HP_SMART_CARD_field_t first_field,
                                                     int length,
                                                     void *dst,
                                                     SMART_CARD_mode_t mode)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    uint8_t                 achar;
    int                     i;
    uint8_t                 *pdst;
    HP_SMART_CARD_field_t   src_field;

    // If no size then just return
    if (length < 1)
        return(result);

    // read each character separately
    pdst        = (uint8_t *) dst;
    src_field   = first_field;
    for (i = 0; i < length; i++)
    {
        // read the next character
        result = smart_card_tag_read(device, src_field, 1, (void *) (&achar), mode);
        if (result != HP_SMART_CARD_OK)
            return result;
        // map and copy character; increment dst pointers
        *pdst++ = (achar + 0x20);
        src_field++;
    }

    return(result);
}


//*****************************************************************************
// smart_card_tag_write -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_tag_write(HP_SMART_CARD_id_t device,
                                                   HP_SMART_CARD_field_t field,
                                                   int size,
                                                   void           *src,
                                                   SMART_CARD_mode_t mode)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    uint32_t                oldvalue;
    uint32_t                newvalue;
    uint32_t                orginal_val;
    uint32_t                vmask;
    uint8_t                 shift;
    field_info_t            *pfield = &template_info[device].field_info[field];

    uint8_t                 tag_id  = 0;
    int32_t                 i       = 0;
    uint32_t                tag_start_addr;
    uint32_t                tag_end_addr;

    // If no size then just return
    if (size == 0)
        return(result);

    // If field has not been initialized then
    // return an error.
    if (pfield->num_bytes == 0)
        result = HP_SMART_CARD_NOT_INITIALIZED;

    /* Get the access mode */
    for (i = 0; i < SMART_CARD_MAX_NUM_OF_TAGS; i++)
    {
        tag_start_addr  = pub_tag_info[device][i].start_addr;
        tag_end_addr    = pub_tag_info[device][i].end_addr;
        if ((pfield->start_byte >= tag_start_addr) &&
            (pfield->start_byte <= tag_end_addr))
        {
            tag_id = (uint8_t) i;
            break;
        }
    }

    /* Assert if we cann't find the tag for the requested field */
    HP_ASSERT(i < SMART_CARD_MAX_NUM_OF_TAGS);

    /* If access mode is RO mode, return HP_SMART_CARD_WRITE_ERROR */
    if (C_MODE_RO == pub_tag_info[device][tag_id].access_mode)
    {
        return HP_SMART_CARD_WRITE_ERROR;
    }

    // If the size is 3 bytes or greater than 4 bytes then assume a character
    // array there is no need to modify what is written to smart card.  Another
    // assumption here is that character arrays in smart card will start on
    // a byte boundary.  REVISIT: make sure these assumptions are valid.
    if ((result == HP_SMART_CARD_OK) && ((size > 4) || (size == 3)))
    {
        if (0 == pfield->shift)
        {
            result = smart_card_cache_write(device,
                                            pfield->start_byte,
                                            size,
                                            src,
                                            mode);
        }
        else
        {
            uint8_t start_byte_index    = 0;
            uint8_t end_byte_index      = 0;
            uint8_t start_byte_mask     = 0;
            uint8_t end_byte_mask       = 0;
            uint8_t buf[64];
            uint8_t buf_src[64];
            int32_t j = 0;

            /* Not aligned to byte boundry */

            HP_ASSERT(size < 64);

            /* Clear the buffer */
            memset(buf, 0, 64);
            memset(buf_src, 0, 64);

            memcpy(buf_src, src, size);
            shift_right(buf_src, pfield->num_bytes, 8 - pfield->shift);

            result = smart_card_cache_read(device,
                                           pfield->start_byte,
                                           pfield->num_bytes,
                                           buf,
                                           mode,
                                           FALSE);

            start_byte_index    = 0;
            end_byte_index      = pfield->num_bytes - 1;

            start_byte_mask = (uint8_t) mask[pfield->shift];
            end_byte_mask   = (uint8_t) ~mask[8 - ((pfield->mask_index - pfield->shift) % 8)];

            buf[start_byte_index]   &= ~start_byte_mask;
            buf[start_byte_index]   |= buf_src[start_byte_index];

            buf[end_byte_index] &= ~end_byte_mask;
            buf[end_byte_index] |= buf_src[end_byte_index];

            for (j = 1; j < (pfield->num_bytes - 1); j++)
            {
                buf[j] = buf_src[j];
            }

            /* Now write the bytes to cache */
            result = smart_card_cache_write(device,
                                            pfield->start_byte,
                                            pfield->num_bytes,
                                            buf,
                                            mode);
        }
    }

    // If the size is <= 4 (but not 3) then read the number of bytes in the
    // field info structure into "oldvalue".  Mask "oldvalue", mask and shift
    // the new value and or them together and then write.
    else if ((result == HP_SMART_CARD_OK) && (size <= 4))
    {
        result = smart_card_cache_read(device,
                                       pfield->start_byte,
                                       pfield->num_bytes,
                                       &oldvalue,
                                       SMART_CARD_MODE_CACHE,
                                       FALSE);
        if (result == HP_SMART_CARD_OK)
        {
            orginal_val = oldvalue;

            if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
            {
                oldvalue = bswap_32(oldvalue);                                                                                                        // make little endian conversion
            }

            vmask   = mask[pfield->mask_index];
            shift   = pfield->shift;

            // mask the cached value and zero out field
            oldvalue &= ~(vmask << shift);

            // now mask and shift the new value
            switch (size)
            {
            case 1:
                newvalue = (uint32_t) *(uint8_t *) src;
                break;
            case 2:
                newvalue = (uint32_t) *(uint16_t *) src;
                break;
            case 4:
                newvalue = *(uint32_t *) src;
                break;
            default:
                HP_ASSERT(0);
                break;
            }
            newvalue    &= vmask;
            newvalue    <<= shift;

            // 'or' the values together
            newvalue |= oldvalue;

            if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
            {
                newvalue = bswap_32(newvalue);                                                                                                        // make little endian conversion
            }

            /* If access mode is RW_OR mode and we are trying to make a 1 to 0,
               then return HP_SMART_CARD_WRITE_ERROR */
            if (C_MODE_RW_OR == pub_tag_info[device][tag_id].access_mode)
            {
                if ((orginal_val & newvalue) != orginal_val)
                {
                    return HP_SMART_CARD_WRITE_ERROR;
                }
            }
            // write out the new value
            result = smart_card_cache_write(device,
                                            pfield->start_byte,
                                            pfield->num_bytes,
                                            &newvalue,
                                            mode);
        }
    }

    return(result);
}


//*****************************************************************************
// smart_card_string_write -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_string_write(HP_SMART_CARD_id_t device,
                                                      HP_SMART_CARD_field_t first_field,
                                                      int length,
                                                      void           *src,
                                                      SMART_CARD_mode_t mode)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    uint8_t                 achar;
    int                     i;
    uint8_t                 *psrc;
    HP_SMART_CARD_field_t   dst_field;

    // If no size then just return
    if (length < 1)
        return(result);

    // write each character separately
    psrc        = (uint8_t *) src;
    dst_field   = first_field;
    for (i = 0; i < length; i++)
    {
        // copy next character; increment src pointer
        achar = (*psrc++);
        // map character, checking invalid values
        if (achar >= 'a' && achar <= 'z')
            achar -= 0x20;                                                                                                                              // force uppercase
        if (achar < 0x20 || achar > 0x5f)
            achar = '?';                                                                                                                                // change invalid characters to '?'
        achar -= 0x20;                                                                                                                                  // map into 6 bits
        // write the character
        result = smart_card_tag_write(device, dst_field, 1, (void *) (&achar), mode);
        if (result != HP_SMART_CARD_OK)
            return result;
        // increment the field pointer
        dst_field++;
    }

    return(result);
}


//*****************************************************************************
//*****************************************************************************
//                              SMART CARD CACHE LAYER
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//
// The CACHE Layer (mode options: SMART_CARD_MODE_CACHE)
// Provides cached r/w access to the whole smart_card part. By default, reads and
// writes go through the cache to the lower layers.  Cache reads and writes
// are protected with a semaphore.
//
// Modes:
//    SMART_CARD_MODE_CACHE - reads and writes are made only via the cache.
//
// Returned values:
//   HP_SMART_CARD_OK        - operation successful.
//   HP_SMART_CARD_ERROR     - unknown error performing operation.
//
// Function prototypes:
//
//    smart_card_cache_init         (void)
//    smart_card_cache_device_init  (device, size)
//    smart_card_cache_write        (device, addr, size, void*, mode)
//    smart_card_cache_read         (device, addr, size, void*, mode)
//    smart_card_cache_flush        (device)
//
//*****************************************************************************


//*****************************************************************************
// smart_card_cache_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static void smart_card_cache_init(void)
{
    int device;

    HP_mutex_init(&_smart_card_mutex);

    flushq_create();
    HP_mutex_lock(&_smart_card_mutex);

    for (device = 0; device < NUM_SMART_CARD_DEVICES; device++)
    {
        //HP_ASSERT(_pub_cache_lock[device] == NULL);
        HP_mutex_init(&_pub_cache_lock[device]);

        pub_cache[device].size = MAX_SMART_CARD_SIZE;
        memset(pub_cache[device].data, 0, MAX_SMART_CARD_SIZE);
    }

    HP_mutex_unlock(&_smart_card_mutex);
}


//*****************************************************************************
// smart_card_cache_device_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_cache_device_init(HP_SMART_CARD_id_t device,
                                                           int size)
{
    HP_SMART_CARD_result_t result;
    // Set the size of the smart card device
    pub_cache[device].size = size;

    // Read the smart card part into the cache
    result = smart_card_trans_read(device,
                                   0,
                                   size,
                                   pub_cache[device].data,
                                   SMART_CARD_MODE_NONE,
                                   FALSE);
    return(result);
}

//*****************************************************************************
// smart_card_cache_write -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_cache_write(HP_SMART_CARD_id_t device,
                                                     int addr,
                                                     int size,
                                                     void          *src,
                                                     SMART_CARD_mode_t mode)
{
    HP_SMART_CARD_result_t  result  = HP_SMART_CARD_OK;
    int                     i       = 0;

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_cache_write: device %d, addr %d - entry\n", device, addr);

    // check parameters, addr within the part, size assures data within part
    HP_ASSERT(addr >= 0 && addr < pub_cache[device].size);
    HP_ASSERT(size >= 0 && size <= pub_cache[device].size - addr);

    // Update cache with the (possibly) new data
    // Lock the following two operation to make them atomic:
    //  - updating the pub_cache
    //  - marking the modified tag as dirty

    HP_mutex_lock(&_pub_cache_lock[device]);

    // if data is not changing, don't queue the device
    if (memcmp(pub_cache[device].data + addr,
               src,
               size) != 0)
    {
        // Update public cache data before starting
        memcpy(pub_cache[device].data + addr, src, size);

        // CACHE_MODE is TRUE: mark the cache as dirty and flush
        HP_ASSERT(SMART_CARD_MODE_DEFAULT == mode);

        // Mark the appropriate tag as dirty
        for (i = 0; i < SMART_CARD_MAX_NUM_OF_TAGS; i++)
        {
            // if the tag containing this data has been found ...
            if ((addr >= (int) pub_tag_info[device][i].start_addr) &&
                (addr <= (int) pub_tag_info[device][i].end_addr))
            {
                // ... mark tag as dirty, and quit looping tags
                pub_tag_info[device][i].dirty = TRUE;
                break;
            }
        }
        HP_ASSERT(i < SMART_CARD_MAX_NUM_OF_TAGS);

        /* If this is a C_MODE_RW2RO partition we don't know if this
                partition was already locked. So after initiating a cache
                flush, will wait for the response below.
         */
#ifndef SINGLE_THREADED
        if (C_MODE_RW2RO == pub_tag_info[device][i].access_mode)
        {
            flushq_will_wait_for_flush_complete();
        }
#endif

        // queue the device for flushing
        flushq_queue_device((int) device);
    }

    HP_mutex_unlock(&_pub_cache_lock[device]);

    /* C_MODE_RW2RO partition - see comment above */
    if (C_MODE_RW2RO == pub_tag_info[device][i].access_mode)
    {
#ifndef SINGLE_THREADED
        // MULTI THREADED - wait for background flush to complete and get result
        result = flushq_wait_for_flush_complete((int) device);
        // NOTE: if flush failed, data was reverted in smart_card_cache_flush()
#endif
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_cache_write: device %d, addr %d - exit\n", device, addr);

    return(result);
}


//*****************************************************************************
// smart_card_cache_read -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_cache_read(HP_SMART_CARD_id_t device,
                                                    int addr,
                                                    int size,
                                                    void          *dst,
                                                    SMART_CARD_mode_t mode,
                                                    HP_SMART_CARD_bool_t cache_flushing)
{
    HP_SMART_CARD_result_t result = HP_SMART_CARD_OK;

    // check parameters, addr within the part, size assures data within part
    HP_ASSERT(addr >= 0 && addr < pub_cache[device].size);
    HP_ASSERT(size >= 0 && size <= pub_cache[device].size - addr);

    // If SMART_CARD_MODE_CACHE flag is set read from the cache
    if (SMART_CARD_MODE_CACHE & mode)
    {
        memcpy(dst, pub_cache[device].data + addr, size);
    }

    // if SMART_CARD_MODE_CACHE flag is not set read from device
    else
    {
        result = smart_card_trans_read(device, addr, size, dst, mode, cache_flushing);
    }

    return(result);
}



#ifndef SINGLE_THREADED
//*****************************************************************************
// _cache_monitor -
//-----------------------------------------------------------------------------
// Description -
//
//   This function is in charge of flushing data automatically as soon as it
//   becomes available.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
void *_cache_monitor(void *arg)
{
    int signal_count = 0;

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_IFC_TRACE, 1,
                    "Created cache monitor thread\n");

    while (1)
    {
        // wait on "something to flush" semaphore
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                        "Cache Monitor will wait for signal\n");
        signal_count = flushq_wait_to_flush();

        // flush
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                        "Cache Monitor signaled (%d) to flush all devices...\n", signal_count);
        _flush_all_pending();

        // signal waiters
        flushq_signal_all_waiters();
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID,
                    HP_DBG_LEVEL_ERROR, 0,
                    "Exiting cache monitor thread\n");

    return (void *) NULL;
}
#endif /* not SINGLE_THREADED */



//*****************************************************************************
// _flush_all_pending
//-----------------------------------------------------------------------------
// Flush a devices that are waiting to flush.
// MULTI-THREAD - called from cache monitor.
// SINGLE_THREAD = called directly.
//*****************************************************************************
static HP_SMART_CARD_result_t _flush_all_pending(void)
{
    uint32_t                device_bits;
    int                     device, i;
    uint32_t                check_bit       = 0;
    HP_SMART_CARD_result_t  result          = HP_SMART_CARD_OK;
    HP_SMART_CARD_result_t  return_result   = HP_SMART_CARD_OK;

    // get device bits
    device_bits = flushq_get_device_bits();

    // if there are devices to flush...
    while (device_bits != 0)
    {
        //
        // FLUSH A DEVICE
        //

        // find next device to flush
        device = _last_flush_device;
        for (i = 0; i < 100; i++)                                                   // (failsafe loop)
        {
            // looping increment device [also checking for invalid values]
            if (device < 0 || device > 30)
                device = 0;
            else
                device += 1;
            // if the device bit is set, break;
            check_bit = (0x1 << device);
            if ((device_bits & check_bit) != 0)
                break;
        }

        // clear the bit for this device and remember it
        device_bits         &= ~check_bit;
        _last_flush_device  = device;

        // do the actual flush and report errors (does NOT retry)
        result = _flush_device((int) device);

        // if an error, remember it AND report to callbacks
        if (result != HP_SMART_CARD_OK)
        {
            // remember the first error
            if (return_result == HP_SMART_CARD_OK)
                return_result = result;

            //call callback into ink_supply in case of an error.
            if (result == HP_SMART_CARD_DEVICE_ABSENT)
            {
                CACHE_MONITOR_impl((HP_SMART_CARD_device_id_t) device, result);
            }
            else
            {
                CACHE_MONITOR_impl((HP_SMART_CARD_device_id_t) device,
                                   HP_SMART_CARD_CACHE_NOT_FLUSHED);
            }
        }

        // Done flushing a single device -- loop around to see if other devices need to be flushed
    }

    // Set the combined result (HP_SMART_CARD_OK or first error) and also return it
    flushq_set_last_result(-1, return_result);
    return return_result;
}



//*****************************************************************************
// _flush_device
//-----------------------------------------------------------------------------
// Flush a specific device.
// MULTI-THREAD - called from cache monitor via _flush_all_pending().
// SINGLE_THREAD = called directly.
//*****************************************************************************
static HP_SMART_CARD_result_t _flush_device(int device)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    int                     i;

    // NOTE: Does not change _last_flush_device

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "_flush_device: device %d - entry\n", device);

    // clear the flush bit for this device (which may have been previously set)
    flushq_clear_device_bit(device);

    HP_mutex_lock(&_pub_cache_lock[device]);

    // lock the public cache for copying to private cache
    memcpy(&priv_cache, &pub_cache[device], sizeof(pub_cache[device]));
    // also copy the "tag dirty" info to the private data
    // then mark the "public dirty bits" for tags as clean since smart_card_cache_flush
    // will take care of flushing the tags if dirty.  Don't want to do that any later than here
    // since as soon as we give the _pub_cache_lock, the tMECH is free to write again
    for (i = 0; i < SMART_CARD_MAX_NUM_OF_TAGS; i++)
    {
        priv_tag_info[i].dirty          |= pub_tag_info[device][i].dirty;
        pub_tag_info[device][i].dirty   = FALSE;
    }

    HP_mutex_unlock(&_pub_cache_lock[device]);

    // do the actual flush and return errors (does NOT retry)
    result = smart_card_cache_flush((int) device);

    // remember result of last flush
    flushq_set_last_result((int) device, result);

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "_flush_device: device %d - exit\n", device);

    return result;
}


//*****************************************************************************
// smart_card_cache_flush -
//-----------------------------------------------------------------------------
//
// Only called from _flush_device() which is a single thread so private data/and info are set.
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_cache_flush(HP_SMART_CARD_id_t device)
{
    HP_SMART_CARD_result_t  last_error, result;
    int                     i, addr, size, tag_start_addr, tag_end_addr, tagcount, bytes;

    result      = HP_SMART_CARD_OK;
    last_error  = HP_SMART_CARD_OK;
    tagcount    = 0;
    bytes       = 0;

    // For each tag
    for (i = 0; i < SMART_CARD_MAX_NUM_OF_TAGS; i++)
    {
        // If tag is dirty (data in the tag has changed), write it to the part
        while (priv_tag_info[i].dirty)
        {
            tag_start_addr  = priv_tag_info[i].start_addr;
            tag_end_addr    = priv_tag_info[i].end_addr;
            addr            = tag_start_addr;
            size            = tag_end_addr - addr;
            addr++;                                                                         //leave the tag header alone
            tagcount++;
            bytes += size;

            result = smart_card_trans_write(device,
                                            addr,
                                            size,
                                            &priv_cache.data[addr],
                                            SMART_CARD_MODE_DEFAULT,
                                            TRUE);

            // Clear dirty bit for the tag (even if failed)
            priv_tag_info[i].dirty = FALSE;

            // if write failed
            if (HP_SMART_CARD_OK != result)
            {
                // remember last error
                last_error = result;

                /* Could not update smart card memory. pub_cache contents
                   are out of sync with smart card memory contents.
                   copy the corresponding bytes from smart_card_shadow[]
                 */
                memcpy(&pub_cache[device].data[tag_start_addr],
                       &smart_card_shadow[device][tag_start_addr],
                       (tag_end_addr - tag_start_addr + 1));

                HP_DEBUG_printf(DBG_SMART_CARD_ID,
                                HP_DBG_LEVEL_ERROR, 0,
                                "Cache flush failed, tag data reverted. Result = %d\n", result);
            }
        }
    }

    if (tagcount < 1)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                        "Device %d - nothing to flush (%d)\n",
                        device, last_error);
    }
    else
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                        "Device %d flushed %d tags, %d bytes (%d)\n",
                        device, tagcount, bytes, last_error);
    }
    return(last_error);
}


//*****************************************************************************
//*****************************************************************************
//                              SMART CARD TRANSACTION LAYER
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//
// The TRANSACTION Layer (mode options: SMART_CARD_MODE_TRANS)
// Provides r/w access to the whole smart card part. By default, reads and
// writes go through the trans to the lower layers.  Cache reads and writes
// are protected with a semaphore.
//
// Modes:
//    SMART_CARD_MODE_TRANS - uses transaction blocks to perform write operation.
//
// Returned values:
//    HP_SMART_CARD_OK        - operation successful.
//    HP_SMART_CARD_ERROR     - unknown error performing operation.
//
// Function prototypes:
//
//    smart_card_trans_init          (void)
//    smart_card_trans_device_init   (device, size, update_shadow)
//    smart_card_trans_write         (device, addr, size, void*, mode)
//    smart_card_trans_read          (device, addr, size, void*, mode)
//    smart_card_trans_verify_parity (device)
//    smart_card_trans_verify_cache_flushed (device)
//
//*****************************************************************************

//-----------------------------------------------------------------------------
// Defines/Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------
static int      smart_card_parity_offset[NUM_SMART_CARD_DEVICES];
static uint8_t  smart_card_parity[NUM_SMART_CARD_DEVICES];
static int      current_parity_ptr[NUM_SMART_CARD_DEVICES];


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------


//*****************************************************************************
// smart_card_trans_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static void smart_card_trans_init(void)
{
    int i;

    // Initialize all of the data fields
    for (i = 0; i < NUM_SMART_CARD_DEVICES; i++)
    {
        smart_card_parity_offset[i] = 0;
        smart_card_parity[i]        = 0;
        current_parity_ptr[i]       = 0;

        // Need to zero initialize the shadow at startup just like the cache
        memset(smart_card_shadow[i], 0, MAX_SMART_CARD_SIZE);
    }
}


//*****************************************************************************
// smart_card_trans_device_init -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_trans_device_init(HP_SMART_CARD_id_t device,
                                                           int parity_off,
                                                           HP_SMART_CARD_bool_t update_shadow)
{
    //int retries;
    uint8_t                         cparity;
    int                             i;
    volatile HP_SMART_CARD_result_t result = HP_SMART_CARD_OK;

    // Save the parity offsets for this layer
    smart_card_parity_offset[device] = parity_off;

    if (update_shadow)
    {
        // Need to read the entire part then compute the parity
        result = smart_card_raw_read(device,
                                     0,
                                     MAX_SMART_CARD_SIZE,
                                     smart_card_shadow[device],
                                     SMART_CARD_MODE_NONE,
                                     FALSE);
    }

    // Don't want to do the parity check if the read failed
    if (result != HP_SMART_CARD_OK)
    {
        return(result);
    }

    // Calculate the parity over the device
    cparity = 0;
    for (i = 0; i < parity_off; i++)
    {
        cparity ^= smart_card_shadow[device][i];
    }

    // Verify that one of the parity bytes is the right one
    if (cparity == smart_card_shadow[device][parity_off])
    {
        current_parity_ptr[device]  = 0;
        smart_card_parity[device]   = cparity;
    }
    else if (cparity == smart_card_shadow[device][parity_off + 1])
    {
        current_parity_ptr[device]  = 1;
        smart_card_parity[device]   = cparity;
    }
    else
    {
        /* if the parity calculated doesn't
           match with the one stored in smart_card, report smart_card failure.
         */
        result = HP_SMART_CARD_DATA_NOT_VALID;
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_ERROR, 0,
                        "smart_card_trans_device_init: bad parity device %d\n", device);
#if 0
        //current_parity_ptr[device] = 0;
        //smart_card_parity[device] = cparity;
        //smart_card_shadow[device][parity_off] = cparity;
        //smart_card_shadow[device][parity_off + 1] = cparity;
        //result = HP_SMART_CARD_OK;
#endif
    }

    return(result);
}


//*****************************************************************************
// smart_card_trans_write -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_trans_write(HP_SMART_CARD_id_t device,
                                                     int addr,
                                                     int size,
                                                     uint8_t          *src,
                                                     SMART_CARD_mode_t mode,
                                                     HP_SMART_CARD_bool_t cache_flushing)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    int                     i;
    uint8_t                 cparity     = 0;
    uint8_t                 old_parity  = 0;

    HP_ASSERT(SMART_CARD_MODE_DEFAULT == mode);

    // Build a transaction message and queue it up for processing

    // Get the current parity
    cparity     = smart_card_parity[device];
    old_parity  = cparity;


    // Do a quick sanity check, make sure the current parity is valid
    if (smart_card_shadow[device][smart_card_parity_offset[device] + current_parity_ptr[device]] != cparity)
    {
        return HP_SMART_CARD_DATA_NOT_VALID;
    }


    // Compute new parity
    for (i = 0; i < size; i++)
    {
        cparity = cparity ^ src[i] ^ smart_card_shadow[device][addr + i];
    }

    // Update parity pointer and copy of parity in memory
    current_parity_ptr[device]  ^= 1;
    smart_card_parity[device]   = cparity;

    // Write parity
    result = smart_card_raw_write(device,
                                  smart_card_parity_offset[device] + current_parity_ptr[device],
                                  1,
                                  &cparity,
                                  mode,
                                  FALSE,
                                  cache_flushing);
    if (HP_SMART_CARD_OK == result)
    {
        // Write data
        result = smart_card_raw_write(device, addr, size, src, mode, FALSE, cache_flushing);
    }

    if (result != HP_SMART_CARD_OK)
    {
        /* Failed to write either parity or tag data, so consider previous
                parity byte as valid parity.
         */
        current_parity_ptr[device]  ^= 1;
        smart_card_parity[device]   = old_parity;
    }

    return(result);
}

//*****************************************************************************
// smart_card_trans_read -
//-----------------------------------------------------------------------------
// Description -
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_trans_read(HP_SMART_CARD_id_t device,
                                                    int addr,
                                                    int size,
                                                    void          *dst,
                                                    SMART_CARD_mode_t mode,
                                                    HP_SMART_CARD_bool_t cache_flushing)
{
    HP_SMART_CARD_result_t result;

    result = smart_card_raw_read(device, addr, size, dst, mode, cache_flushing);
    return(result);
}



//*****************************************************************************
//*****************************************************************************
//                              SMART CARD RAW LAYER
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//
// The RAW Layer:
// Provides blocked r/w access to the smart card parts.
//
// Modes:
//
// Returned values: The following errors can be returned from the raw layer:
//   HP_SMART_CARD_OK             - operation was successful.
//   HP_SMART_CARD_READ_ERROR     - failed read operation.
//   HP_SMART_CARD_WRITE_ERROR    - failed write operation.
//   HP_SMART_CARD_READBACK_ERROR - failed readback verify after write.
//   HP_SMART_CARD_DEVICE_ABSENT  - device not found.
//
// Function prototypes:
//    smart_card_raw_init         (void)
//    smart_card_raw_present      (device)
//    smart_card_raw_write        (device, addr, size, void*, mode)
//    smart_card_raw_read         (device, addr, size, void*, mode)
//
//*****************************************************************************

//-----------------------------------------------------------------------------
// Defines/Macros
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------


//*****************************************************************************
// smart_card_raw_init - initialize the raw layer
//-----------------------------------------------------------------------------
// Description - Initialize the raw layer of the smart card protocol stack.
//
// Inputs: none
//
// Return: none
//
//*****************************************************************************
static void smart_card_raw_init(void)
{
}


//*****************************************************************************
// smart_card_raw_present - determine presence of smart card device
//-----------------------------------------------------------------------------
// Description - Read data from the specified smart card part to determine part
//               presence or absence from printer.
//
// Inputs: device  - smart card device to read from.
//
// Return: HP_SMART_CARD_OK            (present),
//         HP_SMART_CARD_DEVICE_ABSENT (absent or error)
//         HP_SMART_CARD_READ_ERROR    (present but a read error occured)
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_raw_present(HP_SMART_CARD_id_t device)
{
    HP_SMART_CARD_result_t  result;
    HW_SMART_CARD_status_t  status;
    uint8_t                 *data_p = (uint8_t *) "deadbeef";

//    status  = HW_SMART_CARD_echo(&hw_smart_card_device[device], data_p, 8);
    status  = HW_SMART_CARD_echo(&hw_smart_card_device[device], data_p, sizeof("deadbeef"));
    result  = _xlat_hw_result(status, HP_SMART_CARD_DEVICE_ABSENT);
    return(result);
}


//*****************************************************************************
// smart_card_raw_read - raw layer read
//-----------------------------------------------------------------------------
// Description - Read the requested data from the smart card part.  This function
//               uses the nvm_raw_sem to ensure mutual exclusion to the I2C
//               bus.
//
// Inputs: device  - smart card device to read from.
//         addr    - starting address of the read.
//         size    - number of bytes to read.
//         dst     - pointer to store data to.
//         mode    - read mode flags.
//
// Return: This function will pass back the following:
//           HP_SMART_CARD_OK             - operation was successful.
//           HP_SMART_CARD_READ_ERROR     - failed read operation.
//           HP_SMART_CARD_DEVICE_ABSENT  - device not found.
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_raw_read(HP_SMART_CARD_id_t device,
                                                  int addr,
                                                  int size,
                                                  void *dst,
                                                  SMART_CARD_mode_t mode,
                                                  HP_SMART_CARD_bool_t cache_flushing)
{
    HP_SMART_CARD_result_t  result = HP_SMART_CARD_OK;
    HW_SMART_CARD_status_t  status;
    uint8_t                 *pdst = dst;
    int                     idx, rlen;

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_raw_read: device %d, addr %d - entry\n", device, addr);

    // Now perform the read
    idx     = device;
    status  = HW_SMART_CARD_raw_read(&hw_smart_card_device[idx], addr, size, (uint8_t *) dst, &rlen);
    result  = _xlat_hw_result(status, HP_SMART_CARD_READ_ERROR);

    if (result == HP_SMART_CARD_OK)
    {
        // If the read is successful update the shadow memory
        memcpy((void *) &smart_card_shadow[device][addr],
               (void *) pdst,
               size);
    }
    if (result != HP_SMART_CARD_OK)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_ERROR, 0,
                        "smart_card_raw_read: I2C_READ: device %d, addr %d, size %d, result %d\n", device, addr, size, result);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_raw_read: device %d, addr %d - exit\n", device, addr);

    return(result);
}

//*****************************************************************************
// smart_card_raw_write - raw layer write
//-----------------------------------------------------------------------------
// Description - Write the requested data to the smart card part.  This function
//               uses the nvm_raw_sem to ensure mutual exclusion to the I2C
//               bus.  In addition, this function tries to optimize the
//               writes to smart card by using multi-byte writes when possible.
//
// Inputs: device  - smart card device to write to.
//         addr    - starting address of the write.
//         size    - number of bytes to write.
//         src     - pointer to data to be written.
//         mode    - write mode flags.
//         smart_card_shadow_locked - flag specifying whether smart_card_shadow is
//                                already locked.
//
// Return: This function will pass back the following:
//           HP_SMART_CARD_OK             - operation was successful.
//           HP_SMART_CARD_WRITE_ERROR    - failed write operation.
//           HP_SMART_CARD_READBACK_ERROR - failed readback verify after write.
//           HP_SMART_CARD_DEVICE_ABSENT  - device not found.
//
//*****************************************************************************
static HP_SMART_CARD_result_t smart_card_raw_write(HP_SMART_CARD_id_t device,
                                                   int addr,
                                                   int size,
                                                   void          *src,
                                                   SMART_CARD_mode_t mode,
                                                   HP_SMART_CARD_bool_t smart_card_shadow_locked,
                                                   HP_SMART_CARD_bool_t cache_flushing)
{
    HP_SMART_CARD_result_t  result  = HP_SMART_CARD_OK;
    uint8_t                 *psrc   = src;
    HW_SMART_CARD_status_t  status;
    int                     wlen, idx;

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_raw_write: device %d, addr %d - entry\n", device, addr);

    idx = device;

    // Call to hw_smart_card_raw_write
    status  = HW_SMART_CARD_raw_write(&hw_smart_card_device[idx], addr, size, src, &wlen);
    result  = _xlat_hw_result(status, HP_SMART_CARD_WRITE_ERROR);

    if (result == HP_SMART_CARD_OK)
    {
        // Update the shadow memory
        memcpy((void *) &smart_card_shadow[device][addr],
               (void *) psrc,
               size);
    }

    if (result != HP_SMART_CARD_OK)
    {
        HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 2,
                        "smart_card_raw_write: I2C_WRITE: device %d, addr %d, size %d, result %d\n", device, addr, size, result);
    }

    HP_DEBUG_printf(DBG_SMART_CARD_ID, HP_DBG_LEVEL_SC_TRACE, 3,
                    "smart_card_raw_write: device %d, addr %d - exit\n", device, addr);

    return(result);
}

//*****************************************************************************
// _xlat_hw_result - translate i2c error to component code
//-----------------------------------------------------------------------------
// Description - Translate from lib_i2c result code to a corresponding
//               smart_card component return result.
//
// Inputs: hw_result - result from lib_i2c
//         def_value - default value to return if nothing appropriate
//
// Return: Will return a smart card component result corresponding to the
//         hw_smart_card result passed in as a parameter, or the default
//         value passed in if no additional detail can be determined.
//
//*****************************************************************************
static HP_SMART_CARD_result_t _xlat_hw_result(HW_SMART_CARD_status_t hw_result,
                                              HP_SMART_CARD_result_t def_val)
{
    HP_SMART_CARD_result_t          result;
    static HW_SMART_CARD_status_t   last_hw_result;

    last_hw_result = hw_result;
    HP_ASSERT(last_hw_result == hw_result);

    _last_status = last_hw_result;                                                  // keep Last Status for LIB_HP_SMART_CARD_last_status()

    HP_ASSERT(hw_result != HW_SMART_CARD_not_auth_e);
    HP_ASSERT(hw_result != HW_SMART_CARD_mac_verification_failure_e);
    HP_ASSERT(hw_result != HW_SMART_CARD_illegal_access_e);
    HP_ASSERT(hw_result != HW_SMART_CARD_memory_error_e);
    //HP_ASSERT (hw_result != HW_SMART_CARD_xpt_cmd_failed_e);
    //HP_ASSERT (hw_result != HW_SMART_CARD_xpt_rsp_failed_e);
    HP_ASSERT(hw_result != HW_SMART_CARD_xpt_not_set_failure_e);
    HP_ASSERT(hw_result != HW_SMART_CARD_aux_failure_e);

    /* Translate smart_card_physical_read error code to an HP_SMART_CARD error code */
    switch (hw_result)
    {
    case HW_SMART_CARD_success_e:
        result = HP_SMART_CARD_OK;
        break;

    case HW_SMART_CARD_failed_e:
    case HW_SMART_CARD_xpt_no_device_e:
    case HW_SMART_CARD_unexpected_device_change_e:
    case HW_SMART_CARD_xpt_cmd_failed_e:
        result = HP_SMART_CARD_DEVICE_ABSENT;
        break;

    case HW_SMART_CARD_illegal_access_e:
        result = HP_SMART_CARD_ILLEGAL_ACCESS;
        break;

    case HW_SMART_CARD_memory_error_e:
    default:
        result = def_val;
        break;
    }

    return(result);
}

