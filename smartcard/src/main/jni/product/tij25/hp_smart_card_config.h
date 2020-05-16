/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2016 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card_config.h
//-----------------------------------------------------------------------------
// Description: Interface header for HP smart card library. This header file
//              contains product specific smart card library configuration.
//
//*****************************************************************************
#ifndef _HP_SMART_CARD_CONFIG_H_
#define _HP_SMART_CARD_CONFIG_H_

# define INCLUDE_HP_SMART_CARD_SUPPLY

// Configure device ids in this enumeration
// NOTE: only 32 Component devices are supported.
typedef enum
{
    HP_SMART_CARD_DEVICE_ID_0,                              // This is the device id of the first Component Smart Card
// H.M.Wang 2019-10-17 Added the following 3 devices
#ifdef INCLUDE_HP_SMART_CARD_SUPPLY
    HP_SMART_CARD_DEVICE_ID_1,                              // If supply is supported, a second Component device is assumed
#endif
    //HP_SMART_CARD_DEVICE_ID_2,				            // Example - add enum entries for up to 32 component devices as needed

    /* Add smart card device ids above this line. */
    HP_SMART_CARD_DEVICE_NUM_DEVICES,                       // There is no valid device with this id
    HP_SMART_CARD_DEVICE_HOST,                              // This is the device id of the Host Smart Card
} HP_SMART_CARD_device_id_t;

#define HP_SMART_CARD_FIRST_NON_HEADER_FIELD_ID    11 // used to start field identifiers



/// >>>>>>>>>>>> HP Smart Card <<<<<<<<<<<<

#define HP_SMART_CARD_INK_FAMILY_ID    27

/* Add product specific smart chip fields here */
enum HP_SMART_CARD_product_fields
{
    /* HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG */                  // TAG1 的 项目索引，但是缺少文档中的最后一行
    HP_SMART_CARD_INK_UD0_FUSE                          = 11,       // HP_SMART_CARD_FIRST_NON_HEADER_FIELD_ID
    HP_SMART_CARD_INK_EXTENDED_OEM_ID                   = 12,       // H.M.Wang seems not to be an item of Tag 1
    HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR             = 13,       // H.M.Wang seems not to be an item of Tag 1
    HP_SMART_CARD_INK_MANUFACTURE_ID                    = 14,
    HP_SMART_CARD_INK_MANUFACTURE_LINE                  = 15,
    HP_SMART_CARD_INK_MANUFACTURE_YEAR                  = 16,
    HP_SMART_CARD_INK_MANUFACTURE_WEEK                  = 17,
    HP_SMART_CARD_INK_SERIAL_NUM_DOW                    = 18,
    HP_SMART_CARD_INK_SERIAL_NUM_HOD                    = 19,
    HP_SMART_CARD_INK_SERIAL_NUM_MOH                    = 20,
    HP_SMART_CARD_INK_SERIAL_NUM_SOM                    = 21,
    HP_SMART_CARD_INK_DRY_PROCESS_POSITION              = 22,
    HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME       = 23,
    HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION       = 24,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，一行
//  HP_BLANK

    /* HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG  */            // TAG2 的 项目索引，但是缺少文档当中的最后四行
    HP_SMART_CARD_INK_TSR                               = 25,
    HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT           = 26,
    HP_SMART_CARD_INK_BULK_CTRDG                        = 27,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，四行
//  HP_BLANK
//  HP_BLANK
//  HP_BLANK
//  HP_BLANK

    /* HP_SMART_CARD_INK_INK_FILL_MFG_DATA_TAG */                   // TAG3 的 项目索引，似乎从HP_SMART_CARD_INK_REGIONALIZATION_ID至HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_12对应于四行的blank
    HP_SMART_CARD_INK_UD1_FUSE                          = 28,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID            = 29,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE               = 30,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR               = 31,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY                = 32,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW                = 33,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD                = 34,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH                = 35,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM                = 36,
    HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION   = 37,
    HP_SMART_CARD_INK_REGIONALIZATION_ID                = 38,       // 从这项开始，对应的是四行blank
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1           = 39,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_2           = 40,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_3           = 41,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_4           = 42,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_5           = 43,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_6           = 44,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_7           = 45,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_8           = 46,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_9           = 47,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_10          = 48,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_11          = 49,
    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_12          = 50,


    /* HP_SMART_CARD_INK_INK_DATA_AND_PROPERTIES_TAG  */            // TAG4 的 项目的索引，但是缺少最后一行blank
    HP_SMART_CARD_INK_FORMULATOR_ID                     = 51,
    HP_SMART_CARD_INK_INK_FAMILY                        = 52,
    HP_SMART_CARD_INK_COLOR_CODES_GENERAL               = 53,
    HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC              = 54,
    HP_SMART_CARD_INK_INK_FAMILY_MEMBER                 = 55,
    HP_SMART_CARD_INK_INK_ID_NUM                        = 56,
    HP_SMART_CARD_INK_INK_REVISION                      = 57,
    HP_SMART_CARD_INK_DENSITY                           = 58,
    HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION             = 59,
    HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR          = 60,
    HP_SMART_CARD_INK_SHELF_LIFE_WEEKS                  = 61,
    HP_SMART_CARD_INK_SHELF_LIFE_DAYS                   = 62,
    HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS              = 63,
    HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS               = 64,
    HP_SMART_CARD_INK_USABLE_INK_WEIGHT                 = 65,
    HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL = 66,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，一行
//  HP_BLANK

    /* HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG  */         // TAG5 的 项目的索引，但是缺少最后一行blank
    HP_SMART_CARD_INK_FIRING_FREQ                       = 67,
    HP_SMART_CARD_INK_PULSE_WIDTH                       = 68,
    HP_SMART_CARD_INK_FIRING_VOLTAGE                    = 69,
    HP_SMART_CARD_INK_TURN_ON_ENERGY                    = 70,
    HP_SMART_CARD_INK_PULSE_WARMING_TEMP                = 71,
    HP_SMART_CARD_INK_MAX_TEMP                          = 72,
    HP_SMART_CARD_INK_DROP_VOLUME                       = 73,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，两行
//  HP_BLANK
//  HP_BLANK

    /* HP_SMART_CARD_INK_CTRDG_FIRST_INSTALLED_DATA_TAG  */         // TAG6 的 项目的索引，但是第二项分解为12个项目，缺少最后一行blank
    HP_SMART_CARD_INK_WRITE_PROTECT                     = 74,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1          = 75,       // 此项目开始的12个项目对应于文档的一个项目
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_2          = 76,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_3          = 77,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_4          = 78,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_5          = 79,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_6          = 80,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_7          = 81,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_8          = 82,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_9          = 83,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_10         = 84,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_11         = 85,
    HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_12         = 86,
    HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR           = 87,
    HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY            = 88,
    HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY        = 89,
    HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION  = 90,
    HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION  = 91,
    HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT     = 92,
    HP_SMART_CARD_INK_FIRST_INSTALL_YEAR                = 93,
    HP_SMART_CARD_INK_FIRST_INSTALL_WOY                 = 94,
    HP_SMART_CARD_INK_FIRST_INSTALL_DOW                 = 95,
    HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION        = 96,
    ////////////////////////////////////////////////////////
    /*2016-05 New OEM field*/
    HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD           = 97,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，一行
//  HP_BLANK

    /* HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG  */
    HP_SMART_CARD_INK_UD3_FUSE                          = 98,       // TAG7 的 项目的索引
    HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1               = 99,
    HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2               = 100,

    /* HP_SMART_CARD_INK_HP_TRADEMARK_TAG */                        // TAG8 的 项目的索引。第一项包含5个字符，缺少最后一个blank
    HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1               = 101,
    HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_2               = 102,
    HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_3               = 103,
    HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_4               = 104,
    HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_5               = 105,
    HP_SMART_CARD_INK_UD4_FUSE                          = 106,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，一行
//  HP_BLANK

    /* HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG */                // TAG9 的 项目的索引。缺少最后两行blank
    HP_SMART_CARD_INK_OUT_OF_INK_BIT                    = 107,
    HP_SMART_CARD_INK_ILG_BIT_01_25                     = 108,
    HP_SMART_CARD_INK_ILG_BIT_26_50                     = 109,
    HP_SMART_CARD_INK_ILG_BIT_51_75                     = 110,
    HP_SMART_CARD_INK_ILG_BIT_76_100                    = 111,
    HP_SMART_CARD_INK_TUG_BIT_01_25                     = 112,
    HP_SMART_CARD_INK_TUG_BIT_26_50                     = 113,
    HP_SMART_CARD_INK_TUG_BIT_51_75                     = 114,
    HP_SMART_CARD_INK_TUG_BIT_76_100                    = 115,
    HP_SMART_CARD_INK_FIRST_FAILURE_CODE                = 116,
    HP_SMART_CARD_INK_ALTERED_SUPPLY                    = 117,
    HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK              = 118,
    HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK              = 119,
    HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY        = 120,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是blank，两行
//  HP_BLANK
//  HP_BLANK

    /* HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG */                    // TAG10 的 项目的索引
    HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1                 = 121,
    HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2                 = 122,

    /* HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG */                      // TAG11 的 项目的索引。INK_MRU_PLATFORM_ID占12项，后面缺少5个Reserved项目
    HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR                = 123,
    HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY                 = 124,
    HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW                 = 125,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1            = 126,      // 此项目开始12项（字符）对应于文档一个项目
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_2            = 127,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_3            = 128,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_4            = 129,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_5            = 130,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_6            = 131,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_7            = 132,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_8            = 133,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_9            = 134,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_10           = 135,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_11           = 136,
    HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_12           = 137,
    HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR             = 138,
    HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY              = 139,
    HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY          = 140,
    HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR         = 141,
    HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR         = 142,
    HP_SMART_CARD_INK_INSERTION_COUNT                   = 143,
    HP_SMART_CARD_INK_STALL_INSERTION_COUNT             = 144,
    HP_SMART_CARD_INK_LAST_FAILURE_CODE                 = 145,
    HP_SMART_CARD_INK_USER_REPORTED_STATUS              = 146,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是Reserved，两行
//  HP_RESERVED
//  HP_RESERVED
    HP_SMART_CARD_INK_MARKETING_DATA_REVISION           = 147,
// H.M.Wang 2019-9-29 追加占位定义，文档中这里是Reserved，三行
//  HP_RESERVED
//  HP_RESERVED
//  HP_RESERVED

    /* HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG */                     // TAG12 的 项目的索引
    HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1                = 148,
    HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2                = 149,

    ////////////////////////////////////////////////////////
    /*Newly added entries for Memory Map Revision 2.0.1*/           // TAG13 的 项目的索引。但项目定义不太一致，好像与TAG14有重叠
    HP_SMART_CARD_INK_UD7_FUSE                          = 150,
    HP_SMART_CARD_INK_UD8_FUSE                          = 151,
    HP_SMART_CARD_INK_BLANK_1                           = 152,
    HP_SMART_CARD_INK_BLANK_2                           = 153,
                                                                    // 缺少TAG14的索引
    /* this should be the last entry in this enum */
    HP_SMART_CARD_INK_LAST_FIELD                        = 154,
};



// >>>>>>>>>>>> HP Smart Card (Supply) <<<<<<<<<<<<

#define HPSCS_FAMILY_ID    28

enum hpscs_product_fields
{
    /* HPSCS_DRY_MFG_TAG */
    HPSCS_EXT_OEM_ID                            = 11,       // HP_SMART_CARD_FIRST_NON_HEADER_FIELD_ID
    HPSCS_HP_INK                                = 12,
    HPSCS_SN_MFG_SITE                           = 13,
    HPSCS_SN_MFG_LINE                           = 14,
    HPSCS_SN_MFG_YEAR                           = 15,
    HPSCS_SN_WEEK                               = 16,
    HPSCS_SN_DOW                                = 17,
    HPSCS_SN_HOD                                = 18,
    HPSCS_SN_MOH                                = 19,
    HPSCS_SN_SOM                                = 20,
    HPSCS_SN_DRY_PROCESS_POSITION               = 21,
    HPSCS_MAX_USABLE_CARTRIDGE_VOLUME           = 22,
    HPSCS_PRINTER_LOCK_DOWN_PARTITION           = 23,
    /* HPSCS_DRY_TAG */
    HPSCS_PILS_MFG_YEAR                         = 24,
    HPSCS_PILS_MFG_WEEK                         = 25,
    HPSCS_PILS_MFG_ID                           = 26,
    HPSCS_PILS_MFG_LINE                         = 27,
    HPSCS_PILS_SENSOR_GAIN                      = 28,
    HPSCS_PILS_PHASE3_AVAIL                     = 29,
    HPSCS_PILS_PT1_ABS_VOLUME                   = 30,
    HPSCS_PILS_PT2_ABS_VOLUME                   = 31,
    HPSCS_PILS_PT3_ABS_VOLUME                   = 32,
    HPSCS_PILS_PT1_PRESSURE                     = 33,
    HPSCS_PILS_PT2_PRESSURE                     = 34,
    HPSCS_PILS_PT3_PRESSURE                     = 35,
    HPSCS_PILS_2_MULTIPLIER                     = 36,
    HPSCS_PILS_2_SUBTRACTOR                     = 37,
    HPSCS_PILS_ORIENTATION                      = 38,
    HPSCS_PILS_TEMP_COEFFICIENT                 = 39,
    HPSCS_PILS_REFILL_EMPTY_LIMIT               = 40,
    /* HPSCS_INK_MFG_TAG */
    HPSCS_UD1_FUSE                              = 41,
    HPSCS_FILL_SITE                             = 42,
    HPSCS_FILL_LINE                             = 43,
    HPSCS_FILL_YEAR                             = 44,
    HPSCS_FILL_WEEK                             = 45,
    HPSCS_FILL_DOW                              = 46,
    HPSCS_FILL_HOD                              = 47,
    HPSCS_FILL_MOH                              = 48,
    HPSCS_FILL_SOM                              = 49,
    HPSCS_FILL_PROCESS_POSITION                 = 50,
    HPSCS_REGION_ID                             = 51,
    HPSCS_REORDER_PN_CHAR_1                     = 52,
    HPSCS_REORDER_PN_CHAR_2                     = 53,
    HPSCS_REORDER_PN_CHAR_3                     = 54,
    HPSCS_REORDER_PN_CHAR_4                     = 55,
    HPSCS_REORDER_PN_CHAR_5                     = 56,
    HPSCS_REORDER_PN_CHAR_6                     = 57,
    HPSCS_REORDER_PN_CHAR_7                     = 58,
    HPSCS_REORDER_PN_CHAR_8                     = 59,
    HPSCS_REORDER_PN_CHAR_9                     = 60,
    HPSCS_REORDER_PN_CHAR_10                    = 61,
    HPSCS_REORDER_PN_CHAR_11                    = 62,
    HPSCS_REORDER_PN_CHAR_12                    = 63,
    /* HPSCS_INK_TAG */
    HPSCS_INK_FORMULATOR_ID                     = 64,
    HPSCS_INK_FAMILY                            = 65,
    HPSCS_INK_COLOR_CODES_GENERAL               = 66,
    HPSCS_INK_COLOR_CODES_SPECIFIC              = 67,
    HPSCS_INK_FAMILY_MEMBER                     = 68,
    HPSCS_INK_ID_NUM                            = 69,
    HPSCS_INK_REVISION                          = 70,
    HPSCS_INK_DENSITY                           = 71,
    HPSCS_INK_CARTRIDGE_DISTINCTION             = 72,
    HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR          = 73,
    HPSCS_INK_SHELF_LIFE_WEEKS                  = 74,
    HPSCS_INK_SHELF_LIFE_DAYS                   = 75,
    HPSCS_INK_INSTALLED_LIFE_WEEKS              = 76,
    HPSCS_INK_INSTALLED_LIFE_DAYS               = 77,
    HPSCS_INK_USABLE_INK_WEIGHT                 = 78,
    HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL = 79,
    /* HPSCS_FILLED_CTRDG_SPECIFIC_TAG */
    HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1          = 80,
    HPSCS_FILLED_CTRDG_SPECIFIC_DATA_2          = 81,
    HPSCS_FILLED_CTRDG_SPECIFIC_DATA_3          = 82,
    /* HPSCS_FIRST_INSTALL_TAG */
    HPSCS_UD2_FUSE                              = 83,
    HPSCS_FIRST_PLATFORM_ID_CHAR_1              = 84,
    HPSCS_FIRST_PLATFORM_ID_CHAR_2              = 85,
    HPSCS_FIRST_PLATFORM_ID_CHAR_3              = 86,
    HPSCS_FIRST_PLATFORM_ID_CHAR_4              = 87,
    HPSCS_FIRST_PLATFORM_ID_CHAR_5              = 88,
    HPSCS_FIRST_PLATFORM_ID_CHAR_6              = 89,
    HPSCS_FIRST_PLATFORM_ID_CHAR_7              = 90,
    HPSCS_FIRST_PLATFORM_ID_CHAR_8              = 91,
    HPSCS_FIRST_PLATFORM_ID_CHAR_9              = 92,
    HPSCS_FIRST_PLATFORM_ID_CHAR_10             = 93,
    HPSCS_FIRST_PLATFORM_ID_CHAR_11             = 94,
    HPSCS_FIRST_PLATFORM_ID_CHAR_12             = 95,
    HPSCS_FIRST_PLATFORM_MFG_YEAR               = 96,
    HPSCS_FIRST_PLATFORM_MFG_WOY                = 97,
    HPSCS_FIRST_PLATFORM_MFG_COUNTRY            = 98,
    HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION      = 99,
    HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION      = 100,
    HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT         = 101,
    HPSCS_FIRST_INSTALL_YEAR                    = 102,
    HPSCS_FIRST_INSTALL_WOY                     = 103,
    HPSCS_FIRST_INSTALL_DOW                     = 104,
    HPSCS_INK_LEVEL_GAUGE_RESOLUTION            = 105,
    /* HPSCS_OEM_RW2RO_TAG */
    HPSCS_UD3_FUSE                              = 106,
    HPSCS_OEM_DEFINED_FIELD_1                   = 107,
    HPSCS_OEM_DEFINED_FIELD_2                   = 108,
    /* HPSCS_TM_TAG */
    HPSCS_HP_TRADEMARK_CHAR_1                   = 109,
    HPSCS_HP_TRADEMARK_CHAR_2                   = 110,
    HPSCS_HP_TRADEMARK_CHAR_3                   = 111,
    HPSCS_HP_TRADEMARK_CHAR_4                   = 112,
    HPSCS_HP_TRADEMARK_CHAR_5                   = 113,
    /* HPSCS_USE_TAG */
    HPSCS_INK_OUT_OF_INK_BIT                    = 114,
    HPSCS_INK_ILG_BIT_01_25                     = 115,
    HPSCS_INK_ILG_BIT_26_50                     = 116,
    HPSCS_INK_ILG_BIT_51_75                     = 117,
    HPSCS_INK_ILG_BIT_76_100                    = 118,
    HPSCS_INK_TIUG_BIT_01_25                    = 119,
    HPSCS_INK_TIUG_BIT_26_50                    = 120,
    HPSCS_INK_TIUG_BIT_51_75                    = 121,
    HPSCS_INK_TIUG_BIT_76_100                   = 122,
    HPSCS_INK_FIRST_FAILURE_CODE                = 123,
    HPSCS_INK_ALTERED_SUPPLY                    = 124,
    HPSCS_INK_USER_ACK_NON_ORG_INK              = 125,
    HPSCS_INK_USER_ACK_EXPIRED_INK              = 126,
    HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY        = 127,
    HPSCS_DROP_COUNT_ONLY_ILS                   = 128,
    HPSCS_PILS_FULL_VOLTAGE_COMPLETE            = 129,
    HPSCS_PILS_STATE                            = 130,
    HPSCS_PURGE_OCCURRED                        = 131,
    /* HPSCS_OEM_RW_OR_TAG */
    HPSCS_OEM_DEFINED_RW_OR_FIELD_1             = 132,
    HPSCS_OEM_DEFINED_RW_OR_FIELD_2             = 133,
    /* HPSCS_HP_SCRATCH_TAG */
    HPSCS_CARTRIDGE_MRU_YEAR                    = 134,
    HPSCS_CARTRIDGE_MRU_WOY                     = 135,
    HPSCS_CARTRIDGE_MRU_DOW                     = 136,
    HPSCS_MRU_PLATFORM_ID_CHAR_1                = 137,
    HPSCS_MRU_PLATFORM_ID_CHAR_2                = 138,
    HPSCS_MRU_PLATFORM_ID_CHAR_3                = 139,
    HPSCS_MRU_PLATFORM_ID_CHAR_4                = 140,
    HPSCS_MRU_PLATFORM_ID_CHAR_5                = 141,
    HPSCS_MRU_PLATFORM_ID_CHAR_6                = 142,
    HPSCS_MRU_PLATFORM_ID_CHAR_7                = 143,
    HPSCS_MRU_PLATFORM_ID_CHAR_8                = 144,
    HPSCS_MRU_PLATFORM_ID_CHAR_9                = 145,
    HPSCS_MRU_PLATFORM_ID_CHAR_10               = 146,
    HPSCS_MRU_PLATFORM_ID_CHAR_11               = 147,
    HPSCS_MRU_PLATFORM_ID_CHAR_12               = 148,
    HPSCS_MRU_PLATFORM_MFG_YEAR                 = 149,
    HPSCS_MRU_PLATFORM_MFG_WOY                  = 150,
    HPSCS_MRU_PLATFORM_MFG_COUNTRY              = 151,
    HPSCS_MRU_PLATFORM_FW_REV_MAJOR             = 152,
    HPSCS_MRU_PLATFORM_FW_REV_MINOR             = 153,
    HPSCS_INSERTION_COUNT                       = 154,
    HPSCS_STALL_INSERTION_COUNT                 = 155,
    HPSCS_LAST_FAILURE_CODE                     = 156,
    HPSCS_USER_REPORTED_STATUS                  = 157,
    HPSCS_FINE_ILG_COUNTER                      = 158,
    HPSCS_PFV_REGISTER_0                        = 159,
    HPSCS_PFV_REGISTER_1                        = 160,
    HPSCS_PFV_REGISTER_2                        = 161,
    HPSCS_PFV_REGISTER_3                        = 162,
    HPSCS_PFV_REGISTER_4                        = 163,
    HPSCS_PFV_REGISTER_5                        = 164,
    HPSCS_PFV_LAST_REGISTER_NUM                 = 165,
    HPSCS_PILS_VOLTAGE_OFFSET                   = 166,
    HPSCS_MKT_DATA_REV                          = 167,
    /* HPSCS_OEM_SCRATCH_TAG */
    HPSCS_OEM_DEFINED_RW_FIELD_1                = 168,
    HPSCS_OEM_DEFINED_RW_FIELD_2                = 169,
    /* this should be the last entry in this enum */
    HPSCS_LAST_FIELD                            = 170,
};



// >>>>>>>>>>>> Common <<<<<<<<<<<<

// HP_SMART_CARD_NUM_FIELD_TYPES must be set based on the LARGER field map!
#ifdef INCLUDE_HP_SMART_CARD_SUPPLY
#define HP_SMART_CARD_NUM_FIELD_TYPES       (HPSCS_LAST_FIELD + 1)
#else
#define HP_SMART_CARD_NUM_FIELD_TYPES       (HP_SMART_CARD_INK_LAST_FIELD + 1)
#endif  /* INCLUDE_HP_SMART_CARD_SUPPLY */

#define MAX_SMART_CARD_SIZE                 256
#define PARITY_BLOCK_SIZE                   2
#endif /* _HP_SMART_CARD_CONFIG_H_ */
