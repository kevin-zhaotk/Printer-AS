//
// Created by hmwan on 2020/4/26.
//

#include "../common_log.h"
#include "sc_supply_mem_access.h"
#include "sc_common_mem_access.h"

////////////////////////////////// TAG 1 ////////////////////////////////////////////////
// Dry Cartridge Manufacturing Data, 13 bytes(PDF document might be wrong as 12), Managed by HP
// HPSCS_DRY_MFG_TAG            = 0x01,

// Read HPSCS_EXT_OEM_ID (Tag 1 - Extended OEM ID - 1, 7, 8)
HP_SMART_CARD_result_t supplyReadTag1ExtendedOEMID(HP_SMART_CARD_device_id_t cardId, uint8_t *extended_oem_id) {
    return readByte(cardId, HPSCS_EXT_OEM_ID, extended_oem_id);
}

// Read HPSCS_HP_INK (Tag 1 - HP / OEM Ink Designator - 2, 7, 1)
HP_SMART_CARD_result_t supplyReadTag1OEMInkDesignator(HP_SMART_CARD_device_id_t cardId, uint8_t *designator) {
    return readByte(cardId, HPSCS_HP_INK, designator);
}

// Read HPSCS_SN_MFG_SITE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 6, 6)
HP_SMART_CARD_result_t supplyReadTag1ManufactureID(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_id) {
    return readByte(cardId, HPSCS_SN_MFG_SITE, manufacture_id);
}

// Read HPSCS_SN_MFG_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 2, 0, 5)
HP_SMART_CARD_result_t supplyReadTag1ManufactureLine(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_line) {
    return readByte(cardId, HPSCS_SN_MFG_LINE, manufacture_line);
}

// Read HPSCS_SN_MFG_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 3, 5)
// Year the dry cartridge was manufactured, offset from 2006.
//    m = 1, b = 2006, range = 2006 - 2037, in the equation = mx + b where the y slope equals 1, and the intercept is 2006.
HP_SMART_CARD_result_t supplyReadTag1ManufactureYear(HP_SMART_CARD_device_id_t cardId, uint16_t *manufacture_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_SN_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *manufacture_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_SN_MFG_YEAR = %d", *manufacture_year);
    }
    return result;
}

// Read HPSCS_SN_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 6, 6)
HP_SMART_CARD_result_t supplyReadTag1ManufactureWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_woy) {
    return readByte(cardId, HPSCS_SN_WEEK, manufacture_woy);
}

// Read HPSCS_SN_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 4, 0, 3)
HP_SMART_CARD_result_t supplyReadTag1ManufactureDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_dow) {
    return readByte(cardId, HPSCS_SN_DOW, manufacture_dow);
}

// Read HPSCS_SN_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 5, 5)
HP_SMART_CARD_result_t supplyReadTag1ManufactureHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_hod) {
    return readByte(cardId, HPSCS_SN_HOD, manufacture_hod);
}

// Read HPSCS_SN_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 5, 0, 6)
HP_SMART_CARD_result_t supplyReadTag1ManufactureMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_moh) {
    return readByte(cardId, HPSCS_SN_MOH, manufacture_moh);
}

// Read HPSCS_SN_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 2, 6)
HP_SMART_CARD_result_t supplyReadTag1ManufactureSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_som) {
    return readByte(cardId, HPSCS_SN_SOM, manufacture_som);
}

// Read HPSCS_SN_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 4, 3)
HP_SMART_CARD_result_t supplyReadTag1DryProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *process_position) {
    return readByte(cardId, HPSCS_SN_DRY_PROCESS_POSITION, process_position);
}

// Read HPSCS_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 1, 16)
// Maximum usable cartridge volume
//    m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag1MaxUsableCartridgeVolume(HP_SMART_CARD_device_id_t cardId, float *max_usable_cartridge_volume) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HPSCS_MAX_USABLE_CARTRIDGE_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *max_usable_cartridge_volume = buf * 0.1 + 0;
        LOGD(">>> HPSCS_MAX_USABLE_CARTRIDGE_VOLUME = %f", *max_usable_cartridge_volume);
    }
    return result;
}

// Read HPSCS_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Partition number of the printer RW2RO memory space. Used for capturing 1st install data - 9, 1, 5)
HP_SMART_CARD_result_t supplyReadTag1PrinterLockdownPartition(HP_SMART_CARD_device_id_t cardId, uint8_t *lock_down_partition ) {
    return readByte(cardId, HPSCS_PRINTER_LOCK_DOWN_PARTITION, lock_down_partition);
}

// Skipped (Tag 1 - Blank - 10, 4, 21)

////////////////////////////////// TAG 2 ////////////////////////////////////////////////
// Dry Cartridge Specific Data, 17 bytes(PDF document might be wrong as 16), Managed by HP
// HPSCS_DRY_TAG       = 0x02,

// Read HPSCS_PILS_MFG_YEAR (Tag 2 - PILS manufacture year - 1, 7, 5)
// The year the PILS was manufactured, offset from 2006.
//    m = 1, b = 2006, range = 2006 - 2037, in the equation = mx + b where the y slope equals 1, and the intercept is 2006.
HP_SMART_CARD_result_t supplyReadTag2PILSManufactureYear(HP_SMART_CARD_device_id_t cardId, uint16_t *manufacture_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *manufacture_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_PILS_MFG_YEAR = %d", *manufacture_year);
    }
    return result;
}

// Read HPSCS_PILS_MFG_WEEK (Tag 2 - PILS manufacture week - 1, 2, 6)
HP_SMART_CARD_result_t supplyReadTag2PILSManufactureWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_dow) {
    return readByte(cardId, HPSCS_PILS_MFG_WEEK, manufacture_dow);
}

// Read HPSCS_PILS_MFG_ID (Tag 2 - PILS Manufacture ID - 2, 4, 4)
HP_SMART_CARD_result_t supplyReadTag2PILSManufactureID(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_id) {
    return readByte(cardId, HPSCS_PILS_MFG_ID, manufacture_id);
}

// Read HPSCS_PILS_MFG_LINE (Tag 2 - PILS Manufacture Line - 2, 0, 4)
HP_SMART_CARD_result_t supplyReadTag2PILSManufactureLine(HP_SMART_CARD_device_id_t cardId, uint8_t *manufacture_line) {
    return readByte(cardId, HPSCS_PILS_MFG_LINE, manufacture_line);
}

// Read HPSCS_PILS_SENSOR_GAIN (Tag 2 - PILS Manufacture Sensor Gain - 3, 4, 8)
// The unique sensor gain vlue in mV/PSI
//    m = 0.1 b = 12 range = 12 – 37.5 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSSensorGain(HP_SMART_CARD_device_id_t cardId, float *sensor_gain) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_SENSOR_GAIN, &buf);

    if (HP_SMART_CARD_OK == result) {
        *sensor_gain = buf * 0.1 + 12;
        LOGD(">>> HPSCS_PILS_SENSOR_GAIN = %f", *sensor_gain);
    }
    return result;
}

// Read HPSCS_PILS_PHASE3_AVAIL (Tag 2 - PILS Phase 3 Available - 4, 4, 8)
// The amount of ink that the printer should deliver (in ml) during phase 3 drop counting.
//    m = 0.25 b = 0 range = 0 – 63.75 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPhase3Available(HP_SMART_CARD_device_id_t cardId, float *available) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PHASE3_AVAIL, &buf);

    if (HP_SMART_CARD_OK == result) {
        *available = buf * 0.25 + 0;
        LOGD(">>> HPSCS_PILS_PHASE3_AVAIL = %f", *available);
    }
    return result;
}

// Read HPSCS_PILS_PT1_ABS_VOLUME (Tag 2 - PILS Point 1 Absolute Volume - 5, 4, 8)
// The volume point (in ml) that begins phase 2.
//    m = 2 b = 0 range = 0 – 510 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint1AbsVolume(HP_SMART_CARD_device_id_t cardId, uint16_t *volume) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT1_ABS_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *volume = buf * 2 + 0;
        LOGD(">>> HPSCS_PILS_PT1_ABS_VOLUME = %d", *volume);
    }
    return result;
}

// Read HPSCS_PILS_PT2_ABS_VOLUME (Tag 2 - PILS Point 2 Absolute Volume - 6, 4, 8)
// The volume point (in ml) in phase 2 where the ink level calculation changes from straight-line estimating to curve-fitting estimating
//    m = 2 b = 0 range = 0 – 510 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint2AbsVolume(HP_SMART_CARD_device_id_t cardId, uint16_t *volume) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT2_ABS_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *volume = buf * 2 + 0;
        LOGD(">>> HPSCS_PILS_PT2_ABS_VOLUME = %d", *volume);
    }
    return result;
}

// Read HPSCS_PILS_PT3_ABS_VOLUME (Tag 2 - PILS Phase 3 Available - 7, 4, 7)
// This is the maximum ink volume (in ml) that could be extracted in phase 3, if margins were ignored.
//    m = 0.5 b = 0 range = 0 – 63.5 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint3AbsVolume(HP_SMART_CARD_device_id_t cardId, float *volume) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT3_ABS_VOLUME, &buf);

    if (HP_SMART_CARD_OK == result) {
        *volume = buf * 0.5 + 0;
        LOGD(">>> HPSCS_PILS_PT3_ABS_VOLUME = %f", *volume);
    }
    return result;
}

// Read HPSCS_PILS_PT1_PRESSURE (Tag 2 - PILS Point 1 Pressure - 8, 5, 7)
// Pressure point (in PSI) that begins phase 2. Drop counting (phase 1) should continue until PILS point 1 pressure is detected.
//    m = 0.005 b = 0 range = 0 – 0.635 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint1Pressure(HP_SMART_CARD_device_id_t cardId, float *pressure) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT1_PRESSURE, &buf);

    if (HP_SMART_CARD_OK == result) {
        *pressure = buf * 0.005 + 0;
        LOGD(">>> HPSCS_PILS_PT1_PRESSURE = %f", *pressure);
    }
    return result;
}

// Read HPSCS_PILS_PT2_PRESSURE (Tag 2 - PILS Point 2 Pressure - 9, 6, 7)
// The pressure point in phase 2 (in PSI) where the ink level calculation changes from straight-line estimating to curve-fitting estimating.
//    m = 0.005 b = 0 range = 0 – 0.635 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint2Pressure(HP_SMART_CARD_device_id_t cardId, float *pressure) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT2_PRESSURE, &buf);

    if (HP_SMART_CARD_OK == result) {
        *pressure = buf * 0.005 + 0;
        LOGD(">>> HPSCS_PILS_PT2_PRESSURE = %f", *pressure);
    }
    return result;
}

// Read HPSCS_PILS_PT3_PRESSURE (Tag 2 - PILS Point 3 Pressure - 10, 7, 7)
// This is the pressure point (in PSI) that begins phase 3.
//    m = 0.005 b = 0 range = 0 – 0.635 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint3Pressure(HP_SMART_CARD_device_id_t cardId, float *pressure) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_PT3_PRESSURE, &buf);

    if (HP_SMART_CARD_OK == result) {
        *pressure = buf * 0.005 + 0;
        LOGD(">>> HPSCS_PILS_PT3_PRESSURE = %f", *pressure);
    }
    return result;
}

// Read HPSCS_PILS_2_MULTIPLIER (Tag 2 - PILS Point 2 Multiplier - 10, 0, 7)
// This curve fit coefficient used during phase 2, following point 2 on the PILS curve.
//    m = 0.2 b = 10 range = 10 – 35.4 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSPoint2Multiplier(HP_SMART_CARD_device_id_t cardId, float *multiplier) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_2_MULTIPLIER, &buf);

    if (HP_SMART_CARD_OK == result) {
        *multiplier = buf * 0.2 + 10;
        LOGD(">>> HPSCS_PILS_2_MULTIPLIER = %f", *multiplier);
    }
    return result;
}

// Read HPSCS_PILS_2_SUBTRACTOR (Tag 2 - PILS Point 2 Subtractor - 11, 1, 6)
HP_SMART_CARD_result_t supplyReadTag2PILSPoint2Subtractor(HP_SMART_CARD_device_id_t cardId, uint8_t *subtractor) {
    return readByte(cardId, HPSCS_PILS_2_SUBTRACTOR, subtractor);
}

// Read HPSCS_PILS_ORIENTATION (Tag 2 - PILS Orientation - 12, 3, 3)
HP_SMART_CARD_result_t supplyReadTag2PILSOrientation(HP_SMART_CARD_device_id_t cardId, uint8_t *orientation) {
    return readByte(cardId, HPSCS_PILS_ORIENTATION, orientation);
}

// Read HPSCS_PILS_TEMP_COEFFICIENT (Tag 2 - PILS Tempco - 12, 0, 6)
// Temperature coefficient for the pressure sensor.
//    m = 0.01 b = -0.32 range = -0.32 – 0.31 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSTempco(HP_SMART_CARD_device_id_t cardId, float *tempco) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_TEMP_COEFFICIENT, &buf);

    if (HP_SMART_CARD_OK == result) {
        *tempco = buf * 0.01 - 0.32;
        LOGD(">>> HPSCS_PILS_TEMP_COEFFICIENT = %f", *tempco);
    }
    return result;
}

// Read HPSCS_PILS_REFILL_EMPTY_LIMIT (Tag 2 - PILS Refill Empty Limit - 13, 2, 4)
// A field used only in non-HP ink mode to determine when the refilled cartridge is empty.
//    m = 3 b = 10 range = 10 – 55 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag2PILSRefillEmptyLimit(HP_SMART_CARD_device_id_t cardId, uint8_t *limit) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_PILS_REFILL_EMPTY_LIMIT, &buf);

    if (HP_SMART_CARD_OK == result) {
        *limit = buf * 3 + 10;
        LOGD(">>> HPSCS_PILS_REFILL_EMPTY_LIMIT = %d", *limit);
    }
    return result;
}

// Skipped (Tag 2 - Blank - 14, 6, 23)

////////////////////////////////// TAG 3 ////////////////////////////////////////////////
// Ink Fill Manufacturing Data, 21 bytes, Managed by Ink Filter
// HPSCS_INK_MFG_TAG             = 0x03,

// Read HPSCS_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1) - (PDF Document written UD0_FUSE)
// state of the Ink Fill memory partition (Partition #1)
//    0 = Ink fill partition unlocked
//    1 = Ink fill partition locked
HP_SMART_CARD_result_t supplyReadTag3UD1Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud1_fuse) {
    return readByte(cardId, HPSCS_UD1_FUSE, ud1_fuse);
}

// Read HPSCS_FILL_SITE (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillSiteID(HP_SMART_CARD_device_id_t cardId, uint8_t *site_id) {
    return readByte(cardId, HPSCS_FILL_SITE, site_id);
}

// Read HPSCS_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillLine(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_line) {
    return readByte(cardId, HPSCS_FILL_LINE, fill_line);
}

// Read HPSCS_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
// Year that cartridge was filled.
//      m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillYear(HP_SMART_CARD_device_id_t cardId, uint16_t *fill_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_FILL_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *fill_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_FILL_YEAR = %d", *fill_year);
    }
    return result;
}

// Read HPSCS_FILL_WEEK (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_woy) {
    return readByte(cardId, HPSCS_FILL_WEEK, fill_woy);
}

// Read HPSCS_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_dow) {
    return readByte(cardId, HPSCS_FILL_DOW, fill_dow);
}

// Read HPSCS_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillHourOfDay(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_hod) {
    return readByte(cardId, HPSCS_FILL_HOD, fill_hod);
}

// Read HPSCS_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillMinuteOfHour(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_moh) {
    return readByte(cardId, HPSCS_FILL_MOH, fill_moh);
}

// Read HPSCS_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillSecondOfMinute(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_som) {
    return readByte(cardId, HPSCS_FILL_SOM, fill_som);
}

// Read HPSCS_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
HP_SMART_CARD_result_t supplyReadTag3CartridgeFillProcessPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *fill_process_position) {
    return readByte(cardId, HPSCS_FILL_PROCESS_POSITION, fill_process_position);
}

// Read HPSCS_REGION_ID (Tag 3 - Regionalization ID - 7, 7, 8)
// The region where this cartridge will be accepted by the printer.
//    0 = unknown
//    1 = universal
//    255 = test
HP_SMART_CARD_result_t supplyReadTag3RegionalizationID(HP_SMART_CARD_device_id_t cardId, uint8_t *regionalization_id) {
    return readByte(cardId, HPSCS_REGION_ID, regionalization_id);
}

// Read HPSCS_REORDER_PN_CHAR (Tag 3 - Cartridge Reorder P/ N - 12 Char - 8, 7, 72)
HP_SMART_CARD_result_t supplyReadTag3ReorderPartNum(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HPSCS_REORDER_PN_CHAR_1, 12, buffer);
}

// Skipped (Tag 3 - Blank - 17, 7, 20)
// Skipped (Tag 3 - Blank - 19, 3, 12)

////////////////////////////////// TAG 4 ////////////////////////////////////////////////
// Ink Fill Data & Properties, 17 bytes, Managed by Ink Filter
// HPSCS_INK_TAG       = 0x04,

// Read HPSCS_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
HP_SMART_CARD_result_t supplyReadTag4FormulatorID(HP_SMART_CARD_device_id_t cardId, uint8_t *formulator_id) {
    return readByte(cardId, HPSCS_INK_FORMULATOR_ID, formulator_id);
}

// Read HPSCS_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
HP_SMART_CARD_result_t supplyReadTag4InkFamily(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family) {
    return readByte(cardId, HPSCS_INK_FAMILY, ink_family);
}

// Read HPSCS_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
HP_SMART_CARD_result_t supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_general) {
    return readByte(cardId, HPSCS_INK_COLOR_CODES_GENERAL, ink_color_code_general);
}

// Read HPSCS_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
HP_SMART_CARD_result_t supplyReadTag4ColorCodesSpecific(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_color_code_specific) {
    return readByte(cardId, HPSCS_INK_COLOR_CODES_SPECIFIC, ink_color_code_specific);
}

// Read HPSCS_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
HP_SMART_CARD_result_t supplyReadTag4InkFamilyMember(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_family_member) {
    return readByte(cardId, HPSCS_INK_FAMILY_MEMBER, ink_family_member);
}

// Read HPSCS_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
HP_SMART_CARD_result_t supplyReadTag4InkIDNumber(HP_SMART_CARD_device_id_t cardId, uint16_t *ink_id_num) {
    return read2Bytes(cardId, HPSCS_INK_ID_NUM, ink_id_num);
}

// Read HPSCS_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
HP_SMART_CARD_result_t supplyReadTag4InkRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_revision) {
    return readByte(cardId, HPSCS_INK_REVISION, ink_revision);
}

// Read HPSCS_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
// Specifies the ink density.
//      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag4Density(HP_SMART_CARD_device_id_t cardId, float *ink_density) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HPSCS_INK_DENSITY, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_density = 0.001 * buf + 0.7;
        LOGD(">>> HPSCS_INK_DENSITY = %f", *ink_density);
    }
    return result;
}

// Read HPSCS_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
HP_SMART_CARD_result_t supplyReadTag4CartridgeDistinction(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_cartridge_distinction) {
    return readByte(cardId, HPSCS_INK_CARTRIDGE_DISTINCTION, ink_cartridge_distinction);
}

// Read HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
HP_SMART_CARD_result_t supplyReadTag4SupplyKeyingDescriptor(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_supply_keying_descriptor) {
    return readByte(cardId, HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR, ink_supply_keying_descriptor);
}

// Read HPSCS_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
// ??????????? 不明白
//    Shelf Weeks + Shelf Days + Ink filled time / date <= current date / time
HP_SMART_CARD_result_t supplyReadTag4ShelfLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_weeks) {
    return readByte(cardId, HPSCS_INK_SHELF_LIFE_WEEKS, ink_shelf_life_weeks);
}

// Read HPSCS_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
HP_SMART_CARD_result_t supplyReadTag4ShelfLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_shelf_life_days) {
    return readByte(cardId, HPSCS_INK_SHELF_LIFE_DAYS, ink_shelf_life_days);
}

// Read HPSCS_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
// ??????????? 不明白
//    Installed Weeks + Installed Days + First installed time / date <= current date / time
HP_SMART_CARD_result_t supplyReadTag4InstalledLifeWeeks(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_weeks) {
    return readByte(cardId, HPSCS_INK_INSTALLED_LIFE_WEEKS, ink_installed_life_weeks);
}

// Read HPSCS_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
HP_SMART_CARD_result_t supplyReadTag4InstalledLifeDays(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_installed_life_days) {
    return readByte(cardId, HPSCS_INK_INSTALLED_LIFE_DAYS, ink_installed_life_days);
}

// Read HPSCS_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
// m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag4UsableInkWeight(HP_SMART_CARD_device_id_t cardId, float *ink_usable_weigh) {
    uint16_t buf;
    HP_SMART_CARD_result_t result = read2Bytes(cardId, HPSCS_INK_USABLE_INK_WEIGHT, &buf);

    if (HP_SMART_CARD_OK == result) {
        *ink_usable_weigh = buf * 0.1 + 0;
        LOGD(">>> HPSCS_INK_USABLE_INK_WEIGHT = %f", *ink_usable_weigh);
    }
    return result;
}

// Read HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
// Percentage Value: 10 means 10%
HP_SMART_CARD_result_t supplyReadTag4AlteredSupplyNotificationLevel(HP_SMART_CARD_device_id_t cardId, uint8_t *ink_notification_level) {
    return readByte(cardId, HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL, ink_notification_level);
}

// Skipped (Tag 4 - Blank - 15, 4, 15)

////////////////////////////////// TAG 5 ////////////////////////////////////////////////
// Filled Cartridge Specific Data, 13 bytes, Managed by Ink Filter
// HPSCS_FILLED_CTRDG_SPECIFIC_TAG    = 0x05,

// Read HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1 (Tag 5 - Blank - 1, 7, 32)
HP_SMART_CARD_result_t supplyReadTag5FilledCartridgeSpecificData1(HP_SMART_CARD_device_id_t cardId, uint32_t *data1) {
    return read4Bytes(cardId, HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1, data1);
}

// Read HPSCS_FILLED_CTRDG_SPECIFIC_DATA_2 (Tag 5 - Blank - 5, 7, 32)
HP_SMART_CARD_result_t supplyReadTag5FilledCartridgeSpecificData2(HP_SMART_CARD_device_id_t cardId, uint32_t *data2) {
    return read4Bytes(cardId, HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1, data2);
}

// Read HPSCS_FILLED_CTRDG_SPECIFIC_DATA_3 (Tag 5 - Blank - 9, 7, 32)
HP_SMART_CARD_result_t supplyReadTag5FilledCartridgeSpecificData3(HP_SMART_CARD_device_id_t cardId, uint32_t *data3) {
    return read4Bytes(cardId, HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1, data3);
}

////////////////////////////////// TAG 6 ////////////////////////////////////////////////
// Cartridge First Install Data, 21 bytes, Managed by Platform
// HPSCS_FIRST_INSTALL_TAG    = 0x06,

// Read HPSCS_UD2_FUSE (Tag 6 - Write Protect Fuse (#2)- 1, 7, 1)
// Bit indicating lock/unlock state of the Printer 1st Install memory partition (Partition #2)
//    0 = OEM Lockable partition unlocked
//    1 = OEM Lockable partition locked
HP_SMART_CARD_result_t supplyReadTag6UD2Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *write_protect) {
    return readByte(cardId, HPSCS_UD2_FUSE, write_protect);
}

// Write HPSCS_UD2_FUSE (Tag 6 - Write Protect Fuse - 1, 7, 1)
HP_SMART_CARD_result_t supplyWriteTag6UD2Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t write_protect) {
    return writeByte(cardId, HPSCS_UD2_FUSE, write_protect);
}

// Read HPSCS_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t supplyReadTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HPSCS_FIRST_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Write HPSCS_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HPSCS_FIRST_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Read HPSCS_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
// The first install printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mfg_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_FIRST_PLATFORM_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *mfg_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_FIRST_PLATFORM_MFG_YEAR = %d", *mfg_year);
    }
    return result;
}

// Write HPSCS_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mfg_year) {
    uint8_t buffer = mfg_year - 2006;
    return writeByte(cardId, HPSCS_FIRST_PLATFORM_MFG_YEAR, buffer);
}

// Read HPSCS_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t supplyReadTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_woy) {
    return readByte(cardId, HPSCS_FIRST_PLATFORM_MFG_WOY, mfg_woy);
}

// Write HPSCS_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_woy) {
    return writeByte(cardId, HPSCS_FIRST_PLATFORM_MFG_WOY, mfg_woy);
}

// Read HPSCS_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t supplyReadTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mfg_country) {
    return readByte(cardId, HPSCS_FIRST_PLATFORM_MFG_COUNTRY, mfg_country);
}

// Write HPSCS_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_country) {
    return writeByte(cardId, HPSCS_FIRST_PLATFORM_MFG_COUNTRY, mfg_country);
}

// Read HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t supplyReadTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_major) {
    return readByte(cardId, HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION, fw_revision_major);
}

// Write HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_major) {
    return writeByte(cardId, HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION, fw_revision_major);
}

// Read HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t supplyReadTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *fw_revision_minor) {
    return readByte(cardId, HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION, fw_revision_minor);
}

// Write HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t supplyWriteTag61stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_minor) {
    return writeByte(cardId, HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION, fw_revision_minor);
}

// Read HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t supplyReadTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_count) {
    return read2Bytes(cardId, HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT, first_install_count);
}

// Write HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t supplyWriteTag61stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_count) {
    return write2Bytes(cardId, HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT, first_install_count);
}

// Read HPSCS_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
// The year of the first installation of this cartridge into this printer platform.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t *first_install_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_FIRST_INSTALL_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *first_install_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_FIRST_INSTALL_YEAR = %d", *first_install_year);
    }
    return result;
}

// Write HPSCS_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t supplyWriteTag61stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_year) {
    uint8_t buffer = first_install_year - 2006;
    return writeByte(cardId, HPSCS_FIRST_INSTALL_YEAR, buffer);
}

// Read HPSCS_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t supplyReadTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_woy) {
    return readByte(cardId, HPSCS_FIRST_INSTALL_WOY, first_install_woy);
}

// Write HPSCS_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t supplyWriteTag61stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_woy) {
    return writeByte(cardId, HPSCS_FIRST_INSTALL_WOY, first_install_woy);
}

// Read HPSCS_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t supplyReadTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *first_install_dow) {
    return readByte(cardId, HPSCS_FIRST_INSTALL_DOW, first_install_dow);
}

// Write HPSCS_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t supplyWriteTag61stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_dow) {
    return writeByte(cardId, HPSCS_FIRST_INSTALL_DOW, first_install_dow);
}

// Read HPSCS_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
// If this filed is programmed with a value of 50(decimal), then it means each bit represent 2% of in usage
HP_SMART_CARD_result_t supplyReadTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t *gauge_resolution) {
    return readByte(cardId, HPSCS_INK_LEVEL_GAUGE_RESOLUTION, gauge_resolution);
}

// Write HPSCS_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t supplyWriteTag6InkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t gauge_resolution) {
    return writeByte(cardId, HPSCS_INK_LEVEL_GAUGE_RESOLUTION, gauge_resolution);
}

// Skipped (Tag 6 - Blank - 20, 7, 29)

////////////////////////////////// TAG 7 ////////////////////////////////////////////////
// OEM Lockable memory, 9 bytes, Managed by OEM
// HPSCS_OEM_RW2RO_TAG           = 0x07,

// Read HPSCS_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the OEM Lockable memory partition (Partition #3)
//    0 = OEM Lockable partition unlocked
//    1 = OEM Lockable partition locked
HP_SMART_CARD_result_t supplyReadTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t *ud3_fuse) {
    return readByte(cardId, HPSCS_UD3_FUSE, ud3_fuse);
}

// Write HPSCS_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t supplyWriteTag7UD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud3_fuse) {
    return writeByte(cardId, HPSCS_UD3_FUSE, ud3_fuse);
}

// Read HPSCS_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t supplyReadTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_1) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_FIELD_1, oem_defined_filed_1);
}

// Write HPSCS_OEM_DEFINED_FIELD_1 (Tag 7 - OEM Defined Field 1 - 1, 6, 31)
HP_SMART_CARD_result_t supplyWriteTag7OEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_1) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_FIELD_1, oem_defined_filed_1);
}

// Read HPSCS_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t supplyReadTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_defined_filed_2) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_FIELD_2, oem_defined_filed_2);
}

// Write HPSCS_OEM_DEFINED_FIELD_2 (Tag 7 - OEM Defined Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t supplyWriteTag7OEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_2) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_FIELD_2, oem_defined_filed_2);
}

////////////////////////////////// TAG 8 ////////////////////////////////////////////////
// Trademark, 5 bytes(defined as 8 bytes in hp_smart_card_layout.h), Managed by HP or Ink Filler
// HPSCS_TM_TAG                  = 0x08,

// Read HPSCS_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t supplyReadTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HPSCS_HP_TRADEMARK_CHAR_1, 5, buffer);
}

// Write HPSCS_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t supplyWriteTag8HPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HPSCS_HP_TRADEMARK_CHAR_1, 5, buffer);
}

// Skipped (Tag 8 - Blank - 4, 1, 2)

////////////////////////////////// TAG 9 ////////////////////////////////////////////////
// OEM Lockable Memory, 33 bytes, Managed by OEM
// HPSCS_USE_TAG          = 0x09,

// Read HPSCS_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
// This bit is set by the printer once the ILG indicates out of ink condition.
//    0 = not reached,
//    1 = reached
HP_SMART_CARD_result_t supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t *out_of_ink) {
    return readByte(cardId, HPSCS_INK_OUT_OF_INK_BIT, out_of_ink);
}

// Write HPSCS_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t out_of_ink) {
    return writeByte(cardId, HPSCS_INK_OUT_OF_INK_BIT, out_of_ink);
}

// Read HPSCS_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
// Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_25_01) {
    return read4Bytes(cardId, HPSCS_INK_ILG_BIT_01_25, ilg_bit_25_01);
}

// Write HPSCS_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
HP_SMART_CARD_result_t supplyWriteTag9ILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_25_01) {
    return write4Bytes(cardId, HPSCS_INK_ILG_BIT_01_25, ilg_bit_25_01);
}

// Read HPSCS_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
// Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_50_26) {
    return read4Bytes(cardId, HPSCS_INK_ILG_BIT_26_50, ilg_bit_50_26);
}

// Write HPSCS_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
HP_SMART_CARD_result_t supplyWriteTag9ILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_50_26) {
    return write4Bytes(cardId, HPSCS_INK_ILG_BIT_26_50, ilg_bit_50_26);
}

// Read HPSCS_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
// Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_75_51) {
    return read4Bytes(cardId, HPSCS_INK_ILG_BIT_51_75, ilg_bit_75_51);
}

// Write HPSCS_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
HP_SMART_CARD_result_t supplyWriteTag9ILGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_75_51) {
    return write4Bytes(cardId, HPSCS_INK_ILG_BIT_51_75, ilg_bit_75_51);
}

// Read HPSCS_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
// Ink level gauge bits 76-100 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit_100_76) {
    return read4Bytes(cardId, HPSCS_INK_ILG_BIT_76_100, ilg_bit_100_76);
}

// Write HPSCS_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
HP_SMART_CARD_result_t supplyWriteTag9ILGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_100_76) {
    return write4Bytes(cardId, HPSCS_INK_ILG_BIT_76_100, ilg_bit_100_76);
}

// Read HPSCS_INK_TIUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
// Total Ink Usage Gauge bits 1 - 25 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_25_01) {
    return read4Bytes(cardId, HPSCS_INK_TIUG_BIT_01_25, tiug_bit_25_01);
}

// Write HPSCS_INK_TIUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
HP_SMART_CARD_result_t supplyWriteTag9TUGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_25_01) {
    return write4Bytes(cardId, HPSCS_INK_TIUG_BIT_01_25, tiug_bit_25_01);
}

// Read HPSCS_INK_TIUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
// Total Ink Usage Gauge bits 26 - 50 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_50_26) {
    return read4Bytes(cardId, HPSCS_INK_TIUG_BIT_26_50, tiug_bit_50_26);
}

// Write HPSCS_INK_TIUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
HP_SMART_CARD_result_t supplyWriteTag9TUGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_50_26) {
    return write4Bytes(cardId, HPSCS_INK_TIUG_BIT_26_50, tiug_bit_50_26);
}

// Read HPSCS_INK_TIUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
// Total Ink Usage Gauge bits 51 - 75 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_75_51) {
    return read4Bytes(cardId, HPSCS_INK_TIUG_BIT_51_75, tiug_bit_75_51);
}

// Write HPSCS_INK_TIUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
HP_SMART_CARD_result_t supplyWriteTag9TUGBit51To75(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_75_51) {
    return write4Bytes(cardId, HPSCS_INK_TIUG_BIT_51_75, tiug_bit_75_51);
}

// Read HPSCS_INK_TIUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
// Total Ink Usage Gauge bits 76 - 100 of a 100-bit field. This enables up to 1% resolution.
HP_SMART_CARD_result_t supplyReadTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t *tiug_bit_100_76) {
    return read4Bytes(cardId, HPSCS_INK_TIUG_BIT_76_100, tiug_bit_100_76);
}

// Write HPSCS_INK_TIUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
HP_SMART_CARD_result_t supplyWriteTag9TUGBit76To100(HP_SMART_CARD_device_id_t cardId, uint32_t tiug_bit_100_76) {
    return write4Bytes(cardId, HPSCS_INK_TIUG_BIT_76_100, tiug_bit_100_76);
}

// Read HPSCS_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
HP_SMART_CARD_result_t supplyReadTag91stFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *first_failure_code) {
    return readByte(cardId, HPSCS_INK_FIRST_FAILURE_CODE, first_failure_code);
}

// Read HPSCS_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
// This bit is set by the printer when the cartridge has positively exceeded available supply.
// This is set once the TIUG target has been reached.
//    0 = not reached,
//    1 = reached
HP_SMART_CARD_result_t supplyReadTag9AlteredSupply(HP_SMART_CARD_device_id_t cardId, uint8_t *altered_supply) {
    return readByte(cardId, HPSCS_INK_ALTERED_SUPPLY, altered_supply);
}

// Read HPSCS_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
HP_SMART_CARD_result_t supplyReadTag9UserAckNonOrgInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_altered_supply) {
    return readByte(cardId, HPSCS_INK_USER_ACK_NON_ORG_INK, user_ack_altered_supply);
}

// Read HPSCS_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
HP_SMART_CARD_result_t supplyReadTag9UserAckExpiredInk(HP_SMART_CARD_device_id_t cardId, uint8_t *user_ack_expired_ink) {
    return readByte(cardId, HPSCS_INK_USER_ACK_EXPIRED_INK, user_ack_expired_ink);
}

// Read HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
HP_SMART_CARD_result_t supplyReadTag9FaultyReplaceImmediately(HP_SMART_CARD_device_id_t cardId, uint8_t *faulty_replace_immediately) {
    return readByte(cardId, HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY, faulty_replace_immediately);
}

// Read HPSCS_DROP_COUNT_ONLY_ILS (Tag 9 - Drop Count only ILS - 27, 4, 1)
HP_SMART_CARD_result_t supplyReadTag9DropCountOnlyILS(HP_SMART_CARD_device_id_t cardId, uint8_t *dropcount) {
    return readByte(cardId, HPSCS_DROP_COUNT_ONLY_ILS, dropcount);
}

// Read HPSCS_PILS_FULL_VOLTAGE_COMPLETE (Tag 9 - PILS Full Voltage Complete - 27, 3, 1)
HP_SMART_CARD_result_t supplyReadTag9PILSFullVoltageComplete(HP_SMART_CARD_device_id_t cardId, uint8_t *voltage) {
    return readByte(cardId, HPSCS_PILS_FULL_VOLTAGE_COMPLETE, voltage);
}

// Read HPSCS_PILS_STATE (Tag 9 - PILS State - 27, 2, 2)
HP_SMART_CARD_result_t supplyReadTag9PILSState(HP_SMART_CARD_device_id_t cardId, uint8_t *state) {
    return readByte(cardId, HPSCS_PILS_STATE, state);
}

// Read HPSCS_PURGE_OCCURRED (Tag 9 - Purge Occurred - 27, 0, 1)
HP_SMART_CARD_result_t supplyReadTag9PurgeOccurred(HP_SMART_CARD_device_id_t cardId, uint8_t *occurred) {
    return readByte(cardId, HPSCS_PURGE_OCCURRED, occurred);
}

// Skipped (Tag 9 - Blank - 28, 7, 32)
// Skipped (Tag 9 - Blank - 32, 7, 8)

////////////////////////////////// TAG 10 ////////////////////////////////////////////////
// OEM RW_OR Memory, 9 bytes, Managed by OEM
// HPSCS_OEM_RW_OR_TAG              = 0x0A,

// Read HPSCS_OEM_DEFINED_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t supplyReadTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_1) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_RW_OR_FIELD_1, oem_rw_or_filed_1);
}

// Write HPSCS_OEM_DEFINED_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t supplyWriteTag10OEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_1) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_RW_OR_FIELD_1, oem_rw_or_filed_1);
}

// Read HPSCS_OEM_DEFINED_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t supplyReadTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_or_filed_2) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_RW_OR_FIELD_2, oem_rw_or_filed_2);
}

// Write HPSCS_OEM_DEFINED_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t supplyWriteTag10OEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_2) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_RW_OR_FIELD_2, oem_rw_or_filed_2);
}

////////////////////////////////// TAG 11 ////////////////////////////////////////////////
// HP Scratchpad, 33 bytes, Managed by
// HPSCS_HP_SCRATCH_TAG                = 0x0B,

// Read HPSCS_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
// Most recent use (MRU) of this cartridge - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
HP_SMART_CARD_result_t supplyReadTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t *cartridge_mru_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_CARTRIDGE_MRU_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *cartridge_mru_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_CARTRIDGE_MRU_YEAR = %d", *cartridge_mru_year);
    }
    return result;
}

// Write HPSCS_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
HP_SMART_CARD_result_t supplyWriteTag11CartridgeMRUYear(HP_SMART_CARD_device_id_t cardId, uint16_t cartridge_mru_year) {
    uint8_t buffer = cartridge_mru_year - 2006;
    return writeByte(cardId, HPSCS_CARTRIDGE_MRU_YEAR, buffer);
}

// Read HPSCS_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t supplyReadTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_woy) {
    return readByte(cardId, HPSCS_CARTRIDGE_MRU_WOY, cartridge_mru_woy);
}

// Write HPSCS_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
HP_SMART_CARD_result_t supplyWriteTag11CartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_woy) {
    return writeByte(cardId, HPSCS_CARTRIDGE_MRU_WOY, cartridge_mru_woy);
}

// Read HPSCS_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t supplyReadTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t *cartridge_mru_dow) {
    return readByte(cardId, HPSCS_CARTRIDGE_MRU_DOW, cartridge_mru_dow);
}

// Write HPSCS_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
HP_SMART_CARD_result_t supplyWriteTag11CartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t cartridge_mru_dow) {
    return writeByte(cardId, HPSCS_CARTRIDGE_MRU_DOW, cartridge_mru_dow);
}

// Read HPSCS_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return readString(cardId, HPSCS_MRU_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Write HPSCS_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    return writeString(cardId, HPSCS_MRU_PLATFORM_ID_CHAR_1, 12, buffer);
}

// Read HPSCS_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
// The most recent use (MRU) printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b.
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t *mru_platform_mfg_year) {
    uint8_t buf;
    HP_SMART_CARD_result_t result = readByte(cardId, HPSCS_MRU_PLATFORM_MFG_YEAR, &buf);

    if (HP_SMART_CARD_OK == result) {
        *mru_platform_mfg_year = buf * 1 + 2006;
        LOGD(">>> HPSCS_MRU_PLATFORM_MFG_YEAR = %d", *mru_platform_mfg_year);
    }
    return result;
}

// Write HPSCS_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mru_platform_mfg_year) {
    uint8_t buffer = mru_platform_mfg_year - 2006;
    return writeByte(cardId, HPSCS_MRU_PLATFORM_MFG_YEAR, buffer);
}

// Read HPSCS_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_woy) {
    return readByte(cardId, HPSCS_MRU_PLATFORM_MFG_WOY, mru_platform_mfg_woy);
}

// Write HPSCS_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_woy) {
    return writeByte(cardId, HPSCS_MRU_PLATFORM_MFG_WOY, mru_platform_mfg_woy);
}

// Read HPSCS_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_mfg_country) {
    return readByte(cardId, HPSCS_MRU_PLATFORM_MFG_COUNTRY, mru_platform_mfg_country);
}

// Write HPSCS_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_mfg_country) {
    return writeByte(cardId, HPSCS_MRU_PLATFORM_MFG_COUNTRY, mru_platform_mfg_country);
}

// Read HPSCS_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_major) {
    return readByte(cardId, HPSCS_MRU_PLATFORM_FW_REV_MAJOR, mru_platform_fw_rev_major);
}

// Write HPSCS_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_major) {
    return writeByte(cardId, HPSCS_MRU_PLATFORM_FW_REV_MAJOR, mru_platform_fw_rev_major);
}

// Read HPSCS_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t supplyReadTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t *mru_platform_fw_rev_minor) {
    return readByte(cardId, HPSCS_MRU_PLATFORM_FW_REV_MINOR, mru_platform_fw_rev_minor);
}

// Write HPSCS_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
HP_SMART_CARD_result_t supplyWriteTag11MRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId, uint8_t mru_platform_fw_rev_minor) {
    return writeByte(cardId, HPSCS_MRU_PLATFORM_FW_REV_MINOR, mru_platform_fw_rev_minor);
}

// Read HPSCS_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t supplyReadTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t *insertion_count) {
    return readByte(cardId, HPSCS_INSERTION_COUNT, insertion_count);
}

// Write HPSCS_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
HP_SMART_CARD_result_t supplyWriteTag11InsertionCount(HP_SMART_CARD_device_id_t cardId, uint8_t insertion_count) {
    return writeByte(cardId, HPSCS_INSERTION_COUNT, insertion_count);
}

// Read HPSCS_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t supplyReadTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t *stall_insertion_count) {
    return read2Bytes(cardId, HPSCS_STALL_INSERTION_COUNT, stall_insertion_count);
}

// Write HPSCS_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
HP_SMART_CARD_result_t supplyWriteTag11StallInsertionCount(HP_SMART_CARD_device_id_t cardId, uint16_t stall_insertion_count) {
    return write2Bytes(cardId, HPSCS_STALL_INSERTION_COUNT, stall_insertion_count);
}

// Read HPSCS_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t supplyReadTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t *last_failure_code) {
    return readByte(cardId, HPSCS_LAST_FAILURE_CODE, last_failure_code);
}

// Write HPSCS_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
HP_SMART_CARD_result_t supplyWriteTag11LastFailureCode(HP_SMART_CARD_device_id_t cardId, uint8_t last_failure_code) {
    return writeByte(cardId, HPSCS_LAST_FAILURE_CODE, last_failure_code);
}

// Read HPSCS_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t supplyReadTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t *last_user_reported_status) {
    return readByte(cardId, HPSCS_USER_REPORTED_STATUS, last_user_reported_status);
}

// Write HPSCS_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
HP_SMART_CARD_result_t supplyWriteTag11UserReportedStatus(HP_SMART_CARD_device_id_t cardId, uint8_t last_user_reported_status) {
    return writeByte(cardId, HPSCS_USER_REPORTED_STATUS, last_user_reported_status);
}

// Read HPSCS_FINE_ILG_COUNTER (Tag 11 - Fine ILG Counter - 18, 0, 16)
HP_SMART_CARD_result_t supplyReadTag11FineILGCounter(HP_SMART_CARD_device_id_t cardId, uint16_t *last_user_reported_status) {
    return read2Bytes(cardId, HPSCS_FINE_ILG_COUNTER, last_user_reported_status);
}

// Write HPSCS_USER_REPORTED_STATUS (Tag 11 - Fine ILG Counter - 18, 0, 16)
HP_SMART_CARD_result_t supplyWriteTag11FineILGCounter(HP_SMART_CARD_device_id_t cardId, uint16_t last_user_reported_status) {
    return write2Bytes(cardId, HPSCS_FINE_ILG_COUNTER, last_user_reported_status);
}

// Not in use (Tag 11 - HPSCS_PFV_REGISTER_0 - 20, 0, 12)
// Not in use (Tag 11 - HPSCS_PFV_REGISTER_1 - 22, 4, 10)
// Not in use (Tag 11 - HPSCS_PFV_REGISTER_2 - 23, 2, 10)
// Not in use (Tag 11 - HPSCS_PFV_REGISTER_3 - 24, 0, 10)
// Not in use (Tag 11 - HPSCS_PFV_REGISTER_4 - 26, 6, 12)
// Not in use (Tag 11 - HPSCS_PFV_REGISTER_5 - 27, 4, 10)
// Not in use (Tag 11 - HPSCS_PFV_LAST_REGISTER_NUM - 28, 2, 3)
// Not in use (Tag 11 - HPSCS_PILS_VOLTAGE_OFFSET - 29, 7, 12)

// Read HPSCS_MKT_DATA_REV (Tag 11 - Marketing Data Revision - 30, 3, 4)
HP_SMART_CARD_result_t supplyReadTag11MarketingDataRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *marketing_data_revision) {
    return readByte(cardId, HPSCS_MKT_DATA_REV, marketing_data_revision);
}

// Skipped (Tag 11 - Blank - 31, 7, 16)

////////////////////////////////// TAG 12 ////////////////////////////////////////////////
// OEM R/W Memory, 9 bytes, Managed by OEM
// HPSCS_OEM_SCRATCH_TAG               = 0x0C,

// Read HPSCS_OEM_DEFINED_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t supplyReadTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_1) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_RW_FIELD_1, oem_rw_filed_1);
}

// Write HPSCS_OEM_DEFINED_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_1) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_RW_FIELD_1, oem_rw_filed_1);
}

// Read HPSCS_OEM_DEFINED_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t supplyReadTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t *oem_rw_filed_2) {
    return read4Bytes(cardId, HPSCS_OEM_DEFINED_RW_FIELD_2, oem_rw_filed_2);
}

// Write HPSCS_OEM_DEFINED_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t supplyWriteTag12OEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_2) {
    return write4Bytes(cardId, HPSCS_OEM_DEFINED_RW_FIELD_2, oem_rw_filed_2);
}

// Skipped (Tag 11 - Blank(Reserved for HP Use) - 9, 7, 8)
// Skipped (Tag 11 - Blank(Reserved for HP Use) - 10, 7, 8)
// Skipped (Tag 11 - Blank(Parity byte) - 11, 7, 8)
// Skipped (Tag 11 - Blank(Parity byte2) - 12, 7, 8)

char *supplyFamilyGetFiledName(uint8_t id) {
    switch(id) {
        /* HPSCS_DRY_MFG_TAG */
        case HPSCS_EXT_OEM_ID:
            return "HPSCS_EXT_OEM_ID";
            break;
        case HPSCS_HP_INK:
            return "HPSCS_HP_INK";
            break;
        case HPSCS_SN_MFG_SITE:
            return "HPSCS_SN_MFG_SITE";
            break;
        case HPSCS_SN_MFG_LINE:
            return "HPSCS_SN_MFG_LINE";
            break;
        case HPSCS_SN_MFG_YEAR:
            return "HPSCS_SN_MFG_YEAR";
            break;
        case HPSCS_SN_WEEK:
            return "HPSCS_SN_WEEK";
            break;
        case HPSCS_SN_DOW:
            return "HPSCS_SN_DOW";
            break;
        case HPSCS_SN_HOD:
            return "HPSCS_SN_HOD";
            break;
        case HPSCS_SN_MOH:
            return "HPSCS_SN_MOH";
            break;
        case HPSCS_SN_SOM:
            return "HPSCS_SN_SOM";
            break;
        case HPSCS_SN_DRY_PROCESS_POSITION:
            return "HPSCS_SN_DRY_PROCESS_POSITION";
            break;
        case HPSCS_MAX_USABLE_CARTRIDGE_VOLUME:
            return "HPSCS_MAX_USABLE_CARTRIDGE_VOLUME";
            break;
        case HPSCS_PRINTER_LOCK_DOWN_PARTITION:
            return "HPSCS_PRINTER_LOCK_DOWN_PARTITION";
            break;
        /* HPSCS_DRY_TAG */
        case HPSCS_PILS_MFG_YEAR:
            return "HPSCS_PILS_MFG_YEAR";
            break;
        case HPSCS_PILS_MFG_WEEK:
            return "HPSCS_PILS_MFG_WEEK";
            break;
        case HPSCS_PILS_MFG_ID:
            return "HPSCS_PILS_MFG_ID";
            break;
        case HPSCS_PILS_MFG_LINE:
            return "HPSCS_PILS_MFG_LINE";
            break;
        case HPSCS_PILS_SENSOR_GAIN:
            return "HPSCS_PILS_SENSOR_GAIN";
            break;
        case HPSCS_PILS_PHASE3_AVAIL:
            return "HPSCS_PILS_PHASE3_AVAIL";
            break;
        case HPSCS_PILS_PT1_ABS_VOLUME:
            return "HPSCS_PILS_PT1_ABS_VOLUME";
            break;
        case HPSCS_PILS_PT2_ABS_VOLUME:
            return "HPSCS_PILS_PT2_ABS_VOLUME";
            break;
        case HPSCS_PILS_PT3_ABS_VOLUME:
            return "HPSCS_PILS_PT3_ABS_VOLUME";
            break;
        case HPSCS_PILS_PT1_PRESSURE:
            return "HPSCS_PILS_PT1_PRESSURE";
            break;
        case HPSCS_PILS_PT2_PRESSURE:
            return "HPSCS_PILS_PT2_PRESSURE";
            break;
        case HPSCS_PILS_PT3_PRESSURE:
            return "HPSCS_PILS_PT3_PRESSURE";
            break;
        case HPSCS_PILS_2_MULTIPLIER:
            return "HPSCS_PILS_2_MULTIPLIER";
            break;
        case HPSCS_PILS_2_SUBTRACTOR:
            return "HPSCS_PILS_2_SUBTRACTOR";
            break;
        case HPSCS_PILS_ORIENTATION:
            return "HPSCS_PILS_ORIENTATION";
            break;
        case HPSCS_PILS_TEMP_COEFFICIENT:
            return "HPSCS_PILS_TEMP_COEFFICIENT";
            break;
        case HPSCS_PILS_REFILL_EMPTY_LIMIT:
            return "HPSCS_PILS_REFILL_EMPTY_LIMIT";
            break;
        /* HPSCS_INK_MFG_TAG */
        case HPSCS_UD1_FUSE:
            return "HPSCS_UD1_FUSE";
            break;
        case HPSCS_FILL_SITE:
            return "HPSCS_FILL_SITE";
            break;
        case HPSCS_FILL_LINE:
            return "HPSCS_FILL_LINE";
            break;
        case HPSCS_FILL_YEAR:
            return "HPSCS_FILL_YEAR";
            break;
        case HPSCS_FILL_WEEK:
            return "HPSCS_FILL_WEEK";
            break;
        case HPSCS_FILL_DOW:
            return "HPSCS_FILL_DOW";
            break;
        case HPSCS_FILL_HOD:
            return "HPSCS_FILL_HOD";
            break;
        case HPSCS_FILL_MOH:
            return "HPSCS_FILL_MOH";
            break;
        case HPSCS_FILL_SOM:
            return "HPSCS_FILL_SOM";
            break;
        case HPSCS_FILL_PROCESS_POSITION:
            return "HPSCS_FILL_PROCESS_POSITION";
            break;
        case HPSCS_REGION_ID:
            return "HPSCS_REGION_ID";
            break;
        case HPSCS_REORDER_PN_CHAR_1:
        case HPSCS_REORDER_PN_CHAR_2:
        case HPSCS_REORDER_PN_CHAR_3:
        case HPSCS_REORDER_PN_CHAR_4:
        case HPSCS_REORDER_PN_CHAR_5:
        case HPSCS_REORDER_PN_CHAR_6:
        case HPSCS_REORDER_PN_CHAR_7:
        case HPSCS_REORDER_PN_CHAR_8:
        case HPSCS_REORDER_PN_CHAR_9:
        case HPSCS_REORDER_PN_CHAR_10:
        case HPSCS_REORDER_PN_CHAR_11:
        case HPSCS_REORDER_PN_CHAR_12:
            return "HPSCS_REORDER_PN_CHAR";
            break;
        /* HPSCS_INK_TAG */
        case HPSCS_INK_FORMULATOR_ID:
            return "HPSCS_INK_FORMULATOR_ID";
            break;
        case HPSCS_INK_FAMILY:
            return "HPSCS_INK_FAMILY";
            break;
        case HPSCS_INK_COLOR_CODES_GENERAL:
            return "HPSCS_INK_COLOR_CODES_GENERAL";
            break;
        case HPSCS_INK_COLOR_CODES_SPECIFIC:
            return "HPSCS_INK_COLOR_CODES_SPECIFIC";
            break;
        case HPSCS_INK_FAMILY_MEMBER:
            return "HPSCS_INK_FAMILY_MEMBER";
            break;
        case HPSCS_INK_ID_NUM:
            return "HPSCS_INK_ID_NUM";
            break;
        case HPSCS_INK_REVISION:
            return "HPSCS_INK_REVISION";
            break;
        case HPSCS_INK_DENSITY:
            return "HPSCS_INK_DENSITY";
            break;
        case HPSCS_INK_CARTRIDGE_DISTINCTION:
            return "HPSCS_INK_CARTRIDGE_DISTINCTION";
            break;
        case HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR:
            return "HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR";
            break;
        case HPSCS_INK_SHELF_LIFE_WEEKS:
            return "HPSCS_INK_SHELF_LIFE_WEEKS";
            break;
        case HPSCS_INK_SHELF_LIFE_DAYS:
            return "HPSCS_INK_SHELF_LIFE_DAYS";
            break;
        case HPSCS_INK_INSTALLED_LIFE_WEEKS:
            return "HPSCS_INK_INSTALLED_LIFE_WEEKS";
            break;
        case HPSCS_INK_INSTALLED_LIFE_DAYS:
            return "HPSCS_INK_INSTALLED_LIFE_DAYS";
            break;
        case HPSCS_INK_USABLE_INK_WEIGHT:
            return "HPSCS_INK_USABLE_INK_WEIGHT";
            break;
        case HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL:
            return "HPSCS_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL";
            break;
        /* HPSCS_FILLED_CTRDG_SPECIFIC_TAG */
        case HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1:
            return "HPSCS_FILLED_CTRDG_SPECIFIC_DATA_1";
            break;
        case HPSCS_FILLED_CTRDG_SPECIFIC_DATA_2:
            return "HPSCS_FILLED_CTRDG_SPECIFIC_DATA_2";
            break;
        case HPSCS_FILLED_CTRDG_SPECIFIC_DATA_3:
            return "HPSCS_FILLED_CTRDG_SPECIFIC_DATA_3";
            break;
        /* HPSCS_FIRST_INSTALL_TAG */
        case HPSCS_UD2_FUSE:
            return "HPSCS_UD2_FUSE";
            break;
        case HPSCS_FIRST_PLATFORM_ID_CHAR_1:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_2:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_3:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_4:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_5:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_6:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_7:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_8:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_9:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_10:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_11:
        case HPSCS_FIRST_PLATFORM_ID_CHAR_12:
            return "HPSCS_FIRST_PLATFORM_ID_CHAR";
            break;
        case HPSCS_FIRST_PLATFORM_MFG_YEAR:
            return "HPSCS_FIRST_PLATFORM_MFG_YEAR";
            break;
        case HPSCS_FIRST_PLATFORM_MFG_WOY:
            return "HPSCS_FIRST_PLATFORM_MFG_WOY";
            break;
        case HPSCS_FIRST_PLATFORM_MFG_COUNTRY:
            return "HPSCS_FIRST_PLATFORM_MFG_COUNTRY";
            break;
        case HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION:
            return "HPSCS_FIRST_PLATFORM_FW_MAJOR_REVISION";
            break;
        case HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION:
            return "HPSCS_FIRST_PLATFORM_FW_MINOR_REVISION";
            break;
        case HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT:
            return "HPSCS_FIRST_INSTALL_CARTRIDGE_COUNT";
            break;
        case HPSCS_FIRST_INSTALL_YEAR:
            return "HPSCS_FIRST_INSTALL_YEAR";
            break;
        case HPSCS_FIRST_INSTALL_WOY:
            return "HPSCS_FIRST_INSTALL_WOY";
            break;
        case HPSCS_FIRST_INSTALL_DOW:
            return "HPSCS_FIRST_INSTALL_DOW";
            break;
        case HPSCS_INK_LEVEL_GAUGE_RESOLUTION:
            return "HPSCS_INK_LEVEL_GAUGE_RESOLUTION";
            break;
        /* HPSCS_OEM_RW2RO_TAG */
        case HPSCS_UD3_FUSE:
            return "HPSCS_UD3_FUSE";
            break;
        case HPSCS_OEM_DEFINED_FIELD_1:
            return "HPSCS_OEM_DEFINED_FIELD_1";
            break;
        case HPSCS_OEM_DEFINED_FIELD_2:
            return "HPSCS_OEM_DEFINED_FIELD_2";
            break;
        /* HPSCS_TM_TAG */
        case HPSCS_HP_TRADEMARK_CHAR_1:
        case HPSCS_HP_TRADEMARK_CHAR_2:
        case HPSCS_HP_TRADEMARK_CHAR_3:
        case HPSCS_HP_TRADEMARK_CHAR_4:
        case HPSCS_HP_TRADEMARK_CHAR_5:
            return "HPSCS_HP_TRADEMARK_CHAR";
            break;
        /* HPSCS_USE_TAG */
        case HPSCS_INK_OUT_OF_INK_BIT:
            return "HPSCS_INK_OUT_OF_INK_BIT";
            break;
        case HPSCS_INK_ILG_BIT_01_25:
            return "HPSCS_INK_ILG_BIT_01_25";
            break;
        case HPSCS_INK_ILG_BIT_26_50:
            return "HPSCS_INK_ILG_BIT_26_50";
            break;
        case HPSCS_INK_ILG_BIT_51_75:
            return "HPSCS_INK_ILG_BIT_51_75";
            break;
        case HPSCS_INK_ILG_BIT_76_100:
            return "HPSCS_INK_ILG_BIT_76_100";
            break;
        case HPSCS_INK_TIUG_BIT_01_25:
            return "HPSCS_INK_TIUG_BIT_01_25";
            break;
        case HPSCS_INK_TIUG_BIT_26_50:
            return "HPSCS_INK_TIUG_BIT_26_50";
            break;
        case HPSCS_INK_TIUG_BIT_51_75:
            return "HPSCS_INK_TIUG_BIT_51_75";
            break;
        case HPSCS_INK_TIUG_BIT_76_100:
            return "HPSCS_INK_TIUG_BIT_76_100";
            break;
        case HPSCS_INK_FIRST_FAILURE_CODE:
            return "HPSCS_INK_FIRST_FAILURE_CODE";
            break;
        case HPSCS_INK_ALTERED_SUPPLY:
            return "HPSCS_INK_ALTERED_SUPPLY";
            break;
        case HPSCS_INK_USER_ACK_NON_ORG_INK:
            return "HPSCS_INK_USER_ACK_NON_ORG_INK";
            break;
        case HPSCS_INK_USER_ACK_EXPIRED_INK:
            return "HPSCS_INK_USER_ACK_EXPIRED_INK";
            break;
        case HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY:
            return "HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY";
            break;
        case HPSCS_DROP_COUNT_ONLY_ILS:
            return "HPSCS_DROP_COUNT_ONLY_ILS";
            break;
        case HPSCS_PILS_FULL_VOLTAGE_COMPLETE:
            return "HPSCS_PILS_FULL_VOLTAGE_COMPLETE";
            break;
        case HPSCS_PILS_STATE:
            return "HPSCS_PILS_STATE";
            break;
        case HPSCS_PURGE_OCCURRED:
            return "HPSCS_PURGE_OCCURRED";
            break;
        /* HPSCS_OEM_RW_OR_TAG */
        case HPSCS_OEM_DEFINED_RW_OR_FIELD_1:
            return "HPSCS_OEM_DEFINED_RW_OR_FIELD_1";
            break;
        case HPSCS_OEM_DEFINED_RW_OR_FIELD_2:
            return "HPSCS_OEM_DEFINED_RW_OR_FIELD_2";
            break;
        /* HPSCS_HP_SCRATCH_TAG */
        case HPSCS_CARTRIDGE_MRU_YEAR:
            return "HPSCS_CARTRIDGE_MRU_YEAR";
            break;
        case HPSCS_CARTRIDGE_MRU_WOY:
            return "HPSCS_CARTRIDGE_MRU_WOY";
            break;
        case HPSCS_CARTRIDGE_MRU_DOW:
            return "HPSCS_CARTRIDGE_MRU_DOW";
            break;
        case HPSCS_MRU_PLATFORM_ID_CHAR_1:
        case HPSCS_MRU_PLATFORM_ID_CHAR_2:
        case HPSCS_MRU_PLATFORM_ID_CHAR_3:
        case HPSCS_MRU_PLATFORM_ID_CHAR_4:
        case HPSCS_MRU_PLATFORM_ID_CHAR_5:
        case HPSCS_MRU_PLATFORM_ID_CHAR_6:
        case HPSCS_MRU_PLATFORM_ID_CHAR_7:
        case HPSCS_MRU_PLATFORM_ID_CHAR_8:
        case HPSCS_MRU_PLATFORM_ID_CHAR_9:
        case HPSCS_MRU_PLATFORM_ID_CHAR_10:
        case HPSCS_MRU_PLATFORM_ID_CHAR_11:
        case HPSCS_MRU_PLATFORM_ID_CHAR_12:
            return "HPSCS_MRU_PLATFORM_ID_CHAR";
            break;
        case HPSCS_MRU_PLATFORM_MFG_YEAR:
            return "HPSCS_MRU_PLATFORM_MFG_YEAR";
            break;
        case HPSCS_MRU_PLATFORM_MFG_WOY:
            return "HPSCS_MRU_PLATFORM_MFG_WOY";
            break;
        case HPSCS_MRU_PLATFORM_MFG_COUNTRY:
            return "HPSCS_MRU_PLATFORM_MFG_COUNTRY";
            break;
        case HPSCS_MRU_PLATFORM_FW_REV_MAJOR:
            return "HPSCS_MRU_PLATFORM_FW_REV_MAJOR";
            break;
        case HPSCS_MRU_PLATFORM_FW_REV_MINOR:
            return "HPSCS_MRU_PLATFORM_FW_REV_MINOR";
            break;
        case HPSCS_INSERTION_COUNT:
            return "HPSCS_INSERTION_COUNT";
            break;
        case HPSCS_STALL_INSERTION_COUNT:
            return "HPSCS_STALL_INSERTION_COUNT";
            break;
        case HPSCS_LAST_FAILURE_CODE:
            return "HPSCS_LAST_FAILURE_CODE";
            break;
        case HPSCS_USER_REPORTED_STATUS:
            return "HPSCS_USER_REPORTED_STATUS";
            break;
        case HPSCS_FINE_ILG_COUNTER:
            return "HPSCS_FINE_ILG_COUNTER";
            break;
        case HPSCS_PFV_REGISTER_0:
            return "HPSCS_PFV_REGISTER_0";
            break;
        case HPSCS_PFV_REGISTER_1:
            return "HPSCS_PFV_REGISTER_1";
            break;
        case HPSCS_PFV_REGISTER_2:
            return "HPSCS_PFV_REGISTER_2";
            break;
        case HPSCS_PFV_REGISTER_3:
            return "HPSCS_PFV_REGISTER_3";
            break;
        case HPSCS_PFV_REGISTER_4:
            return "HPSCS_PFV_REGISTER_4";
            break;
        case HPSCS_PFV_REGISTER_5:
            return "HPSCS_PFV_REGISTER_5";
            break;
        case HPSCS_PFV_LAST_REGISTER_NUM:
            return "HPSCS_PFV_LAST_REGISTER_NUM";
            break;
        case HPSCS_PILS_VOLTAGE_OFFSET:
            return "HPSCS_PILS_VOLTAGE_OFFSET";
            break;
        case HPSCS_MKT_DATA_REV:
            return "HPSCS_MKT_DATA_REV";
            break;
        /* HPSCS_OEM_SCRATCH_TAG */
        case HPSCS_OEM_DEFINED_RW_FIELD_1:
            return "HPSCS_OEM_DEFINED_RW_FIELD_1";
            break;
        case HPSCS_OEM_DEFINED_RW_FIELD_2:
            return "HPSCS_OEM_DEFINED_RW_FIELD_2";
            break;
        default:
            return tag0GetFieldName(id);
    }
}
