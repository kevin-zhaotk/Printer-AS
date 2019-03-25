/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card_layout.h
//-----------------------------------------------------------------------------
// Description: Smart card memory layout configuration. This layout might be
//              different for different products.
//              This is layout is for Memory Map revision 1.5
//
//*****************************************************************************
#ifndef _HP_SMART_CARD_LAYOUT_H_
#define _HP_SMART_CARD_LAYOUT_H_

#include "hp_smart_card_types.h"


// >>>>>>>>>>>> HP Smart Card map <<<<<<<<<<<<

enum hp_smart_card_ink_tags             // (must begin with 1 and be consecutive values)
{
    HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG            = 0x01,
    HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG       = 0x02,
    HP_SMART_CARD_INK_INK_FILL_MFG_DATA_TAG             = 0x03,
    HP_SMART_CARD_INK_INK_DATA_AND_PROPERTIES_TAG       = 0x04,
    HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG    = 0x05,
    HP_SMART_CARD_INK_CTRDG_FIRST_INSTALLED_DATA_TAG    = 0x06,
    HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG           = 0x07,
    HP_SMART_CARD_INK_HP_TRADEMARK_TAG                  = 0x08,
    HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG          = 0x09,
    HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG              = 0x0A,
    HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG                = 0x0B,
    HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG               = 0x0C,
    HP_SMART_CARD_INK_EXT_OEM_ID_TAG                    = 0x0D,
    HP_SMART_CARD_INK_REORDER_PART_NUM_TAG              = 0x0E,
};

/* HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_dry_cartridge_mfg_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD0_FUSE,                    1, 7,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_BLANK_1,                     1, 6,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_BLANK_2,                     2, 6,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MANUFACTURE_ID,              2, 5,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MANUFACTURE_LINE,            3, 7,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MANUFACTURE_YEAR,            3, 2,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MANUFACTURE_WEEK,            4, 5,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SERIAL_NUM_DOW,              5, 7,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SERIAL_NUM_HOD,              5, 4,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SERIAL_NUM_MOH,              6, 7,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SERIAL_NUM_SOM,              6, 1,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_DRY_PROCESS_POSITION,        7, 3,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME, 7, 0, 16 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION, 9, 0,  5 },
};

#define N_INK_SPS_DRY_CTRDG_MFG_DATA_BYTES      12
#define N_INK_SPS_DRY_CTRDG_MFG_DATA_FIELDS     (sizeof(smart_card_ink_sps_dry_cartridge_mfg_data) / sizeof(HP_SMART_CARD_field_desc_t))


/* HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_dry_cartridge_specific_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TSR,                     1, 7, 7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT, 1, 0, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_BULK_CTRDG,              2, 0, 1 },
};

#define N_INK_SPS_DRY_CTRDG_SPECIFIC_DATA_BYTES     16
#define N_INK_SPS_DRY_CTRDG_SPECIFIC_DATA_FIELDS    (sizeof(smart_card_ink_sps_dry_cartridge_specific_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_INK_FILL_MFG_DATA_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_ink_fill_mfg_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD1_FUSE,                        1, 7, 1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID,          1, 6, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE,             2, 6, 5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR,             2, 1, 5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY,              3, 4, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW,              4, 6, 3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD,              4, 3, 5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH,              5, 6, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM,              5, 0, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION, 6, 2, 3 },
};

#define N_INK_SPS_INK_FILL_MFG_DATA_BYTES       20
#define N_INK_SPS_INK_FILL_MFG_DATA_FIELDS      (sizeof(smart_card_ink_sps_ink_fill_mfg_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_INK_DATA_AND_PROPERTIES_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_data_and_properties_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FORMULATOR_ID,                      1, 7,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INK_FAMILY,                         2, 7,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_COLOR_CODES_GENERAL,                3, 7,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC,               3, 4,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INK_FAMILY_MEMBER,                  3, 1,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INK_ID_NUM,                         4, 1, 12 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INK_REVISION,                       6, 5,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_DENSITY,                            7, 5, 10 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION,              8, 3,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR,           9, 6,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SHELF_LIFE_WEEKS,                   9, 3,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_SHELF_LIFE_DAYS,                   10, 3,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS,              10, 0,  8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS,               11, 0,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_USABLE_INK_WEIGHT,                 12, 5, 16 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL, 14, 5,  7 },
};

#define N_INK_SPS_DATA_AND_PROPERTIES_DATA_BYTES        16
#define N_INK_SPS_DATA_AND_PROPERTIES_DATA_FIELDS       (sizeof(smart_card_ink_sps_data_and_properties_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_filled_cartridge_specific_data[] =
{
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRING_FREQ,        1, 7, 7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_PULSE_WIDTH,        1, 0, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRING_VOLTAGE,     2, 0, 7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TURN_ON_ENERGY,     3, 1, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_PULSE_WARMING_TEMP, 4, 1, 7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MAX_TEMP,           5, 2, 7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_DROP_VOLUME,        6, 3, 6 },
};

#define N_INK_SPS_FILLED_CTRDG_SPECIFIC_DATA_BYTES      12
#define N_INK_SPS_FILLED_CTRDG_SPECIFIC_DATA_FIELDS     (sizeof(smart_card_ink_sps_filled_cartridge_specific_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_CTRDG_FIRST_INSTALLED_DATA_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_first_installed_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_WRITE_PROTECT,                     1, 7,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1,          1, 6,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_2,          1, 0,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_3,          2, 2,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_4,          3, 4,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_5,          4, 6,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_6,          4, 0,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_7,          5, 2,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_8,          6, 4,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_9,          7, 6,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_10,         7, 0,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_11,         8, 2,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_12,         9, 4,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR,          10, 6,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY,           10, 1,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY,       11, 3,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION, 12, 5,  4 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION, 12, 1,  4 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT,    13, 5, 12 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_INSTALL_YEAR,               14, 1,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_INSTALL_WOY,                15, 4,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_INSTALL_DOW,                16, 6,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION,       16, 3,  7 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD,          17, 4, 21 },
};
#define N_INK_SPS_FIRST_INSTALLED_DATA_BYTES        20
#define N_INK_SPS_FIRST_INSTALLED_DATA_FIELDS       (sizeof(smart_card_ink_sps_first_installed_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG  */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_oem_locable_memory_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD3_FUSE,            1, 7,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1, 1, 6, 31 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2, 5, 7, 32 },
};

#define N_INK_SPS_OEM_LOCKABLE_MEMORY_DATA_BYTES        8
#define N_INK_SPS_OEM_LOCKABLE_MEMORY_DATA_FIELDS       (sizeof(smart_card_ink_sps_oem_locable_memory_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_HP_TRADEMARK_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_trademark_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1, 1, 7, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_2, 1, 1, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_3, 2, 3, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_4, 3, 5, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_5, 4, 7, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD4_FUSE,            4, 1, 1 },
};
#define N_INK_SPS_HP_TRADEMARK_DATA_BYTES       8
#define N_INK_SPS_HP_TRADEMARK_DATA_FIELDS      (sizeof(smart_card_ink_sps_trademark_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_ink_use_and_warranty_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OUT_OF_INK_BIT,              1, 7,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ILG_BIT_01_25,               1, 6, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ILG_BIT_26_50,               4, 5, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ILG_BIT_51_75,               7, 4, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ILG_BIT_76_100,             10, 3, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TUG_BIT_01_25,              13, 2, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TUG_BIT_26_50,              16, 1, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TUG_BIT_51_75,              19, 0, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_TUG_BIT_76_100,             23, 7, 25 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FIRST_FAILURE_CODE,         26, 6,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_ALTERED_SUPPLY,             26, 0,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK,       27, 7,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK,       27, 6,  1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY, 27, 5,  1 },
};
#define N_INK_SPS_INK_USE_AND_WARRANTY_DATA_BYTES       32
#define N_INK_SPS_INK_USE_AND_WARRANTY_DATA_FIELDS      (sizeof(smart_card_ink_sps_ink_use_and_warranty_data) / sizeof(HP_SMART_CARD_field_desc_t))


/* HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_oem_rw_or_memory_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1, 1, 7, 32 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2, 5, 7, 32 },
};
#define N_INK_SPS_OEM_RW_OR_MEMORY_DATA_BYTES       8
#define N_INK_SPS_OEM_RW_OR_MEMORY_DATA_FIELDS      (sizeof(smart_card_ink_sps_oem_rw_or_memory_data) / sizeof(HP_SMART_CARD_field_desc_t))


/* HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_hp_scratch_pad_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR,         1, 7,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY,          1, 2,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW,          2, 4,  3 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,     2, 1,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_2,     3, 3,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_3,     4, 5,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_4,     5, 7,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_5,     5, 1,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_6,     6, 3,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_7,     7, 5,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_8,     8, 7,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_9,     8, 1,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_10,    9, 3,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_11,   10, 5,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_12,   11, 7,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR,     11, 1,  5 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY,      12, 4,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY,  13, 6,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR, 13, 0,  4 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR, 14, 4,  4 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_INSERTION_COUNT,           14, 0,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_STALL_INSERTION_COUNT,     15, 2, 16 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_LAST_FAILURE_CODE,         17, 2,  6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_USER_REPORTED_STATUS,      18, 4,  4 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_MARKETING_DATA_REVISION,   23, 0,  4 },
};
#define N_INK_SPS_HP_SCRATCH_PAD_DATA_BYTES     32
#define N_INK_SPS_HP_SCRATCH_PAD_DATA_FIELDS    (sizeof(smart_card_ink_sps_hp_scratch_pad_data) / sizeof(HP_SMART_CARD_field_desc_t))


/* HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_oem_scratch_pad_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1, 1, 7, 32 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2, 5, 7, 32 },
};
#define N_INK_SPS_OEM_SCRATCH_PAD_DATA_BYTES        8
#define N_INK_SPS_OEM_SCRATCH_PAD_DATA_FIELDS       (sizeof(smart_card_ink_sps_oem_scratch_pad_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_EXT_OEM_ID_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_sps_ext_oem_id_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD7_FUSE,              1, 7, 1 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_EXTENDED_OEM_ID,       1, 6, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR, 2, 6, 1 },
};

#define N_INK_SPS_EXT_OEM_ID_BYTES      4
#define N_INK_SPS_EXT_OEM_ID_FIELDS     (sizeof(smart_card_ink_sps_ext_oem_id_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HP_SMART_CARD_INK_REORDER_PART_NUM_TAG */
const HP_SMART_CARD_field_desc_t smart_card_ink_reorder_part_num_data[] =
{
    /* field,                                              start byte, Start bit, size */
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REGIONALIZATION_ID,        1, 7, 8 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1,   2, 7, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_2,   2, 1, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_3,   3, 3, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_4,   4, 5, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_5,   5, 7, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_6,   5, 1, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_7,   6, 3, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_8,   7, 5, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_9,   8, 7, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_10,  8, 1, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_11,  9, 3, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_12, 10, 5, 6 },
    { (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_UD8_FUSE,                 11, 7, 1 },
};

#define N_INK_SPS_REORDER_PART_NUM_BYTES        12
#define N_INK_SPS_REORDER_PART_NUM_FIELDS       (sizeof(smart_card_ink_reorder_part_num_data) / sizeof(HP_SMART_CARD_field_desc_t))


const HP_SMART_CARD_tag_desc_t hp_smart_card_ink_tag_fields[] =
{
    /*                         TAG,                                                                 bytes,                              number of fields,                 pointer to fields in tag                                      */

    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG,         N_INK_SPS_DRY_CTRDG_MFG_DATA_BYTES,         N_INK_SPS_DRY_CTRDG_MFG_DATA_FIELDS,         smart_card_ink_sps_dry_cartridge_mfg_data,         C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG,    N_INK_SPS_DRY_CTRDG_SPECIFIC_DATA_BYTES,    N_INK_SPS_DRY_CTRDG_SPECIFIC_DATA_FIELDS,    smart_card_ink_sps_dry_cartridge_specific_data,    C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_INK_FILL_MFG_DATA_TAG,          N_INK_SPS_INK_FILL_MFG_DATA_BYTES,          N_INK_SPS_INK_FILL_MFG_DATA_FIELDS,          smart_card_ink_sps_ink_fill_mfg_data,              C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_INK_DATA_AND_PROPERTIES_TAG,    N_INK_SPS_DATA_AND_PROPERTIES_DATA_BYTES,   N_INK_SPS_DATA_AND_PROPERTIES_DATA_FIELDS,   smart_card_ink_sps_data_and_properties_data,       C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG, N_INK_SPS_FILLED_CTRDG_SPECIFIC_DATA_BYTES, N_INK_SPS_FILLED_CTRDG_SPECIFIC_DATA_FIELDS, smart_card_ink_sps_filled_cartridge_specific_data, C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_CTRDG_FIRST_INSTALLED_DATA_TAG, N_INK_SPS_FIRST_INSTALLED_DATA_BYTES,       N_INK_SPS_FIRST_INSTALLED_DATA_FIELDS,       smart_card_ink_sps_first_installed_data,           C_MODE_RW2RO },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG,        N_INK_SPS_OEM_LOCKABLE_MEMORY_DATA_BYTES,   N_INK_SPS_OEM_LOCKABLE_MEMORY_DATA_FIELDS,   smart_card_ink_sps_oem_locable_memory_data,        C_MODE_RW2RO },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_HP_TRADEMARK_TAG,               N_INK_SPS_HP_TRADEMARK_DATA_BYTES,          N_INK_SPS_HP_TRADEMARK_DATA_FIELDS,          smart_card_ink_sps_trademark_data,                 C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG,       N_INK_SPS_INK_USE_AND_WARRANTY_DATA_BYTES,  N_INK_SPS_INK_USE_AND_WARRANTY_DATA_FIELDS,  smart_card_ink_sps_ink_use_and_warranty_data,      C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG,           N_INK_SPS_OEM_RW_OR_MEMORY_DATA_BYTES,      N_INK_SPS_OEM_RW_OR_MEMORY_DATA_FIELDS,      smart_card_ink_sps_oem_rw_or_memory_data,          C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG,             N_INK_SPS_HP_SCRATCH_PAD_DATA_BYTES,        N_INK_SPS_HP_SCRATCH_PAD_DATA_FIELDS,        smart_card_ink_sps_hp_scratch_pad_data,            C_MODE_RW    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG,            N_INK_SPS_OEM_SCRATCH_PAD_DATA_BYTES,       N_INK_SPS_OEM_SCRATCH_PAD_DATA_FIELDS,       smart_card_ink_sps_oem_scratch_pad_data,           C_MODE_RW    },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_EXT_OEM_ID_TAG,                 N_INK_SPS_EXT_OEM_ID_BYTES,                 N_INK_SPS_EXT_OEM_ID_FIELDS,                 smart_card_ink_sps_ext_oem_id_data,                C_MODE_RW2RO },
    { (HP_SMART_CARD_tag_t) HP_SMART_CARD_INK_REORDER_PART_NUM_TAG,           N_INK_SPS_REORDER_PART_NUM_BYTES,           N_INK_SPS_REORDER_PART_NUM_FIELDS,           smart_card_ink_reorder_part_num_data,              C_MODE_RW2RO },
};

#define NUM_TAGGED_INK_FIELDS    (sizeof(hp_smart_card_ink_tag_fields) / sizeof(HP_SMART_CARD_tag_desc_t))

// END - HP Smart Card map



// >>>>>>>>>>>> HP Smart Card (Supply) map <<<<<<<<<<<<

#ifdef INCLUDE_HP_SMART_CARD_SUPPLY

enum hpscs_tags         // (must begin with 1 and be consecutive values)
{
    HPSCS_DRY_MFG_TAG               = 1,
    HPSCS_DRY_TAG                   = 2,
    HPSCS_INK_MFG_TAG               = 3,
    HPSCS_INK_TAG                   = 4,
    HPSCS_FILLED_CTRDG_SPECIFIC_TAG = 5,
    HPSCS_FIRST_INSTALL_TAG         = 6,
    HPSCS_OEM_RW2RO_TAG             = 7,
    HPSCS_TM_TAG                    = 8,
    HPSCS_USE_TAG                   = 9,
    HPSCS_OEM_RW_OR_TAG             = 10,
    HPSCS_HP_SCRATCH_TAG            = 11,
    HPSCS_OEM_SCRATCH_TAG           = 12,
};


/* HPSCS_DRY_MFG_TAG */
const HP_SMART_CARD_field_desc_t hpscs_dry_mfg_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_EXT_OEM_ID,                  1, 7,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_HP_INK,                      2, 7,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_MFG_SITE,                 2, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_MFG_LINE,                 2, 0,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_MFG_YEAR,                 3, 3,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_WEEK,                     4, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_DOW,                      4, 0,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_HOD,                      5, 5,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_MOH,                      5, 0,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_SOM,                      6, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_SN_DRY_PROCESS_POSITION,     7, 4,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_MAX_USABLE_CARTRIDGE_VOLUME, 7, 1, 16 },
    { (HP_SMART_CARD_field_t) HPSCS_PRINTER_LOCK_DOWN_PARTITION, 9, 1,  5 },
};
#define N_HPSCS_DRY_MFG_BYTES       13
#define N_HPSCS_DRY_MFG_FIELDS      (sizeof(hpscs_dry_mfg_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_DRY_TAG */
const HP_SMART_CARD_field_desc_t hpscs_dry_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_PILS_MFG_YEAR,            1, 7, 5 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_MFG_WEEK,            1, 2, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_MFG_ID,              2, 4, 4 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_MFG_LINE,            2, 0, 4 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_SENSOR_GAIN,         3, 4, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PHASE3_AVAIL,        4, 4, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT1_ABS_VOLUME,      5, 4, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT2_ABS_VOLUME,      6, 4, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT3_ABS_VOLUME,      7, 4, 7 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT1_PRESSURE,        8, 5, 7 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT2_PRESSURE,        9, 6, 7 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_PT3_PRESSURE,       10, 7, 7 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_2_MULTIPLIER,       10, 0, 7 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_2_SUBTRACTOR,       11, 1, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_ORIENTATION,        12, 3, 3 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_TEMP_COEFFICIENT,   12, 0, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_REFILL_EMPTY_LIMIT, 13, 2, 4 },
};
#define N_HPSCS_DRY_BYTES       17
#define N_HPSCS_DRY_FIELDS      (sizeof(hpscs_dry_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_INK_MFG_TAG */
const HP_SMART_CARD_field_desc_t hpscs_ink_mfg_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_UD1_FUSE,               1, 7, 1 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_SITE,              1, 6, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_LINE,              2, 6, 5 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_YEAR,              2, 1, 5 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_WEEK,              3, 4, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_DOW,               4, 6, 3 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_HOD,               4, 3, 5 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_MOH,               5, 6, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_SOM,               5, 0, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_FILL_PROCESS_POSITION,  6, 2, 3 },
    { (HP_SMART_CARD_field_t) HPSCS_REGION_ID,              7, 7, 8 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_1,      8, 7, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_2,      8, 1, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_3,      9, 3, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_4,     10, 5, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_5,     11, 7, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_6,     11, 1, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_7,     12, 3, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_8,     13, 5, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_9,     14, 7, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_10,    14, 1, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_11,    15, 3, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_REORDER_PN_CHAR_12,    16, 5, 6 },
};
#define N_HPSCS_INK_MFG_BYTES       21
#define N_HPSCS_INK_MFG_FIELDS      (sizeof(hpscs_ink_mfg_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_INK_TAG */
const HP_SMART_CARD_field_desc_t hpscs_ink_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_INK_FORMULATOR_ID,                      1, 7,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_FAMILY,                             2, 7,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_COLOR_CODES_GENERAL,                3, 7,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_COLOR_CODES_SPECIFIC,               3, 4,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_FAMILY_MEMBER,                      3, 1,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ID_NUM,                             4, 1, 12 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_REVISION,                           6, 5,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_DENSITY,                            7, 5, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_CARTRIDGE_DISTINCTION,              8, 3,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR,           9, 6,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_SHELF_LIFE_WEEKS,                   9, 3,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_SHELF_LIFE_DAYS,                   10, 3,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_INSTALLED_LIFE_WEEKS,              10, 0,  8 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_INSTALLED_LIFE_DAYS,               11, 0,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_USABLE_INK_WEIGHT,                 12, 5, 16 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL, 14, 5,  7 },
};
#define N_HPSCS_INK_BYTES       17
#define N_HPSCS_INK_FIELDS      (sizeof(hpscs_ink_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_FILLED_CTRDG_SPECIFIC_TAG */
const HP_SMART_CARD_field_desc_t hpscs_filled_ctrdg_specific_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1, 1, 7, 32 },
    { (HP_SMART_CARD_field_t) HPSCS_FILLED_CTRDG_SPECIFIC_DATA_2, 5, 7, 32 },
    { (HP_SMART_CARD_field_t) HPSCS_FILLED_CTRDG_SPECIFIC_DATA_3, 9, 7, 32 },
};
#define N_HPSCS_FILLED_CTRDG_SPECIFIC_BYTES     13
#define N_HPSCS_FILLED_CTRDG_SPECIFIC_FIELDS    (sizeof(hpscs_filled_ctrdg_specific_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_FIRST_INSTALL_TAG */
const HP_SMART_CARD_field_desc_t hpscs_first_install_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_UD2_FUSE,                          1, 7,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_1,          1, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_2,          1, 0,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_3,          2, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_4,          3, 4,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_5,          4, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_6,          4, 0,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_7,          5, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_8,          6, 4,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_9,          7, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_10,         7, 0,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_11,         8, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_ID_CHAR_12,         9, 4,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_MFG_YEAR,          10, 6,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_MFG_WOY,           10, 1,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_MFG_COUNTRY,       11, 3,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION, 12, 5,  4 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION, 12, 1,  4 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT,    13, 5, 12 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_INSTALL_YEAR,               14, 1,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_INSTALL_WOY,                15, 4,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_FIRST_INSTALL_DOW,                16, 6,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_LEVEL_GAUGE_RESOLUTION,       16, 3,  7 },
};
#define N_HPSCS_FIRST_INSTALL_BYTES     21
#define N_HPSCS_FIRST_INSTALL_FIELDS    (sizeof(hpscs_first_install_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_OEM_RW2RO_TAG */
const HP_SMART_CARD_field_desc_t hpscs_oem_rw2ro_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_UD3_FUSE,            1, 7,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_FIELD_1, 1, 6, 31 },
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_FIELD_2, 5, 7, 32 },
};
#define N_HPSCS_OEM_RW2RO_BYTES     9
#define N_HPSCS_OEM_RW2RO_FIELDS    (sizeof(hpscs_oem_rw2ro_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_TM_TAG */
const HP_SMART_CARD_field_desc_t hpscs_tm_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_HP_TRADEMARK_CHAR_1, 1, 7, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_HP_TRADEMARK_CHAR_2, 1, 1, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_HP_TRADEMARK_CHAR_3, 2, 3, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_HP_TRADEMARK_CHAR_4, 3, 5, 6 },
    { (HP_SMART_CARD_field_t) HPSCS_HP_TRADEMARK_CHAR_5, 4, 7, 6 },
};
#define N_HPSCS_TM_BYTES        5
#define N_HPSCS_TM_FIELDS       (sizeof(hpscs_tm_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_USE_TAG */
const HP_SMART_CARD_field_desc_t hpscs_use_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_INK_OUT_OF_INK_BIT,              1, 7,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ILG_BIT_01_25,               1, 6, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ILG_BIT_26_50,               4, 5, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ILG_BIT_51_75,               7, 4, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ILG_BIT_76_100,             10, 3, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_TIUG_BIT_01_25,             13, 2, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_TIUG_BIT_26_50,             16, 1, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_TIUG_BIT_51_75,             19, 0, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_TIUG_BIT_76_100,            23, 7, 25 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_FIRST_FAILURE_CODE,         26, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_ALTERED_SUPPLY,             26, 0,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_USER_ACK_NON_ORG_INK,       27, 7,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_USER_ACK_EXPIRED_INK,       27, 6,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY, 27, 5,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_DROP_COUNT_ONLY_ILS,            27, 4,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_FULL_VOLTAGE_COMPLETE,     27, 3,  1 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_STATE,                     27, 2,  2 },
    { (HP_SMART_CARD_field_t) HPSCS_PURGE_OCCURRED,                 27, 0,  1 },
};
#define N_HPSCS_USE_BYTES       33
#define N_HPSCS_USE_FIELDS      (sizeof(hpscs_use_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_OEM_RW_OR_TAG */
const HP_SMART_CARD_field_desc_t hpscs_oem_rw_or_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_RW_OR_FIELD_1, 1, 7, 32 },
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_RW_OR_FIELD_2, 5, 7, 32 },
};
#define N_HPSCS_OEM_RW_OR_BYTES     9
#define N_HPSCS_OEM_RW_OR_FIELDS    (sizeof(hpscs_oem_rw_or_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_HP_SCRATCH_TAG */
const HP_SMART_CARD_field_desc_t hpscs_hp_scratch_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_CARTRIDGE_MRU_YEAR,         1, 7,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_CARTRIDGE_MRU_WOY,          1, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_CARTRIDGE_MRU_DOW,          2, 4,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_1,     2, 1,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_2,     3, 3,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_3,     4, 5,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_4,     5, 7,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_5,     5, 1,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_6,     6, 3,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_7,     7, 5,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_8,     8, 7,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_9,     8, 1,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_10,    9, 3,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_11,   10, 5,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_ID_CHAR_12,   11, 7,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_MFG_YEAR,     11, 1,  5 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_MFG_WOY,      12, 4,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_MFG_COUNTRY,  13, 6,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_FW_REV_MAJOR, 13, 0,  4 },
    { (HP_SMART_CARD_field_t) HPSCS_MRU_PLATFORM_FW_REV_MINOR, 14, 4,  4 },
    { (HP_SMART_CARD_field_t) HPSCS_INSERTION_COUNT,           14, 0,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_STALL_INSERTION_COUNT,     15, 2, 16 },
    { (HP_SMART_CARD_field_t) HPSCS_LAST_FAILURE_CODE,         17, 2,  6 },
    { (HP_SMART_CARD_field_t) HPSCS_USER_REPORTED_STATUS,      18, 4,  4 },
    { (HP_SMART_CARD_field_t) HPSCS_FINE_ILG_COUNTER,          18, 0, 16 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_0,            20, 0, 12 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_1,            22, 4, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_2,            23, 2, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_3,            24, 0, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_4,            26, 6, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_REGISTER_5,            27, 4, 10 },
    { (HP_SMART_CARD_field_t) HPSCS_PFV_LAST_REGISTER_NUM,     28, 2,  3 },
    { (HP_SMART_CARD_field_t) HPSCS_PILS_VOLTAGE_OFFSET,       29, 7, 12 },
    { (HP_SMART_CARD_field_t) HPSCS_MKT_DATA_REV,              30, 3,  4 },
};
#define N_HPSCS_HP_SCRATCH_BYTES        33
#define N_HPSCS_HP_SCRATCH_FIELDS       (sizeof(hpscs_hp_scratch_data) / sizeof(HP_SMART_CARD_field_desc_t))

/* HPSCS_OEM_SCRATCH_TAG */
const HP_SMART_CARD_field_desc_t hpscs_oem_scratch_data[] =
{
    /* field, start byte, start bit, size */
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_RW_FIELD_1, 1, 7, 32 },
    { (HP_SMART_CARD_field_t) HPSCS_OEM_DEFINED_RW_FIELD_2, 5, 7, 32 },
};
#define N_HPSCS_OEM_SCRATCH_BYTES       9
#define N_HPSCS_OEM_SCRATCH_FIELDS      (sizeof(hpscs_oem_scratch_data) / sizeof(HP_SMART_CARD_field_desc_t))


const HP_SMART_CARD_tag_desc_t hpscs_tag_fields[] =
{
    /* TAG, bytes, number of fields, pointer to field data, access type (change RW2RO to RO as needed) */
    { (HP_SMART_CARD_tag_t) HPSCS_DRY_MFG_TAG,               N_HPSCS_DRY_MFG_BYTES,               N_HPSCS_DRY_MFG_FIELDS,               hpscs_dry_mfg_data,               C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HPSCS_DRY_TAG,                   N_HPSCS_DRY_BYTES,                   N_HPSCS_DRY_FIELDS,                   hpscs_dry_data,                   C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HPSCS_INK_MFG_TAG,               N_HPSCS_INK_MFG_BYTES,               N_HPSCS_INK_MFG_FIELDS,               hpscs_ink_mfg_data,               C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HPSCS_INK_TAG,                   N_HPSCS_INK_BYTES,                   N_HPSCS_INK_FIELDS,                   hpscs_ink_data,                   C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HPSCS_FILLED_CTRDG_SPECIFIC_TAG, N_HPSCS_FILLED_CTRDG_SPECIFIC_BYTES, N_HPSCS_FILLED_CTRDG_SPECIFIC_FIELDS, hpscs_filled_ctrdg_specific_data, C_MODE_RO    },
    { (HP_SMART_CARD_tag_t) HPSCS_FIRST_INSTALL_TAG,         N_HPSCS_FIRST_INSTALL_BYTES,         N_HPSCS_FIRST_INSTALL_FIELDS,         hpscs_first_install_data,         C_MODE_RW2RO },
    { (HP_SMART_CARD_tag_t) HPSCS_OEM_RW2RO_TAG,             N_HPSCS_OEM_RW2RO_BYTES,             N_HPSCS_OEM_RW2RO_FIELDS,             hpscs_oem_rw2ro_data,             C_MODE_RW2RO },
    { (HP_SMART_CARD_tag_t) HPSCS_TM_TAG,                    N_HPSCS_TM_BYTES,                    N_HPSCS_TM_FIELDS,                    hpscs_tm_data,                    C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HPSCS_USE_TAG,                   N_HPSCS_USE_BYTES,                   N_HPSCS_USE_FIELDS,                   hpscs_use_data,                   C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HPSCS_OEM_RW_OR_TAG,             N_HPSCS_OEM_RW_OR_BYTES,             N_HPSCS_OEM_RW_OR_FIELDS,             hpscs_oem_rw_or_data,             C_MODE_RW_OR },
    { (HP_SMART_CARD_tag_t) HPSCS_HP_SCRATCH_TAG,            N_HPSCS_HP_SCRATCH_BYTES,            N_HPSCS_HP_SCRATCH_FIELDS,            hpscs_hp_scratch_data,            C_MODE_RW    },
    { (HP_SMART_CARD_tag_t) HPSCS_OEM_SCRATCH_TAG,           N_HPSCS_OEM_SCRATCH_BYTES,           N_HPSCS_OEM_SCRATCH_FIELDS,           hpscs_oem_scratch_data,           C_MODE_RW    },
};
#define NUM_HPCSS_TAG_FIELDS    (sizeof(hpscs_tag_fields) / sizeof(HP_SMART_CARD_tag_desc_t))
#endif  /* INCLUDE_HP_SMART_CARD_SUPPLY */

// END - HP Smart Card map



// >>>>>>>>>>>> Common <<<<<<<<<<<<

#define HP_SMART_CARD_INK_CHIP_ID    2

#ifdef INCLUDE_HP_SMART_CARD_LAYOUT_DATA
const HP_SMART_CARD_device_info_t hp_smart_card_device[] =
{
    { HP_SMART_CARD_INK_CHIP_ID, HP_SMART_CARD_INK_FAMILY_ID, NUM_TAGGED_INK_FIELDS, HP_SMART_CARD_INK_LAST_FIELD, hp_smart_card_ink_tag_fields },
#ifdef INCLUDE_HP_SMART_CARD_SUPPLY
    { HP_SMART_CARD_INK_CHIP_ID, HPSCS_FAMILY_ID,             NUM_HPCSS_TAG_FIELDS,  HPSCS_LAST_FIELD,             hpscs_tag_fields             },
#endif  /* INCLUDE_HP_SMART_CARD_SUPPLY */
};
#define NUM_HP_SMART_CARD_DEVICES    (sizeof(hp_smart_card_device) / sizeof(HP_SMART_CARD_device_info_t))
#endif  /* INCLUDE_HP_SMART_CARD_LAYOUT_DATA */
#endif  /* _HP_SMART_CARD_LAYOUT_H_ */

