//
// Created by hmwan on 2020/4/2.
//

#ifndef PRINTER_AS_GIT_SC_MEMORY_ACCESS_H
#define PRINTER_AS_GIT_SC_MEMORY_ACCESS_H

#include <stdint.h>
#include "../product/tij25/hp_smart_card_config.h"
#include "hp_smart_card.h"

// Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
uint8_t readTag0ChipTag(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
uint8_t readTag0Recerved1(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
#define HP_SMART_CARD_FAMILY_ID_UNKNOWN     0
// For reference
// #define HP_SMART_CARD_INK_FAMILY_ID      27
// #define HPSCS_FAMILY_ID                  28

HP_SMART_CARD_result_t readTag0FamilyID(HP_SMART_CARD_device_id_t cardId, uint8_t *family_id);

// Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
#define HP_SMART_CARD_HP_OEM_APPROVED       0
#define HP_SMART_CARD_HP_OEM_NOT_APPROVED   1

uint8_t readTag0ApprovedOEMBit(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
#define HP_SMART_CARD_OEM_ID_UNKNOWN        0
#define HP_SMART_CARD_OEM_ID_HP             1
#define HP_SMART_CARD_OEM_ID_SPS_1          2
#define HP_SMART_CARD_OEM_ID_SPS_2          3

uint8_t readTag0OEMID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
#define HP_SMART_CARD_ADDRESS_POSITION_UNKNOWN        0
#define HP_SMART_CARD_ADDRESS_POSITION_HOBBES_SMART_CARD        66

uint8_t readTag0AddressPosition(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
uint8_t readTag0TemplateVersionMajor(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
uint8_t readTag0TemplateVersionMinor(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

// Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
uint8_t readTag0PersoRevision(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 0 - Reserved for Perso - 6, 3, 4)
// Skipped (Tag 0 - Blank - 7, 7, 8)

/////////////////////////////////////////////////////////////////////////////////////////
//                       HP Smart Card Ink Facilities
/////////////////////////////////////////////////////////////////////////////////////////

// Partition #0(Tag 1,2)
// Partition #1(Tag 3,4,5)
// Partition #2(Tag 6)
// Partition #3(Tag 7)
// Partition #4(Tag 8)
// Partition #5(Tag 9,10)
// Partition #6(Tag 11,12)
// Partition #7(Tag 13)
// Partition #8(Tag 14)
// Partition #9(Tag 15)

////////////////////////////////// TAG 1 ////////////////////////////////////////////////
// Dry Cartridge Manufacturing Data, 12 bytes, Managed by HP

// Read HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
// Read HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
// Read HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)

#define HP_SMART_CARD_FUSE_UNLOCKED                 0
#define HP_SMART_CARD_FUSE_LOCKED                   1

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

uint8_t readTag1InkUD0Fuse(HP_SMART_CARD_device_id_t cardId);

// Skipped HP_SMART_CARD_INK_BLANK_1 (Tag 1 - Blank - 1, 6, 8)
// Skipped HP_SMART_CARD_INK_BLANK_2 (Tag 1 - Blank - 2, 6, 1)
// HP_SMART_CARD_INK_EXTENDED_OEM_ID seems not an item of Tag1, might be (Tag 13 - Extended OEM ID - 1, 6, 8)
// HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR seems not an item of Tag1, might be (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)

// Read HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
#define HP_SMART_CARD_INK_MANUFACTURE_ID_UNKNOWN      0

uint8_t readTag1InkManufactureID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
#define HP_SMART_CARD_INK_MANUFACTURE_LINE_UNKNOWN      0

uint8_t readTag1InkManufactureLine(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MANUFACTURE_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 2, 5)
uint16_t readTag1InkManufactureYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
uint8_t readTag1InkManufactureWeekOfYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
#define HP_SMART_CARD_SUNDAY                        0
#define HP_SMART_CARD_MONDAY                        1
#define HP_SMART_CARD_TUESDAY                       2
#define HP_SMART_CARD_WEDNESDAY                     3
#define HP_SMART_CARD_THURSDAY                      4
#define HP_SMART_CARD_FRIDAY                        5
#define HP_SMART_CARD_SATURDAY                      6

uint8_t readTag1InkManufactureDayOfWeek(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
uint8_t readTag1InkManufactureHourOfDay(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
uint8_t readTag1InkManufactureMinuteOfHour(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
uint8_t readTag1InkManufactureSecondOfMinute(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
#define HP_SMART_CARD_PROCESS_POSITION_0          0
#define HP_SMART_CARD_PROCESS_POSITION_1          1
#define HP_SMART_CARD_PROCESS_POSITION_2          2
#define HP_SMART_CARD_PROCESS_POSITION_3          3
#define HP_SMART_CARD_PROCESS_POSITION_4          4
#define HP_SMART_CARD_PROCESS_POSITION_5          5
#define HP_SMART_CARD_PROCESS_POSITION_6          6
#define HP_SMART_CARD_PROCESS_POSITION_7          7

uint8_t readTag1InkDryProcessPosition(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 0, 16)
float readTag1InkMaxUsableCartridgeVolume(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Printer Lockdown Partition - 9, 0, 5)
uint8_t readTag1InkPrinterLockdownPartition(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 1 - Blank - 10, 3, 20)

////////////////////////////////// TAG 2 ////////////////////////////////////////////////
// Dry Cartridge Specific Data, 16 bytes, Managed by HP

// Read HP_SMART_CARD_INK_TSR (Tag 2 - Thermal Sense Resistor (TSR) - 1, 7, 7)
uint16_t readTag2InkTSR(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
uint16_t readTag2InkTSRThermalCoefficient(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
#define HP_SMART_CARD_INK_SINGLE_CTRDG     0
#define HP_SMART_CARD_INK_BULK_CTRDG       1

uint8_t readTag2InkBulkCartridge(HP_SMART_CARD_device_id_t cardId);
// Skipped (Tag 2 - Blank - 3, 7, 16)
// Skipped (Tag 2 - Blank - 5, 7, 32)
// Skipped (Tag 2 - Blank - 9, 7, 32)
// Skipped (Tag 2 - Blank - 13, 7, 32)

////////////////////////////////// TAG 3 ////////////////////////////////////////////////
// Ink Fill Manufacturing Data, 20 bytes, Managed by Ink Filter

uint8_t readTag3InkUD1Fuse(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID_UNKNOWN         0
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID_HP_DEFINED(a)      ((a) >= 1 && (a) <= 63)
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID_OEM_SPECIFIC(a)      ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID_TEST            255

uint8_t readTag3InkCartridgeFillSiteID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE_UNKNOWN         0
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE_ICODE1          1
#define HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE_PROTO           31

uint8_t readTag3InkCartridgeFillLine(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
uint16_t readTag3InkCartridgeFillYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
uint8_t readTag3InkCartridgeFillWeekOfYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
uint8_t readTag3InkCartridgeFillDayOfWeek(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
uint8_t readTag3InkCartridgeFillHourOfDay(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
uint8_t readTag3InkCartridgeFillMinuteOfHour(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
uint8_t readTag3InkCartridgeFillSecondOfMinute(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
uint8_t readTag3InkCartridgeFillProcessPosition(HP_SMART_CARD_device_id_t cardId);

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
#define HP_SMART_CARD_INK_FORMULATOR_ID_UNKNOWN             0
#define HP_SMART_CARD_INK_FORMULATOR_ID_HP_DEFINED(a)       ((a) >= 1 && (a) <= 63)         // 资料当中的定义只有1，可能不对
#define HP_SMART_CARD_INK_FORMULATOR_ID_OEM_SPECIFIC(a)     ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_INK_FORMULATOR_ID_TEST                255

uint8_t readTag4InkFormulatorID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
#define HP_SMART_CARD_INK_FAMILY_UNKNOWN             0
#define HP_SMART_CARD_INK_FAMILY_HP_DEFINED(a)       ((a) >= 1 && (a) <= 63)
#define HP_SMART_CARD_INK_FAMILY_OEM_SPECIFIC(a)     ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_INK_FAMILY_TEST                255

uint8_t readTag4InkInkFamily(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
#define HP_SMART_CARD_INK_COLOR_CODES_BLACK       0
#define HP_SMART_CARD_INK_COLOR_CODES_CYAN        1
#define HP_SMART_CARD_INK_COLOR_CODES_MAGENTA     2
#define HP_SMART_CARD_INK_COLOR_CODES_YELLOW      3
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_A   4
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_B   5
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_C   6
#define HP_SMART_CARD_INK_COLOR_CODES_TBD         7

uint8_t readTag4InkColorCodesGeneral(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_UNDEFINED       0
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_PHOTOK          2
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_TBD             7

uint8_t readTag4InkColorCodesSpecific(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
#define HP_SMART_CARD_INK_FAMILY_MEMBER_UNDEFINED       0
#define HP_SMART_CARD_INK_FAMILY_MEMBER_BASE            1
#define HP_SMART_CARD_INK_FAMILY_MEMBER_TEST            255

uint8_t readTag4InkInkFamilyMember(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
#define HP_SMART_CARD_INK_ID_NEMBER_UNDEFINED       0
//#define HP_SMART_CARD_INK_ID_NEMBER_NOT_USED        1
#define HP_SMART_CARD_INK_ID_NEMBER_HP4030CYAN      2
#define HP_SMART_CARD_INK_ID_NEMBER_HP4030MAGENTA   3
#define HP_SMART_CARD_INK_ID_NEMBER_HP4030YELLOW    4
#define HP_SMART_CARD_INK_ID_NEMBER_HP4500BLACK     5
#define HP_SMART_CARD_INK_ID_NEMBER_HP2531BLACK     6
#define HP_SMART_CARD_INK_ID_NEMBER_SPOTBLUE        10
#define HP_SMART_CARD_INK_ID_NEMBER_HP1918BLACK     13
#define HP_SMART_CARD_INK_ID_NEMBER_HP4250VBLACK    14
#define HP_SMART_CARD_INK_ID_NEMBER_HP2242BLUE      22
#define HP_SMART_CARD_INK_ID_NEMBER_HP4040BLACK     27
//#define HP_SMART_CARD_INK_ID_NEMBER_NOT_USED        32
#define HP_SMART_CARD_INK_ID_NEMBER_HP2580SBLACK    33
#define HP_SMART_CARD_INK_ID_NEMBER_HP2510BLACK     34

uint16_t readTag4InkInkIDNumber(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
#define HP_SMART_CARD_INK_ID_REVISION_UNDEFINED       0
#define HP_SMART_CARD_INK_ID_REVISION_INK             1
#define HP_SMART_CARD_INK_ID_REVISION_TEST            255

uint8_t readTag4InkInkRevision(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
// Specifies the ink density.
//      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
float readTag4InkDensity(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
#define HP_SMART_CARD_INK_CARTRIDGE_NO_DISTINCTION       0

uint8_t readTag4InkCartridgeDistinction(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
#define HP_SMART_CARD_INK_KEY_SIZE_UNKNOWN       0
#define HP_SMART_CARD_INK_KEY_SIZE_NORMAL        5
#define HP_SMART_CARD_INK_KEY_SIZE_XL            6
#define HP_SMART_CARD_INK_KEY_SIZE_BULK          7

uint8_t readTag4InkSupplyKeyingDescriptor(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
// ??????????? 不明白
//    Shelf Weeks + Shelf Days + Ink filled time / date <= current date / time
uint8_t readTag4InkShelfLifeWeeks(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
uint8_t readTag4InkShelfLifeDays(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
// ??????????? 不明白
//    Installed Weeks + Installed Days + First installed time / date <= current date / time
uint8_t readTag4InkInstalledLifeWeeks(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
uint8_t readTag4InkInstalledLifeDays(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
// m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
float readTag4InkUsableInkWeight(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
// Percentage Value: 10 means 10%
uint8_t readTag4InkAlteredSupplyNotificationLevel(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 4 - Blank - 15, 4, 15)

////////////////////////////////// TAG 5 ////////////////////////////////////////////////
// Filled Cartridge Specific Data, 12 bytes, Managed by Ink Filter

// Read HP_SMART_CARD_INK_FIRING_FREQ (Tag 5 - Firing Frequency - 1, 7, 7)
float readTag5InkFiringFreq(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
float readTag5InkPulseWidth(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
float readTag5InkFiringVoltage(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
float readTag5InkTurnOnEnergy(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
uint8_t readTag5InkPulseWarmingTemperature(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
uint8_t readTag5InkMaxTemperature(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
uint8_t readTag5InkDropVolume(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 5 - Blank - 7, 5, 14)
// Skipped (Tag 5 - Blank - 9, 7, 32)

////////////////////////////////// TAG 6 ////////////////////////////////////////////////
// Cartridge First Install Data, 20 bytes, Managed by Platform

// Read HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse - 1, 7, 1)
#define HP_SMART_CARD_INK_WRITE_PROTECT_UNLOCKED       0
#define HP_SMART_CARD_INK_WRITE_PROTECT_LOCKED         1

uint8_t readTag6InkWriteProtect(HP_SMART_CARD_device_id_t cardId);
HP_SMART_CARD_result_t writeTag6InkWriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t write_protect);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t readTag6Ink1stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
HP_SMART_CARD_result_t writeTag6Ink1stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
uint16_t readTag6Ink1stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId);
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mfg_year);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
uint8_t readTag6Ink1stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId);
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_woy);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
#define HP_SMART_CARD_COUNTRY_US       0        // United States
#define HP_SMART_CARD_COUNTRY_SG       1        // Singapore
#define HP_SMART_CARD_COUNTRY_ES       2        // Spain
#define HP_SMART_CARD_COUNTRY_MX       3        // Mexico
#define HP_SMART_CARD_COUNTRY_MY       4        // Malaysia
#define HP_SMART_CARD_COUNTRY_CN       5        // China
#define HP_SMART_CARD_COUNTRY_TW       6        // Taiwan
#define HP_SMART_CARD_COUNTRY_DE       32       // Germany
#define HP_SMART_CARD_COUNTRY_UNKNOWN  255      // Unknown

uint8_t readTag6Ink1stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_country);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
uint8_t readTag6Ink1stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_major);

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
uint8_t readTag6Ink1stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_minor);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
uint16_t readTag6Ink1stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t writeTag6Ink1stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_count);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
uint16_t readTag6Ink1stInstallYear(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t writeTag6Ink1stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_year);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
uint8_t readTag6Ink1stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t writeTag6Ink1stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_woy);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
uint8_t readTag6Ink1stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t writeTag6Ink1stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_dow);

// Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
uint8_t readTag6InkInkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t writeTag6InkInkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t gauge_resolution);

// Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
uint32_t readTag6Ink1stInstallOEMField(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t writeTag6Ink1stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t oem_field);

// Skipped (Tag 6 - Blank - 20, 7, 8)

////////////////////////////////// TAG 7 ////////////////////////////////////////////////
// OEM Lockable memory, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
uint8_t readTag7InkUD3Fuse(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t writeTag7InkUD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud3_fuse);

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
uint32_t readTag7InkOEMDefinedField1(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
HP_SMART_CARD_result_t writeTag7InkOEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_1);

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
uint32_t readTag7InkOEMDefinedField2(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
HP_SMART_CARD_result_t writeTag7InkOEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_2);

////////////////////////////////// TAG 8 ////////////////////////////////////////////////
// Trademark, 4 bytes, Managed by HP or Ink Filler

// Read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t readTag8InkHPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
// Write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t writeTag8InkHPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
uint8_t readTag8InkUD4Fuse(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
HP_SMART_CARD_result_t writeTag8InkUD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud4_fuse);

// Skipped (Tag 8 - Blank - 4, 0, 1)

////////////////////////////////// TAG 9 ////////////////////////////////////////////////
// Ink Use & Warranty, 32 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
#define HP_SMART_CARD_INK_ILG_OUT_OF_INK_UNREACH       0
#define HP_SMART_CARD_INK_ILG_OUT_OF_INK_REACHED       1

uint8_t readTag9InkILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t writeTag9InkILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t out_of_ink);

// Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
// Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit01To25(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
HP_SMART_CARD_result_t writeTag9InkILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_25_01);

// Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
// Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit26To50(HP_SMART_CARD_device_id_t cardId);
// Write HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
HP_SMART_CARD_result_t writeTag9InkILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_50_26);

// Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
// Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit51To75(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
uint32_t readTag9InkILGBit76To100(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
uint32_t readTag9InkTUGBit01To25(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
uint32_t readTag9InkTUGBit26To50(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
uint32_t readTag9InkTUGBit51To75(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
uint32_t readTag9InkTUGBit76To100(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
#define HP_SMART_CARD_INK_FIRST_FAILURE_CODE_NOEEROR       0

uint8_t readTag9Ink1stFailureCode(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
#define HP_SMART_CARD_INK_ALTERED_SUPPLY_NOT_EXCEEDED       0
#define HP_SMART_CARD_INK_ALTERED_SUPPLY_EXCEEDED           1

uint8_t readTag9InkAlteredSupply(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
#define HP_SMART_CARD_INK_USER_NOT_ACKNOWLEDGED       0
#define HP_SMART_CARD_INK_USER_ACKNOWLEDGED           1

uint8_t readTag9InkUserAckNonOrgInk(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
#define HP_SMART_CARD_INK_USER_ACK_NOT_EXPIRED       0
#define HP_SMART_CARD_INK_USER_ACK_EXPIRED           1

uint8_t readTag9InkUserAckExpiredInk(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
#define HP_SMART_CARD_INK_FAULTY_NOT_FATAL       0
#define HP_SMART_CARD_INK_FAULTY_FATAL           1

uint8_t readTag9InkFaultyReplaceImmediately(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 9 - Blank - 27, 4, 32)
// Skipped (Tag 9 - Blank - 31, 4, 13)

////////////////////////////////// TAG 10 ////////////////////////////////////////////////
// OEM RW_OR Memory, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
uint32_t readTag10InkOEMRWORField1(HP_SMART_CARD_device_id_t cardId);

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t writeTag10InkOEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_1);

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
uint32_t readTag10InkOEMRWORField2(HP_SMART_CARD_device_id_t cardId);

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t writeTag10InkOEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_2);

////////////////////////////////// TAG 11 ////////////////////////////////////////////////
// HP Scratchpad, 32 bytes, Managed by

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
uint16_t readTag11InkCartridgeMRUYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
uint8_t readTag11InkCartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
uint8_t readTag11InkCartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t readTag11InkMRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);
HP_SMART_CARD_result_t writeTag11InkMRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
uint16_t readTag11InkMRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
uint8_t readTag11InkMRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
uint8_t readTag11InkMRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
uint8_t readTag11InkMRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
uint8_t readTag11InkMRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
uint8_t readTag11InkInsertionCount(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
uint8_t readTag11InkStallInsertionCount(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
#define HP_SMART_CARD_INK_LAST_FAILURE_CODE_NOEEROR       0

uint8_t readTag11InkLastFailureCode(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
uint8_t readTag11InkUserReportedStatus(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 11 - Blank - 18, 0, 16)
// Skipped (Tag 11 - Blank - 20, 0, 24)

// Read HP_SMART_CARD_INK_MARKETING_DATA_REVISION (Tag 11 - Marketing Data Revision - 23, 0, 4)
uint8_t readTag11InkMarketingDataRevision(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 11 - Blank - 24, 4, 24)
// Skipped (Tag 11 - Blank - 27, 4, 24)
// Skipped (Tag 11 - Blank - 30, 4, 21)

////////////////////////////////// TAG 12 ////////////////////////////////////////////////
// OEM Scratchpad, 8 bytes, Managed by OEM

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
uint32_t readTag12InkOEMDefRWField1(HP_SMART_CARD_device_id_t cardId);

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t writeTag12InkOEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_1);

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
uint32_t readTag12InkOEMDefRWField2(HP_SMART_CARD_device_id_t cardId);

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t writeTag12InkOEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_2);

////////////////////////////////// TAG 13 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by HP or Ink Filler

uint8_t readTag13InkUD7Fuse(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_UNKNOWN             0
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_TBD(a)              ((a) >= 1 && (a) <= 14)
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_HP                  15
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_OEM_SPECIFIC(a)     ((a) >= 16 && (a) <= 29)
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_OEM_A               30
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_OEM_SPECIFICA(a)    ((a) >= 31 && (a) <= 254)
#define HP_SMART_CARD_INK_EXTENDED_OEM_ID_TEST_PEN_ID         255

uint8_t readTag13InkExtendedOEMID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
#define HP_SMART_CARD_INK_OEM_INK             0
#define HP_SMART_CARD_INK_HP_INK              1

uint8_t readTag13InkOEMInkDesignator(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 11 - Blank - 2, 5, 22)

////////////////////////////////// TAG 14 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by Ink Filler

// Read HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
#define HP_SMART_CARD_INK_REGIONALIZATION_ID_UNKNOWN             0
#define HP_SMART_CARD_INK_REGIONALIZATION_ID_UNIVERSAL           1
#define HP_SMART_CARD_INK_REGIONALIZATION_ID_TEST                255

uint8_t readTag14InkRegionalizationID(HP_SMART_CARD_device_id_t cardId);

// Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
HP_SMART_CARD_result_t readTag14InkReorderPartNum(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer);

uint8_t readTag14InkUD8Fuse(HP_SMART_CARD_device_id_t cardId);

// Skipped (Tag 14 - Blank - 11, 6, 15)
// Skipped (Tag 14 - HP reserved - 13, 7, 224)
// Skipped (Tag 14 - Parity Byte - 41, 7, 8)
// Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)

#endif //PRINTER_AS_GIT_SC_MEMORY_ACCESS_H

/////////////////////////////////////////////////////////////////////////////////////////
//                       HP Smart Card Supply Facilities
/////////////////////////////////////////////////////////////////////////////////////////
