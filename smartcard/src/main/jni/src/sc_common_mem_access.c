//
// Created by hmwan on 2020/4/26.
//

#include <unistd.h>
#include <hp_debug_log_internal.h>
#include <common_log.h>
#include "sc_common_mem_access.h"

extern char *inkFamilyGetFiledName(uint8_t id);
extern char *supplyFamilyGetFiledName(uint8_t id);

// static char *(*FIELD_NAME[HP_SMART_CARD_DEVICE_NUM_DEVICES])(uint8_t id) = {tag0GetFiledName, tag0GetFiledName};

// H.M.Wang 2020-12-16 定义移到.c里面做，如果在.h里面做的话，可能会出现重复定义
char *(*FIELD_NAME[HP_SMART_CARD_DEVICE_NUM_DEVICES])(uint8_t id);
// End of H.M.Wang 2020-12-16 定义移到.c里面做，如果在.h里面做的话，可能会出现重复定义

HP_SMART_CARD_result_t readByte(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t *data) {
    HP_SMART_CARD_result_t result = LIB_HP_SMART_CARD_read_field(cardId, fieldId, sizeof(*data), data);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Read %s = 0x%02x", FIELD_NAME[cardId](fieldId), *data);
    } else {
        LOGE(">>> Failed to read %s", FIELD_NAME[cardId](fieldId));
    }

    return result;
}

HP_SMART_CARD_result_t writeByte(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t data) {
    HP_SMART_CARD_result_t result = HP_SMART_CARD_ERROR;

    for(int i=0; i<100 && result != HP_SMART_CARD_OK; i++) {
        result = LIB_HP_SMART_CARD_write_field(cardId, fieldId, sizeof(data), &data);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write %s = 0x%02x", FIELD_NAME[cardId](fieldId), data);

            usleep(1000);                 // 10 ms delay
            result = LIB_HP_SMART_CARD_flush();
            if (HP_SMART_CARD_OK != result) {
                LOGE(">>> Failed to flush %s", FIELD_NAME[cardId](fieldId));
            }
        } else {
            LOGE(">>> Failed to write %s", FIELD_NAME[cardId](fieldId));
        }
    }

    return result;
}

HP_SMART_CARD_result_t read2Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint16_t *data) {
    HP_SMART_CARD_result_t result = LIB_HP_SMART_CARD_read_field(cardId, fieldId, sizeof(*data), data);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Read %s = 0x%04x", FIELD_NAME[cardId](fieldId), *data);
    } else {
        LOGE(">>> Failed to read %s", FIELD_NAME[cardId](fieldId));
    }

    return result;
}

HP_SMART_CARD_result_t write2Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint16_t data) {
    HP_SMART_CARD_result_t result = HP_SMART_CARD_ERROR;

    for(int i=0; i<100 && result != HP_SMART_CARD_OK; i++) {
        result = LIB_HP_SMART_CARD_write_field(cardId, fieldId, sizeof(data), &data);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write %s = 0x%04x", FIELD_NAME[cardId](fieldId), data);

            usleep(1000);                 // 10 ms delay
            result = LIB_HP_SMART_CARD_flush();
            if (HP_SMART_CARD_OK != result) {
                LOGE(">>> Failed to flush %s", FIELD_NAME[cardId](fieldId));
            }
        } else {
            LOGE(">>> Failed to write %s", FIELD_NAME[cardId](fieldId));
        }
    }

    return result;
}

HP_SMART_CARD_result_t read4Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint32_t *data) {
    HP_SMART_CARD_result_t result = LIB_HP_SMART_CARD_read_field(cardId, fieldId, sizeof(*data), data);
    if (HP_SMART_CARD_OK == result) {
        LOGD(">>> Read %s = 0x%08x", FIELD_NAME[cardId](fieldId), *data);
    } else {
        LOGE(">>> Failed to read %s", FIELD_NAME[cardId](fieldId));
    }

    return result;
}

HP_SMART_CARD_result_t write4Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint32_t data) {
    HP_SMART_CARD_result_t result = HP_SMART_CARD_ERROR;

    for(int i=0; i<100 && result != HP_SMART_CARD_OK; i++) {
        result = LIB_HP_SMART_CARD_write_field(cardId, fieldId, sizeof(data), &data);
        if (HP_SMART_CARD_OK == result) {
            LOGD(">>> Write %s = 0x%08x", FIELD_NAME[cardId](fieldId), data);

            usleep(1000);                 // 1 ms delay
            result = LIB_HP_SMART_CARD_flush();
            if (HP_SMART_CARD_OK != result) {
                LOGE(">>> Failed to flush %s", FIELD_NAME[cardId](fieldId));
            }
        } else {
            LOGE(">>> Failed to write %s", FIELD_NAME[cardId](fieldId));
        }
    }

    return result;
}

HP_SMART_CARD_result_t readString(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t size, void *data) {
    if(NULL == data) {
        LOGE(">>> Null pointer for data buffer to read %s", FIELD_NAME[cardId](fieldId));
        return HP_SMART_CARD_ERROR;
    }

    HP_SMART_CARD_result_t result = LIB_HP_SMART_CARD_read_string(cardId, fieldId, size, data);
    if (HP_SMART_CARD_OK == result) {
        char buf[1024] = {0x00};
        toHexString(data, buf, size, ',');
        LOGD(">>> Read %s = [%s]", FIELD_NAME[cardId](fieldId), buf);
    } else {
        LOGE(">>> Failed to read %s", FIELD_NAME[cardId](fieldId));
    }
    return result;
}

HP_SMART_CARD_result_t writeString(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t size, void *data) {
    if(NULL == data) {
        LOGE(">>> Null pointer for data buffer to read %s", FIELD_NAME[cardId](fieldId));
        return HP_SMART_CARD_ERROR;
    }

    HP_SMART_CARD_result_t result = HP_SMART_CARD_ERROR;

    for(int i=0; i<100 && result != HP_SMART_CARD_OK; i++) {
        result = LIB_HP_SMART_CARD_write_string(cardId, fieldId, size, data);
        if (HP_SMART_CARD_OK == result) {
            char buf[1024] = {0x00};
            toHexString(data, buf, size, ',');
            LOGD(">>> Write %s = [%s]", FIELD_NAME[cardId](fieldId), buf);

            usleep(1000);                 // 1 ms delay
            result = LIB_HP_SMART_CARD_flush();
            if (HP_SMART_CARD_OK != result) {
                LOGE(">>> Failed to flush %s", FIELD_NAME[cardId](fieldId));
            }
        } else {
            LOGE(">>> Failed to write %s", FIELD_NAME[cardId](fieldId));
        }
    }

    return result;
}

////////////////////////////////// TAG 0 ////////////////////////////////////////////////
// Smart card header area

// Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
HP_SMART_CARD_result_t readTag0ChipTag(HP_SMART_CARD_device_id_t cardId, uint8_t *chip_tag) {
    return readByte(cardId, HP_SMART_CARD_CHIP_TAG, chip_tag);
}

// Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
HP_SMART_CARD_result_t readTag0Recerved1(HP_SMART_CARD_device_id_t cardId, uint8_t *reserved_1) {
    return readByte(cardId, HP_SMART_CARD_RESERVED_1, reserved_1);
}

// Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
HP_SMART_CARD_result_t readTag0FamilyID(HP_SMART_CARD_device_id_t cardId, uint8_t *family_id) {
    HP_SMART_CARD_result_t result = readByte(cardId, HP_SMART_CARD_FAMILY_ID, family_id);
    if (HP_SMART_CARD_OK == result) {
        if(*family_id == HP_SMART_CARD_INK_FAMILY_ID) {
            FIELD_NAME[cardId] = inkFamilyGetFiledName;
        } else {
            FIELD_NAME[cardId] = supplyFamilyGetFiledName;
        }
    }
    return result;
}

// Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
HP_SMART_CARD_result_t readTag0ApprovedOEMBit(HP_SMART_CARD_device_id_t cardId, uint8_t *approved_hp_oem) {
    return readByte(cardId, HP_SMART_CARD_APPROVED_HP_OEM_BIT, approved_hp_oem);
}

// Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
HP_SMART_CARD_result_t readTag0OEMID(HP_SMART_CARD_device_id_t cardId, uint8_t *oem_id) {
    return readByte(cardId, HP_SMART_CARD_OEM_ID, oem_id);
}

// Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
HP_SMART_CARD_result_t readTag0AddressPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *address_position) {
    return readByte(cardId, HP_SMART_CARD_ADDRESS_POSITION, address_position);
}

// Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
HP_SMART_CARD_result_t readTag0TemplateVersionMajor(HP_SMART_CARD_device_id_t cardId, uint8_t *temp_ver_major) {
    return readByte(cardId, HP_SMART_CARD_TEMPLATE_VERSION_MAJOR, temp_ver_major);
}

// Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
HP_SMART_CARD_result_t readTag0TemplateVersionMinor(HP_SMART_CARD_device_id_t cardId, uint8_t *temp_ver_minor) {
    return readByte(cardId, HP_SMART_CARD_TEMPLATE_VERSION_MINOR, temp_ver_minor);
}

// Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

// Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
HP_SMART_CARD_result_t readTag0PersoRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *perso_revision) {
    return readByte(cardId, HP_SMART_CARD_MEMORY_PARTITION_REV, perso_revision);
}

// Skipped (Tag 0 - Reserved for Perso - 6, 3, 4)
// Skipped (Tag 0 - Blank - 7, 7, 8)

char *tag0GetFieldName(uint8_t id) {
    switch (id) {
        case HP_SMART_CARD_CHIP_TAG:
            return "HP_SMART_CARD_CHIP_TAG";
            break;
        case HP_SMART_CARD_RESERVED_1:
            return "HP_SMART_CARD_RESERVED_1";
            break;
        case HP_SMART_CARD_FAMILY_ID:
            return "HP_SMART_CARD_FAMILY_ID";
            break;
        case HP_SMART_CARD_APPROVED_HP_OEM_BIT:
            return "HP_SMART_CARD_APPROVED_HP_OEM_BIT";
            break;
        case HP_SMART_CARD_OEM_ID:
            return "HP_SMART_CARD_OEM_ID";
            break;
        case HP_SMART_CARD_ADDRESS_POSITION:
            return "HP_SMART_CARD_ADDRESS_POSITION";
            break;
        case HP_SMART_CARD_TEMPLATE_VERSION_MAJOR:
            return "HP_SMART_CARD_TEMPLATE_VERSION_MAJOR";
            break;
        case HP_SMART_CARD_TEMPLATE_VERSION_MINOR:
            return "HP_SMART_CARD_TEMPLATE_VERSION_MINOR";
            break;
        case HP_SMART_CARD_MEMORY_PARTITION_REV:
            return "HP_SMART_CARD_MEMORY_PARTITION_REV";
            break;
        default:
            return "HP_SMART_CARD_UNKNOWN_FILED";
    }
}
