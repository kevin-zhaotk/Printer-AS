/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file ids_ifc.h
 * Interface APIs to the Ink Delivery System (IDS).
 */
#ifndef __IDS_IFC_H__
#define __IDS_IFC_H__

#include "max_common_types.h"
#include "common_service.h"
#include "service.h"

/**
 * @brief IDS API return codes.
 */
typedef enum
{
    IDS_OK                       =  0, /**< Success */
    IDS_ERROR                    = -1, /**< Error */
    IDS_IO_ERROR                 = -2  /**< IO Error */
} IDSResult_t;

#ifndef NUM_IDS_INSTANCES
    #error "NUM_IDS_INSTANCES should be defined in oem_common_types.h"
#endif


typedef struct
{
  uint8_t   fw_minor_rev;
  uint8_t   fw_major_rev;
  uint8_t   fpga_minor_rev;
  uint8_t   fpga_major_rev;
  uint8_t   board_rev_bd1;
  uint8_t   board_rev_bd0;
  uint8_t   board_rev_bd;
  uint32_t  status;
  uint8_t   bootload_minor;
  uint8_t   bootload_major;
  uint16_t  board_id;
} IdsSysInfo_t;


/* SupplyStatus_t state values */
#define SUPPLY_NOT_PRESENT  0
#define SUPPLY_NOT_VERIFIED 1
#define SUPPLY_SC_VALID     2
#define SUPPLY_SC_INVALID   3

/* SupplyStatus_t status bits */
#define STATUS_OOI                  0x01
#define STATUS_ALTERED              0x02
#define STATUS_EXPIRED              0x04
#define STATUS_FAULTY               0x08
#define STATUS_WRONG_SUPPLY_TYPE    0x10
#define STATUS_SC_ERROR             0x20
#define STATUS_WRONG_INK_TYPE       0x40

/* Smart Card numeric fields */
#define OEM_RW2RO_1     1
#define OEM_RW2RO_2     2
#define OEM_RW2RO_3     3
#define OEM_RW2RO_4     4
#define OEM_RW2RO_5     5
#define OEM_RW2RO_6     6
#define OEM_RW2RO_FUSE  7
#define OEM_RWOR_1      8
#define OEM_RWOR_2      9
#define OEM_RW_1        10
#define OEM_RW_2        11
#define OEM_RW_3        12
#define OEM_RW_4        13
#define PILS_STATE      14
#define REORDER_REGION  15
#define REORDER_FUSE    16

/* Smart Card string fields */
#define STR_TRADEMARK   1
#define STR_REORDER_PN  2

/* Smart Card partitions */
#define PARTITION_OEM_RW2RO     1
#define PARTITION_REORDER_RW2RO 2



/*! \struct SupplyStatus
 *  Ink Supply Status
 */
typedef struct
{
    uint8_t state;               /**< Non-zero indicates Out of ink. */
    uint8_t status_bits;        /**< Bits set to indicate various conditions */
    uint16_t consumed_volume;   /**< Consumed ink volume (10ths of ml). */
}SupplyStatus_t;

/*! \struct SupplyStatusDetail
 *  Ink Supply Status Detail
 */
typedef struct
{
    uint16_t current_shelf_days;    /**< Current shelf time (days). */
    uint16_t shelf_life_days;       /**< Shelf life (days). */
    uint16_t current_install_days;  /**< Current install time (days). */
    uint16_t install_life_days;     /**< Install life (days). */
    uint8_t first_failure_code;     /**< First failure code for this supply. */
    uint8_t last_failure_code;      /**< Most recent failure code for this supply. */
}SupplyStatusDetail_t;

/*! \struct SupplyID
 *  Ink Supply ID (unique supply identification data)
 */
typedef struct
{
    uint8_t mfg_site;   /**< Dry cartridge manufacture site. */
    uint8_t mfg_line;   /**< Dry cartridge manufacture line. */
    uint16_t mfg_year;  /**< Dry cartridge manufacture year (e.g. 2018). */
    uint8_t mfg_woy;    /**< Dry cartridge manufacture week of year (1-52). */
    uint8_t mfg_dow;    /**< Dry cartridge manufacture day of week (0-6 = Sunday-Saturday). */
    uint8_t mfg_hour;   /**< Dry cartridge manufacture hour (0-23). */
    uint8_t mfg_min;    /**< Dry cartridge manufacture minute (0-59). */
    uint8_t mfg_sec;    /**< Dry cartridge manufacture second (0-59). */
    uint8_t mfg_pos;    /**< Dry cartridge manufacture position. */
}SupplyID_t;

/*! \struct SupplyInfo
 *  Ink Supply Information (static data)
 */
typedef struct
{
    uint8_t mfg_site;           /**< Ink fill site. */
    uint8_t mfg_line;           /**< Ink fill line. */
    uint16_t mfg_year;          /**< Ink fill year (e.g. 2018). */
    uint8_t mfg_woy;            /**< Ink fill week of year (1-52). */
    uint8_t mfg_dow;            /**< Ink fill day of week (0-6 = Sunday-Saturday). */
    uint8_t mfg_hour;           /**< Ink fill hour (0-23). */
    uint8_t mfg_min;            /**< Ink fill minute (0-59). */
    uint8_t mfg_sec;            /**< Ink fill second (0-59). */
    uint8_t mfg_pos;            /**< Ink fill position. */
    uint16_t sensor_gain;       /**< PILS sensor gain (mV/PSI * 100). */
    uint16_t ink_density;       /**< Ink density (g/ml * 1000). */
    uint16_t usable_vol;        /**< Usable ink volume (10ths of ml). */
    uint8_t insert_count;       /**< Insertion count for this cartridge. */
    uint8_t ext_oem_id;         /**< Extended OEM ID. */
    uint8_t hp_oem_designate;   /**< HP/OEM ink designator. */
    uint8_t formulator_id;      /**< Ink formulator ID. */
    uint8_t ink_vehicle;        /**< Ink vehicle. */
    uint16_t ink_family;        /**< Ink family. */
    uint16_t ink_member;        /**< Ink family member. */
    uint8_t ink_revision;       /**< Ink revision. */
}SupplyInfo_t;

/*! \struct SupplyUseInfo
 *  Ink Supply First Install or MRU Information
 */
typedef struct
{
    uint16_t year;                  /**< Install year (e.g. 2018). */
    uint8_t woy;                    /**< Install week of year (1-52). */
    uint8_t dow;                    /**< Install day of week (0-6 = Sunday-Saturday). */
    uint8_t HP_rev_major;           /**< HP firmware major revision. */
    uint8_t HP_rev_minor;           /**< HP firmware minor revision. */
    uint16_t stall_count;           /**< Insertion count for the stall at time of cartridge insertion. */
    char platform_model[12];        /**< Platform model string. */
    uint16_t platform_mfg_year;     /**< Platform manufacture year (e.g. 2018). */
    uint8_t platform_mfg_woy;       /**< Platform manufacture week of year (1-52). */
    uint8_t platform_mfg_country;   /**< Platform manufacture. */
    uint8_t platform_mfg_rev_major; /**< Platform firmware major revision. */
    uint8_t platform_mfg_rev_minor; /**< Platform firmware minor revision. */
}SupplyUseInfo_t;

/*! \struct IdsPairingStatus_t
 *  IDS status for pairing
 */
typedef struct
{
    uint8_t present_0;              /**< True (!=0) if Supply 0 present (valid SC). */
    uint8_t present_1;              /**< True (!=0) if Supply 1 present (valid SC). */
    uint8_t paired;                 /**< True (!=0) if IDS is paired. */
}IdsPairingStatus_t;



/**
 * This function should be called once to initialize the IDS
 * library before any other functions can be invoked.
 * @Return result
 */
IDSResult_t ids_lib_init();

/**
 * This function will clean-up any data structures. This is a place holder
 * for now.
 * @Return result
 */
IDSResult_t ids_lib_shutdown();

/**
 * Initializes the specified IDS instance.
 * @param instance          IDS instance id.
 * @Return result
 */
IDSResult_t ids_init(int32_t instance);

/**
 * Shutdown specified Printhead Driver instance.
 * @param instance         IDS instance id.
 * @Return result
 */
IDSResult_t ids_shutdown(int32_t instance);

/**
 * Returns a string that describes the given IDSResult_t result code.
 * @param r             IDSResult_t Result code
 * @Return              Returns a string that describes the given error code
 *                      as NULL terminated ASCII string.
 */
const char *ids_get_error_description(IDSResult_t r);

/**
 * @brief                   Reflash(update) IDS µC firmware.
 *                          During µC reflash, unintended error or warning interrupts
 *                          could be asserted by µC as it loses control over the GPIO pins.
 *                          It is recommended that the application disables/ignores these interrupts
 *                          during µC reflash.
 * @param instance          IDS instance id.
 * @param fw_file_name      Path to file containing firmware binary (.s19 format)
 * @param verify            When set to true, during reflash, flash contents are
 *                          read and verified against the input file contents.
 */
IDSResult_t ids_micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify);


/**
 * Convert Smart Card year/week-of-year/day-of-week into calendar (year)/month/day
 */
//int to_month_and_day(int32_t year, int32_t woy, int32_t dow, int32_t *month, int32_t *dom);


/**
 * Deletes all pairing for the specified instance.
 * @param instance      uC instance
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_pairing_delete(int32_t instance);

/**
 * Pairing
 * @param instance      uC instance
 * @param step          Pairing step
 * @param pd_id         Pairing Print Driver ID
 * @param ids_idx       IDS supply index (0,1)
 * @param in            Pointer to input payload (from PD Step 1)
 * @param in_size       Size of input payload
 * @param out           Pointer to buffer where payload is returned
 * @param out_size      Pointer to size of returned payload
 * @param out_buffer_size   Buffer size (set by caller)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_pairing(int32_t instance, int32_t step, int32_t pd_id, int32_t ids_idx, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);

/**
 * Get overrides for supply.
 * @param instance      uC instance
 * @param blur_id       OEM assigned blur id to provide overrides to (0-255)
 * @param supply_idx    Supply index (0/1)
 * @param status        Command status
 * @param overrides     Pointer to returned Overrides structure
 * @param out_size      size of output buffer
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_overrides(int32_t instance, int32_t blur_id, int32_t supply_idx, int32_t *status, uint8_t *overrides, int32_t *out_size, int32_t out_buffer_size);

/**
 * Process ink use data from print system.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param in            Pointer to input ink payload
 * @param in_size       Size of input ink payload
 * @param status        Command status
 * @param supply_status Supply status
 * @param out           Pointer to buffer where status payload is returned
 * @param out_size      Pointer to size of returned status payload
 * @param out_buffer_size   Buffer size (set by caller)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_ink_use(int32_t instance, int32_t pd_id, int32_t supply_idx, uint8_t *in, int32_t in_size, int32_t* status, int32_t *supply_status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);

/**
 * Read an OEM Smart Card field.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param field_id      Smart Card field ID
 * @param value         Value to write
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_read_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value);

/**
 * Read an OEM Smart Card OEM string.
 * @param instance     uC instance
 * @param supply_idx   Supply index (0/1)
 * @param field_id     Smart Card field ID
 * @param buffer_size   size of buffer
 * @param str          string to read
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_read_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t buffer_size, uint8_t *str);

/**
 * Write an OEM Smart Card field.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param field_id      Smart Card field ID
 * @param value         value to write
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_write_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value);

/**
 * Write an OEM Smart Card string.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param field_id      Field id
 * @param str_len       length of string to write
 * @param string        Smart Card OEM string
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_write_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);

/**
 * Write an OEM Smart Card string.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param field_idx     OEM field index
 * @param partition_id  Smart Card partition ID
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_lock_partition(int32_t instance, int32_t supply_idx, int32_t partition_id);

/**
 * Set platform information.
 * @param instance      uC instance
 * @param platform_info Pointer to PlatformInfo structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_set_platform_info(int32_t instance, PlatformInfo_t *platform_info);

/**
 * Set date.
 * @param instance      uC instance
 * @param year          Year (e.g. 2018)
 * @param month         Month (1-12)
 * @param day           Day of month (1-31)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_set_date(int32_t instance, int32_t year, int32_t month, int32_t day);

/**
 * Set stall insertion count for supply.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param count         Insertion count
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_set_stall_insert_count(int32_t instance, int32_t supply_idx, int32_t count);

/**
 * Set Out of Ink for supply.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_set_out_of_ink(int32_t instance, int32_t supply_idx);

/**
 * Set Faulty for supply.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_set_faulty(int32_t instance, int32_t supply_idx);

/**
 * Flush all Smart Card data for supply.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_flush_smart_card(int32_t instance, int32_t supply_idx);

/**
 * Get dynamic supply status.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param supply_status     Pointer to returned SupplyStatus structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_supply_status(int32_t instance, int32_t supply_idx, SupplyStatus_t *supply_status);

/**
 * Get dynamic supply status detail.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param supply_status     Pointer to returned SupplyStatusDetail structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_supply_status_detail(int32_t instance, int32_t supply_idx, SupplyStatusDetail_t *supply_status_detail);

/**
 * Get Smart Card fields which uniquely identify a supply.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param supply_status     Pointer to returned SupplyID structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_supply_id(int32_t instance, int32_t supply_idx, SupplyID_t *supply_id);

/**
 * Get static supply information.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param supply_info   Pointer to returned SupplyInfo structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_supply_info(int32_t instance, int32_t supply_idx, SupplyInfo_t *supply_info);

/**
 * Get MRU information.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param mru_info      Pointer to returned SupplyUseInfo structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_mru_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *mru_info);

/**
 * Get First Install information.
 * @param instance      uC instance
 * @param supply_idx    Supply index (0/1)
 * @param first_install_info     Pointer to returned SupplyUseInfo structure
 * @Return              IDSResult_t Result code
 *
 */
IDSResult_t ids_get_first_install_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *first_install_info);

IDSResult_t ids_info(int32_t instance, IdsSysInfo_t *ids_resp);

IDSResult_t ids_terminate_session(int32_t instance);

/* direct SC read/write services are only valid for debug builds */
IDSResult_t ids_read_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value);
IDSResult_t ids_read_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);
IDSResult_t ids_write_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value);
IDSResult_t ids_write_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);

/* test functions */
IDSResult_t ids_increment(int32_t instance, uint8_t in, uint8_t *out);

#endif /* __IDS_IFC_H__ */

