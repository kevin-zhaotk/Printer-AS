//
// Created by hmwan on 2020/3/31.
//

//#include <stdint.h>
//#include <unistd.h>
#include "../common_log.h"
#include "sc_ink_mem_access.h"
#include "sc_common_mem_access.h"

////////////////////////////////// TAG 1 ////////////////////////////////////////////////
// Dry Cartridge Manufacturing Data, 12 bytes, Managed by HP
// HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG            = 0x01,

// Read HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the Dry Cartridge memory partition (Partition #0)
// This value provided by SPS.
//    0 = Dry cartridge mfg partition unlocked
//    1 = Dry cartridge mfg partition locked
HP_SMART_CARD_result_t inkReadTag1UD0Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud0_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD0_FUSE, ud0_fuse);
}

// Skipped HP_SMART_CARD_INK_BLANK_1 (Tag 1 - Blank - 1, 6, 8)
// Skipped HP_SMART_CARD_INK_BLANK_2 (Tag 1 - Blank - 2, 6, 1)
// HP_SMART_CARD_INK_EXTENDED_OEM_ID seems not an item of Tag1, might be (Tag 13 - Extended OEM ID - 1, 6, 8)
// HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR seems not an item of Tag1, might be (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)

// Read HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureID(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_id) {
    return readByte(cardId, HP_SMART_CARD_INK_MANUFACTURE_ID, manufacture_id);
}

// Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
HP_SMART_CARD_result_t inkReadTag1ManufactureLine(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_line) {
    return readByte(cardId, HP_SMART_CARD_INK_MANUFACTURE_LINE, manufacture_line);
}

// Read HP_SMART_CARD_INK_MANUFACTURE_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 2, 5)
// Year the dry cartridge was manufactured.
//    m = 1, b = 2006, range = 2006 - 2037, in the equation = mx + b where the y slope equals 1, and the intercept is 2006.
HP_SMART_CARD_result_t inkReadTag1ManufactureYear(HP_SMART_CARD_device_id_t cardId, uint16_t *manufacture_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_MANUFACTURE_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *manufacture_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_YEAR = %d", *manufacture_year);
    }
    return result;
}

// Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_MANUFACTURE_WEEK, manufacture_woy);
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
HP_SMART_CARD_result_t inkReadTag1ManufactureDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_dow) {
    return readByte(cardId, HP_SMART_CARD_INK_SERIAL_NUM_DOW, manufacture_dow);
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
HP_SMART_CARD_result_t inkReadTag1ManufactureHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_hod) {
    return readByte(cardId, HP_SMART_CARD_INK_SERIAL_NUM_HOD, manufacture_hod);
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_moh) {
    return readByte(cardId, HP_SMART_CARD_INK_SERIAL_NUM_MOH, manufacture_moh);
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
HP_SMART_CARD_result_t inkReadTag1ManufactureSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_som) {
    return readByte(cardId, HP_SMART_CARD_INK_SERIAL_NUM_SOM, manufacture_som);
}

// Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
HP_SMART_CARD_result_t inkReadTag1DryProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *process_position) {
    return readByte(cardId, HP_SMART_CARD_INK_DRY_PROCESS_POSITION, process_position);
}

// Read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 0, 16)
// Maximum usable cartridge volume
//    m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag1MaxUsableCartridgeVolume(HP_SMART_CARD_device_id_t cardId, float *max_usable_cartridge_volume) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *max_usable_cartridge_volume = buf * 0.1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME = %f", *max_usable_cartridge_volume);
    }
    return result;
}

// Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Partition number of the printer RW2RO memory space. Used for capturing 1st install data - 9, 0, 5)
HP_SMART_CARD_result_t inkReadTag1PrinterLockdownPartition(HP_SMART_CARD_device_id_t cardId, uint8_t *lock_down_partition ) {
    return readByte(cardId, HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION, lock_down_partition);
}

// Skipped (Tag 1 - Blank - 10, 3, 20)

////////////////////////////////// TAG 2 ////////////////////////////////////////////////
// Dry Cartridge Specific Data, 16 bytes, Managed by HP
// HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG       = 0x02,

// Read HP_SMART_CARD_INK_TSR (Tag 2 - Thermal Sense Resistor (TSR) - 1, 7, 7)
// Value of the thermal sense resistor:
//    m = 1 b = 250 range = 250 – 377 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag2TSR(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_tsr) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_TSR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_tsr = buf * 1 + 250;
        LOGD(">>> HP_SMART_CARD_INK_TSR = %d", *ink_tsr);
    }
    return result;
}

// Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
// Value of the TSR thermal coefficient
//    m = 5 b = 2900 range = 2900 -  in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag2TSRThermalCoefficient(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_tcr) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_tcr = buf * 5 + 2900;
        LOGD(">>> HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT = %d", *ink_tcr);
    }
    return result;
}

// Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
// Bulk cartridge identifier
//    0 = single use cartridge
//    1 = bulk ink cartridge
HP_SMART_CARD_result_t inkReadTag2BulkCartridge(HP_SMART_CARD_device_id_t cardId, uint8_t *bulk_ctrdg) {
    return readByte(cardId, HP_SMART_CARD_INK_BULK_CTRDG, bulk_ctrdg);
}

// Skipped (Tag 2 - Blank - 3, 7, 16)
// Skipped (Tag 2 - Blank - 5, 7, 32)
// Skipped (Tag 2 - Blank - 9, 7, 32)
// Skipped (Tag 2 - Blank - 13, 7, 32)

////////////////////////////////// TAG 3 ////////////////////////////////////////////////
// Ink Fill Manufacturing Data, 20 bytes, Managed by Ink Filter
// HP_SMART_CARD_INK_INK_FILL_MFG_DATA_TAG             = 0x03,

// Read HP_SMART_CARD_INK_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1)
// state of the Ink Fill memory partition (Partition #1)
//    0 = Ink fill partition unlocked
//    1 = Ink fill partition locked
HP_SMART_CARD_result_t inkReadTag3UD1Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud1_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD1_FUSE, ud1_fuse);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillSiteID(HP_SMART_CARD_device_id_t cardId, uint8_t *site_id) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID, site_id);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillLine(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_line) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE, fill_line);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
// Year that cartridge was filled.
//      m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag3CartridgeFillYear(HP_SMART_CARD_device_id_t cardId, uint16_t *fill_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *fill_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR = %d", *fill_year);
    }
    return result;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY, fill_woy);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_dow) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW, fill_dow);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_hod) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD, fill_hod);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_moh) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH, fill_moh);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_som) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM, fill_som);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
HP_SMART_CARD_result_t inkReadTag3CartridgeFillProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_process_position) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION, fill_process_position);
}

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
// HP_SMART_CARD_INK_INK_DATA_AND_PROPERTIES_TAG       = 0x04,

// Read HP_SMART_CARD_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
HP_SMART_CARD_result_t inkReadTag4FormulatorID(HP_SMART_CARD_device_id_t cardId, uint8_t *formulator_id) {
    return readByte(cardId, HP_SMART_CARD_INK_FORMULATOR_ID, formulator_id);
}

// Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
HP_SMART_CARD_result_t inkReadTag4InkFamily(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family) {
    return readByte(cardId, HP_SMART_CARD_INK_INK_FAMILY, ink_family);
}

// Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
HP_SMART_CARD_result_t inkReadTag4ColorCodesGeneral(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_general) {
    return readByte(cardId, HP_SMART_CARD_INK_COLOR_CODES_GENERAL, ink_color_code_general);
}

// Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
HP_SMART_CARD_result_t inkReadTag4ColorCodesSpecific(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_specific) {
    return readByte(cardId, HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC, ink_color_code_specific);
}

// Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
HP_SMART_CARD_result_t inkReadTag4InkFamilyMember(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family_member) {
    return readByte(cardId, HP_SMART_CARD_INK_INK_FAMILY_MEMBER, ink_family_member);
}

// Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
HP_SMART_CARD_result_t inkReadTag4InkIDNumber(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_id_num) {
    return read2Bytes(cardId, HP_SMART_CARD_INK_INK_ID_NUM, ink_id_num);
}

// Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
HP_SMART_CARD_result_t inkReadTag4InkRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_revision) {
    return readByte(cardId, HP_SMART_CARD_INK_INK_REVISION, ink_revision);
}

// Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
// Specifies the ink density.
//      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag4Density(HP_SMART_CARD_device_id_t cardId, float *ink_density) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HP_SMART_CARD_INK_DENSITY, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_density = 0.001 * buf + 0.7;
        LOGD(">>> HP_SMART_CARD_INK_DENSITY = %f", *ink_density);
    }
    return result;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
HP_SMART_CARD_result_t inkReadTag4CartridgeDistinction(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_cartridge_distinction) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION, ink_cartridge_distinction);
}

// Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
HP_SMART_CARD_result_t inkReadTag4SupplyKeyingDescriptor(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_supply_keying_descriptor) {
    return readByte(cardId, HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR, ink_supply_keying_descriptor);
}

// Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
// ??????????? 不明白
//    Shelf Weeks + Shelf Days + Ink filled time / date <= current date / time
HP_SMART_CARD_result_t inkReadTag4ShelfLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_weeks) {
    return readByte(cardId, HP_SMART_CARD_INK_SHELF_LIFE_WEEKS, ink_shelf_life_weeks);
}

// Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
HP_SMART_CARD_result_t inkReadTag4ShelfLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_days) {
    return readByte(cardId, HP_SMART_CARD_INK_SHELF_LIFE_DAYS, ink_shelf_life_days);
}

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
// ??????????? 不明白
//    Installed Weeks + Installed Days + First installed time / date <= current date / time
HP_SMART_CARD_result_t inkReadTag4InstalledLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_weeks) {
    return readByte(cardId, HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS, ink_installed_life_weeks);
}

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
HP_SMART_CARD_result_t inkReadTag4InstalledLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_days) {
    return readByte(cardId, HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS, ink_installed_life_days);
}

// Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
// m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag4UsableInkWeight(HP_SMART_CARD_device_id_t cardId, float *ink_usable_ink_weigh) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HP_SMART_CARD_INK_USABLE_INK_WEIGHT, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_usable_ink_weigh = buf * 0.1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_USABLE_INK_WEIGHT = %f", *ink_usable_ink_weigh);
    }
    return result;
}

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
// Percentage Value: 10 means 10%
HP_SMART_CARD_result_t inkReadTag4AlteredSupplyNotificationLevel(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_notification_level) {
    return readByte(cardId, HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL, ink_notification_level);
}

// Skipped (Tag 4 - Blank - 15, 4, 15)

////////////////////////////////// TAG 5 ////////////////////////////////////////////////
// Filled Cartridge Specific Data, 12 bytes, Managed by Ink Filter
// HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG    = 0x05,

// Read HP_SMART_CARD_INK_FIRING_FREQ (Tag 5 - Firing Frequency - 1, 7, 7)
// Maximum firing frequency of the cartridge, based on the filled ink.
//    m = 0.2, b = 0, range = 0 – 25.4 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag5FiringFreq(HP_SMART_CARD_device_id_t cardId, float *firing_freq) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_FIRING_FREQ, &buf);

    if (HP_SMART_CARD_OK == result) {
        *firing_freq = buf * 0.2 + 0;
        LOGD(">>> HP_SMART_CARD_INK_FIRING_FREQ = %f", *firing_freq);
    }
    return result;
}

// Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
// Firing pulse width, based on the filled ink type.
//    m = 0.01 b = 1.5, range = 1.50 – 4.05 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag5PulseWidth(HP_SMART_CARD_device_id_t cardId, float *pulse_width) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_PULSE_WIDTH, &buf);

    if (HP_SMART_CARD_OK == result) {
        *pulse_width = buf * 0.1 + 1.5;
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WIDTH = %f", *pulse_width);
    }
    return result;
}

// Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
// Firing voltage required by the cartridge, based on the filled ink type.
//    m = 0.1, b = 5.0, range = 5.0 – 17.7 y = mx + b
HP_SMART_CARD_result_t inkReadTag5FiringVoltage(HP_SMART_CARD_device_id_t cardId, float *firing_voltage) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_FIRING_VOLTAGE, &buf);

    if (HP_SMART_CARD_OK == result) {
        *firing_voltage = buf * 0.1 + 5.0;
        LOGD(">>> HP_SMART_CARD_INK_FIRING_VOLTAGE = %f", *firing_voltage);
    }
    return result;
}

// Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
// Turn on energy for a firing resistor, based on the filled ink type.
//    m = 0.02, b = 2.0, range = 2.0 – 7.1 y = mx + b
HP_SMART_CARD_result_t inkReadTag5TurnOnEnergy(HP_SMART_CARD_device_id_t cardId, float *turn_on_energy) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_TURN_ON_ENERGY, &buf);

    if (HP_SMART_CARD_OK == result) {
        *turn_on_energy = buf * 0.02 + 2.0;
        LOGD(">>> HP_SMART_CARD_INK_TURN_ON_ENERGY = %f", *turn_on_energy);
    }
    return result;
}

// Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
// Pulse warming temperature required for optimal firing for this cartridge, based on the filled ink type.
//    m = 1, b = 0, range = 0 – 127 y = mx + b
HP_SMART_CARD_result_t inkReadTag5PulseWarmingTemperature(HP_SMART_CARD_device_id_t cardId, uint8_t *pulse_warming_temp) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_PULSE_WARMING_TEMP, &buf);

    if (HP_SMART_CARD_OK == result) {
        *pulse_warming_temp = buf * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WARMING_TEMP = %d", *pulse_warming_temp);
    }
    return result;
}

// Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
// Maximum head temperature for this cartridge, based on the filled ink type.
//    m = 1, b = 0, range = 0 – 127 y = mx + b
HP_SMART_CARD_result_t inkReadTag5MaxTemperature(HP_SMART_CARD_device_id_t cardId, uint8_t *max_temp) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_MAX_TEMP, &buf);

    if (HP_SMART_CARD_OK == result) {
        *max_temp = buf * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_MAX_TEMP = %d", *max_temp);
    }
    return result;
}

// Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
// Ink drop volume, based on filled ink type firing parameters.
//    m = 1, b = 0, range = 0 – 63 y = mx + b
HP_SMART_CARD_result_t inkReadTag5DropVolume(HP_SMART_CARD_device_id_t cardId, uint8_t *drop_volume) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_DROP_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *drop_volume = buf * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_DROP_VOLUME = %d", *drop_volume);
    }
    return result;
}

// Skipped (Tag 5 - Blank - 7, 5, 14)
// Skipped (Tag 5 - Blank - 9, 7, 32)

////////////////////////////////// TAG 6 ////////////////////////////////////////////////
// Cartridge First Install Data, 20 bytes, Managed by Platform
// HP_SMART_CARD_INK_CTRDG_FIRST_INSTALLED_DATA_TAG    = 0x06,

// Read HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the Printer 1st Install memory partition (Partition #2)
//    0 = OEM Lockable partition unlocked
//    1 = OEM Lockable partition locked
HP_SMART_CARD_result_t inkReadTag6WriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t *write_protect) {
    return readByte(cardId, HP_SMART_CARD_INK_WRITE_PROTECT, write_protect);
}

// Write HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag6WriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t write_protect) {
    return writeByte(cardId, HP_SMART_CARD_INK_WRITE_PROTECT, write_protect);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t inkReadTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t inkWriteTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
// The first install printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mfg_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *mfg_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR = %d", *mfg_year);
    }
    return result;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mfg_year) {
    uint8_t buffer = mfg_year - 2006;
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR, buffer);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY, mfg_woy);
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_woy) {
    return writeByte(cardId, HP_SMART_CARD_INK_WRITE_PROTECT, mfg_woy);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t inkReadTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_country) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY, mfg_country);
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t inkWriteTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_country) {
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY, mfg_country);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t inkReadTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_major) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION, fw_revision_major);
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t inkWriteTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_major) {
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION, fw_revision_major);
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t inkReadTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_minor) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION, fw_revision_minor);
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t inkWriteTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_minor) {
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION, fw_revision_minor);
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t inkReadTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_count) {
    return read2Bytes(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT, first_install_count);
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t inkWriteTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_count) {
    return write2Bytes(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT, first_install_count);
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
// The year of the first installation of this cartridge into this printer platform.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *first_install_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_YEAR = %d", *first_install_year);
    }
    return result;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t inkWriteTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_year) {
    uint8_t buffer = first_install_year - 2006;
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR, buffer);
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t inkReadTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_WOY, first_install_woy);
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t inkWriteTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_woy) {
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_WOY, first_install_woy);
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t inkReadTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_dow) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_DOW, first_install_dow);
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t inkWriteTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_dow) {
    return writeByte(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_DOW, first_install_dow);
}

// Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
// If this filed is programmed with a value of 50(decimal), then it means each bit represent 2% of in usage
HP_SMART_CARD_result_t inkReadTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t *gauge_resolution) {
    return readByte(cardId, HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION, gauge_resolution);
}

// Write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t inkWriteTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t gauge_resolution) {
    return writeByte(cardId, HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION, gauge_resolution);
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t inkReadTag61stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_field) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD, oem_field);
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t inkWriteTag61stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t oem_field) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD, oem_field);
}

// Skipped (Tag 6 - Blank - 20, 7, 8)

////////////////////////////////// TAG 7 ////////////////////////////////////////////////
// OEM Lockable memory, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG           = 0x07,

// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the OEM Lockable memory partition (Partition #3)
//    0 = OEM Lockable partition unlocked
//    1 = OEM Lockable partition locked
HP_SMART_CARD_result_t inkReadTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud3_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD3_FUSE, ud3_fuse);
}

// Write HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud3_fuse) {
    return writeByte(cardId, HP_SMART_CARD_INK_UD3_FUSE, ud3_fuse);
}

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t inkReadTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_1) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1, oem_defined_filed_1);
}

// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t inkWriteTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_1) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1, oem_defined_filed_1);
}

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_2) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2, oem_defined_filed_2);
}

// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_2) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2, oem_defined_filed_2);
}

////////////////////////////////// TAG 8 ////////////////////////////////////////////////
// Trademark, 4 bytes(defined as 8 bytes in hp_smart_card_layout.h), Managed by HP or Ink Filler
// HP_SMART_CARD_INK_HP_TRADEMARK_TAG                  = 0x08,

// Read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t inkReadTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1, 5, buffer);
}

// Write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t inkWriteTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1, 5, buffer);
}

// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
// Bit indicating lock/unlock state of the Trademark memory partition (Partition #4)
//    0 = Trademark partition unlocked
//    1 = Trademark partition locked
HP_SMART_CARD_result_t inkReadTag8UD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud4_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD4_FUSE, ud4_fuse);
}

// Write HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
HP_SMART_CARD_result_t inkWriteTag8UD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud4_fuse) {
    return writeByte(cardId, HP_SMART_CARD_INK_UD4_FUSE, ud4_fuse);
}

// Skipped (Tag 8 - Blank - 4, 0, 1)

////////////////////////////////// TAG 9 ////////////////////////////////////////////////
// Ink Use & Warranty, 32 bytes, Managed by OEM
// HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG          = 0x09,

// Read HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
// This bit is set by the printer once the ILG indicates out of ink condition.
//    0 = not reached,
//    1 = reached
HP_SMART_CARD_result_t inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t *out_of_ink) {
    return readByte(cardId, HP_SMART_CARD_INK_OUT_OF_INK_BIT, out_of_ink);
}

// Write HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t out_of_ink) {
    return writeByte(cardId, HP_SMART_CARD_INK_OUT_OF_INK_BIT, out_of_ink);
}

// Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
// Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_25_01) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_01_25, ilg_bit_25_01);
}

// Write HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_25_01) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_01_25, ilg_bit_25_01);
}

// Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
// Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_50_26) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_26_50, ilg_bit_50_26);
}

// Write HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_50_26) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_26_50, ilg_bit_50_26);
}

// Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
// Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_75_51) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_51_75, ilg_bit_75_51);
}

// Write HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_75_51) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_51_75, ilg_bit_75_51);
}

// Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
// Ink level gauge bits 76-100 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_100_76) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_76_100, ilg_bit_100_76);
}

// Write HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
HP_SMART_CARD_result_t inkWriteTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_100_76) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_ILG_BIT_76_100, ilg_bit_100_76);
}

// Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
// Total Ink Usage Gauge bits 1 - 25 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_25_01) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_01_25, tiug_bit_25_01);
}

// Write HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_25_01) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_01_25, tiug_bit_25_01);
}

// Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
// Total Ink Usage Gauge bits 26 - 50 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_50_26) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_26_50, tiug_bit_50_26);
}

// Write HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_50_26) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_26_50, tiug_bit_50_26);
}

// Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
// Total Ink Usage Gauge bits 51 - 75 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_75_51) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_51_75, tiug_bit_75_51);
}

// Write HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_75_51) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_51_75, tiug_bit_75_51);
}

// Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
// Total Ink Usage Gauge bits 76 - 100 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t inkReadTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_100_76) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_76_100, tiug_bit_100_76);
}

// Write HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
HP_SMART_CARD_result_t inkWriteTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_100_76) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_TUG_BIT_76_100, tiug_bit_100_76);
}

// Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
HP_SMART_CARD_result_t inkReadTag91stFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *first_failure_code) {
    return readByte(cardId, HP_SMART_CARD_INK_FIRST_FAILURE_CODE, first_failure_code);
}

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
// This bit is set by the printer when the cartridge has positively exceeded available ink.
// This is set once the TIUG target has been reached.
//    0 = not reached,
//    1 = reached
HP_SMART_CARD_result_t inkReadTag9AlteredSupply(HP_SMART_CARD_device_id_t cardId, uint8_t *altered_supply) {
    return readByte(cardId, HP_SMART_CARD_INK_ALTERED_SUPPLY, altered_supply);
}

// Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
HP_SMART_CARD_result_t inkReadTag9UserAckNonOrgInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_altered_supply) {
    return readByte(cardId, HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK, user_ack_altered_supply);
}

// Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
HP_SMART_CARD_result_t inkReadTag9UserAckExpiredInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_expired_ink) {
    return readByte(cardId, HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK, user_ack_expired_ink);
}

// Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
HP_SMART_CARD_result_t inkReadTag9FaultyReplaceImmediately(HP_SMART_CARD_device_id_t cardId, uint8_t *faulty_replace_immediately) {
    return readByte(cardId, HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY, faulty_replace_immediately);
}

// Skipped (Tag 9 - Blank - 27, 4, 32)
// Skipped (Tag 9 - Blank - 31, 4, 13)

////////////////////////////////// TAG 10 ////////////////////////////////////////////////
// OEM RW_OR Memory, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG              = 0x0A,

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkReadTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_1) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1, oem_rw_or_filed_1);
}

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkWriteTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_1) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1, oem_rw_or_filed_1);
}

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_2) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2, oem_rw_or_filed_2);
}

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_2) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2, oem_rw_or_filed_2);
}

////////////////////////////////// TAG 11 ////////////////////////////////////////////////
// HP Scratchpad, 32 bytes, Managed by
// HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG                = 0x0B,

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
// Most recent use (MRU) of this cartridge - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t *cartridge_mru_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *cartridge_mru_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR = %d", *cartridge_mru_year);
    }
    return result;
}

// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t cartridge_mru_year) {
    uint8_t buffer = cartridge_mru_year - 2006;
    return writeByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR, buffer);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY, cartridge_mru_woy);
}

// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_woy) {
    return writeByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY, cartridge_mru_woy);
}

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t inkReadTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_dow) {
    return readByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW, cartridge_mru_dow);
}

// Write HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t inkWriteTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_dow) {
    return writeByte(cardId, HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW, cartridge_mru_dow);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
// The most recent use (MRU) printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b.
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mru_platform_mfg_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *mru_platform_mfg_year = buf * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR = %d", *mru_platform_mfg_year);
    }
    return result;
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mru_platform_mfg_year) {
    uint8_t buffer = mru_platform_mfg_year - 2006;
    return writeByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR, buffer);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_woy) {
    return readByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY, mru_platform_mfg_woy);
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_woy) {
    return writeByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY, mru_platform_mfg_woy);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_country) {
    return readByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY, mru_platform_mfg_country);
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_country) {
    return writeByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY, mru_platform_mfg_country);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_major) {
    return readByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR, mru_platform_fw_rev_major);
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_major) {
    return writeByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR, mru_platform_fw_rev_major);
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t inkReadTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_minor) {
    return readByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR, mru_platform_fw_rev_minor);
}

// Write HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t inkWriteTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_minor) {
    return writeByte(cardId, HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR, mru_platform_fw_rev_minor);
}

// Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t inkReadTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t *insertion_count) {
    return readByte(cardId, HP_SMART_CARD_INK_INSERTION_COUNT, insertion_count);
}

// Write HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t inkWriteTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t insertion_count) {
    return writeByte(cardId, HP_SMART_CARD_INK_INSERTION_COUNT, insertion_count);
}

// Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t inkReadTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t *stall_insertion_count) {
    return read2Bytes(cardId, HP_SMART_CARD_INK_STALL_INSERTION_COUNT, stall_insertion_count);
}

// Write HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t inkWriteTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t stall_insertion_count) {
    return write2Bytes(cardId, HP_SMART_CARD_INK_STALL_INSERTION_COUNT, stall_insertion_count);
}

// Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t inkReadTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *last_failure_code) {
    return readByte(cardId, HP_SMART_CARD_INK_LAST_FAILURE_CODE, last_failure_code);
}

// Write HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t inkWriteTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t last_failure_code) {
    return writeByte(cardId, HP_SMART_CARD_INK_LAST_FAILURE_CODE, last_failure_code);
}

// Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t inkReadTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t *last_user_reported_status) {
    return readByte(cardId, HP_SMART_CARD_INK_USER_REPORTED_STATUS, last_user_reported_status);
}

// Write HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t inkWriteTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t last_user_reported_status) {
    return writeByte(cardId, HP_SMART_CARD_INK_USER_REPORTED_STATUS, last_user_reported_status);
}

// Skipped (Tag 11 - Blank - 18, 0, 16)
// Skipped (Tag 11 - Blank - 20, 0, 24)

// Read HP_SMART_CARD_INK_MARKETING_DATA_REVISION (Tag 11 - Marketing Data Revision - 23, 0, 4)
HP_SMART_CARD_result_t inkReadTag11MarketingDataRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *marketing_data_revision) {
    return readByte(cardId, HP_SMART_CARD_INK_MARKETING_DATA_REVISION, marketing_data_revision);
}

// Skipped (Tag 11 - Blank - 24, 4, 24)
// Skipped (Tag 11 - Blank - 27, 4, 24)
// Skipped (Tag 11 - Blank - 30, 4, 21)

////////////////////////////////// TAG 12 ////////////////////////////////////////////////
// OEM Scratchpad, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG               = 0x0C,

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkReadTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_1) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1, oem_rw_filed_1);
}

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t inkWriteTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_1) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1, oem_rw_filed_1);
}

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t inkReadTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_2) {
    return read4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2, oem_rw_filed_2);
}

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t inkWriteTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_2) {
    return write4Bytes(cardId, HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2, oem_rw_filed_2);
}

////////////////////////////////// TAG 13 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by HP or Ink Filler
// HP_SMART_CARD_INK_EXT_OEM_ID_TAG                    = 0x0D,

// Read HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the OEM ID memory partition (Partition #7)
//    0 = OEM ID partition unlocked
//    1 = OEM ID partition locked
HP_SMART_CARD_result_t inkReadTag13UD7Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud7_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD7_FUSE, ud7_fuse);
}

// Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
HP_SMART_CARD_result_t inkReadTag13ExtendedOEMID(HP_SMART_CARD_device_id_t cardId, uint8_t *extended_oem_id) {
    return readByte(cardId, HP_SMART_CARD_INK_EXTENDED_OEM_ID, extended_oem_id);
}

// Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
HP_SMART_CARD_result_t inkReadTag13OEMInkDesignator(HP_SMART_CARD_device_id_t cardId, uint8_t *oem_ink_designator) {
    return readByte(cardId, HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR, oem_ink_designator);
}

// Skipped (Tag 11 - Blank - 2, 5, 22)

////////////////////////////////// TAG 14 ////////////////////////////////////////////////
// Ext OEM ID, 12 bytes, Managed by Ink Filler
// HP_SMART_CARD_INK_REORDER_PART_NUM_TAG              = 0x0E,

// Read HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
// The region where this cartridge will be accepted by the printer.
//    0 = unknown
//    1 = universal
//    255 = test
HP_SMART_CARD_result_t inkReadTag14RegionalizationID(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_regionalization_id) {
    return readByte(cardId, HP_SMART_CARD_INK_REGIONALIZATION_ID, ink_regionalization_id);
}

// Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
HP_SMART_CARD_result_t inkReadTag14ReorderPartNum(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1, 12, buffer);
}

// Read HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)
// Bit indicating lock/unlock state of the Re-order part number memory partition (Partition #8)
//    0 = Re-order partition unlocked
//    1 = Re-order partition locked
HP_SMART_CARD_result_t inkReadTag14UD8Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud8_fuse) {
    return readByte(cardId, HP_SMART_CARD_INK_UD8_FUSE, ud8_fuse);
}

// Skipped (Tag 14 - Blank - 11, 6, 15)
// Skipped (Tag 14 - HP reserved - 13, 7, 224)
// Skipped (Tag 14 - Parity Byte - 41, 7, 8)
// Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)

char *inkFamilyGetFiledName(uint8_t id) {
    switch(id) {
        case HP_SMART_CARD_INK_UD0_FUSE:
            return "HP_SMART_CARD_INK_UD0_FUSE";
            break;
        case HP_SMART_CARD_INK_MANUFACTURE_ID:
            return "HP_SMART_CARD_INK_MANUFACTURE_ID";
            break;
        case HP_SMART_CARD_INK_MANUFACTURE_LINE:
            return "HP_SMART_CARD_INK_MANUFACTURE_LINE";
            break;
        case HP_SMART_CARD_INK_MANUFACTURE_YEAR:
            return "HP_SMART_CARD_INK_MANUFACTURE_YEAR";
            break;
        case HP_SMART_CARD_INK_MANUFACTURE_WEEK:
            return "HP_SMART_CARD_INK_MANUFACTURE_WEEK";
            break;
        case HP_SMART_CARD_INK_SERIAL_NUM_DOW:
            return "HP_SMART_CARD_INK_SERIAL_NUM_DOW";
            break;
        case HP_SMART_CARD_INK_SERIAL_NUM_HOD:
            return "HP_SMART_CARD_INK_SERIAL_NUM_HOD";
            break;
        case HP_SMART_CARD_INK_SERIAL_NUM_MOH:
            return "HP_SMART_CARD_INK_SERIAL_NUM_MOH";
            break;
        case HP_SMART_CARD_INK_SERIAL_NUM_SOM:
            return "HP_SMART_CARD_INK_SERIAL_NUM_SOM";
            break;
        case HP_SMART_CARD_INK_DRY_PROCESS_POSITION:
            return "HP_SMART_CARD_INK_DRY_PROCESS_POSITION";
            break;
        case HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME:
            return "HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME";
            break;
        case HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION:
            return "HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION";
            break;
        case HP_SMART_CARD_INK_TSR:
            return "HP_SMART_CARD_INK_TSR";
            break;
        case HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT:
            return "HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT";
            break;
        case HP_SMART_CARD_INK_BULK_CTRDG:
            return "HP_SMART_CARD_INK_BULK_CTRDG";
            break;
        case HP_SMART_CARD_INK_UD1_FUSE:
            return "HP_SMART_CARD_INK_UD1_FUSE";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM";
            break;
        case  HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION:
            return "HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION";
            break;
        case HP_SMART_CARD_INK_FORMULATOR_ID:
            return "HP_SMART_CARD_INK_FORMULATOR_ID";
            break;
        case HP_SMART_CARD_INK_INK_FAMILY:
            return "HP_SMART_CARD_INK_INK_FAMILY";
            break;
        case HP_SMART_CARD_INK_COLOR_CODES_GENERAL:
            return "HP_SMART_CARD_INK_COLOR_CODES_GENERAL";
            break;
        case HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC:
            return "HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC";
            break;
        case HP_SMART_CARD_INK_INK_FAMILY_MEMBER:
            return "HP_SMART_CARD_INK_INK_FAMILY_MEMBER";
            break;
        case HP_SMART_CARD_INK_INK_ID_NUM:
            return "HP_SMART_CARD_INK_INK_ID_NUM";
            break;
        case HP_SMART_CARD_INK_INK_REVISION:
            return "HP_SMART_CARD_INK_INK_REVISION";
            break;
        case HP_SMART_CARD_INK_DENSITY:
            return "HP_SMART_CARD_INK_DENSITY";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION:
            return "HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION";
            break;
        case HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR:
            return "HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR";
            break;
        case HP_SMART_CARD_INK_SHELF_LIFE_WEEKS:
            return "HP_SMART_CARD_INK_SHELF_LIFE_WEEKS";
            break;
        case HP_SMART_CARD_INK_SHELF_LIFE_DAYS:
            return "HP_SMART_CARD_INK_SHELF_LIFE_DAYS";
            break;
        case HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS:
            return "HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS";
            break;
        case HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS:
            return "HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS";
            break;
        case HP_SMART_CARD_INK_USABLE_INK_WEIGHT:
            return "HP_SMART_CARD_INK_USABLE_INK_WEIGHT";
            break;
        case HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL:
            return "HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL";
            break;
        case HP_SMART_CARD_INK_FIRING_FREQ:
            return "HP_SMART_CARD_INK_FIRING_FREQ";
            break;
        case HP_SMART_CARD_INK_PULSE_WIDTH:
            return "HP_SMART_CARD_INK_PULSE_WIDTH";
            break;
        case HP_SMART_CARD_INK_FIRING_VOLTAGE:
            return "HP_SMART_CARD_INK_FIRING_VOLTAGE";
            break;
        case HP_SMART_CARD_INK_TURN_ON_ENERGY:
            return "HP_SMART_CARD_INK_TURN_ON_ENERGY";
            break;
        case HP_SMART_CARD_INK_PULSE_WARMING_TEMP:
            return "HP_SMART_CARD_INK_PULSE_WARMING_TEMP";
            break;
        case HP_SMART_CARD_INK_MAX_TEMP:
            return "HP_SMART_CARD_INK_MAX_TEMP";
            break;
        case HP_SMART_CARD_INK_DROP_VOLUME:
            return "HP_SMART_CARD_INK_DROP_VOLUME";
            break;
        case HP_SMART_CARD_INK_WRITE_PROTECT:
            return "HP_SMART_CARD_INK_WRITE_PROTECT";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_2:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_3:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_4:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_5:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_6:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_7:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_8:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_9:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_10:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_11:
        case HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_12:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION";
            break;
        case HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION:
            return "HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION";
            break;
        case HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT:
            return "HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT";
            break;
        case HP_SMART_CARD_INK_FIRST_INSTALL_YEAR:
            return "HP_SMART_CARD_INK_FIRST_INSTALL_YEAR";
            break;
        case HP_SMART_CARD_INK_FIRST_INSTALL_WOY:
            return "HP_SMART_CARD_INK_FIRST_INSTALL_WOY";
            break;
        case HP_SMART_CARD_INK_FIRST_INSTALL_DOW:
            return "HP_SMART_CARD_INK_FIRST_INSTALL_DOW";
            break;
        case HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION:
            return "HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION";
            break;
        case HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD:
            return "HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD";
            break;
        case HP_SMART_CARD_INK_UD3_FUSE:
            return "HP_SMART_CARD_INK_UD3_FUSE";
            break;
        case HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1:
            return "HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1";
            break;
        case HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2:
            return "HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2";
            break;
        case HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1:
        case HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_2:
        case HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_3:
        case HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_4:
        case HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_5:
            return "HP_SMART_CARD_INK_HP_TRADEMARK_CHAR";
            break;
        case HP_SMART_CARD_INK_UD4_FUSE:
            return "HP_SMART_CARD_INK_UD4_FUSE";
            break;
        case HP_SMART_CARD_INK_OUT_OF_INK_BIT:
            return "HP_SMART_CARD_INK_OUT_OF_INK_BIT";
            break;
        case HP_SMART_CARD_INK_ILG_BIT_01_25:
            return "HP_SMART_CARD_INK_ILG_BIT_01_25";
            break;
        case HP_SMART_CARD_INK_ILG_BIT_26_50:
            return "HP_SMART_CARD_INK_ILG_BIT_26_50";
            break;
        case HP_SMART_CARD_INK_ILG_BIT_51_75:
            return "HP_SMART_CARD_INK_ILG_BIT_51_75";
            break;
        case HP_SMART_CARD_INK_ILG_BIT_76_100:
            return "HP_SMART_CARD_INK_ILG_BIT_76_100";
            break;
        case HP_SMART_CARD_INK_TUG_BIT_01_25:
            return "HP_SMART_CARD_INK_TUG_BIT_01_25";
            break;
        case HP_SMART_CARD_INK_TUG_BIT_26_50:
            return "HP_SMART_CARD_INK_TUG_BIT_26_50";
            break;
        case HP_SMART_CARD_INK_TUG_BIT_51_75:
            return "HP_SMART_CARD_INK_TUG_BIT_51_75";
            break;
        case HP_SMART_CARD_INK_TUG_BIT_76_100:
            return "HP_SMART_CARD_INK_TUG_BIT_76_100";
            break;
        case HP_SMART_CARD_INK_FIRST_FAILURE_CODE:
            return "HP_SMART_CARD_INK_FIRST_FAILURE_CODE";
            break;
        case HP_SMART_CARD_INK_ALTERED_SUPPLY:
            return "HP_SMART_CARD_INK_ALTERED_SUPPLY";
            break;
        case HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK:
            return "HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK";
            break;
        case HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK:
            return "HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK";
            break;
        case HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY:
            return "HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY";
            break;
        case HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1:
            return "HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1";
            break;
        case HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2:
            return "HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR:
            return "HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY:
            return "HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY";
            break;
        case HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW:
            return "HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_2:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_3:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_4:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_5:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_6:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_7:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_8:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_9:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_10:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_11:
        case HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_12:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR";
            break;
        case HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR:
            return "HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR";
            break;
        case HP_SMART_CARD_INK_INSERTION_COUNT:
            return "HP_SMART_CARD_INK_INSERTION_COUNT";
            break;
        case HP_SMART_CARD_INK_STALL_INSERTION_COUNT:
            return "HP_SMART_CARD_INK_STALL_INSERTION_COUNT";
            break;
        case HP_SMART_CARD_INK_LAST_FAILURE_CODE:
            return "HP_SMART_CARD_INK_LAST_FAILURE_CODE";
            break;
        case HP_SMART_CARD_INK_USER_REPORTED_STATUS:
            return "HP_SMART_CARD_INK_USER_REPORTED_STATUS";
            break;
        case HP_SMART_CARD_INK_MARKETING_DATA_REVISION:
            return "HP_SMART_CARD_INK_MARKETING_DATA_REVISION";
            break;
        case HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1:
            return "HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1";
            break;
        case HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2:
            return "HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2";
            break;
        case HP_SMART_CARD_INK_UD7_FUSE:
            return "HP_SMART_CARD_INK_UD7_FUSE";
            break;
        case HP_SMART_CARD_INK_EXTENDED_OEM_ID:
            return "HP_SMART_CARD_INK_EXTENDED_OEM_ID";
            break;
        case HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR:
            return "HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR";
            break;
        case HP_SMART_CARD_INK_REGIONALIZATION_ID:
            return "HP_SMART_CARD_INK_REGIONALIZATION_ID";
            break;
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_2:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_3:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_4:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_5:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_6:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_7:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_8:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_9:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_10:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_11:
        case HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_12:
            return "HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR";
            break;
        case HP_SMART_CARD_INK_UD8_FUSE:
            return "HP_SMART_CARD_INK_UD8_FUSE";
            break;
        default:
            return tag0GetFieldName(id);
    }
}