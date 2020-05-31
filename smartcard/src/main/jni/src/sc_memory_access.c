//
// Created by hmwan on 2020/3/31.
//

#include <stdint.h>
#include "../ifc/hp_smart_card.h"
#include "../common_log.h"
#include "sc_memory_access.h"

////////////////////////////////// TAG 0 ////////////////////////////////////////////////
// Smart card header area

// Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
uint8_t readTag0ChipTag(HP_SMART_CARD_device_id_t cardId) {
    uint8_t chip_tag = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_CHIP_TAG,
                                          sizeof(chip_tag),
                                          &chip_tag);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_CHIP_TAG = %d", chip_tag);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_CHIP_TAG");
    }

    return chip_tag;
}

// Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
uint8_t readTag0Recerved1(HP_SMART_CARD_device_id_t cardId) {
    uint8_t reserved_1 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_RESERVED_1,
                                          sizeof(reserved_1),
                                          &reserved_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_RESERVED_1 = %d", reserved_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_RESERVED_1");
    }

    return reserved_1;
}

// Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
HP_SMART_CARD_result_t readTag0FamilyID(HP_SMART_CARD_device_id_t cardId, uint8_t *family_id) {
    *family_id = HP_SMART_CARD_FAMILY_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_FAMILY_ID,
                                          sizeof(*family_id),
                                          family_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_FAMILY_ID = %d", *family_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_FAMILY_ID");
    }

    return result;
}

// Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
uint8_t readTag0ApprovedOEMBit(HP_SMART_CARD_device_id_t cardId) {
    uint8_t approved_hp_oem = HP_SMART_CARD_HP_OEM_NOT_APPROVED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_APPROVED_HP_OEM_BIT,
                                          sizeof(approved_hp_oem),
                                          &approved_hp_oem);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_APPROVED_HP_OEM_BIT = %d", approved_hp_oem);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_APPROVED_HP_OEM_BIT");
    }

    return approved_hp_oem;
}

// Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
uint8_t readTag0OEMID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t oem_id = HP_SMART_CARD_OEM_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_OEM_ID,
                                          sizeof(oem_id),
                                          &oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_OEM_ID = %d", oem_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_OEM_ID");
    }

    return oem_id;
}

// Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
uint8_t readTag0AddressPosition(HP_SMART_CARD_device_id_t cardId) {
    uint8_t address_position = HP_SMART_CARD_ADDRESS_POSITION_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_ADDRESS_POSITION,
                                          sizeof(address_position),
                                          &address_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_ADDRESS_POSITION = %d", address_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_ADDRESS_POSITION");
    }

    return address_position;
}

// Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
uint8_t readTag0TemplateVersionMajor(HP_SMART_CARD_device_id_t cardId) {
    uint8_t temp_ver_major = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MAJOR,
                                          sizeof(temp_ver_major),
                                          &temp_ver_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_TEMPLATE_VERSION_MAJOR = %d", temp_ver_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR");
    }

    return temp_ver_major;
}

// Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
uint8_t readTag0TemplateVersionMinor(HP_SMART_CARD_device_id_t cardId) {
    uint8_t temp_ver_minor = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MINOR,
                                          sizeof(temp_ver_minor),
                                          &temp_ver_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_TEMPLATE_VERSION_MINOR = %d", temp_ver_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MINOR");
    }

    return temp_ver_minor;
}

// Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

// Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
uint8_t readTag0PersoRevision(HP_SMART_CARD_device_id_t cardId) {
    uint8_t perso_revision = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_MEMORY_PARTITION_REV,
                                          sizeof(perso_revision),
                                          &perso_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_MEMORY_PARTITION_REV = %d", perso_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_MEMORY_PARTITION_REV");
    }

    return perso_revision;
}

// Skipped (Tag 0 - Reserved for Perso - 6, 3, 4)
// Skipped (Tag 0 - Blank - 7, 7, 8)

////////////////////////////////// TAG 1 ////////////////////////////////////////////////
// Dry Cartridge Manufacturing Data, 12 bytes, Managed by HP
// HP_SMART_CARD_INK_DRY_CTRDG_MFG_DATA_TAG            = 0x01,

// Read HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the Dry Cartridge memory partition (Partition #0)
// This value provided by SPS.
//    0 = Dry cartridge mfg partition unlocked
//    1 = Dry cartridge mfg partition locked
uint8_t readTag1InkUD0Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud0_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD0_FUSE,
                                          sizeof(ud0_fuse),
                                          &ud0_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD0_FUSE = %d", ud0_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD0_FUSE");
    }

    return ud0_fuse;
}

// Skipped HP_SMART_CARD_INK_BLANK_1 (Tag 1 - Blank - 1, 6, 8)
// Skipped HP_SMART_CARD_INK_BLANK_2 (Tag 1 - Blank - 2, 6, 1)
// HP_SMART_CARD_INK_EXTENDED_OEM_ID seems not an item of Tag1, might be (Tag 13 - Extended OEM ID - 1, 6, 8)
// HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR seems not an item of Tag1, might be (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)

// Read HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
uint8_t readTag1InkManufactureID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_id = HP_SMART_CARD_INK_MANUFACTURE_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_ID,
                                          sizeof(manufacture_id),
                                          &manufacture_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_ID = %d", manufacture_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_ID");
    }

    return manufacture_id;
}

// Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
uint8_t readTag1InkManufactureLine(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_line = HP_SMART_CARD_INK_MANUFACTURE_LINE_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_LINE,
                                          sizeof(manufacture_line),
                                          &manufacture_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_LINE = %d", manufacture_line);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_LINE");
    }

    return manufacture_line;
}

// Read HP_SMART_CARD_INK_MANUFACTURE_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 2, 5)
// Year the dry cartridge was manufactured and the year the cartridge was filled with ink.
//    m = 1, b = 2006, range = 2006 - 2037, in the equation = mx + b where the y slope equals 1, and the intercept is 2006.
uint16_t readTag1InkManufactureYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t manufacture_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        manufacture_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_YEAR = %d", manufacture_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_YEAR");
    }

    return manufacture_year;
}

// Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
uint8_t readTag1InkManufactureWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_WEEK,
                                          sizeof(manufacture_woy),
                                          &manufacture_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_WEEK = %d", manufacture_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_WEEK");
    }

    return manufacture_woy;
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
uint8_t readTag1InkManufactureDayOfWeek(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_dow = HP_SMART_CARD_SUNDAY;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_DOW,
                                          sizeof(manufacture_dow),
                                          &manufacture_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_DOW = %d", manufacture_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_DOW");
    }

    return manufacture_dow;
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
uint8_t readTag1InkManufactureHourOfDay(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_hod = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_HOD,
                                          sizeof(manufacture_hod),
                                          &manufacture_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_HOD = %d", manufacture_hod);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_HOD");
    }

    return manufacture_hod;
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
uint8_t readTag1InkManufactureMinuteOfHour(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_moh = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_MOH,
                                          sizeof(manufacture_moh),
                                          &manufacture_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_MOH = %d", manufacture_moh);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_MOH");
    }

    return manufacture_moh;
}

// Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
uint8_t readTag1InkManufactureSecondOfMinute(HP_SMART_CARD_device_id_t cardId) {
    uint8_t manufacture_som = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_SOM,
                                          sizeof(manufacture_som),
                                          &manufacture_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_SOM = %d", manufacture_som);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_SOM");
    }

    return manufacture_som;
}

// Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
uint8_t readTag1InkDryProcessPosition(HP_SMART_CARD_device_id_t cardId) {
    uint8_t process_position = HP_SMART_CARD_PROCESS_POSITION_0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DRY_PROCESS_POSITION,
                                          sizeof(process_position),
                                          &process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_DRY_PROCESS_POSITION = %d", process_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DRY_PROCESS_POSITION");
    }

    return process_position;
}

// Read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 0, 16)
// Maximum usable cartridge volume
//    m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
float readTag1InkMaxUsableCartridgeVolume(HP_SMART_CARD_device_id_t cardId) {
    float max_usable_cartridge_volume = 0;
    uint16_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        max_usable_cartridge_volume = buffer * 0.1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME = %f", max_usable_cartridge_volume);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME");
    }

    return max_usable_cartridge_volume;
}

// Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Printer Lockdown Partition - 9, 0, 5)
uint8_t readTag1InkPrinterLockdownPartition(HP_SMART_CARD_device_id_t cardId) {
    uint8_t  lock_down_partition = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION,
                                          sizeof(lock_down_partition),
                                          &lock_down_partition);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION = %d", lock_down_partition);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION");
    }

    return lock_down_partition;
}

// Skipped (Tag 1 - Blank - 10, 3, 20)

////////////////////////////////// TAG 2 ////////////////////////////////////////////////
// Dry Cartridge Specific Data, 16 bytes, Managed by HP
// HP_SMART_CARD_INK_DRY_CTRDG_SPECIFIC_DATA_TAG       = 0x02,

// Read HP_SMART_CARD_INK_TSR (Tag 2 - Thermal Sense Resistor (TSR) - 1, 7, 7)
// Value of the thermal sense resistor:
//    m = 1 b = 250 range = 250 – 377 in equation y = mx + b
uint16_t readTag2InkTSR(HP_SMART_CARD_device_id_t cardId) {
    uint16_t ink_tsr = 250;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TSR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        ink_tsr = buffer * 1 + 250;
        LOGD(">>> HP_SMART_CARD_INK_TSR = %d", ink_tsr);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TSR");
    }

    return ink_tsr;
}

// Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
// Value of the TSR thermal coefficient
//    m = 5 b = 2900 range = 2900 -  in equation y = mx + b
uint16_t readTag2InkTSRThermalCoefficient(HP_SMART_CARD_device_id_t cardId) {
    uint16_t ink_tcr = 2900;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        ink_tcr = buffer * 5 + 2900;
        LOGD(">>> HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT = %d", ink_tcr);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT");
    }

    return ink_tcr;
}

// Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
// Bulk cartridge identifier
//    0 = single use cartridge
//    1 = bulk ink cartridge
uint8_t readTag2InkBulkCartridge(HP_SMART_CARD_device_id_t cardId) {
    uint8_t bulk_ctrdg = HP_SMART_CARD_INK_SINGLE_CTRDG;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_BULK_CTRDG,
                                          sizeof(bulk_ctrdg),
                                          &bulk_ctrdg);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_BULK_CTRDG = %d", bulk_ctrdg);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_BULK_CTRDG");
    }

    return bulk_ctrdg;
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
uint8_t readTag3InkUD1Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud1_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD1_FUSE,
                                          sizeof(ud1_fuse),
                                          &ud1_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD1_FUSE = %d", ud1_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD1_FUSE");
    }

    return ud1_fuse;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
uint8_t readTag3InkCartridgeFillSiteID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t site_id = HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID,
                                          sizeof(site_id),
                                          &site_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID = %d", site_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID");
    }

    return site_id;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
uint8_t readTag3InkCartridgeFillLine(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_line = HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE,
                                          sizeof(fill_line),
                                          &fill_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE = %d", fill_line);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE");
    }

    return fill_line;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
// Year that cartridge was filled.
//      m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
uint16_t readTag3InkCartridgeFillYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t fill_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        fill_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR = %d", fill_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR");
    }

    return fill_year;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
uint8_t readTag3InkCartridgeFillWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY,
                                          sizeof(fill_woy),
                                          &fill_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY = %d", fill_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY");
    }

    return fill_woy;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
uint8_t readTag3InkCartridgeFillDayOfWeek(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_dow = HP_SMART_CARD_SUNDAY;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW,
                                          sizeof(fill_dow),
                                          &fill_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW = %d", fill_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW");
    }

    return fill_dow;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
uint8_t readTag3InkCartridgeFillHourOfDay(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_hod = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD,
                                          sizeof(fill_hod),
                                          &fill_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD = %d", fill_hod);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD");
    }

    return fill_hod;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
uint8_t readTag3InkCartridgeFillMinuteOfHour(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_moh = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH,
                                          sizeof(fill_moh),
                                          &fill_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH = %d", fill_moh);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH");
    }

    return fill_moh;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
uint8_t readTag3InkCartridgeFillSecondOfMinute(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_som = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM,
                                          sizeof(fill_som),
                                          &fill_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM = %d", fill_som);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM");
    }

    return fill_som;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
uint8_t readTag3InkCartridgeFillProcessPosition(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fill_process_position = HP_SMART_CARD_PROCESS_POSITION_0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t) HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION,
                                          sizeof(fill_process_position),
                                          &fill_process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION = %d", fill_process_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION");
    }

    return fill_process_position;
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
uint8_t readTag4InkFormulatorID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t formulator_id = HP_SMART_CARD_INK_FORMULATOR_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FORMULATOR_ID,
                                          sizeof(formulator_id),
                                          &formulator_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FORMULATOR_ID = %d", formulator_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FORMULATOR_ID");
    }

    return formulator_id;
}

// Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
uint8_t readTag4InkInkFamily(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_family = HP_SMART_CARD_INK_FAMILY_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY,
                                          sizeof(ink_family),
                                          &ink_family);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_FAMILY = %d", ink_family);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_FAMILY");
    }

    return ink_family;
}

// Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
uint8_t readTag4InkColorCodesGeneral(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_color_code_general = HP_SMART_CARD_INK_COLOR_CODES_BLACK;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_GENERAL,
                                          sizeof(ink_color_code_general),
                                          &ink_color_code_general);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_COLOR_CODES_GENERAL = %d", ink_color_code_general);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_COLOR_CODES_GENERAL");
    }

    return ink_color_code_general;
}

// Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
uint8_t readTag4InkColorCodesSpecific(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_color_code_specific = HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_UNDEFINED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC,
                                          sizeof(ink_color_code_specific),
                                          &ink_color_code_specific);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC = %d", ink_color_code_specific);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC");
    }

    return ink_color_code_specific;
}

// Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
uint8_t readTag4InkInkFamilyMember(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_family_member = HP_SMART_CARD_INK_FAMILY_MEMBER_UNDEFINED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY_MEMBER,
                                          sizeof(ink_family_member),
                                          &ink_family_member);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_FAMILY_MEMBER = %d", ink_family_member);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_FAMILY_MEMBER");
    }

    return ink_family_member;
}

// Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
uint16_t readTag4InkInkIDNumber(HP_SMART_CARD_device_id_t cardId) {
    uint16_t ink_id_num = HP_SMART_CARD_INK_ID_NEMBER_UNDEFINED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_ID_NUM,
                                          sizeof(ink_id_num),
                                          &ink_id_num);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_ID_NUM = %d", ink_id_num);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_ID_NUM");
    }

    return ink_id_num;
}

// Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
uint8_t readTag4InkInkRevision(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_revision = HP_SMART_CARD_INK_ID_REVISION_UNDEFINED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_REVISION,
                                          sizeof(ink_revision),
                                          &ink_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_REVISION = %d", ink_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_REVISION");
    }

    return ink_revision;
}

// Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
// Specifies the ink density.
//      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
float readTag4InkDensity(HP_SMART_CARD_device_id_t cardId) {
    float ink_density = 0.7;
    uint16_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DENSITY,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        ink_density = 0.001 * buffer + 0.7;
        LOGD(">>> HP_SMART_CARD_INK_DENSITY = %f", ink_density);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DENSITY");
    }

    return ink_density;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
uint8_t readTag4InkCartridgeDistinction(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_cartridge_distinction = HP_SMART_CARD_INK_CARTRIDGE_NO_DISTINCTION;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION,
                                          sizeof(ink_cartridge_distinction),
                                          &ink_cartridge_distinction);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION = %d", ink_cartridge_distinction);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION");
    }

    return ink_cartridge_distinction;
}

// Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
uint8_t readTag4InkSupplyKeyingDescriptor(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_supply_keying_descriptor = HP_SMART_CARD_INK_KEY_SIZE_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR,
                                          sizeof(ink_supply_keying_descriptor),
                                          &ink_supply_keying_descriptor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR = %d", ink_supply_keying_descriptor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR");
    }

    return ink_supply_keying_descriptor;
}

// Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
// ??????????? 不明白
//    Shelf Weeks + Shelf Days + Ink filled time / date <= current date / time
uint8_t readTag4InkShelfLifeWeeks(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_shelf_life_weeks = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_WEEKS,
                                          sizeof(ink_shelf_life_weeks),
                                          &ink_shelf_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SHELF_LIFE_WEEKS = %d", ink_shelf_life_weeks);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS");
    }

    return ink_shelf_life_weeks;
}

// Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
uint8_t readTag4InkShelfLifeDays(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_shelf_life_days = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_DAYS,
                                          sizeof(ink_shelf_life_days),
                                          &ink_shelf_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SHELF_LIFE_DAYS = %d", ink_shelf_life_days);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SHELF_LIFE_DAYS");
    }

    return ink_shelf_life_days;
}

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
// ??????????? 不明白
//    Installed Weeks + Installed Days + First installed time / date <= current date / time
uint8_t readTag4InkInstalledLifeWeeks(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_installed_life_weeks = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS,
                                          sizeof(ink_installed_life_weeks),
                                          &ink_installed_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS = %d", ink_installed_life_weeks);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS");
    }

    return ink_installed_life_weeks;
}

// Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
uint8_t readTag4InkInstalledLifeDays(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_installed_life_days = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS,
                                          sizeof(ink_installed_life_days),
                                          &ink_installed_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS = %d", ink_installed_life_days);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS");
    }

    return ink_installed_life_days;
}

// Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
// m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
float readTag4InkUsableInkWeight(HP_SMART_CARD_device_id_t cardId) {
    float ink_usable_ink_weight;
    uint16_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USABLE_INK_WEIGHT,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        ink_usable_ink_weight = buffer * 0.1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_USABLE_INK_WEIGHT = %f", ink_usable_ink_weight);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USABLE_INK_WEIGHT");
    }

    return ink_usable_ink_weight;
}

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
// Percentage Value: 10 means 10%
uint8_t readTag4InkAlteredSupplyNotificationLevel(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_notification_level = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL,
                                          sizeof(ink_notification_level),
                                          &ink_notification_level);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL = %d", ink_notification_level);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL");
    }

    return ink_notification_level;
}

// Skipped (Tag 4 - Blank - 15, 4, 15)

////////////////////////////////// TAG 5 ////////////////////////////////////////////////
// Filled Cartridge Specific Data, 12 bytes, Managed by Ink Filter
// HP_SMART_CARD_INK_FILLED_CTRDG_SPECIFIC_DATA_TAG    = 0x05,

// Read HP_SMART_CARD_INK_FIRING_FREQ (Tag 5 - Firing Frequency - 1, 7, 7)
// Maximum firing frequency of the cartridge, based on the filled ink.
//    m = 0.2, b = 0, range = 0 – 25.4 in equation y = mx + b
float readTag5InkFiringFreq(HP_SMART_CARD_device_id_t cardId) {
    float firing_freq = 0;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRING_FREQ,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        firing_freq = buffer * 0.2 + 0;
        LOGD(">>> HP_SMART_CARD_INK_FIRING_FREQ = %f", firing_freq);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRING_FREQ");
    }

    return firing_freq;
}

// Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
// Firing pulse width, based on the filled ink type.
//    m = 0.01 b = 1.5, range = 1.50 – 4.05 in equation y = mx + b
float readTag5InkPulseWidth(HP_SMART_CARD_device_id_t cardId) {
    float pulse_width = 1.5;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WIDTH,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        pulse_width = buffer * 0.1 + 1.5;
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WIDTH = %f", pulse_width);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PULSE_WIDTH");
    }

    return pulse_width;
}

// Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
// Firing voltage required by the cartridge, based on the filled ink type.
//    m = 0.1, b = 5.0, range = 5.0 – 17.7 y = mx + b
float readTag5InkFiringVoltage(HP_SMART_CARD_device_id_t cardId) {
    float firing_voltage = 5.0;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRING_VOLTAGE,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        firing_voltage = buffer * 0.1 + 5.0;
        LOGD(">>> HP_SMART_CARD_INK_FIRING_VOLTAGE = %f", firing_voltage);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRING_VOLTAGE");
    }

    return firing_voltage;
}

// Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
// Turn on energy for a firing resistor, based on the filled ink type.
//    m = 0.02, b = 2.0, range = 2.0 – 7.1 y = mx + b
float readTag5InkTurnOnEnergy(HP_SMART_CARD_device_id_t cardId) {
    float turn_on_energy = 2.0;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TURN_ON_ENERGY,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        turn_on_energy = buffer * 0.02 + 2.0;
        LOGD(">>> HP_SMART_CARD_INK_TURN_ON_ENERGY = %f", turn_on_energy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TURN_ON_ENERGY");
    }

    return turn_on_energy;
}

// Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
// Pulse warming temperature required for optimal firing for this cartridge, based on the filled ink type.
//    m = 1, b = 0, range = 0 – 127 y = mx + b
uint8_t readTag5InkPulseWarmingTemperature(HP_SMART_CARD_device_id_t cardId) {
    uint8_t pulse_warming_temp = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WARMING_TEMP,
                                          sizeof(pulse_warming_temp),
                                          &pulse_warming_temp);
    if (HP_SMART_CARD_OK == result) {
        pulse_warming_temp = pulse_warming_temp * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WARMING_TEMP = %d", pulse_warming_temp);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PULSE_WARMING_TEMP");
    }

    return pulse_warming_temp;
}

// Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
// Maximum head temperature for this cartridge, based on the filled ink type.
//    m = 1, b = 0, range = 0 – 127 y = mx + b
uint8_t readTag5InkMaxTemperature(HP_SMART_CARD_device_id_t cardId) {
    uint8_t max_temp = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MAX_TEMP,
                                          sizeof(max_temp),
                                          &max_temp);
    if (HP_SMART_CARD_OK == result) {
        max_temp = max_temp * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_MAX_TEMP = %d", max_temp);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MAX_TEMP");
    }

    return max_temp;
}

// Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
// Ink drop volume, based on filled ink type firing parameters.
//    m = 1, b = 0, range = 0 – 63 y = mx + b
uint8_t readTag5InkDropVolume(HP_SMART_CARD_device_id_t cardId) {
    uint8_t drop_volume = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DROP_VOLUME,
                                          sizeof(drop_volume),
                                          &drop_volume);
    if (HP_SMART_CARD_OK == result) {
        drop_volume = drop_volume * 1 + 0;
        LOGD(">>> HP_SMART_CARD_INK_DROP_VOLUME = %d", drop_volume);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DROP_VOLUME");
    }

    return drop_volume;
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
uint8_t readTag6InkWriteProtect(HP_SMART_CARD_device_id_t cardId) {
    uint8_t write_protect = HP_SMART_CARD_INK_WRITE_PROTECT_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_WRITE_PROTECT,
                                          sizeof(write_protect),
                                          &write_protect);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_WRITE_PROTECT = %d", write_protect);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_WRITE_PROTECT");
    }

    return write_protect;
}

// Write HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse - 1, 7, 1)
HP_SMART_CARD_result_t writeTag6InkWriteProtect(HP_SMART_CARD_device_id_t cardId, uint8_t write_protect) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_WRITE_PROTECT,
                                          sizeof(write_protect),
                                          &write_protect);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_WRITE_PROTECT = %d", write_protect);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_WRITE_PROTECT");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t readTag6Ink1stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_read_string(cardId,
                                               (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1,
                                               12,
                                               buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_write_string(cardId,
                                               (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1,
                                               12,
                                               buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
// The first install printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
uint16_t readTag6Ink1stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t mfg_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        mfg_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR = %d", mfg_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR");
    }

    return mfg_year;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGYear(HP_SMART_CARD_device_id_t cardId, uint16_t mfg_year) {
    uint8_t buffer = mfg_year - 2006;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR,
                                           sizeof(buffer),
                                           &buffer);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR = %d", mfg_year);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
uint8_t readTag6Ink1stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mfg_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY,
                                          sizeof(mfg_woy),
                                          &mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY = %d", mfg_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY");
    }

    return mfg_woy;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_woy) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY,
                                           sizeof(mfg_woy),
                                           &mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY = %d", mfg_woy);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
uint8_t readTag6Ink1stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mfg_country = HP_SMART_CARD_COUNTRY_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY,
                                          sizeof(mfg_country),
                                          &mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY = %d", mfg_country);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY");
    }

    return mfg_country;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId, uint8_t mfg_country) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY,
                                           sizeof(mfg_country),
                                           &mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY = %d", mfg_country);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
uint8_t readTag6Ink1stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fw_revision_major = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION,
                                          sizeof(fw_revision_major),
                                          &fw_revision_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION = %d", fw_revision_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION");
    }

    return fw_revision_major;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformFWMajorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_major) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION,
                                           sizeof(fw_revision_major),
                                           &fw_revision_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION = %d", fw_revision_major);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
uint8_t readTag6Ink1stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId) {
    uint8_t fw_revision_minor = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION,
                                          sizeof(fw_revision_minor),
                                          &fw_revision_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION = %d", fw_revision_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION");
    }

    return fw_revision_minor;
}

// Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
HP_SMART_CARD_result_t writeTag6Ink1stPlatformFWMinorRevision(HP_SMART_CARD_device_id_t cardId, uint8_t fw_revision_minor) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION,
                                           sizeof(fw_revision_minor),
                                           &fw_revision_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION = %d", fw_revision_minor);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
uint16_t readTag6Ink1stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId) {
    uint16_t first_install_count;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT,
                                          sizeof(first_install_count),
                                          &first_install_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT = %d", first_install_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT");
    }

    return first_install_count;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
HP_SMART_CARD_result_t writeTag6Ink1stInstallCartridgeCount(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_count) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT,
                                           sizeof(first_install_count),
                                           &first_install_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT = %d", first_install_count);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
// The year of the first installation of this cartridge into this printer platform.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
uint16_t readTag6Ink1stInstallYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t first_install_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        first_install_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_YEAR = %d", first_install_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR");
    }

    return first_install_year;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
HP_SMART_CARD_result_t writeTag6Ink1stInstallYear(HP_SMART_CARD_device_id_t cardId, uint16_t first_install_year) {
    uint8_t buffer = first_install_year - 2006;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_YEAR,
                                           sizeof(buffer),
                                           &buffer);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR = %d", first_install_year);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_INSTALL_YEAR");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
uint8_t readTag6Ink1stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t first_install_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_WOY,
                                          sizeof(first_install_woy),
                                          &first_install_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_WOY = %d", first_install_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_WOY");
    }

    return first_install_woy;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
HP_SMART_CARD_result_t writeTag6Ink1stInstallWeekOfYear(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_woy) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_WOY,
                                           sizeof(first_install_woy),
                                           &first_install_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_INSTALL_WOY = %d", first_install_woy);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_INSTALL_WOY");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
uint8_t readTag6Ink1stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId) {
    uint8_t first_install_dow = HP_SMART_CARD_SUNDAY;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_DOW,
                                          sizeof(first_install_dow),
                                          &first_install_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_DOW = %d", first_install_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_DOW");
    }

    return first_install_dow;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
HP_SMART_CARD_result_t writeTag6Ink1stInstallDayOfWeek(HP_SMART_CARD_device_id_t cardId, uint8_t first_install_dow) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_DOW,
                                           sizeof(first_install_dow),
                                           &first_install_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_INSTALL_DOW = %d", first_install_dow);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_INSTALL_DOW");
    }

    return result;
}

// Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
// If this filed is programmed with a value of 50(decimal), then it means each bit represent 2% of in usage
uint8_t readTag6InkInkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId) {
    uint8_t gauge_resolution = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION,
                                          sizeof(gauge_resolution),
                                          &gauge_resolution);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION = %d", gauge_resolution);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION");
    }

    return gauge_resolution;
}

// Write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
HP_SMART_CARD_result_t writeTag6InkInkLevelGaugeResolution(HP_SMART_CARD_device_id_t cardId, uint8_t gauge_resolution) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION,
                                           sizeof(gauge_resolution),
                                           &gauge_resolution);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION = %d", gauge_resolution);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION");
    }

    return result;
}

// Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
uint32_t readTag6Ink1stInstallOEMField(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_field = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD,
                                          sizeof(oem_field),
                                          &oem_field);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD = %d", oem_field);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD");
    }

    return oem_field;
}

// Write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
HP_SMART_CARD_result_t writeTag6Ink1stInstallOEMField(HP_SMART_CARD_device_id_t cardId, uint32_t oem_field) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD,
                                          sizeof(oem_field),
                                          &oem_field);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD = %d", oem_field);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD");
    }

    return oem_field;
}

// Skipped (Tag 6 - Blank - 20, 7, 8)

////////////////////////////////// TAG 7 ////////////////////////////////////////////////
// OEM Lockable memory, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_LOCKABLE_MEMORY_TAG           = 0x07,

// Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the OEM Lockable memory partition (Partition #3)
//    0 = OEM Lockable partition unlocked
//    1 = OEM Lockable partition locked
uint8_t readTag7InkUD3Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud3_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD3_FUSE,
                                          sizeof(ud3_fuse),
                                          &ud3_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD3_FUSE = %d", ud3_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD3_FUSE");
    }

    return ud3_fuse;
}

// Write HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
HP_SMART_CARD_result_t writeTag7InkUD3Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud3_fuse) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD3_FUSE,
                                           sizeof(ud3_fuse),
                                           &ud3_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_UD3_FUSE = %d", ud3_fuse);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_UD3_FUSE");
    }

    return result;
}

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
uint32_t readTag7InkOEMDefinedField1(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_defined_filed_1 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1,
                                          sizeof(oem_defined_filed_1),
                                          &oem_defined_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 = %d", oem_defined_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1");
    }

    return oem_defined_filed_1;
}

// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
HP_SMART_CARD_result_t writeTag7InkOEMDefinedField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_1) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1,
                                          sizeof(oem_defined_filed_1),
                                          &oem_defined_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 = %d", oem_defined_filed_1);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1");
    }

    return result;
}

// Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
uint32_t readTag7InkOEMDefinedField2(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_defined_filed_2 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2,
                                          sizeof(oem_defined_filed_2),
                                          &oem_defined_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 = %d", oem_defined_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2");
    }

    return oem_defined_filed_2;
}

// Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
HP_SMART_CARD_result_t writeTag7InkOEMDefinedField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_defined_filed_2) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2,
                                           sizeof(oem_defined_filed_2),
                                           &oem_defined_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 = %d", oem_defined_filed_2);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2");
    }

    return result;
}

////////////////////////////////// TAG 8 ////////////////////////////////////////////////
// Trademark, 4 bytes, Managed by HP or Ink Filler
// HP_SMART_CARD_INK_HP_TRADEMARK_TAG                  = 0x08,

// Read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t readTag8InkHPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_read_string(cardId,
                                               (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1,
                                               5,
                                               buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> HP_SMART_CARD_INK_HP_TRADEMARK_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
HP_SMART_CARD_result_t writeTag8InkHPTrademark(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_write_string(cardId,
                                                (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1,
                                                5,
                                                buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1 = %s", buffer);
        } else {
            LOGE(">>> Failed to write HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
// Bit indicating lock/unlock state of the Trademark memory partition (Partition #4)
//    0 = Trademark partition unlocked
//    1 = Trademark partition locked
uint8_t readTag8InkUD4Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud4_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD4_FUSE,
                                          sizeof(ud4_fuse),
                                          &ud4_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD4_FUSE = %d", ud4_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD4_FUSE");
    }

    return ud4_fuse;
}

// Write HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
HP_SMART_CARD_result_t writeTag8InkUD4Fuse(HP_SMART_CARD_device_id_t cardId, uint8_t ud4_fuse) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD4_FUSE,
                                           sizeof(ud4_fuse),
                                           &ud4_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_UD4_FUSE = %d", ud4_fuse);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_UD4_FUSE");
    }

    return result;
}

// Skipped (Tag 8 - Blank - 4, 0, 1)

////////////////////////////////// TAG 9 ////////////////////////////////////////////////
// Ink Use & Warranty, 32 bytes, Managed by OEM
// HP_SMART_CARD_INK_INK_USE_AND_WARRANTY_TAG          = 0x09,

// Read HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
// This bit is set by the printer once the ILG indicates out of ink condition.
//    0 = not reached,
//    1 = reached
uint8_t readTag9InkILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId) {
    uint8_t out_of_ink = HP_SMART_CARD_INK_ILG_OUT_OF_INK_UNREACH;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OUT_OF_INK_BIT,
                                          sizeof(out_of_ink),
                                          &out_of_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OUT_OF_INK_BIT = %d", out_of_ink);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OUT_OF_INK_BIT");
    }

    return out_of_ink;
}

// Write HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
HP_SMART_CARD_result_t writeTag9InkILGOutOfInkBit(HP_SMART_CARD_device_id_t cardId, uint8_t out_of_ink) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OUT_OF_INK_BIT,
                                           sizeof(out_of_ink),
                                           &out_of_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OUT_OF_INK_BIT = %d", out_of_ink);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OUT_OF_INK_BIT");
    }

    return result;
}

// Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
// Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit01To25(HP_SMART_CARD_device_id_t cardId) {
    uint32_t ilg_bit_25_01 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_01_25,
                                          sizeof(ilg_bit_25_01),
                                          &ilg_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_01_25 = %d", ilg_bit_25_01);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_01_25");
    }

    return ilg_bit_25_01;
}

// Write HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
HP_SMART_CARD_result_t writeTag9InkILGBit01To25(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_25_01) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_01_25,
                                           sizeof(ilg_bit_25_01),
                                           &ilg_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_ILG_BIT_01_25 = %d", ilg_bit_25_01);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_ILG_BIT_01_25");
    }

    return result;
}

// Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
// Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit26To50(HP_SMART_CARD_device_id_t cardId) {
    uint32_t ilg_bit_50_26 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_26_50,
                                          sizeof(ilg_bit_50_26),
                                          &ilg_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_26_50 = %d", ilg_bit_50_26);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_26_50");
    }

    return ilg_bit_50_26;
}

// Write HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
HP_SMART_CARD_result_t writeTag9InkILGBit26To50(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit_50_26) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_26_50,
                                           sizeof(ilg_bit_50_26),
                                           &ilg_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_ILG_BIT_26_50 = %d", ilg_bit_50_26);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_ILG_BIT_26_50");
    }

    return result;
}

// Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
// Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit51To75(HP_SMART_CARD_device_id_t cardId) {
    uint32_t ilg_bit_75_51 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_51_75,
                                          sizeof(ilg_bit_75_51),
                                          &ilg_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_51_75 = %d", ilg_bit_75_51);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_51_75");
    }

    return ilg_bit_75_51;
}

// Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
// Ink level gauge bits 76-100 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkILGBit76To100(HP_SMART_CARD_device_id_t cardId) {
    uint32_t ilg_bit_100_76 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_76_100,
                                          sizeof(ilg_bit_100_76),
                                          &ilg_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_76_100 = %d", ilg_bit_100_76);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_76_100");
    }

    return ilg_bit_100_76;
}

// Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
// Total Ink Usage Gauge bits 1 - 25 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkTUGBit01To25(HP_SMART_CARD_device_id_t cardId) {
    uint32_t tiug_bit_25_01 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_01_25,
                                          sizeof(tiug_bit_25_01),
                                          &tiug_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_01_25 = %d", tiug_bit_25_01);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_01_25");
    }

    return tiug_bit_25_01;
}

// Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
// Total Ink Usage Gauge bits 26 - 50 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkTUGBit26To50(HP_SMART_CARD_device_id_t cardId) {
    uint32_t tiug_bit_50_26 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_26_50,
                                          sizeof(tiug_bit_50_26),
                                          &tiug_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_26_50 = %d", tiug_bit_50_26);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_26_50");
    }

    return tiug_bit_50_26;
}

// Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
// Total Ink Usage Gauge bits 51 - 75 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkTUGBit51To75(HP_SMART_CARD_device_id_t cardId) {
    uint32_t tiug_bit_75_51 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_51_75,
                                          sizeof(tiug_bit_75_51),
                                          &tiug_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_51_75 = %d", tiug_bit_75_51);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_51_75");
    }

    return tiug_bit_75_51;
}

// Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
// Total Ink Usage Gauge bits 76 - 100 of a 100-bit field. This enables up to 1% resolution.
uint32_t readTag9InkTUGBit76To100(HP_SMART_CARD_device_id_t cardId) {
    uint32_t tiug_bit_100_76 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_76_100,
                                          sizeof(tiug_bit_100_76),
                                          &tiug_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_76_100 = %d", tiug_bit_100_76);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_76_100");
    }

    return tiug_bit_100_76;
}

// Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
uint8_t readTag9Ink1stFailureCode(HP_SMART_CARD_device_id_t cardId) {
    uint8_t first_failure_code = HP_SMART_CARD_INK_FIRST_FAILURE_CODE_NOEEROR;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_FAILURE_CODE,
                                          sizeof(first_failure_code),
                                          &first_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_FAILURE_CODE = %d", first_failure_code);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_FAILURE_CODE");
    }

    return first_failure_code;
}

// Read HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
// This bit is set by the printer when the cartridge has positively exceeded available ink.
// This is set once the TIUG target has been reached.
//    0 = not reached,
//    1 = reached
uint8_t readTag9InkAlteredSupply(HP_SMART_CARD_device_id_t cardId) {
    uint8_t altered_supply = HP_SMART_CARD_INK_ALTERED_SUPPLY_NOT_EXCEEDED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ALTERED_SUPPLY,
                                          sizeof(altered_supply),
                                          &altered_supply);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ALTERED_SUPPLY = %d", altered_supply);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY");
    }

    return altered_supply;
}

// Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
uint8_t readTag9InkUserAckNonOrgInk(HP_SMART_CARD_device_id_t cardId) {
    uint8_t user_ack_altered_supply = HP_SMART_CARD_INK_USER_NOT_ACKNOWLEDGED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK,
                                          sizeof(user_ack_altered_supply),
                                          &user_ack_altered_supply);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK = %d", user_ack_altered_supply);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK");
    }

    return user_ack_altered_supply;
}

// Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
uint8_t readTag9InkUserAckExpiredInk(HP_SMART_CARD_device_id_t cardId) {
    uint8_t user_ack_expired_ink = HP_SMART_CARD_INK_USER_ACK_NOT_EXPIRED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK,
                                          sizeof(user_ack_expired_ink),
                                          &user_ack_expired_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK = %d", user_ack_expired_ink);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK");
    }

    return user_ack_expired_ink;
}

// Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
uint8_t readTag9InkFaultyReplaceImmediately(HP_SMART_CARD_device_id_t cardId) {
    uint8_t faulty_replace_immediately = HP_SMART_CARD_INK_FAULTY_NOT_FATAL;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY,
                                          sizeof(faulty_replace_immediately),
                                          &faulty_replace_immediately);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY = %d", faulty_replace_immediately);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY");
    }

    return faulty_replace_immediately;
}

// Skipped (Tag 9 - Blank - 27, 4, 32)
// Skipped (Tag 9 - Blank - 31, 4, 13)

////////////////////////////////// TAG 10 ////////////////////////////////////////////////
// OEM RW_OR Memory, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_RW_OR_MEMORY_TAG              = 0x0A,

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
uint32_t readTag10InkOEMRWORField1(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_rw_or_filed_1 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1,
                                          sizeof(oem_rw_or_filed_1),
                                          &oem_rw_or_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 = %d", oem_rw_or_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1");
    }

    return oem_rw_or_filed_1;
}

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t writeTag10InkOEMRWORField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_1) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1,
                                           sizeof(oem_rw_or_filed_1),
                                           &oem_rw_or_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 = %d", oem_rw_or_filed_1);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1");
    }

    return result;
}

// Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
uint32_t readTag10InkOEMRWORField2(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_rw_or_filed_2 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2,
                                          sizeof(oem_rw_or_filed_2),
                                          &oem_rw_or_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 = %d", oem_rw_or_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2");
    }

    return oem_rw_or_filed_2;
}

// Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
HP_SMART_CARD_result_t writeTag10InkOEMRWORField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_or_filed_2) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2,
                                           sizeof(oem_rw_or_filed_2),
                                           &oem_rw_or_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 = %d", oem_rw_or_filed_2);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2");
    }

    return result;
}

////////////////////////////////// TAG 11 ////////////////////////////////////////////////
// HP Scratchpad, 32 bytes, Managed by
// HP_SMART_CARD_INK_HP_SCRATCH_PAD_TAG                = 0x0B,

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
// Most recent use (MRU) of this cartridge - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
uint16_t readTag11InkCartridgeMRUYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t cartridge_mru_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        cartridge_mru_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR = %d", cartridge_mru_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR");
    }

    return cartridge_mru_year;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
uint8_t readTag11InkCartridgeMRUWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t cartridge_mru_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY,
                                          sizeof(cartridge_mru_woy),
                                          &cartridge_mru_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY = %d", cartridge_mru_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY");
    }

    return cartridge_mru_woy;
}

// Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
uint8_t readTag11InkCartridgeMRUDayOfWeek(HP_SMART_CARD_device_id_t cardId) {
    uint8_t cartridge_mru_dow = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW,
                                          sizeof(cartridge_mru_dow),
                                          &cartridge_mru_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW = %d", cartridge_mru_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW");
    }

    return cartridge_mru_dow;
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
HP_SMART_CARD_result_t readTag11InkMRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_read_string(cardId,
                                               (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,
                                               12,
                                               buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

HP_SMART_CARD_result_t writeTag11InkMRUPlatformID(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_write_string(cardId,
                                                (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,
                                                12,
                                                buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to write HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
// The most recent use (MRU) printer platform manufacture date - year.
//    m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b.
uint16_t readTag11InkMRUPlatformMFGYear(HP_SMART_CARD_device_id_t cardId) {
    uint16_t mru_platform_mfg_year = 2006;
    uint8_t buffer;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR,
                                          sizeof(buffer),
                                          &buffer);
    if (HP_SMART_CARD_OK == result) {
        mru_platform_mfg_year = buffer * 1 + 2006;
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR = %d", mru_platform_mfg_year);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR");
    }

    return mru_platform_mfg_year;
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
uint8_t readTag11InkMRUPlatformMFGWeekOfYear(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mru_platform_mfg_woy = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY,
                                          sizeof(mru_platform_mfg_woy),
                                          &mru_platform_mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY = %d", mru_platform_mfg_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY");
    }

    return mru_platform_mfg_woy;
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
uint8_t readTag11InkMRUPlatformMFGCountry(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mru_platform_mfg_country = HP_SMART_CARD_COUNTRY_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY,
                                          sizeof(mru_platform_mfg_country),
                                          &mru_platform_mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY = %d", mru_platform_mfg_country);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY");
    }

    return mru_platform_mfg_country;
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
uint8_t readTag11InkMRUPlatformFWRevMajor(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mru_platform_fw_rev_major = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR,
                                          sizeof(mru_platform_fw_rev_major),
                                          &mru_platform_fw_rev_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR = %d", mru_platform_fw_rev_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR");
    }

    return mru_platform_fw_rev_major;
}

// Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
uint8_t readTag11InkMRUPlatformFWRevMinor(HP_SMART_CARD_device_id_t cardId) {
    uint8_t mru_platform_fw_rev_minor = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR,
                                          sizeof(mru_platform_fw_rev_minor),
                                          &mru_platform_fw_rev_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR = %d", mru_platform_fw_rev_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR");
    }

    return mru_platform_fw_rev_minor;
}

// Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
uint8_t readTag11InkInsertionCount(HP_SMART_CARD_device_id_t cardId) {
    uint8_t insertion_count = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSERTION_COUNT,
                                          sizeof(insertion_count),
                                          &insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSERTION_COUNT = %d", insertion_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSERTION_COUNT");
    }

    return insertion_count;
}

// Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
uint8_t readTag11InkStallInsertionCount(HP_SMART_CARD_device_id_t cardId) {
    uint16_t stall_insertion_count = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_STALL_INSERTION_COUNT,
                                          sizeof(stall_insertion_count),
                                          &stall_insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_STALL_INSERTION_COUNT = %d", stall_insertion_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_STALL_INSERTION_COUNT");
    }

    return stall_insertion_count;
}

// Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
uint8_t readTag11InkLastFailureCode(HP_SMART_CARD_device_id_t cardId) {
    uint8_t last_failure_code = HP_SMART_CARD_INK_LAST_FAILURE_CODE_NOEEROR;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_LAST_FAILURE_CODE,
                                          sizeof(last_failure_code),
                                          &last_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_LAST_FAILURE_CODE = %d", last_failure_code);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_LAST_FAILURE_CODE");
    }

    return last_failure_code;
}

// Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
uint8_t readTag11InkUserReportedStatus(HP_SMART_CARD_device_id_t cardId) {
    uint8_t last_user_reported_status = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_REPORTED_STATUS,
                                          sizeof(last_user_reported_status),
                                          &last_user_reported_status);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_REPORTED_STATUS = %d", last_user_reported_status);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_REPORTED_STATUS");
    }

    return last_user_reported_status;
}

// Skipped (Tag 11 - Blank - 18, 0, 16)
// Skipped (Tag 11 - Blank - 20, 0, 24)

// Read HP_SMART_CARD_INK_MARKETING_DATA_REVISION (Tag 11 - Marketing Data Revision - 23, 0, 4)
uint8_t readTag11InkMarketingDataRevision(HP_SMART_CARD_device_id_t cardId) {
    uint8_t marketing_data_revision = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MARKETING_DATA_REVISION,
                                          sizeof(marketing_data_revision),
                                          &marketing_data_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MARKETING_DATA_REVISION = %d", marketing_data_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MARKETING_DATA_REVISION");
    }

    return marketing_data_revision;
}

// Skipped (Tag 11 - Blank - 24, 4, 24)
// Skipped (Tag 11 - Blank - 27, 4, 24)
// Skipped (Tag 11 - Blank - 30, 4, 21)

////////////////////////////////// TAG 12 ////////////////////////////////////////////////
// OEM Scratchpad, 8 bytes, Managed by OEM
// HP_SMART_CARD_INK_OEM_SCRATCH_PAD_TAG               = 0x0C,

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
uint32_t readTag12InkOEMDefRWField1(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_rw_filed_1 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1,
                                          sizeof(oem_rw_filed_1),
                                          &oem_rw_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 = %d", oem_rw_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1");
    }

    return oem_rw_filed_1;
}

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
HP_SMART_CARD_result_t writeTag12InkOEMDefRWField1(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_1) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1,
                                           sizeof(oem_rw_filed_1),
                                           &oem_rw_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 = %d", oem_rw_filed_1);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1");
    }

    return result;
}

// Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
uint32_t readTag12InkOEMDefRWField2(HP_SMART_CARD_device_id_t cardId) {
    uint32_t oem_rw_filed_2 = 0;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2,
                                          sizeof(oem_rw_filed_2),
                                          &oem_rw_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 = %d", oem_rw_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2");
    }

    return oem_rw_filed_2;
}

// Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
HP_SMART_CARD_result_t writeTag12InkOEMDefRWField2(HP_SMART_CARD_device_id_t cardId, uint32_t oem_rw_filed_2) {
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_write_field(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2,
                                           sizeof(oem_rw_filed_2),
                                           &oem_rw_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 = %d", oem_rw_filed_2);
    } else {
        LOGE(">>> Failed to write HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2");
    }

    return result;
}

////////////////////////////////// TAG 13 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by HP or Ink Filler
// HP_SMART_CARD_INK_EXT_OEM_ID_TAG                    = 0x0D,

// Read HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
// Bit indicating lock/unlock state of the OEM ID memory partition (Partition #7)
//    0 = OEM ID partition unlocked
//    1 = OEM ID partition locked
uint8_t readTag13InkUD7Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud7_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD7_FUSE,
                                          sizeof(ud7_fuse),
                                          &ud7_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD7_FUSE = %d", ud7_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD7_FUSE");
    }

    return ud7_fuse;
}

// Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
uint8_t readTag13InkExtendedOEMID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t extended_oem_id = HP_SMART_CARD_INK_EXTENDED_OEM_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_EXTENDED_OEM_ID,
                                          sizeof(extended_oem_id),
                                          &extended_oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_EXTENDED_OEM_ID = %d", extended_oem_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_EXTENDED_OEM_ID");
    }

    return extended_oem_id;
}

// Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
uint8_t readTag13InkOEMInkDesignator(HP_SMART_CARD_device_id_t cardId) {
    uint8_t oem_ink_designator = HP_SMART_CARD_INK_OEM_INK;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR,
                                          sizeof(oem_ink_designator),
                                          &oem_ink_designator);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR = %d", oem_ink_designator);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR");
    }

    return oem_ink_designator;
}

// Skipped (Tag 11 - Blank - 2, 5, 22)

////////////////////////////////// TAG 14 ////////////////////////////////////////////////
// Ext OEM ID, 4 bytes, Managed by Ink Filler
// HP_SMART_CARD_INK_REORDER_PART_NUM_TAG              = 0x0E,

// Read HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
// The region where this cartridge will be accepted by the printer.
//    0 = unknown
//    1 = universal
//    255 = test
uint8_t readTag14InkRegionalizationID(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ink_regionalization_id = HP_SMART_CARD_INK_REGIONALIZATION_ID_UNKNOWN;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_REGIONALIZATION_ID,
                                          sizeof(ink_regionalization_id),
                                          &ink_regionalization_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_REGIONALIZATION_ID = %d", ink_regionalization_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_REGIONALIZATION_ID");
    }

    return ink_regionalization_id;
}

// Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
HP_SMART_CARD_result_t readTag14InkReorderPartNum(HP_SMART_CARD_device_id_t cardId, unsigned char *buffer) {
    if(NULL != buffer) {
        HP_SMART_CARD_result_t result;
        result = LIB_HP_SMART_CARD_read_string(cardId,
                                               (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1,
                                               12,
                                               buffer);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR = %s", buffer);
        } else {
            LOGE(">>> Failed to read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR");
        }

        return result;
    } else {
        return HP_SMART_CARD_ERROR;
    }
}

// Read HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)
// Bit indicating lock/unlock state of the Re-order part number memory partition (Partition #8)
//    0 = Re-order partition unlocked
//    1 = Re-order partition locked
uint8_t readTag14InkUD8Fuse(HP_SMART_CARD_device_id_t cardId) {
    uint8_t ud8_fuse = HP_SMART_CARD_FUSE_UNLOCKED;
    HP_SMART_CARD_result_t result;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD8_FUSE,
                                          sizeof(ud8_fuse),
                                          &ud8_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD8_FUSE = %d", ud8_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD8_FUSE");
    }

    return ud8_fuse;
}

// Skipped (Tag 14 - Blank - 11, 6, 15)
// Skipped (Tag 14 - HP reserved - 13, 7, 224)
// Skipped (Tag 14 - Parity Byte - 41, 7, 8)
// Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)




////////////////////////////////////////////////////////////////////////////////////////////////
/*
    HP_SMART_CARD_result_t result;

    ////////////////////////////////// TAG 0 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
    uint8_t chip_tag;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_CHIP_TAG,
                                          sizeof(chip_tag),
                                          &chip_tag);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_CHIP_TAG = %d", chip_tag);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_CHIP_TAG");
    }

    // Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
    uint8_t reserved_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_RESERVED_1,
                                          sizeof(reserved_1),
                                          &reserved_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_RESERVED_1 = %d", reserved_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_RESERVED_1");
    }

    // Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
    uint8_t family_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_FAMILY_ID,
                                          sizeof(family_id),
                                          &family_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_FAMILY_ID = %d", family_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_FAMILY_ID");
    }

    // Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
    uint8_t approved_hp_oem;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_APPROVED_HP_OEM_BIT,
                                          sizeof(approved_hp_oem),
                                          &approved_hp_oem);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_APPROVED_HP_OEM_BIT = %d", approved_hp_oem);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_APPROVED_HP_OEM_BIT");
    }

    // Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
    uint8_t oem_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_OEM_ID,
                                          sizeof(oem_id),
                                          &oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_OEM_ID = %d", oem_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_OEM_ID");
    }

    // Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
    uint8_t address_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_ADDRESS_POSITION,
                                          sizeof(address_position),
                                          &address_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_ADDRESS_POSITION = %d", address_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_ADDRESS_POSITION");
    }

    // Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
    uint8_t temp_ver_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MAJOR,
                                          sizeof(temp_ver_major),
                                          &temp_ver_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_TEMPLATE_VERSION_MAJOR = %d", temp_ver_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR");
    }

    // Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
    uint8_t temp_ver_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MINOR,
                                          sizeof(temp_ver_minor),
                                          &temp_ver_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_TEMPLATE_VERSION_MINOR = %d", temp_ver_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MINOR");
    }

    // Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

    // Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
    uint8_t perso_revision;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_MEMORY_PARTITION_REV,
                                          sizeof(perso_revision),
                                          &perso_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_MEMORY_PARTITION_REV = %d", perso_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_MEMORY_PARTITION_REV");
    }

    // Skipped (Tag 0 - Reserved for Perso - 6, 3, 4)
    // Skipped (Tag 0 - Blank - 7, 7, 8)

    ////////////////////////////////// TAG 1 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
    // Bit indicating lock/unlock state of the Dry Cartridge memory partition (Partition #0)
    //    0 = Dry cartridge mfg partition unlocked
    //    1 = Dry cartridge mfg partition locked
    uint8_t ud0_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD0_FUSE,
                                          sizeof(ud0_fuse),
                                          &ud0_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD0_FUSE = %d", ud0_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD0_FUSE");
    }

    // Skipped HP_SMART_CARD_INK_BLANK_1 (Tag 1 - Blank - 1, 6, 8)
    // Skipped HP_SMART_CARD_INK_BLANK_2 (Tag 1 - Blank - 2, 6, 1)
    // HP_SMART_CARD_INK_EXTENDED_OEM_ID seems not an item of Tag1, might be (Tag 13 - Extended OEM ID - 1, 6, 8)
    // HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR seems not an item of Tag1, might be (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)

    // Read HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
    uint8_t manufacture_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_ID,
                                          sizeof(manufacture_id),
                                          &manufacture_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_ID = %d", manufacture_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_ID");
    }

    // Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
    uint8_t manufacture_line;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_LINE,
                                          sizeof(manufacture_line),
                                          &manufacture_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_LINE = %d", manufacture_line);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_LINE");
    }

    // Read HP_SMART_CARD_INK_MANUFACTURE_YEAR (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Year - 3, 2, 5)
    // Year the dry cartridge was manufactured and the year the cartridge was filled with ink.
    // Use the variables m = 1, b = 2006, range = 2006 - 2037, in the equation = mx + b where the y slope equals 1, and the intercept is 2006.
    uint8_t manufacture_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_YEAR,
                                          sizeof(manufacture_year),
                                          &manufacture_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_YEAR = %d", manufacture_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_YEAR");
    }

    // Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
    uint8_t manufacture_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_WEEK,
                                          sizeof(manufacture_woy),
                                          &manufacture_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MANUFACTURE_WEEK = %d", manufacture_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MANUFACTURE_WEEK");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
    uint8_t manufacture_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_DOW,
                                          sizeof(manufacture_dow),
                                          &manufacture_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_DOW = %d", manufacture_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_DOW");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
    uint8_t manufacture_hod;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_HOD,
                                          sizeof(manufacture_hod),
                                          &manufacture_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_HOD = %d", manufacture_hod);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_HOD");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
    uint8_t manufacture_moh;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_MOH,
                                          sizeof(manufacture_moh),
                                          &manufacture_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_MOH = %d", manufacture_moh);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_MOH");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
    uint8_t manufacture_som;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_SOM,
                                          sizeof(manufacture_som),
                                          &manufacture_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SERIAL_NUM_SOM = %d", manufacture_som);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SERIAL_NUM_SOM");
    }

    // Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
    uint8_t process_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DRY_PROCESS_POSITION,
                                          sizeof(process_position),
                                          &process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_DRY_PROCESS_POSITION = %d", process_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DRY_PROCESS_POSITION");
    }

    // Read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME (Tag 1 - Max Usable Cartridge Volume - 7, 0, 16)
    // Maximum usable cartridge volume
    //    m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
    uint16_t max_usable_cartridge_volume;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME,
                                          sizeof(max_usable_cartridge_volume),
                                          &max_usable_cartridge_volume);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME = %f", max_usable_cartridge_volume * 0.1 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME");
    }

    // Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Printer Lockdown Partition - 9, 0, 5)
    uint8_t  lock_down_partition;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION,
                                          sizeof(lock_down_partition),
                                          &lock_down_partition);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION = %d", lock_down_partition);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION");
    }

    // Skipped (Tag 1 - Blank - 10, 3, 20)

    ////////////////////////////////// TAG 2 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_TSR (Tag 2 - Thermal Sense Resistor (TSR) - 1, 7, 7)
    // Value of the thermal sense resistor:
    //    m = 1 b = 250 range = 250 –377 in equation y = mx + b
    uint8_t ink_tsr;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TSR,
                                          sizeof(ink_tsr),
                                          &ink_tsr);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TSR = %d", ink_tsr * 1 + 250);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TSR");
    }

    // Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
    // Value of the TSR thermal coefficient
    //    m = 5 b = 2900 range = 2900 in equation y = mx + b
    uint8_t ink_tcr;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT,
                                          sizeof(ink_tcr),
                                          &ink_tcr);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT = %d", ink_tcr * 5 + 2900);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT");
    }

    // Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
    // Bulk cartridge identifier
    //    0 = single use cartridge
    //    1 = bulk ink cartridge
    uint8_t bulk_ctrdg;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_BULK_CTRDG,
                                          sizeof(bulk_ctrdg),
                                          &bulk_ctrdg);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_BULK_CTRDG = %d", bulk_ctrdg);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_BULK_CTRDG");
    }

    // Skipped (Tag 2 - Blank - 3, 7, 16)
    // Skipped (Tag 2 - Blank - 5, 7, 32)
    // Skipped (Tag 2 - Blank - 9, 7, 32)
    // Skipped (Tag 2 - Blank - 13, 7, 32)

    ////////////////////////////////// TAG 3 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1)
    // state of the Ink Fill memory partition (Partition #1)
    //    0 = Ink fill partition unlocked
    //    1 = Ink fill partition locked
    uint8_t ud1_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD1_FUSE,
                                          sizeof(ud1_fuse),
                                          &ud1_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD1_FUSE = %d", ud1_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD1_FUSE");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
    uint8_t site_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID,
                                          sizeof(site_id),
                                          &site_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID = %d", site_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
    uint8_t fill_line;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE,
                                          sizeof(fill_line),
                                          &fill_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE = %d", fill_line);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
    // Year that cartridge was filled m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t fill_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR,
                                          sizeof(fill_year),
                                          &fill_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR = %d", fill_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
    uint8_t fill_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY,
                                          sizeof(fill_woy),
                                          &fill_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY = %d", fill_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
    uint8_t fill_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW,
                                          sizeof(fill_dow),
                                          &fill_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW = %d", fill_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
    uint8_t fill_hod;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD,
                                          sizeof(fill_hod),
                                          &fill_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD = %d", fill_hod);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
    uint8_t fill_moh;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH,
                                          sizeof(fill_moh),
                                          &fill_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH = %d", fill_moh);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
    uint8_t fill_som;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM,
                                          sizeof(fill_som),
                                          &fill_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM = %d", fill_som);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
    uint8_t fill_process_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION,
                                          sizeof(fill_process_position),
                                          &fill_process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION = %d", fill_process_position);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION");
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

    // Read HP_SMART_CARD_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
    uint8_t formulator_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FORMULATOR_ID,
                                          sizeof(formulator_id),
                                          &formulator_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FORMULATOR_ID = %d", formulator_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FORMULATOR_ID");
    }

    // Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
    uint8_t ink_family;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY,
                                          sizeof(ink_family),
                                          &ink_family);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_FAMILY = %d", ink_family);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_FAMILY");
    }

    // Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
    uint8_t ink_color_code_general;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_GENERAL,
                                          sizeof(ink_color_code_general),
                                          &ink_color_code_general);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_COLOR_CODES_GENERAL = %d", ink_color_code_general);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_COLOR_CODES_GENERAL");
    }

    // Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
    uint8_t ink_color_code_specific;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC,
                                          sizeof(ink_color_code_specific),
                                          &ink_color_code_specific);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC = %d", ink_color_code_specific);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC");
    }

    // Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
    uint8_t ink_family_member;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY_MEMBER,
                                          sizeof(ink_family_member),
                                          &ink_family_member);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_FAMILY_MEMBER = %d", ink_family_member);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_FAMILY_MEMBER");
    }

    // Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
    uint16_t ink_id_num;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_ID_NUM,
                                          sizeof(ink_id_num),
                                          &ink_id_num);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_ID_NUM = %d", ink_id_num);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_ID_NUM");
    }

    // Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
    uint8_t ink_revision;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_REVISION,
                                          sizeof(ink_revision),
                                          &ink_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_REVISION = %d", ink_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_REVISION");
    }

    // Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
    // Specifies the ink density.
    //      m = 0.001 b = 0.7 range = 0.700 – 1.724 in equation y = mx + b
    uint16_t ink_density;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DENSITY,
                                          sizeof(ink_density),
                                          &ink_density);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_DENSITY = %d", ink_density * 0.001 + 0.7);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DENSITY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
    uint8_t ink_cartridge_distinction;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION,
                                          sizeof(ink_cartridge_distinction),
                                          &ink_cartridge_distinction);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION = %d", ink_cartridge_distinction);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION");
    }

    // Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
    uint8_t ink_supply_keying_descriptor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR,
                                          sizeof(ink_supply_keying_descriptor),
                                          &ink_supply_keying_descriptor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR = %d", ink_cartridge_distinction);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR");
    }

    // Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
    uint8_t ink_shelf_life_weeks;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_WEEKS,
                                          sizeof(ink_shelf_life_weeks),
                                          &ink_shelf_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SHELF_LIFE_WEEKS = %d", ink_shelf_life_weeks);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS");
    }

    // Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
    uint8_t ink_shelf_life_days;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_DAYS,
                                          sizeof(ink_shelf_life_days),
                                          &ink_shelf_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_SHELF_LIFE_DAYS = %d", ink_shelf_life_days);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_SHELF_LIFE_DAYS");
    }

    // Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
    uint8_t ink_installed_life_weeks;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS,
                                          sizeof(ink_installed_life_weeks),
                                          &ink_installed_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS = %d", ink_installed_life_weeks);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS");
    }

    // Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
    uint8_t ink_installed_life_days;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS,
                                          sizeof(ink_installed_life_days),
                                          &ink_installed_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS = %d", ink_installed_life_days);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS");
    }

    // Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
    // m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
    uint16_t ink_usable_ink_weight;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USABLE_INK_WEIGHT,
                                          sizeof(ink_usable_ink_weight),
                                          &ink_usable_ink_weight);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USABLE_INK_WEIGHT = %f", ink_usable_ink_weight * 0.1 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USABLE_INK_WEIGHT");
    }

    // Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
    // Percentage Value: 10 means 10%
    uint8_t ink_notification_level;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL,
                                          sizeof(ink_notification_level),
                                          &ink_notification_level);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL = %d", ink_notification_level);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL");
    }

    // Skipped (Tag 4 - Blank - 15, 4, 15)

    ////////////////////////////////// TAG 5 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_FIRING_FREQ (Tag 5 - Firing Frequency - 1, 7, 7)
    // Maximum firing frequency of the cartridge, based on the filled ink.
    //    m = 0.2, b = 0, range = 0 – 25.4 in equation y = mx + b
    uint8_t firing_freq;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRING_FREQ,
                                          sizeof(firing_freq),
                                          &firing_freq);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRING_FREQ = %f", firing_freq * 0.2 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRING_FREQ");
    }

    // Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
    // Firing pulse width, based on the filled ink type. m = 0.01 b = 1.5, range = 1.50 – 4.05 in equation y = mx + b
    uint8_t pulse_width;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WIDTH,
                                          sizeof(pulse_width),
                                          &pulse_width);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WIDTH = %f", pulse_width * 0.1 + 1.5);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PULSE_WIDTH");
    }

    // Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
    // Firing voltage required by the cartridge, based on the filled ink type. m = 0.1, b = 5.0, range = 5.0 – 17.7 y = mx + b
    uint8_t firing_voltage;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRING_VOLTAGE,
                                          sizeof(firing_voltage),
                                          &firing_voltage);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRING_VOLTAGE = %f", firing_voltage * 0.1 + 5.0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRING_VOLTAGE");
    }

    // Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
    // Turn on energy for a firing resistor, based on the filled ink type. m = 0.02, b = 2.0, range = 2.0 – 7.1 y = mx + b
    uint8_t turn_on_energy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TURN_ON_ENERGY,
                                          sizeof(turn_on_energy),
                                          &turn_on_energy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TURN_ON_ENERGY = %f", turn_on_energy * 0.02 + 2.0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TURN_ON_ENERGY");
    }

    // Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
    // Pulse warming temperature required for optimal firing for this cartridge, based on the filled ink type. m = 1, b = 0, range = 0 – 127 y = mx + b
    uint8_t pulse_warming_temp;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WARMING_TEMP,
                                          sizeof(pulse_warming_temp),
                                          &pulse_warming_temp);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_PULSE_WARMING_TEMP = %d", pulse_warming_temp * 1 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_PULSE_WARMING_TEMP");
    }

    // Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
    // Maximum head temperature for this cartridge, based on the filled ink type. m = 1, b = 0, range = 0 – 127 y = mx + b
    uint8_t max_temp;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MAX_TEMP,
                                          sizeof(max_temp),
                                          &max_temp);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MAX_TEMP = %d", max_temp * 1 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MAX_TEMP");
    }

    // Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
    // Ink drop volume, based on filled ink type firing parameters. m = 1, b = 0, range = 0 – 63 y = mx + b
    uint8_t drop_volume;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DROP_VOLUME,
                                          sizeof(drop_volume),
                                          &drop_volume);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_DROP_VOLUME = %d", drop_volume * 1 + 0);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_DROP_VOLUME");
    }

    // Skipped (Tag 5 - Blank - 7, 5, 14)
    // Skipped (Tag 5 - Blank - 9, 7, 32)

    ////////////////////////////////// TAG 6 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_WRITE_PROTECT (Tag 6 - Write Protect Fuse - 1, 7, 1)
    // Bit indicating lock/unlock state of the Printer 1st Install memory partition (Partition #2)
    //    0 = OEM Lockable partition unlocked
    //    1 = OEM Lockable partition locked
    uint8_t write_protect;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_WRITE_PROTECT,
                                          sizeof(write_protect),
                                          &write_protect);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_WRITE_PROTECT = %d", write_protect);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_WRITE_PROTECT");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
    unsigned char first_platform_id[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR_1,
                                           12,
                                           first_platform_id);
    first_platform_id[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR = %s", first_platform_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
    // The first install printer platform manufacture date - year. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t mfg_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR,
                                          sizeof(mfg_year),
                                          &mfg_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR = %d", mfg_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
    uint8_t mfg_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY,
                                          sizeof(mfg_woy),
                                          &mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY = %d", mfg_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
    uint8_t mfg_country;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY,
                                          sizeof(mfg_country),
                                          &mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY = %d", mfg_country);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
    uint8_t fw_revision_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION,
                                          sizeof(fw_revision_major),
                                          &fw_revision_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION = %d", fw_revision_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
    uint8_t fw_revision_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION,
                                          sizeof(fw_revision_minor),
                                          &fw_revision_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION = %d", fw_revision_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
    uint16_t first_install_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT,
                                          sizeof(first_install_count),
                                          &first_install_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT = %d", first_install_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
    // The year of the first installation of this cartridge into this printer platform. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t first_install_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_YEAR,
                                          sizeof(first_install_year),
                                          &first_install_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_YEAR = %d", first_install_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
    uint8_t first_install_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_WOY,
                                          sizeof(first_install_woy),
                                          &first_install_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_WOY = %d", first_install_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_WOY");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
    uint8_t first_install_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_DOW,
                                          sizeof(first_install_dow),
                                          &first_install_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_DOW = %d", first_install_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_DOW");
    }

    // Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
    uint8_t gauge_resolution;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION,
                                          sizeof(gauge_resolution),
                                          &gauge_resolution);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION = %d", gauge_resolution);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
    uint32_t oem_field;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD,
                                          sizeof(oem_field),
                                          &oem_field);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD = %d", oem_field);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD");
    }

    // Skipped (Tag 6 - Blank - 20, 7, 8)

    ////////////////////////////////// TAG 7 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
    // Bit indicating lock/unlock state of the OEM Lockable memory partition (Partition #3)
    //    0 = OEM Lockable partition unlocked
    //    1 = OEM Lockable partition locked
    uint8_t ud3_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD3_FUSE,
                                          sizeof(ud3_fuse),
                                          &ud3_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD3_FUSE = %d", ud3_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD3_FUSE");
    }

    // Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
    uint32_t oem_defined_filed_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1,
                                          sizeof(oem_defined_filed_1),
                                          &oem_defined_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 = %d", oem_defined_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
    uint32_t oem_defined_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2,
                                          sizeof(oem_defined_filed_2),
                                          &oem_defined_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 = %d", oem_defined_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2");
    }

    ////////////////////////////////// TAG 8 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR (Tag 8 - Trademark String - 5 Chars - 1, 7, 30)
    unsigned char ink_trademark[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_TRADEMARK_CHAR_1,
                                           6,
                                           ink_trademark);
    first_platform_id[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_HP_TRADEMARK_CHAR = %s", first_platform_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR");
    }

    // Read HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
    // Bit indicating lock/unlock state of the Trademark memory partition (Partition #4)
    //    0 = Trademark partition unlocked
    //    1 = Trademark partition locked
    uint8_t ud4_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD4_FUSE,
                                          sizeof(ud4_fuse),
                                          &ud4_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD4_FUSE = %d", ud4_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD4_FUSE");
    }

    // Skipped (Tag 8 - Blank - 4, 0, 1)

    ////////////////////////////////// TAG 9 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
    // This bit is set by the printer once the ILG indicates out of ink condition.
    //    0 = not reached,
    //    1 = reached
    uint8_t out_of_ink;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OUT_OF_INK_BIT,
                                          sizeof(out_of_ink),
                                          &out_of_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OUT_OF_INK_BIT = %d", out_of_ink);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OUT_OF_INK_BIT");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
    // Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_25_01;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_01_25,
                                          sizeof(ilg_bit_25_01),
                                          &ilg_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_01_25 = %d", ilg_bit_25_01);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_01_25");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
    // Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_50_26;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_26_50,
                                          sizeof(ilg_bit_50_26),
                                          &ilg_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_26_50 = %d", ilg_bit_50_26);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_26_50");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
    // Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_75_51;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_51_75,
                                          sizeof(ilg_bit_75_51),
                                          &ilg_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_51_75 = %d", ilg_bit_75_51);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_51_75");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
    // Ink level gauge bits 76-100 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_100_76;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_76_100,
                                          sizeof(ilg_bit_100_76),
                                          &ilg_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ILG_BIT_76_100 = %d", ilg_bit_100_76);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ILG_BIT_76_100");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
    // Total Ink Usage Gauge bits 1 - 25 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_25_01;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_01_25,
                                          sizeof(tiug_bit_25_01),
                                          &tiug_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_01_25 = %d", tiug_bit_25_01);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_01_25");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
    // Total Ink Usage Gauge bits 26 - 50 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_50_26;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_26_50,
                                          sizeof(tiug_bit_50_26),
                                          &tiug_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_26_50 = %d", tiug_bit_50_26);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_26_50");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
    // Total Ink Usage Gauge bits 51 - 75 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_75_51;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_51_75,
                                          sizeof(tiug_bit_75_51),
                                          &tiug_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_51_75 = %d", tiug_bit_75_51);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_51_75");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
    // Total Ink Usage Gauge bits 76 - 100 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_100_76;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_76_100,
                                          sizeof(tiug_bit_100_76),
                                          &tiug_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_TUG_BIT_76_100 = %d", tiug_bit_100_76);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_TUG_BIT_76_100");
    }

    // Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
    uint8_t first_failure_code;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_FAILURE_CODE,
                                          sizeof(first_failure_code),
                                          &first_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FIRST_FAILURE_CODE = %d", first_failure_code);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FIRST_FAILURE_CODE");
    }

    // Read HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
    // This bit is set by the printer when the cartridge has positively exceeded available ink.
    // This is set once the TIUG target has been reached.
    //    0 = not reached,
    //    1 = reached
    uint8_t altered_supply;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ALTERED_SUPPLY,
                                          sizeof(altered_supply),
                                          &altered_supply);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_ALTERED_SUPPLY = %d", altered_supply);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY");
    }

    // Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
    uint8_t user_ack_altered_supply;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK,
                                          sizeof(user_ack_altered_supply),
                                          &user_ack_altered_supply);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK = %d", user_ack_altered_supply);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK");
    }

    // Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
    uint8_t user_ack_expired_ink;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK,
                                          sizeof(user_ack_expired_ink),
                                          &user_ack_expired_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK = %d", user_ack_expired_ink);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK");
    }

    // Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
    uint8_t faulty_replace_immediately;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY,
                                          sizeof(faulty_replace_immediately),
                                          &faulty_replace_immediately);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY = %d", faulty_replace_immediately);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY");
    }

    // Skipped (Tag 9 - Blank - 27, 4, 32)
    // Skipped (Tag 9 - Blank - 31, 4, 13)

    ////////////////////////////////// TAG 10 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 (Tag 10 - OEM defined RW_OR Field 1 - 1, 7, 32)
    uint32_t oem_rw_or_filed_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1,
                                          sizeof(oem_rw_or_filed_1),
                                          &oem_rw_or_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 = %d", oem_rw_or_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
    uint32_t oem_rw_or_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2,
                                          sizeof(oem_rw_or_filed_2),
                                          &oem_rw_or_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 = %d", oem_rw_or_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2");
    }

    ////////////////////////////////// TAG 11 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR (Tag 11 - Cartridge MRU Year - 1, 7, 5)
    // Most recent use (MRU) of this cartridge - year. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t cartridge_mru_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR,
                                          sizeof(cartridge_mru_year),
                                          &cartridge_mru_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR = %d", cartridge_mru_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
    uint8_t cartridge_mru_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY,
                                          sizeof(cartridge_mru_woy),
                                          &cartridge_mru_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY = %d", cartridge_mru_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
    uint8_t cartridge_mru_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW,
                                          sizeof(cartridge_mru_dow),
                                          &cartridge_mru_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW = %d", cartridge_mru_dow);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 11 - MRU Platform ID - 12 Char - 2, 1, 72)
    unsigned char mru_platform_id[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,
                                           12,
                                           mru_platform_id);
    mru_platform_id[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR = %s", mru_platform_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
    // The most recent use (MRU) printer platform manufacture date - year. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b.
    uint8_t mru_platform_mfg_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR,
                                          sizeof(mru_platform_mfg_year),
                                          &mru_platform_mfg_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR = %d", mru_platform_mfg_year * 1 + 2006);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
    uint8_t mru_platform_mfg_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY,
                                          sizeof(mru_platform_mfg_woy),
                                          &mru_platform_mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY = %d", mru_platform_mfg_woy);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
    uint8_t mru_platform_mfg_country;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY,
                                          sizeof(mru_platform_mfg_country),
                                          &mru_platform_mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY = %d", mru_platform_mfg_country);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
    uint8_t mru_platform_fw_rev_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR,
                                          sizeof(mru_platform_fw_rev_major),
                                          &mru_platform_fw_rev_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR = %d", mru_platform_fw_rev_major);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
    uint8_t mru_platform_fw_rev_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR,
                                          sizeof(mru_platform_fw_rev_minor),
                                          &mru_platform_fw_rev_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR = %d", mru_platform_fw_rev_minor);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR");
    }

    // Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
    uint8_t insertion_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSERTION_COUNT,
                                          sizeof(insertion_count),
                                          &insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_INSERTION_COUNT = %d", insertion_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_INSERTION_COUNT");
    }

    // Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
    uint16_t stall_insertion_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_STALL_INSERTION_COUNT,
                                          sizeof(stall_insertion_count),
                                          &stall_insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_STALL_INSERTION_COUNT = %d", stall_insertion_count);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_STALL_INSERTION_COUNT");
    }

    // Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
    uint8_t last_failure_code;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_LAST_FAILURE_CODE,
                                          sizeof(last_failure_code),
                                          &last_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_LAST_FAILURE_CODE = %d", last_failure_code);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_LAST_FAILURE_CODE");
    }

    // Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
    uint8_t last_user_reported_status;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_REPORTED_STATUS,
                                          sizeof(last_user_reported_status),
                                          &last_user_reported_status);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_USER_REPORTED_STATUS = %d", last_user_reported_status);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_USER_REPORTED_STATUS");
    }

    // Skipped (Tag 11 - Blank - 18, 0, 16)
    // Skipped (Tag 11 - Blank - 20, 0, 24)

    // Read HP_SMART_CARD_INK_MARKETING_DATA_REVISION (Tag 11 - Marketing Data Revision - 23, 0, 4)
    uint8_t marketing_data_revision;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MARKETING_DATA_REVISION,
                                          sizeof(marketing_data_revision),
                                          &marketing_data_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_MARKETING_DATA_REVISION = %d", marketing_data_revision);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_MARKETING_DATA_REVISION");
    }

    // Skipped (Tag 11 - Blank - 24, 4, 24)
    // Skipped (Tag 11 - Blank - 27, 4, 24)
    // Skipped (Tag 11 - Blank - 30, 4, 21)

    ////////////////////////////////// TAG 12 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 (Tag 12 - OEM defined RW Field 1 - 1, 7, 32)
    uint32_t oem_rw_filed_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1,
                                          sizeof(oem_rw_filed_1),
                                          &oem_rw_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 = %d", oem_rw_filed_1);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
    uint32_t oem_rw_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2,
                                          sizeof(oem_rw_filed_2),
                                          &oem_rw_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 = %d", oem_rw_filed_2);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2");
    }

    ////////////////////////////////// TAG 13 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
    // Bit indicating lock/unlock state of the OEM ID memory partition (Partition #7)
    //    0 = OEM ID partition unlocked
    //    1 = OEM ID partition locked
    uint8_t ud7_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD7_FUSE,
                                          sizeof(ud7_fuse),
                                          &ud7_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD7_FUSE = %d", ud7_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD7_FUSE");
    }

    // Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
    uint8_t extended_oem_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_EXTENDED_OEM_ID,
                                          sizeof(extended_oem_id),
                                          &extended_oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_EXTENDED_OEM_ID = %d", extended_oem_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_EXTENDED_OEM_ID");
    }

    // Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
    uint8_t oem_ink_designator;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR,
                                          sizeof(oem_ink_designator),
                                          &oem_ink_designator);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR = %d", oem_ink_designator);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR");
    }

    // Skipped (Tag 11 - Blank - 2, 5, 22)

    ////////////////////////////////// TAG 14 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
    // The region where this cartridge will be accepted by the printer.
    //    0 = unknown
    //    1 = universal
    //    255 = test
    uint8_t ink_regionalization_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_REGIONALIZATION_ID,
                                          sizeof(ink_regionalization_id),
                                          &ink_regionalization_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_REGIONALIZATION_ID = %d", ink_regionalization_id);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_REGIONALIZATION_ID");
    }

    // Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
    unsigned char reorder_part_num[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1,
                                           12,
                                           reorder_part_num);
    reorder_part_num[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR = %s", reorder_part_num);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR");
    }

    // Read HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)
    // Bit indicating lock/unlock state of the Re-order part number memory partition (Partition #8)
    //    0 = Re-order partition unlocked
    //    1 = Re-order partition locked
    uint8_t ud8_fuse;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_UD8_FUSE,
                                          sizeof(ud8_fuse),
                                          &ud8_fuse);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> HP_SMART_CARD_INK_UD8_FUSE = %d", ud8_fuse);
    } else {
        LOGE(">>> Failed to read HP_SMART_CARD_INK_UD8_FUSE");
    }

    // Skipped (Tag 14 - Blank - 11, 6, 15)
    // Skipped (Tag 14 - HP reserved - 13, 7, 224)
    // Skipped (Tag 14 - Parity Byte - 41, 7, 8)
    // Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)

 */

////////////////////////////////////////////////////////////////////////////////////////////////
