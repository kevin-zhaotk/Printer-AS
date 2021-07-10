/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file print_head_driver_ifc.h
 * Interface APIs to manage the Printhead Driver (PD).
 */
#ifndef __PRINT_HEAD_DRIVER_IFC_H__
#define __PRINT_HEAD_DRIVER_IFC_H__

#include "max_common_types.h"
#include "common_service.h"


/**
 * @brief PD API return codes.
 */
typedef enum 
{
    PD_OK                       =  0, /**< Success */
    PD_ERROR                    = -1, /**< Error */
    PD_ERROR_IO                 = -2, /**< IO (serial port) Error */
    PD_INVALID_FILE             = -3, /**< Invalid file */
    PD_FLASH_WP                 = -4, /**< Flash is write protected */
    PD_FLASH_ADR_RO             = -5, /**< Flash Address is Read Only */
    PD_NOT_IMPLEMENTED          = -6, /**< This feature is not implemented */
} PDResult_t;

#ifndef NUM_BLUR_INSTANCES
    #error "NUM_BLUR_INSTANCES should be defined in max_common_types.h"
#endif

#if (NUM_BLUR_INSTANCES < 0) || (NUM_BLUR_INSTANCES > 16)
    #error "NUM_BLUR_INSTANCES should be between 1 and 16"
#endif

#if BUILD == DEBUG_BUILD
#include "print_head_driver_debug_ifc.h"
#endif


/**
 *Number of supported Printheads
 */
#define NUM_SUPPORTED_PH    2
/**
 *Number of Trench per Printhead
 */
#define NUM_TRENCH_PER_PH   2

/**
 * @brief                   This function should be called once to initialize the PD
 *                          library before any other functions can be invoked.
 * @returns                 It can return PD_ERROR_IO, PD_OK .
 * @par States                 
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)  
 *                          No Error Codes
 */
PDResult_t pd_lib_init();

/**
 * @brief                   This function can be called to shutdown the library cleanly. 
 *                          This is a place holder for now.
 * @returns                 PD_OK.
 * @par States                 
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)  
 *                          No Error Codes
 */
PDResult_t pd_lib_shutdown();

/** 
 * @brief                   Initializes the specified PD instance.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @returns                 It can return PD_ERROR_IO, PD_ERROR and  PD_OK .
 * @par States              
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)   
 *                          No Error codes
 */
PDResult_t pd_init(int32_t instance);

/** 
 * @brief                   Shutdown specified PD instance.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @returns                 It can return PD_OK .
 * @par States              
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)  
 *                          No Error Codes
 */
PDResult_t pd_shutdown(int32_t instance);

/**
 * @brief                   Returns a string that describes the given error code.
 * @param r                 Error code.
 * @returns                 Returns a string that describes the given error code.
 * @par States
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)  
 *                          No Error Codes
 */
const char *pd_get_error_description(PDResult_t r);


/**
 * Convert Smart Card year/week-of-year/day-of-week into calendar (year)/month/day
 */
//int to_month_and_day(int32_t year, int32_t woy, int32_t dow, int32_t *month, int32_t *dom);


/*
 * Pairing
 * @param instance      uC instance
 * @param step         	Pairing step
 * @param slot_bits     Bits indicating Printheads/slots to pair
 * @param ids_id        Pairing IDS ID
 * @param in            Pointer to input payload (from PD Step 1)
 * @param in_size       Size of input payload
 * @param out           Pointer to buffer where payload is returned
 * @param out_size      Pointer to size of returned payload
 * @param out_buffer_size   Buffer size (set by caller)
 * @Return              IDSResult_t Result code
 *
 */
PDResult_t pd_pairing(int32_t instance, int32_t step, int32_t slot_bits, int32_t ids_id, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);

/**
 * @brief                   Reflash(update) PD µC firmware. 
 *                          This function should be invoked only when both the Printheads are powered off.
 *                          During µC reflash, unintended error or warning interrupts 
 *                          could be asserted by µC as it loses control over the GPIO pins.
 *                          It is recommended that the application disables/ignores these interrupts
 *                          during µC reflash. 
 * @param instance          PD instance id.
 * @param fw_file_name      Path to file containing firmware binary (.s19 format)
 * @param verify            When set to true, during reflash, flash contents are 
 *                          read and verified against the input file contents.
 * @returns                 It can return PD_ERROR, PD_INVALID_FILE or PD_OK.
 * @par States                
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API) 
 *                          No Error Codes
 */
PDResult_t pd_micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify);

/**
 * @brief                   Reflash(update) PD µC firmware. 
 *                          This is API is implemented only for development. This API should be
 *                          used only when the µC is already in reflash mode. 
 *                          This function should be invoked only when both the Printheads are powered off.
 *                          During µC reflash, unintended error or warning interrupts 
 *                          could be asserted by µC as it loses control over the GPIO pins.
 *                          It is recommended that the application disables/ignores these interrupts
 *                          during µC reflash. 
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param fw_file_name      Path to file containing firmware binary (.s19 format)
 * @param verify            When set to true, during reflash, flash contents are 
 *                          read and verified against the input file contents.
 * @returns                 It can return PD_ERROR, PD_INVALID_FILE or PD_OK.
 * @par States                
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API) 
 *                          No Error Codes
 */
PDResult_t pd_micro_fw_reflash_no_reset(int32_t instance, const char *fw_file_name, bool verify);

/**
 * @brief                   Reflash (update) PD FPGA code.
 *                          This function should be invoked only when both the Printheads are powered off.
 * @param instance          PD instance id.
 * @param fpga_file_name    Path to file containing firmware binary (.s19 format)
 * @param verify            When set to true, during reflash, flash contents are 
 *                          read and verified against the input file contents.
 * @returns                 It can return PD_ERROR, PD_INVALID_FILE, PD_FLASH_WP, PD_FLASH_ADR_RO, PD_OK .
 * @par States              
 *                          No States . System level API.
 * @par Possible Error Codes (set by FW for this API)   
 *                          No Error Codes
 */
PDResult_t pd_fpga_fw_reflash(int32_t instance, const char *fpga_file_name, bool verify);

/**
 * @brief                   Power on the requested Printhead.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @returns                 It can return PD_ERROR or PD_OK.
 * @par States              
 *                          It is valid during PhPowerOffState and PhPresentState
 *
 * @par Possible Error Codes (set by FW for this API)
 *                           1. PH_DIE_RDWR_ERROR
 *                           2. PH_INKSHORT_ERROR
 *                           3. PH_REG_RDWR_ERROR
 *                           4. PH_VPP_ADJ_FAIL_ERROR
 *                           5. PH_VPP_POWON_FAIL_ERROR
 *                           6. PH_VPP_VALIDATION_ERROR
 *                           7. PH_VDD_POWON_FAIL_ERROR
 *                           8. PH_VDD_UV_ERROR
 *                           9. PH_VDD_OV_ERROR
 *                           10. PH_VPP_SHUNTDISABLE_ERROR 
 *                           11. PH_VPP_GOOD_ERROR 
 *                           12. PH_VPP_GOODSTABLE_ERROR
 */
PDResult_t pd_power_on(int32_t instance, uint8_t ph_id);

/**
 * @brief                   Power Off the requested Printhead
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param ph_id             Printhead id (0 or 1)
 * @returns                 It can return PD_ERROR or PD_OK.
 * @par States              
 *                          It is valid during PhPowerOnState.
 * @par Possible Error Codes (set by FW for this API)
 *                          1. PH_REG_RDWR_ERROR
 *                          2. PH_DIE_RDWR_ERROR
 *                          3. PH_VPP_POWOF_FAIL_ERROR 
 *                          4. PH_DRP_PROC_ERROR
 */
PDResult_t pd_power_off(int32_t instance, uint8_t ph_id);


/**
 * @brief OEM Smart Card fields.
 */
typedef enum
{
    PD_SC_OEM_LOCKABLE_FIELD_1     = 0,    /**< OEM lockable field 1 */
    PD_SC_OEM_LOCKABLE_FIELD_2     = 1,    /**< OEM lockable field 2 */
    PD_SC_OEM_LOCKABLE_FIELD_3     = 2,    /**< OEM lockable field 3 */
    PD_SC_OEM_RWOR_FIELD_1         = 3,    /**< OEM RW OR field. A bit value of '1' can't be changed to '0' */
    PD_SC_OEM_RWOR_FIELD_2         = 4,    /**< OEM RW OR field. A bit value of '1' can't be changed to '0' */
    PD_SC_OEM_RW_FIELD_1           = 5,    /**< OEM RW field 1 */
    PD_SC_OEM_RW_FIELD_2           = 6,    /**< OEM RW field 2 */
    PD_SC_OEM_RW2RO_FUSE           = 7,    /**< 1-bit that can be read to determine if OEM partition is locked */
    PD_SC_REORDER_FUSE             = 8,    /**< 1-bit that can be read to determine if REORDER partition is locked */
    PD_SC_REGIONALIZATION_ID       = 9,
    TOTAL_NUMBER_OF_ID             = 10,
} PDSmartCardOemFieldId_t;

/**
 * @brief                   Read numeric printhead Smart Card field
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param field_id          Field identifier
 * @param data              Returns the numeric data read from printhead smart card.
 *                          Caller need to allocate the memory.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_read_oem_field(int32_t instance, uint8_t ph_id, PDSmartCardOemFieldId_t field_id, uint32_t *data, uint8_t *sc_result);

/**
 * @brief                   Write numeric smart card OEM field.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param field_id          Field identifier
 * @param data              Data to be written to specified field_id.
 * @param sc_result         Smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 *                          
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_write_oem_field(int32_t instance, uint8_t ph_id, PDSmartCardOemFieldId_t field_id, uint32_t data, uint8_t *sc_result);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    PD_SC_OEM_STR_TRADEMARK   = 0,
    PD_SC_OEM_STR_REORDER_PN  = 1,
} PDScOemStrFieldId_t;

/**
 * @brief                   Read printhead string Smart Card field.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param str_field_id      Field identifier
 * @param str               Returns the string as a NULL terminated ASCII.
 *                          Caller need to allocate the memory.
 * @param buf_size          Size of the "str" buffer.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed.
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_read_oem_string_field(int32_t instance, uint8_t ph_id, PDScOemStrFieldId_t str_field_id, char *str, uint8_t buf_size);

/**
 * @brief                   Write printhead string smart card OEM field.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param str_field_id      Field identifier
 * @param str               string to be written.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 *                          
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_write_oem_string_field(int32_t instance, uint8_t ph_id, PDScOemStrFieldId_t str_field_id, char *str);

typedef enum
{
    unknown     = 0,
    Aqueous     = 1,
    Solvent     = 2,
    Bio         = 3,
} PDInkVehicle;

typedef enum
{
    Temp_Off = 0,
    Temp_30C = 1,
    Temp_35C = 2,
    Temp_40C = 3,
    Temp_45C = 4,
    Temp_50C = 5,
    Temp_55C = 6,
    Temp_60C = 7,
    Temp_65C = 8,    
}PDOpTemp;

typedef enum 
{
    OE_NONE         = 0,
    OE_10_percent   = 1,
    OE_15_percent   = 2,
    OE_20_percent   = 3,
    OE_25_percent   = 4,
    OE_30_percent   = 5,
    OE_35_percent   = 6,
    OE_40_percent   = 7,
} PD_OE_Percent;

typedef enum 
{
    VO_NONE     = 0,
    VO_MINUS_10 = 1,
    VO_MINUS_8  = 2,
    VO_MINUS_6  = 3,
    VO_MINUS_4  = 4,
    VO_MINUS_2  = 5,
    VO_PLUS_2   = 6,
    VO_PLUS_4   = 7,
    VO_PLUS_6   = 8,
    VO_PLUS_8   = 9,
    VO_PLUS_10  = 10,    
} PDVoltOffsetPercent;


typedef enum
{
    OP_TEMP_NO_OVERRIDE = 0,
    OP_TEMP_OFF         = 1,
    OP_TEMP_30C         = 2,
    OP_TEMP_35C         = 3,
    OP_TEMP_40C         = 4,
    OP_TEMP_45C         = 5,
    OP_TEMP_50C         = 6,
    OP_TEMP_55C         = 7,
    OP_TEMP_60C         = 8,
    OP_TEMP_65C         = 9,
} PDOpTempOverride;

typedef enum
{
	DRP_WT_CFG_SINGLE	= 0,
	DRP_WT_CFG_DUAL		= 1,
} DropWeightCfg;

typedef enum
{
	NOZZLE_DENSITY_CFG_2400_PER_INCH	= 0,
	NOZZLE_DENSITY_CFG_1200_PER_INCH	= 1,
	NOZZLE_DENSITY_CFG_600_PER_INCH	 	= 2,
	NOZZLE_DENSITY_CFG_300_PER_INCH		= 3,
} NozzleDensityCfg;

typedef struct
{
    uint16_t    max_usable_slot_volume;     /**< Usable volume per slot in ml */
    uint8_t     column_spacing_slota_ldw;   /**< column-to-column spacing in 1/2400th of an inch */
    uint8_t     column_spacing_slota_hdw;   /**< column-to-column spacing in 1/2400th of an inch */
    uint8_t     column_spacing_slotb_ldw;   /**< column-to-column spacing in 1/2400th of an inch */
    uint8_t     column_spacing_slotb_hdw;   /**< column-to-column spacing in 1/2400th of an inch */
    uint32_t    pen_serial_num1;            /**< Pen serial number 1 */
    uint32_t    pen_serial_num2;            /**< Pen serial number 2 */
    
    uint8_t     ctrdg_fill_site_id;         /**< Cartridge Fill manufacturer location identifier. */
    uint8_t     ctrdg_fill_line;            /**< Cartridge Fill manufacturer line identifier */
    uint16_t    ctrdg_fill_year;            /**< Cartridge Fill year (e.g. 2018). */
    uint8_t     ctrdg_fill_woy;             /**< Cartridge Fill week of the year (1-52).*/
    uint8_t     ctrdg_fill_dow;             /**< Cartridge Fill day of week (0-6 = Sunday-Saturday). */
    uint8_t     ctrdg_fill_hour;            /**< Cartridge Fill hour of day */
    uint8_t     ctrdg_fill_min;             /**< Cartridge Fill minute of hour */
    uint8_t     ctrdg_fill_sec;             /**< Cartridge Fill second of minute */
    uint8_t     ctrdg_fill_procpos;         /**< Cartridge Fill tooling process position */
    
    uint8_t     ink_formulator_id_slota;    /**< Ink formulator ID */
    PDInkVehicle ink_vehicle_slota;         /**< ink vehicle type*/
    uint16_t    ink_family_slota;           /**< Ink family */
    uint16_t    ink_family_member_slota;    /**< Ink family member */
    uint8_t     ink_revision_slota;         /**< Ink revision number */
    uint8_t     ink_formulator_id_slotb;    /**< Ink formulator ID */
    PDInkVehicle ink_vechicle_slotb;        /**< ink vehicle type*/
    uint16_t    ink_family_slotb;           /**< Ink family */
    uint16_t    ink_family_member_slotb;    /**< Ink family member */
    uint8_t     ink_revision_slotb;         /**< Ink revision number */
    
    uint8_t     ink_drop_wt_slota_hi;       /**< Drop weight(ng) of High Drop Weight Nozzles in Slot A */
    uint8_t     ink_drop_wt_slota_lo;       /**< Drop weight(ng) of Low Drop Weight Nozzles in Slot A */
    uint8_t     ink_drop_wt_slotb_hi;       /**< Drop weight(ng) of High Drop Weight Nozzles in Slot B */
    uint8_t     ink_drop_wt_slotb_lo;       /**< Drop weight(ng) of Low Drop Weight Nozzles in Slot B */
    float       ink_density_slota;          /**< Ink density in g/ml */
    float       ink_density_slotb;          /**< Ink density in g/ml */
    uint8_t     shelf_life_weeks;           /**< Shelf life of ink in weeks from the time of cartrdige fill */
    uint8_t     shelf_life_days;            /**< Shelf life of ink in days from the time of cartrdige fill */
    uint8_t     installed_life_weeks;       /**< Installed life of the cartridge in weeks from the time it was first installed in the printer */
    uint8_t     installed_life_days;        /**< Installed life of the cartridge in days from the time it was first installed in the printer */
    float       vert_print_usable_ink_wt_slota; /**< Usable ink weight in grams when printing vertically (single-use) */
    float       horz_print_usable_ink_wt_slota; /**< Usable ink weight in grams when printing horizonally (single-use) */
    float       vert_print_usable_ink_wt_slotb; /**< Usable ink weight in grams when printing vertically (single-use) */
    float       horz_print_usable_ink_wt_slotb; /**< Usable ink weight in grams when printing horizonally (single-use) */
    
    PDOpTemp    operating_temp;                         /**< Operating temperature */          
    uint8_t     high_temp_warning;                      /**< Warning temperature */
    uint8_t     max_firing_freq;                        /**< Maximum firing frequency (KHz) */ 
    PD_OE_Percent oe_override_percent_slota;            /**< Over energy percent */
    PD_OE_Percent oe_override_percent_slotb;            /**< Over energy percent */
    PDVoltOffsetPercent volt_offset_override_percent;   /**< Voltage offset override percent */
    PDOpTempOverride operating_temp_ovrride;            /**< Operating Temperature override */
    bool        shf_present_slota;                      /**< "1" if this printhead was shipped with shipping fluid */
    bool        shf_present_slotb;                      /**< "1" if this printhead was shipped with shipping fluid */
    
    uint16_t    first_platform_mfg_year;                /**< Printer platform manufacture year (e.g. 2018) in which this cartrdige was first installed */
    uint8_t     first_platform_mfg_woy;                 /**< Printer platform manufacture week of year (1-52) in which this cartrdige was first installed */
    uint8_t     first_platform_mfg_country;             /**< Printer platform country of manufacture in which this cartrdige was first installed */
    uint8_t     first_platform_fw_rev_major;            /**< Printer platform FW major revision into which this cartrdige was first installed */
    uint8_t     first_platform_fw_rev_minor;            /**< Printer platform FW minor revision into which this cartrdige was first installed */
    /* Check the data type */
    uint8_t     first_install_ctrdg_count;              /**< number of unique cartridges used on the print platform */
    uint16_t    ctrdg_first_install_year;               /**< Year (e.g.2018) in which this cartridge was first installed in a printer platform */
    uint8_t     ctrdg_first_install_woy;                /**< Week of year (1-52) in which this cartridge was first installed in a printer platform */
    uint8_t     ctrdg_first_install_dow;                /**< Day of week (0-6 = Sunday-Saturday) this cartridge was first installed in a printer platform */
    uint8_t     ilg_resolution;                         /**< Ink Level Gauge Resolution */
    /* Check data type */
    uint8_t     ph_orientation;                         /**< PH Orientation. 0=Vertical, 1=Horizontal */
    uint16_t    post_purged_ink_family_member_slota;    /**< Post purged ink family member on slot a */
    uint16_t    post_purged_ink_family_member_slotb;    /**< Post purged ink family member on slot b */
    uint8_t     ext_oem_id;                             /**< Extended OEM ID */
    uint8_t     single_use;                             /**< 0=Single use, 1=Bulk use */
    uint8_t     hp_or_oem_ink_designator_slota;         /**< 0=OEM Ink, 1=HP Ink */
    uint8_t     hp_or_oem_ink_designator_slotb;         /**< 0=OEM Ink, 1=HP Ink */
    uint8_t     regionalization_id;                     /**< Region where this cartridge will be accepted by the printer. */
    
    DropWeightCfg 		drop_weight_cfg_slota;          /**< 0=Single drop weight, 1=Dual drop weight */
    NozzleDensityCfg 	nozzle_density_cfg_slota;       /**< Dots per inch. 0=2400, 1=1200, 2=600, 3=300 */
    DropWeightCfg 		drop_weight_cfg_slotb;          /**< 0=Single drop weight, 1=Dual drop weight */
    NozzleDensityCfg 	nozzle_density_cfg_slotb;       /**< Dots per inch. 0=2400, 1=1200, 2=600, 3=300 */
    
    char        platform_id[12];                        /**< 12-character Platform ID string */
    char        trademark_string[5];                    /**< 5-character trademark string */
    char        ctrdg_reorder_part_num[12];             /**< 12-character reorder part number string */
    
}PDSmartCardInfo_t;

/**
 * @brief                   Returns static smart card data. 
 *                          Intended to be called once at startup or after printhead insertion.
 * @param instance          PD instance id.
 * @param ph_id             
 * @param sc_info           Smartcard information.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_get_info(int32_t instance, uint8_t ph_id, PDSmartCardInfo_t *sc_info, uint8_t *sc_result);
 
/**
 * @brief                   Lock a smart card memory partition. Once the partition
 *                          is locked, the fields in the partition will become read-only.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id. Valid range 0 to (NUM_SUPPORTED_PH - 1)
 * @param partition_id      Partition Identifier. 0 = OEM Partition, 1 = Reorder Partition.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 *                          
 * @par Possible Error Codes None.
 *                          
 */
PDResult_t pd_sc_lock_oem_partition(int32_t instance, uint8_t ph_id, uint8_t partition_id, uint8_t *sc_result);

/**
 * @brief                   Returns the amount of ink used by this printhead as a percentage
 *                          of available ink. This API is valid only for single use printheads.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param gas_gauge         Returns the gas gauge value. Caller need to allocate the memory.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_get_gas_gauge(int32_t instance, uint8_t ph_id, uint32_t *gas_gauge, uint8_t *sc_result);

/*! \struct SmartCardDieUsage
 *  Smart card die usage values. Number of drops ejected equals 
 *  the value in this struct field multiplied by 10^8.
 */
typedef struct 
{
    uint16_t slot_a_high;   /**< Incremented for every 10^8 drops ejected by slot A high-drop volume nozzles */
    uint16_t slot_a_low;    /**< Incremented for every 10^8 drops ejected by slot A low-drop volume nozzles */
    uint16_t slot_b_high;   /**< Incremented for every 10^8 drops ejected by slot B high-drop volume nozzles */
    uint16_t slot_b_low;    /**< Incremented for every 10^8 drops ejected by slot B low-drop volume nozzles */
}PDSmartCardDieUsage;

/**
 * @brief                   Returns the die usage per column. Number of drops ejected
 *                          is equal to 10^8 times the value returned for that column.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param die_usage         Returns the die usage. Caller need to allocate the memory.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_get_die_usage(int32_t instance, uint8_t ph_id, PDSmartCardDieUsage *die_usage, uint8_t *sc_result);

/*! \struct SmartCardStatus
 *  Smart Card Status
 */
typedef struct
{
    bool out_of_ink;                    /**< Out of ink. Used only in the case of single-use printheads.
                                            0 = Not out of ink, 
                                            1 = out of ink */
    bool purge_complete_slot_a;         /**< Shipping fluid Purge complete for slot A. 
                                            0 = Purge not complete, 
                                            1 = Purge completed */
    bool purge_complete_slot_b;         /**< Shipping fluid Purge complete for slot B. 
                                            0 = Purge not completed, 
                                            1 = Purge completed */
    bool altered_ph_detected_slot_a;    /**< Altered Printhead detected on slot A. 
                                            0 = Altered printhead not detected, 
                                            1 = Altered printhead detected */   
    bool altered_ph_detected_slot_b;    /**< Altered Printhead detected on slot B. 
                                            0 = Altered printhead not detected, 
                                            1 = Altered printhead detected */
    bool altered_supply_detected_slot_a;/**< Altered supply detected on slot A. 
                                            0 = Altered supply not detected, 
                                            1 = Altered supply detected */
    bool altered_supply_detected_slot_b;/**< Altered supply detected on slot B 
                                            0 = Altered supply not detected, 
                                            1 = Altered supply detected */
    bool faulty_replace_immediately;     /**< Faulty printhead. 
                                            0 = OK, 
                                            1 = Faulty printhead replace immediately */
    bool pen_short_detected_slot_a;     /**< Pen short short for slot A. 
                                            0 = No short, 
                                            1 = Pen short detected */
    bool pen_short_detected_slot_b;     /**< Pen short detected for slot B. 
                                            0 = No short, 
                                            1 = Pen short detected */
    bool expired_supply_slot_a;        /**< Expired supply for slot A. 
                                            0 = Not expired supply, 
                                            1 = Expired supply */
    bool expired_supply_slot_b;        /**< Expired supply for slot B. 
                                            0 = Not expired supply, 
                                            1 = Expired supply */
    uint8_t crtdg_insertion_count;      /**< Cartridge insertion count */
    uint8_t last_failure_code;          /**< Last failure code */
}PDSmartCardStatus;

/**
 * @brief                   Returns the smart card status fields.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param sc_status         Returns the smart card status. Caller need to allocate the memory.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_get_status(int32_t instance, uint8_t ph_id, PDSmartCardStatus *sc_status, uint8_t *sc_result);

/**
 * @brief                   Write trademark string to printhead smart card.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param trademark_string  5 character ASCII string.
 * @param sc_result         smart card result is returned through this pointer.
 *                          0 = Success, Non-zero on failure.
 * @returns                 Returns result code. PD_ERROR or PD_OK.
 * @par States              Printhead is present and stall door is closed
 * @par Possible Error Codes None
 */
PDResult_t pd_sc_set_trademark_string(int32_t instance, uint8_t ph_id, char trademark_string[], uint8_t *sc_result);

/**
 * @brief                           Write reorder part number string to printhead smart card.
 * @param instance                  PD instance id.
 * @param ph_id                     Printhead id (0 or 1)
 * @param reorder_part_num_string   8 character ASCII string.
 * @param sc_result                 smart card result is returned through this pointer.
 *                                  0 = Success, Non-zero on failure.
 * @returns                         Returns result code. PD_ERROR or PD_OK.
 * @par States                      Printhead is present and stall door is closed
 * @par Possible Error Codes        None
 */
PDResult_t pd_sc_set_reorder_part_num_string(int32_t instance, uint8_t ph_id, char reorder_part_num_string[], uint8_t *sc_result);

/**
 * @brief PD error codes.
 */
typedef enum
{
    PD_STATUS_OK                            = 0, /**< No Error */
    PD_STATUS_ERROR_FW                      = 1, /**< Microcontroller FW Error */
    PD_STATUS_ERROR_FPGA                    = 2, /**< FPGA Error. ?? */
    PD_STATUS_FPGA_NOT_CONFIGURED           = 3, /**< FPGA Error */
    PD_STATUS_FW_AUTH_FAILED                = 4, /**< Invalid Firmware */ 
    PD_STATUS_INCOMPATIBLE_FW               = 5, /**< Incompatible Firmware */ 
    PD_STATUS_FAULTY_DAUGHTER_CARD_ERROR_1  = 6, /**< Faulty daughter card (Error 1) */
    PD_STATUS_SYSTEM_FAILURE                = 7, /**< System Failure */
    PD_STATUS_TEST_MODE                     = 8, /**< System is in Test mode */
    PD_STATUS_REFLASH_MODE                  = 9, /**< Reflash Mode */
    PD_STATUS_SYSTEM_UNPAIRED               = 10,/**< PD is not paired with IDS */
    PD_STATUS_FAULTY_DAUGHTER_CARD_ERROR_2  = 11,/**< Faulty daughter card (Error 2) */
    PD_STATUS_FAULTY_DAUGHTER_CARD_ERROR_3  = 12,/**< Faulty daughter card (Error 3) */
    PD_STATUS_FAULTY_DAUGHTER_CARD_ERROR_4  = 13,/**< Faulty daughter card (Error 4) */
    
    /* Add additional errors here */
} PDError;

/*! \struct PDSystemStatus
 *  PD system status.
 */
typedef struct 
{
    uint8_t        fw_rev_major;       /**< Micro controller firmware Major Revision  */
    uint8_t        fw_rev_minor;       /**< Micro controller firmware Minor Revision  */
    uint8_t        boot_rev_major;     /**< Bootloader Major Revision  */
    uint8_t        boot_rev_minor;     /**< Bootloader Minor Revision  */
    uint8_t        fpga_rev_major;     /**< FPGA code Major Revision  */
    uint8_t        fpga_rev_minor;     /**< FPGA code Minor Revision  */
    uint8_t        blur_board_rev;     /**< PD PCA Revision  */
    uint8_t        driver_board0_rev;  /**< Printhead stall-1 PCA revision  */
    uint8_t        driver_board1_rev;  /**< Printhead stall-2 PCA revision  */
    PDError        pd_status;          /**< PD error code  */
    uint16_t       board_id;           /**< Board ID */
    
}PDSystemStatus;

/**
 * @brief                   Get PD System Status. It is recommended that when PD MCU
 *                          asserts a Warning or Critical Interrupt, OEM controller 
 *                          invokes this function to find what caused the warning or error.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param pd_system_status  Returns the PD System Status. Caller need to allocate memory.
 * @returns                 It can return PD_ERROR or PD_OK.
 * @par States              
 *                          It is valid during all states
 * @par Possible Error Codes (set by FW for this API)   
 *                          No Error Codes
 */
PDResult_t pd_get_system_status(int32_t instance, PDSystemStatus *pd_system_status);

/**
 * @brief Printhead state
 */
typedef enum {
    PH_STATE_POWERED_ON     = 0, /**< Printhead is Powered ON */
    PH_STATE_POWERED_OFF    = 1, /**< Printhead is Powered OFF */
    PH_STATE_PRESENT        = 2, /**< Printhead is present */
    PH_STATE_NOT_PRESENT    = 3, /**< Printhead is not present */
    PH_STATE_INVALID        = 4, /**< Printhead is Invalid */
}PrintHeadState;

/**
 * @brief Printhead Error codes. 
 */
typedef enum{    
    PH_NO_ERROR                         = 0,  /**< No error */
    PH_ACUMEN_RDWR_ERROR                = 1,  /**< Smart Chip access error */
    PH_REG_RDWR_ERROR                   = 2,  /**< FPGA register access error */
    PH_VDD_POWON_FAIL_ERROR             = 3,  /**< VDD Power ON error */
    PH_VDD_UV_ERROR                     = 4,  /**< VDD under voltage error */
    PH_VDD_OV_ERROR                     = 5,  /**< VDD over voltage error */
    PH_VPP_POWON_FAIL_ERROR             = 6,  /**< VPP Power On error */
    PH_VPP_ADJ_FAIL_ERROR               = 7,  /**< VPP adjust error */
    PH_VPP_VALIDATION_ERROR             = 8,  /**< VPP validation error */
    PH_INKSHORT_ERROR                   = 9,  /**< Ink short detected */
    PH_DIE_RDWR_ERROR                   = 10, /**< Printhead die access error */
    PH_OVTEMP_ERROR                     = 11, /**< Over temperature warning */
    PH_OVSPEED_ERROR                    = 12, /**< Over drive detected */
    PH_CONTINUITY_ERROR                 = 13, /**< Continuity error */
    PH_INVALID_STATE_FOR_INPUT_ERROR    = 14, /**< Received command invalid in present state */
    PH_TEMP_SHUTDOWN_ERROR              = 15, /**< Temperature shutdown */
    PH_PULSEWIDTH_VALIDITY_ERROR        = 16, /**< Invalid Pulse width error. This may go away. */
    PH_VOLTAGE_VALIDITY_ERROR           = 17, /**< Invalid input voltage error. This may go away.*/
    PH_GPIO_RDWR_ERROR                  = 18, /**< uController GPIO access error. */
    PH_VPP_POWOF_FAIL_ERROR             = 19, /**< VPP power off error. */
    PH_VPP_UV_ERROR                     = 20, /**< VPP undervoltage error */
    PH_IO_RDWR_ERROR                    = 21, /**< I2C I/O expander access error */
    PH_DRP_PROC_ERROR                   = 22, /**< Drop counting error */
    PH_VPP_GOOD_ERROR                   = 23, /**< VPPGOOD not set error. */
    PH_VPP_GOODSTABLE_ERROR             = 24, /**< VPPGOOD not stable error. */
    PH_VPP_SHUNTDISABLE_ERROR           = 25, /**< VPP Shunt disable error. */
    PRINTHEAD_VDD_VPP_SHORT_ERROR       = 26, /**< When VDD,VPP short condition is detected */
    PRINTHEAD_VPP_NOT_POWOF_ERROR       = 27, /**< When VPP is not going below 5V after powered off, error. */
    PRINTHEAD_EC_PW_SET_ERROR             = 28, /**< When FP width is not possible to set in the die, error. */
    PRINTHEAD_EC_SPIT_ERROR               = 29, /**< When spitting fails during energy calibration process. */
    PRINTHEAD_EC_VOLTSET_ERROR            = 30, /**< When could not set the energy calibration voltage */
    PRINTHEAD_EC_TEMP_TIMEOUT_ERROR       = 31, /**< When target temp is not reached during energy calibration process start in 15s.*/
    PRINTHEAD_EC_INVALID_STATE_ERROR      = 32, /**< When printhead is not in powered on state during energy calibration process.*/
    PRINTHEAD_EC_TEMP_FAULT_ERROR         = 33, /**< When any of the temp fault is set , after spitting during energy calibration process.*/
    PRINTHEAD_PURGE_INVALID_STATE_ERROR   = 34, /**< When printhead is not in powered on state during Purge process.*/
    PRINTHEAD_PURGE_INVALID_PH_ERROR      = 35, /**< When unauthorized / invalid printhead is being purged.*/
    PRINTHEAD_PURGE_SPIT_ERROR            = 36, /**< When spitting fails during Purge process.*/
    PRINTHEAD_PURGE_TEMP_FAULT_ERROR      = 37, /**< When any of the temp fault is set , after spitting during Purge process.*/
    PRINTHEAD_PURGE_INVALID_SLOT_ERROR    = 38, /**< When slot input for purge process is an invalid slot.*/
    PRINTHEAD_INVALID_ERROR               = 39, /**< When printhead doesnot meet the acceptance criteria.*/
    PRINTHEAD_TEMP_VALIDITY_ERROR         = 40, /**< When target temp in acumen is out of acceptable range.*/
    PRINTHEAD_VPP_AVG_ERROR               = 41, /**< When VPP averaging process fails due to failure in adc read.*/
    PRINTHEAD_VPP_NOT_CONVERGING_ERROR    = 42, /**< When VPP voltage is not converging within the max loop counts.*/
    PRINTHEAD_TTOE_SHFPRESENT_ERROR       = 43, /**When SHF in printhead is not purged yet but TTOE process is initiated. */
    PRINTHEAD_IDS_OVRNOTSET_ERROR         = 44, /**< When Overrides from IDS is not set for each slot of the printhead. */
    PRINTHEAD_IDS_NOTPAIRED_ERROR         = 45, /**< When one/both the slots of printhead is not paired with  IDS. */
    PRINTHEAD_IDS_NOHSHAKE_ERROR          = 46, /**< When there is no handshake from IDS in 60seconds period */
    PRINTHEAD_IDS_SUPPLY_ERROR            = 47, /**< When there is supply error in IDS  */
    PRINTHEAD_IDS_MSGNOTVALID_ERROR       = 48,  /**< When message from IDS is not a valid one. */
    PRINTHEAD_TTOEPURGE_IDSNOHSHAKE_ERROR   = 49,  /**< When there is no IDS handshake between two purge/TTOE process */
    PRINTHEAD_ALTERED_ERROR                 = 50,  /**< When altered printhead is detected in singleuse. */
    PRINTHEAD_OUTOFINK_ERROR                = 51,  /**< When out of ink condition is detected. */ 
    PRINTHEAD_PAIRINGDONE_NOTUPDATED_ERROR  = 52,  /**<When pairing done status is not marked correctly for the printhead */
    PRINTHEAD_PAIRINGINFO_READ_ERROR        = 53,   /**<When pairing info read is giving error.*/          
    PRINTHEAD_DIEID_ERROR                   = 54,  /**<When not able to set dieid of the printhead */
    PRINTHEAD_DIECONTINUITY_ERROR           = 55,  /**<When continuity test is failing */
    PRINTHEAD_IDSEXPIRED_ERROR              = 56,  /**<When IDS status is set as expired in IDS handshake */
    PRINTHEAD_PARTIALINKSHORT_ERROR         = 57,  /**When Partial Ink Short condition is detected */
    
} PrintHeadError;

/*! \struct PrintHeadStatus
 *  Printhead status
 */
typedef struct 
{
    PrintHeadState      print_head_state;   /**< Printhead state */
    PrintHeadError      print_head_error;   /**< Printhead error code */
    bool                energy_calibrated;  /**< Is the Printhead energy calibrated */
    bool                temp_calibrated;    /**< Is the Printhead temeprature calibrated */
    bool                slot_a_purge_completed; /**< Is purge completed for slot a */
    bool                slot_b_purge_completed; /**< Is purge completed for slot b */
    bool                overdrive_warning;  /**< Overdrive warning has occured after the last read of status */
    bool                overtemp_warning;   /**< Overtemp warning has occured after the last read of status */
    bool                supplyexpired_warning; /**< Idsexpired warning has occured after the last read of status */
}PrintHeadStatus;

/**
 * @brief                       Get the Printhead Status. It is recommended that when PD MCU 
 *                              asserts a Warning or Critical Interrupt, OEM controller 
 *                              invokes this function to find what caused the warning or error.
 * @param instance              PD instance id.(instance should be between 1 and 16)
 * @param pen_id                Printhead id (0 or 1)
 * @param print_head_status     Returns the PD System Status. Caller need to allocate memory.
 * @returns                     It can return PD_ERROR or PD_OK.
 * @par States                  
 *                              It is valid during PhPowerOnState,PhPowerOffState ,PhPresentState,PhNotValidState and PhAbsentState.
 * @par Possible Error Codes (set by FW for this API)    
 *                              No Error Codes
 */
PDResult_t pd_get_print_head_status(int32_t instance, uint8_t pen_id, PrintHeadStatus *print_head_status);

/**
 * @brief                       Calibrates the firing pulse width.
 *                              This command is allowed only when the Printhead is Powered ON.
 * @param instance              PD instance id.(instance should be between 1 and 16)
 * @param ph_id                 Printhead id (0 or 1)
 * @param slot                  Slot number for which pulse width calibration is requested.(Valid range 0 to (NUM_TRENCH_PER_PH - 1))
 * @param calibrated_fpwidth    Value of calibrated firing pulse width. Its a pointer to unsigned integer.
 *                              Caller need to allocate memory.
 * @returns                     It can return PD_ERROR or PD_OK.
 * @par States                  
 *                              It is valid during PhPowerOnState.
 * @par Possible Error Codes (set by FW for this API)    
 *                              No Error Codes
 */
PDResult_t pd_calibrate_pulsewidth(int32_t instance, uint8_t ph_id, uint8_t slot, uint32_t *calibrated_fpwidth);

/**
 * @brief                       THIS FUNCTION IS DEPRECATED. Returns the amount of ink consumed by requested printhead's slot in cc (cubic centimeter) 
 *                              This command is allowed only when the Printhead is present, irrespective of whether it is powered on/off.
 * @param instance              PD instance id.(instance should be between 1 and 16)
 * @param ph_id                 Printhead id (0 or 1)
 * @param slot                  Slot number for which drops volume is requested.(Valid range 0 to (NUM_TRENCH_PER_PH - 1))
 * @param dropsvolume           Drops volume of Requested Slot number in cc. Caller need to allocate memory. 
 * @returns                     It can return PD_ERROR or PD_OK.
 * @par States                  
 *                              It is valid during PhPowerOnState, PhPowerOffState and PhPresentState.
 * @par Possible Error Codes (set by FW for this API)    
 *                              No Error Codes
 */
PDResult_t pd_get_dropsvolume(int32_t instance, uint8_t ph_id, uint8_t slot, float *dropsvolume);

/**
 * @brief                  	    Returns the amount of ink consumed (grams) since this call was made last time.
 *                           	This command is allowed only when the Printhead is present, irrespective of whether it is powered on/off.
 *                              This function need to be polled by OEM controller when the printhead 
 *                              is powered on to implement IDS PILS algorithm.
 * @param instance              PD instance id.(instance should be between 1 and 16)
 * @param ph_id                 Printhead id (0 or 1)
 * @param slot                  Slot number.
 * @param ink_weight            Ink weight consumed. Caller need to allocate memory. 
 * @returns                     It can return PD_ERROR or PD_OK.
 * @par States                  
 *                              It is valid during PhPowerOnState, PhPowerOffState and PhPresentState.
 * @par Possible Error Codes (set by FW for this API)    
 *                              No Error Codes
 */
PDResult_t pd_get_used_ink_weight(int32_t instance, uint8_t ph_id, uint8_t slot, float *ink_weight);

/**
 * @brief                       Get the printhead temperature in degree C
 * @param instance              PD instance id.(instance should be between 1 and 16)
 * @param ph_id                 Printhead id (0 or 1)
 * @param degree_c              Printhead temperature in degree_c. 
 * @returns                     It can return PD_ERROR and PD_OK 
 * @par States                  
 *                              It is valid during PhPowerOnState
 * @par Possible Error Codes (set by FW for this API)
 *                              No Error Codes
 */
PDResult_t pd_get_temperature(int32_t instance, uint8_t ph_id, uint32_t *degree_c);

/**
 * @brief                       Set printhead target temperature override.
 * @param instance              PD instance id.
 * @param ph_id                 Printhead id (0 or 1)
 * @param t                     Printhead override temperature in degree_c as follows :
 * @par
 *          | t |  Target temperature |
 *          |---|---------------------|
 *          | 0 |  Heating Off        |
 *          | 1 |  30°C               |
 *          | 2 |  35°C               |
 *          | 3 |  40°C               |
 *          | 4 |  45°C               |
 *          | 5 |  50°C               |
 *          | 6 |  55°C               |
 *          | 7 |  60°C               |
 *          | 8 |  65°C               |
 *
 * @returns                     It can return PD_ERROR and PD_OK 
 * @par    States              
 *                              It is valid during PhPowerOffState and PhPresentState
 * @par Possible Error Codes (set by FW for this API)
 *                              No Error Codes
 */
PDResult_t pd_set_temperature_override(int32_t instance, uint8_t ph_id, uint8_t t);

/**
 * @brief                       Get printhead temperature override.
 * @param instance              PD instance id.
 * @param ph_id                 Printhead id (0 or 1)
 * @param t                     Printhead override temperature in degree_c as in below table. 
 *                              Caller need to allocate memory.
 * @par
 *          | *t|  Target temperature |
 *          |---|---------------------|
 *          | 0 |  Heating Off        |
 *          | 1 |  30°C               |
 *          | 2 |  35°C               |
 *          | 3 |  40°C               |
 *          | 4 |  45°C               |
 *          | 5 |  50°C               |
 *          | 6 |  55°C               |
 *          | 7 |  60°C               |
 *          | 8 |  65°C               |
 *
 * @returns                       It can return PD_ERROR and PD_OK 
 * @par States              
 *                                It is valid during PhPowerOnState, PhPowerOffState and PhPresentState
 * @par Possible Error Codes (set by FW for this API)
 *                              No Error Codes
 */
PDResult_t pd_get_temperature_override(int32_t instance, uint8_t ph_id, uint8_t *t);

/**
 * @brief                   Set the printhead voltage override.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param v                 Printhead override voltage as in below table.
 * @par
 *          |v  |   % of default voltage        |
 *          |---|-------------------------------|
 *          |0  |   No Override/Default Voltage |
 *          |1  |   -10% of default voltage     |
 *          |2  |   - 8% of default voltage     |
 *          |3  |   - 6% of default voltage     |
 *          |4  |   - 4% of default voltage     |
 *          |5  |   - 2% of default voltage     |
 *          |6  |   + 2% of default voltage     |
 *          |7  |   + 4% of default voltage     |
 *          |8  |   + 6% of default voltage     |
 *          |9  |   + 8% of default voltage     |
 *          |10 |   +10% of default voltage     |
 *
 * @returns                 It can return PD_ERROR and PD_OK 
 * @par States            
 *                          It is valid during PhPowerOffState and PhPresentState
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_set_voltage_override(int32_t instance, uint8_t ph_id, uint8_t v);

/**
 * @brief                   Get the Printhead voltage override as a 
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param v                 Printhead override voltage as in below table.
 *                          Caller need to allocate memory.
 * @par         
 *          |v  |   % of default voltage        |
 *          |---|-------------------------------|
 *          |0  |   No Override/Default Voltage |
 *          |1  |   -10% of default voltage     |
 *          |2  |   - 8% of default voltage     |
 *          |3  |   - 6% of default voltage     |
 *          |4  |   - 4% of default voltage     |
 *          |5  |   - 2% of default voltage     |
 *          |6  |   + 2% of default voltage     |
 *          |7  |   + 4% of default voltage     |
 *          |8  |   + 6% of default voltage     |
 *          |9  |   + 8% of default voltage     |
 *          |10 |   +10% of default voltage     |
 *
 * @returns                 It can return PD_ERROR and PD_OK 
 * @par States              
 *                          It is valid during PhPowerOnState, PhPowerOffState and PhPresentState
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_get_voltage_override(int32_t instance, uint8_t ph_id, uint8_t *v);

/**
 * @brief                   Set the over energy override on a printhead slot.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param slot              Slot number. (Valid range 0 to (NUM_TRENCH_PER_PH - 1))
 * @param e                 Printhead energy in % as in below table.
 * @par
 *          | e |   Over Energy Override % |
 *          |---|--------------------------|
 *          | 0 | No override              |
 *          | 1 | 10% Over Energy          |
 *          | 2 | 15% Over Energy          |
 *          | 3 | 20% Over Energy          |
 *          | 4 | 25% Over Energy          |
 *          | 5 | 30% Over Energy          |
 *          | 6 | 35% Over Energy          |
 *          | 7 | 40% Over Energy          |
 *
 * @returns                 It can return PD_ERROR and PD_OK 
 * @par	States              
 *                          It is valid during PhPowerOffState and PhPresentState
 * @par	Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_set_over_energy_override(int32_t instance, uint8_t ph_id, uint8_t slot, uint8_t e);

/**
 * @brief                   Get the over energy override on a printhead slot.
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param slot              Slot number. (Valid range 0 to (NUM_TRENCH_PER_PH - 1))
 * @param e                 Printhead energy in % as in below table. Its a pointer to unsigned character. 
 *                          Caller need to allocate memory. 
 * @par
 *          | e |   Over Energy Override % |
 *          |---|--------------------------|
 *          | 0 | No override              |
 *          | 1 | 10% Over Energy          |
 *          | 2 | 15% Over Energy          |
 *          | 3 | 20% Over Energy          |
 *          | 4 | 25% Over Energy          |
 *          | 5 | 30% Over Energy          |
 *          | 6 | 35% Over Energy          |
 *          | 7 | 40% Over Energy          |
 *
 * @returns                 It can return PD_ERROR and PD_OK 
 * @par States              
 *                          It is valid during PhPowerOnState, PhPowerOffState and PhPresentState
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_get_over_energy_override(int32_t instance, uint8_t ph_id,  uint8_t slot, uint8_t *e);


/**
 * @brief                   Get printhead serial number
 * @param instance          PD instance id.
 * @param ph_id             Printhead id (0 or 1)
 * @param serial_number     Printhead serial number. Its a pointer to array of 20 characters.
 *                          Caller need to allocate memory.
 * @returns                 It can return PD_ERROR and PD_OK .
 * @par States              
 *                          It is valid during PhPowerOnState, PhPowerOffState and PhPresentState.
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_get_pen_serial_number(int32_t instance, uint8_t ph_id, char *serial_number);

/*! \struct FpgaRecord_t
 *  FPGA log Report.
 */
typedef struct
{
    uint32_t    timeStamp;  /**< Time Stamp*/
    uint8_t     b1;         /**< Byte 1*/
    uint8_t     b2;         /**< Byte 2*/
    uint8_t     b3;         /**< Byte 1*/
    uint8_t     reply;      /**< Reply */
    uint8_t     result;     /**< Result*/
} FpgaRecord_t;

/**
 * @brief                   Get FPGA logs from PD memory. This is intended for troubleshooting.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param records           Pointer to array of struct type "FpgaRecord_t". Records are filled in to
 *                          this buffer. Caller need to allocate memory.
 * @param records_size      Number of records allocated in "records" pointer.
 * @param records_returned  Actual number of records that returned. If there are no more logs
 *                          available in memory, this value will be zero.
 * @returns                 Result.
 */
PDResult_t pd_get_fpga_log(int32_t instance, FpgaRecord_t *records, size_t records_size, size_t *records_returned);

/**
 * @brief                   Purge shipping and handling fluid from printhead slot(s). 
 *                          This is a blocking call and returns only after the purge is completed.
 *                          This may take up to 90 seconds for it to complete the purge.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param ph_id             Printhead id (0 or 1)
 * @param slot              Slot number(s). 0 = Slot A,
 *                                          1 = Slot B, 
 *                                          2 = Both Slot A & Slot B
 * @returns                 It can return PD_ERROR and PD_OK .
 * @par States              
 *                          It is valid during PhPowerOnState.
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_start_purging(int32_t instance, uint8_t ph_id, uint8_t slot);

/**
 * @brief                   Provide printhead state info.
 * @param instance          PD instance id.(instance should be between 1 and 16)
 * @param ph_id             Printhead id (0 or 1)
 * @param data              Its a pointer to unsigned character array of size 20.
 * @returns                 It can return PD_ERROR and PD_OK .
 * @par States              
 *                          It is valid during PhPowerOnState, PhPowerOffState and PhPresentState.
 * @par Possible Error Codes (set by FW for this API)
 *                          No Error Codes
 */
PDResult_t pd_get_state_info(int32_t instance, uint8_t ph_id, uint8_t *data);

/**
 * @brief               To set the date in PD.
 * @param instance      PD instance id.(instance should be between 1 and 16)
 * @param year          Year (e.g. 2018)
 * @param month         Month (1-12)
 * @param day           Day of month (1-31)
 * @returns             It can return PD_ERROR and PD_OK .
 * @par States              
 *                          It is valid during all states
 * @par Possible Error Codes (set by FW for this API)   
 *                          No Error Codes 
 *
 */
PDResult_t pd_set_date(int32_t instance, uint16_t year, uint8_t month, uint8_t day);

/**
 * @brie                To Set platform information in the PD
 * @param instance      PD instance id.(instance should be between 1 and 16)
 * @param platform_info Pointer to PlatformInfo structure
 * @Return              It can return PD_ERROR and PD_OK .
 * @par States              
 *                          It is valid during all states
 * @par Possible Error Codes (set by FW for this API)   
 *                          No Error Codes 
 *
 */
PDResult_t pd_set_platform_info(int32_t instance, PlatformInfo_t *platform_info);

PDResult_t pd_pairing_delete(int32_t instance);

PDResult_t pd_terminate_session(int32_t instance);

PDResult_t pd_set_secure_overrides(int32_t instance, int32_t ph_id, int32_t slot_idx, uint8_t *in, int32_t in_size, int32_t *status);

PDResult_t pd_ink_use(int32_t instance, uint8_t ph_id, uint8_t slot_id, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);

PDResult_t pd_supply_status(int32_t instance, uint8_t ph_id, uint8_t slot_id, uint8_t *in, uint16_t in_len, int32_t *status);

PDResult_t pd_get_ph_status(int32_t instance, int32_t *status0, int32_t *status1);


#endif /* __PRINT_HEAD_DRIVER_IFC_H__ */

