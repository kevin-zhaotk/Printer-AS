//
// Created by hmwan on 2020/4/2.
//

#ifndef PRINTER_AS_GIT_SC_MEMORY_ACCESS_H
#define PRINTER_AS_GIT_SC_MEMORY_ACCESS_H

#include <stdint.h>
#include "hp_smart_card.h"

/////////////////////////////////////////////////////////////////////////////////////////
//                       HP Smart Card Ink Facilities
/////////////////////////////////////////////////////////////////////////////////////////

/*
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
*/

////////////////////////////////// TAG 1 ////////////////////////////////////////////////
// Dry Cartridge Manufacturing Data, 12 bytes, Managed by HP

// Read HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 6 - UD2 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
// Read HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)

// #define HP_SMART_CARD_INK_UD0_FUSE_UNLOCKED      0
// #define HP_SMART_CARD_INK_UD0_FUSE_LOCKED        1
// #define HP_SMART_CARD_INK_UD1_FUSE_UNLOCKED      0
// #define HP_SMART_CARD_INK_UD1_FUSE_LOCKED        1
// #define HP_SMART_CARD_INK_UD3_FUSE_UNLOCKED       0
// #define HP_SMART_CARD_INK_UD3_FUSE_LOCKED         1
// #define HP_SMART_CARD_INK_UD4_FUSE_UNLOCKED       0
// #define HP_SMART_CARD_INK_UD4_FUSE_LOCKED         1
// #define HP_SMART_CARD_INK_UD7_FUSE_UNLOCKED       0
// #define HP_SMART_CARD_INK_UD7_FUSE_LOCKED         1
// #define HP_SMART_CARD_INK_UD8_FUSE_UNLOCKED       0
// #define HP_SMART_CARD_INK_UD8_FUSE_LOCKED         1

HP_SMART_CARD_result_t inkReadTag1UD0Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud0_fuse);

// Skipped HP_SMART_CARD_INK_BLANK_1 (Tag 1 - Blank - 1, 6, 8)
// Skipped HP_SMART_CARD_INK_BLANK_2 (Tag 1 - Blank - 2, 6, 1)
// HP_SMART_CARD_INK_EXTENDED_OEM_ID seems not an item of Tag1, might be (Tag 13 - Extended OEM ID - 1, 6, 8)
// HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR seems not an item of Tag1, might be (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)

// Read HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureID(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_id);

// Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
HP_SMART_CARD_result_t inkReadTag1ManufactureLine(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_line);

// Read HP_SMART_CARD_INK_MANUFACTURE_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 2, 5)
HP_SMART_CARD_result_t inkReadTag1ManufactureYear(HP_SMART_CARD_device_id_t cardId, uint16_t *manufacture_year);

// Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_woy);

// Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
HP_SMART_CARD_result_t inkReadTag1ManufactureDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_dow);

// Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
HP_SMART_CARD_result_t inkReadTag1ManufactureHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_hod);

// Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_moh);

// Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_som);

// Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
HP_SMART_CARD_result_t inkReadTag1DryProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *process_position);

// Read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 0, 16)
HP_SMART_CARD_result_t inkReadTag1MaxUsableCartridgeVolume(HP_SMART_CARD_device_id_t cardId, float *max_usable_cartridge_volume);

// Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Partition number of the printer RW2RO memory space. Used for capturing 1st install data - 9, 0, 5)
HP_SMART_CARD_result_t inkReadTag1PrinterLockdownPartition(HP_SMART_CARD_device_id_t cardId, uint8_t *lock_down_partition );

// Skipped (Tag 1 - Blank - 10, 3, 20)

////////////////////////////////// TAG 2 ////////////////////////////////////////////////
// Dry Cartridge Specific Data, 16 bytes, Managed by HP

// Read HP_SMART_CARD_INK_TSR (Tag 2 - Thermal Sense Resistor (TSR) - 1, 7, 7)
HP_SMART_CARD_result_t inkReadTag2TSR(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_tsr);

// Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
HP_SMART_CARD_result_t inkReadTag2TSRThermalCoefficient(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_tcr);

// HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
#define HP_SMART_CARD_INK_SINGLE     0
#define HP_SMART_CARD_INK_BULK       1

// Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
HP_SMART_CARD_result_t inkReadTag2BulkCartridge(HP_SMART_CARD_device_id_t cardId, uint8_t *bulk_ctrdg);

// Skipped (Tag 2 - Blank - 3, 7, 16)
// Skipped (Tag 2 - Blank - 5, 7, 32)
// Skipped (Tag 2 - Blank - 9, 7, 32)
// Skipped (Tag 2 - Blank - 13, 7, 32)

////////////////////////////////// TAG 3 ////////////////////////////////////////////////
// Ink Fill Manufacturing Data, 20 bytes, Managed by Ink Filter

HP_SMART_CARD_result_t inkReadTag3UD1Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud1_fuse);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillSiteID(HP_SMART_CARD_device_id_t cardId, uint8_t *site_id);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillLine(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_line);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillYear(HP_SMART_CARD_device_id_t cardId, uint16_t *fill_year);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_woy);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_dow);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_hod);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_moh);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_som);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_process_position);

// Skipped (Tag 3 - Blank - 7, 7, 8)
// Skipped (Tag 3 - Blank - 8, 7, 72)
// Skipped (Tag 3 - Blank - 17, 7, 20)
// Skipped (Tag 3 - Blank - 19, 3, 12)
//    HP_SMART_CARD_INK_REGIONALIZATION_ID                = 38,       // seems not an item of Tag3, might be (Tag 14)
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1           = 39,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_2           = 40,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_3           = 41,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_4           = 42,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_5           = 43,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_6           = 44,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_7           = 45,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_8           = 46,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_9           = 47,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_10          = 48,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_11          = 49,
//    HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_12          = 50,

////////////////////////////////// TAG 4 ////////////////////////////////////////////////
// Ink Data & Properties, 16 bytes, Managed by Ink Filter

// Read HP_SMART_CARD_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
HP_SMART_CARD_result_t inkReadTag4FormulatorID(HP_SMART_CARD_device_id_t cardId, uint8_t *formulator_id);

// Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
HP_SMART_CARD_result_t inkReadTag4InkFamily(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family);

// Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
HP_SMART_CARD_result_t inkReadTag4ColorCodesGeneral(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_general);

// Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
HP_SMART_CARD_result_t inkReadTag4ColorCodesSpecific(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_specific);

// Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
HP_SMART_CARD_result_t inkReadTag4InkFamilyMember(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family_member);

// Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
HP_SMART_CARD_result_t inkReadTag4InkIDNumber(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_id_num);

// Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
HP_SMART_CARD_result_t inkReadTag4InkRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_revision);

// Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
// Specifies the ink density.
//      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag4Density(HP_SMART_CARD_device_id_t cardId, float *ink_density);

// Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
HP_SMART_CARD_result_t inkReadTag4CartridgeDistinction(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_cartridge_distinction);

// Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
HP_SMART_CARD_result_t inkReadTag4SupplyKeyingDescriptor(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_supply_keying_descriptor);

// Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
// ??????????? 不明白
//    Shelf Weeks + Shelf Days + Ink filled time / date <= current date / time
HP_SMART_CARD_result_t inkReadTag4ShelfLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_weeks);

// Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
HP_SMART_CARD_result_t inkReadTag4ShelfLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_days);

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
// ??????????? 不明白
//    Installed Weeks + Installed Days + First installed time / date <= current date / time
HP_SMART_CARD_result_t inkReadTag4InstalledLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_weeks);

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
HP_SMART_CARD_result_t inkReadTag4InstalledLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_days);

// Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
// m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag4UsableInkWeight(HP_SMART_CARD_device_id_t cardId, float *ink_usable_ink_weigh);

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
// Percentage Value: 10 means 10%
HP_SMART_CARD_result_t inkReadTag4AlteredSupplyNotificationLevel(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_notification_level);

// Skipped (Tag 4 - Blank - 15, 4, 15)

////////////////////////////////// TAG 5 ////////////////////////////////////////////////
// Filled Cartridge Specific Data, 12 bytes, Managed by Ink Filter

// Read HP_SMART_CARD_INK_FIRING_FREQ (Tag 5 - Firing Frequency - 1, 7, 7)
HP_SMART_CARD_result_t inkReadTag5FiringFreq(HP_SMART_CARD_device_id_t cardId, float *firing_freq);

// Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
HP_SMART_CARD_result_t inkReadTag5PulseWidth(HP_SMART_CARD_device_id_t cardId, float *pulse_width);

// Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
HP_SMART_CARD_result_t inkReadTag5FiringVoltage(HP_SMART_CARD_device_id_t cardId, float *firing_voltage);

// Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
HP_SMART_CARD_result_t inkReadTag5TurnOnEnergy(HP_SMART_CARD_device_id_t cardId, float *turn_on_energy);

// Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
HP_SMART_CARD_result_t inkReadTag5PulseWarmingTemperature(HP_SMART_CARD_device_id_t cardId, uint8_t *pulse_warming_temp);

// Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
HP_SMART_CARD_result_t inkReadTag5MaxTemperature(HP_SMART_CARD_device_id_t cardId, uint8_t *max_temp);

// Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
HP_SMART_CARD_result_t inkReadTag5DropVolume(HP_SMART_CARD_device_id_t cardId, uint8_t *drop_volume);

// Skipped (Tag 5 - Blank - 7, 5, 14)
// Skipped (Tag 5 - Blank - 9, 7, 32)

////////////////////////////////// TAG 6 ////////////////////////////////////////////////
// Cartridge First Install Data, 20 bytes, Managed by Platform

// Read HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse (#2) - 1, 7, 1)
HP_SMART_CARD_result_t inkReadTag6WriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t *write_protect);
// Write HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse (#2) - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag6WriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t write_protect);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t inkReadTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t inkWriteTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mfg_year);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mfg_year);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_woy);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_woy);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_country);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_country);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t inkReadTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_major);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t inkWriteTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_major);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t inkReadTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_minor);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t inkWriteTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_minor);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t inkReadTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_count);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t inkWriteTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_count);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t inkReadTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_year);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t inkWriteTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_year);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t inkReadTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_woy);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t inkWriteTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_woy);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t inkReadTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_dow);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t inkWriteTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_dow);

// Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t inkReadTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t *gauge_resolution);
// Write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t inkWriteTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t gauge_resolution);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t inkReadTag61stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_field);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t inkWriteTag61stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t oem_field);

// Skipped (Tag 6 - Blank - 20, 7, 8)

////////////////////////////////// TAG 7 ////////////////////////////////////////////////
// OEM Lockable memory, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t inkReadTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud3_fuse);
// Write HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud3_fuse);

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t inkReadTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_1);
// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t inkWriteTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_1);

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_2);
// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_2);

////////////////////////////////// TAG 8 ////////////////////////////////////////////////
// Trademark, 4 bytes(defined as 8 bytes in hp_smart_card_layout.h), Managed by HP or Ink Filler

// Read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t inkReadTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
// Write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t inkWriteTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
HP_SMART_CARD_result_t inkReadTag8UD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud4_fuse);
// Write HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
HP_SMART_CARD_result_t inkWriteTag8UD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud4_fuse);

// Skipped (Tag 8 - Blank - 4, 0, 1)

////////////////////////////////// TAG 9 ////////////////////////////////////////////////
// Ink Use & Warranty, 32 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t *out_of_ink);
// Write HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t out_of_ink);

// Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
// Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_25_01);
// Write HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_25_01);

// Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
// Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_50_26);
// Write HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_50_26);

// Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
// Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_75_51);
// Write HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_75_51);

// Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
HP_SMART_CARD_result_t inkReadTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_100_76);
// Write HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_100_76);

// Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
HP_SMART_CARD_result_t inkReadTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_25_01);
// Write HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_25_01);

// Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
HP_SMART_CARD_result_t inkReadTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_50_26);
// Write HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_50_26);

// Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
HP_SMART_CARD_result_t inkReadTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_75_51);
// Write HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_75_51);

// Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
HP_SMART_CARD_result_t inkReadTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_100_76);
// Write HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_100_76);

// Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
HP_SMART_CARD_result_t inkReadTag91stFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *first_failure_code);

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
HP_SMART_CARD_result_t inkReadTag9AlteredSupply(HP_SMART_CARD_device_id_t cardId, uint8_t *altered_supply);

// Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
HP_SMART_CARD_result_t inkReadTag9UserAckNonOrgInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_altered_supply);

// Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
HP_SMART_CARD_result_t inkReadTag9UserAckExpiredInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_expired_ink);

// Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
HP_SMART_CARD_result_t inkReadTag9FaultyReplaceImmediately(HP_SMART_CARD_device_id_t cardId, uint8_t *faulty_replace_immediately);

// Skipped (Tag 9 - Blank - 27, 4, 32)
// Skipped (Tag 9 - Blank - 31, 4, 13)

////////////////////////////////// TAG 10 ////////////////////////////////////////////////
// OEM RW_OR Memory, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkReadTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_1);
// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkWriteTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_1);

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_2);
// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_2);

////////////////////////////////// TAG 11 ////////////////////////////////////////////////
// HP Scratchpad, 32 bytes, Managed by

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t *cartridge_mru_year);
// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t cartridge_mru_year);

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_woy);
// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_woy);

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_dow);
// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_dow);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mru_platform_mfg_year);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mru_platform_mfg_year);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_woy);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_woy);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_country);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_country);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_major);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_major);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_minor);
// Write HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_minor);

// Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t inkReadTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t *insertion_count);
// Write HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t inkWriteTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t insertion_count);

// Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t inkReadTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t *stall_insertion_count);
// Write HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t inkWriteTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t stall_insertion_count);

// Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t inkReadTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *last_failure_code);
// Write HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t inkWriteTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t last_failure_code);

// Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t inkReadTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t *last_user_reported_status);
// Write HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t inkWriteTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t last_user_reported_status);

// Skipped (Tag 11 - Blank - 18, 0, 16)
// Skipped (Tag 11 - Blank - 20, 0, 24)

// Read HP_SMART_CARD_INK_MARKETING_DATA_REVISION (Tag 11 - Marketing Data Revision - 23, 0, 4)
HP_SMART_CARD_result_t inkReadTag11MarketingDataRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *marketing_data_revision);

// Skipped (Tag 11 - Blank - 24, 4, 24)
// Skipped (Tag 11 - Blank - 27, 4, 24)
// Skipped (Tag 11 - Blank - 30, 4, 21)

////////////////////////////////// TAG 12 ////////////////////////////////////////////////
// OEM Scratchpad, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkReadTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_1);
// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkWriteTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_1);

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_2);
// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_2);

////////////////////////////////// TAG 13 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by HP or Ink Filler

HP_SMART_CARD_result_t inkReadTag13UD7Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud7_fuse);

// Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
HP_SMART_CARD_result_t inkReadTag13ExtendedOEMID(HP_SMART_CARD_device_id_t cardId, uint8_t *extended_oem_id);

// Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
HP_SMART_CARD_result_t inkReadTag13OEMInkDesignator(HP_SMART_CARD_device_id_t cardId, uint8_t *oem_ink_designator);

// Skipped (Tag 11 - Blank - 2, 5, 22)

////////////////////////////////// TAG 14 ////////////////////////////////////////////////
// Ext OEM ID, 12 bytes, Managed by Ink Filler

// Read HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
HP_SMART_CARD_result_t inkReadTag14RegionalizationID(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_regionalization_id);

// Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
HP_SMART_CARD_result_t inkReadTag14ReorderPartNum(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

HP_SMART_CARD_result_t inkReadTag14UD8Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud8_fuse);

// Skipped (Tag 14 - Blank - 11, 6, 15)
// Skipped (Tag 14 - HP reserved - 13, 7, 224)
// Skipped (Tag 14 - Parity Byte - 41, 7, 8)
// Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)

#endif //PRINTER_AS_GIT_SC_MEMORY_ACCESS_H

/////////////////////////////////////////////////////////////////////////////////////////
//                       HP Smart Card Supply Facilities
/////////////////////////////////////////////////////////////////////////////////////////
