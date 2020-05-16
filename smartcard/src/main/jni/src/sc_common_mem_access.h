//
// Created by hmwan on 2020/4/26.
//

#ifndef PRINTER_AS_GIT_SC_COMMON_MEM_ACCESS_H
#define PRINTER_AS_GIT_SC_COMMON_MEM_ACCESS_H

#include <stdint.h>
#include "hp_smart_card.h"

char *(*FIELD_NAME[HP_SMART_CARD_DEVICE_NUM_DEVICES])(uint8_t id);

HP_SMART_CARD_result_t readByte(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t *data);
HP_SMART_CARD_result_t writeByte(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t data);
HP_SMART_CARD_result_t read2Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint16_t *data);
HP_SMART_CARD_result_t write2Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint16_t data);
HP_SMART_CARD_result_t read4Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint32_t *data);
HP_SMART_CARD_result_t write4Bytes(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint32_t data);
HP_SMART_CARD_result_t readString(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t size, void *data);
HP_SMART_CARD_result_t writeString(HP_SMART_CARD_device_id_t cardId, uint8_t fieldId, uint8_t size, void *data);

// Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
HP_SMART_CARD_result_t readTag0ChipTag(HP_SMART_CARD_device_id_t cardId, uint8_t *chip_tag);

// Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
HP_SMART_CARD_result_t readTag0Recerved1(HP_SMART_CARD_device_id_t cardId, uint8_t *reserved_1);

// Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
// For reference
// #define HP_SMART_CARD_INK_FAMILY_ID      27
// #define HPSCS_FAMILY_ID                  28

HP_SMART_CARD_result_t readTag0FamilyID(HP_SMART_CARD_device_id_t cardId, uint8_t *family_id);

// Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
#define HP_SMART_CARD_HP_OEM_APPROVED       0
#define HP_SMART_CARD_HP_OEM_NOT_APPROVED   1

HP_SMART_CARD_result_t readTag0ApprovedOEMBit(HP_SMART_CARD_device_id_t cardId, uint8_t *approved_hp_oem);

// Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
HP_SMART_CARD_result_t readTag0OEMID(HP_SMART_CARD_device_id_t cardId, uint8_t *oem_id);

// Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
HP_SMART_CARD_result_t readTag0AddressPosition(HP_SMART_CARD_device_id_t cardId, uint8_t *address_position);

// Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
HP_SMART_CARD_result_t readTag0TemplateVersionMajor(HP_SMART_CARD_device_id_t cardId, uint8_t *temp_ver_major);

// Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
HP_SMART_CARD_result_t readTag0TemplateVersionMinor(HP_SMART_CARD_device_id_t cardId, uint8_t *temp_ver_minor);

// Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

// Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
HP_SMART_CARD_result_t readTag0PersoRevision(HP_SMART_CARD_device_id_t cardId, uint8_t *perso_revision);

// Skipped (Tag 0 - Reserved for Perso - 6, 3, 4)
// Skipped (Tag 0 - Blank - 7, 7, 8)

char *tag0GetFieldName(uint8_t id);

// -- For Ink
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
// HP_SMART_CARD_INK_UD0_FUSE (Tag 1 - UD0 Fuse - 1, 7, 1)
// HP_SMART_CARD_INK_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1)
// HP_SMART_CARD_INK_UD3_FUSE (Tag 6 - UD2 Fuse - 1, 7, 1)
// HP_SMART_CARD_INK_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
// HP_SMART_CARD_INK_UD4_FUSE (Tag 8 - UD4 Fuse - 4, 1, 1)
// HP_SMART_CARD_INK_UD7_FUSE (Tag 13 - UD7 Fuse - 1, 7, 1)
// HP_SMART_CARD_INK_UD8_FUSE (Tag 14 - UD8 Fuse - 11, 7, 1)
// -- For Supply
// Partition #0(Tag 1,2)
// Partition #1(Tag 3,4,5)
// Partition #2(Tag 6)
// Partition #3(Tag 7)
// Partition #4(Tag 8,9,10)
// Partition #5(Tag 11,12)
// HPSCS_UD1_FUSE (Tag 3 - UD1 Fuse - 1, 7, 1) - (PDF Document written UD0_FUSE)
// HPSCS_UD2_FUSE (Tag 6 - Write Protect Fuse (#2)- 1, 7, 1)
// HPSCS_UD3_FUSE (Tag 7 - UD3 Fuse - 1, 7, 1)
#define HP_SMART_CARD_FUSE_UNLOCKED                 0
#define HP_SMART_CARD_FUSE_LOCKED                   1

// HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
#define HPSC_OEM_ID_UNKNOWN        0
#define HPSC_OEM_ID_HP             1
#define HPSC_OEM_ID_SPS_1          2
#define HPSC_OEM_ID_SPS_2          3

// HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
#define HPSC_ADDRESS_POSITION_UNKNOWN        0
#define HPSC_ADDRESS_POSITION_HOBBES_SMART_CARD        66

// HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
// HPSCS_EXT_OEM_ID (Tag 1 - Extended OEM ID - 1, 7,  8)
#define HPSC_EXT_OEM_ID_UNKNOWN             0
#define HPSC_EXT_OEM_ID_TBD(a)              ((a) >= 1 && (a) <= 14)
#define HPSC_EXT_OEM_ID_HP                  15
#define HPSC_EXT_OEM_ID_OEM_SPECIFIC(a)     ((a) >= 16 && (a) <= 29)
#define HPSC_EXT_OEM_ID_OEM_A               30
#define HPSC_EXT_OEM_ID_OEM_SPECIFICA(a)    ((a) >= 31 && (a) <= 254)
#define HPSC_EXT_OEM_ID_TEST_PEN_ID         255

// HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
// HPSCS_HP_INK (Tag 1 - HP / OEM Ink Designator - 2, 7, 1)
#define HP_SMART_CARD_OEM_INK             0
#define HP_SMART_CARD_HP_INK              1

// HP_SMART_CARD_INK_MANUFACTURE_ID (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 5, 6)
// HPSCS_SN_MFG_SITE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Site ID - 2, 6, 6)
#define HP_SMART_CARD_MANUFACTURE_ID_UNKNOWN      0

// HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
// HPSCS_SN_MFG_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 2, 0, 5)
#define HP_SMART_CARD_MANUFACTURE_LINE_UNKNOWN      0

// HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
// HPSCS_SN_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 4, 0, 3)
#define HP_SMART_CARD_SUNDAY                        0
#define HP_SMART_CARD_MONDAY                        1
#define HP_SMART_CARD_TUESDAY                       2
#define HP_SMART_CARD_WEDNESDAY                     3
#define HP_SMART_CARD_THURSDAY                      4
#define HP_SMART_CARD_FRIDAY                        5
#define HP_SMART_CARD_SATURDAY                      6

// HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
// HPSCS_SN_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 4, 3)
// HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
// HPSCS_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
#define HP_SMART_CARD_PROCESS_POSITION_0          0
#define HP_SMART_CARD_PROCESS_POSITION_1          1
#define HP_SMART_CARD_PROCESS_POSITION_2          2
#define HP_SMART_CARD_PROCESS_POSITION_3          3
#define HP_SMART_CARD_PROCESS_POSITION_4          4
#define HP_SMART_CARD_PROCESS_POSITION_5          5
#define HP_SMART_CARD_PROCESS_POSITION_6          6
#define HP_SMART_CARD_PROCESS_POSITION_7          7

// HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
// HPSCS_FILL_SITE (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
#define HP_SMART_CARD_CARTRIDGE_FILL_SITE_ID_UNKNOWN         0
#define HP_SMART_CARD_CARTRIDGE_FILL_SITE_ID_HP_DEFINED(a)      ((a) >= 1 && (a) <= 63)
#define HP_SMART_CARD_CARTRIDGE_FILL_SITE_ID_OEM_SPECIFIC(a)      ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_CARTRIDGE_FILL_SITE_ID_TEST            255

// HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
// HPSCS_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
#define HP_SMART_CARD_CARTRIDGE_FILL_LINE_UNKNOWN         0
#define HP_SMART_CARD_CARTRIDGE_FILL_LINE_1          1
#define HP_SMART_CARD_CARTRIDGE_FILL_LINE_TEST           31

// HP_SMART_CARD_INK_REGIONALIZATION_ID (Tag 14 - Regionalization ID - 1, 7, 8)
// HPSCS_REGION_ID (Tag 14 - Regionalization ID - 7, 7, 8)
#define HP_SMART_CARD_REGIONALIZATION_ID_UNKNOWN             0
#define HP_SMART_CARD_REGIONALIZATION_ID_UNIVERSAL           1
#define HP_SMART_CARD_REGIONALIZATION_ID_TEST                255

// HP_SMART_CARD_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
// HPSCS_INK_FORMULATOR_ID (Tag 4 - Ink Formulator ID - 1, 7, 8)
#define HP_SMART_CARD_INK_FORMULATOR_ID_UNKNOWN             0
#define HP_SMART_CARD_INK_FORMULATOR_ID_HP_DEFINED(a)       ((a) >= 1 && (a) <= 63)         // 资料当中的定义只有1，可能不对
#define HP_SMART_CARD_INK_FORMULATOR_ID_OEM_SPECIFIC(a)     ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_INK_FORMULATOR_ID_TEST                255

// HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
// HPSCS_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
#define HP_SMART_CARD_INK_FAMILY_UNKNOWN             0
#define HP_SMART_CARD_INK_FAMILY_HP_DEFINED(a)       ((a) >= 1 && (a) <= 63)
#define HP_SMART_CARD_INK_FAMILY_OEM_SPECIFIC(a)     ((a) >= 64 && (a) <= 254)
#define HP_SMART_CARD_INK_FAMILY_TEST                255

// HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
// HPSCS_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
#define HP_SMART_CARD_INK_COLOR_CODES_BLACK       0
#define HP_SMART_CARD_INK_COLOR_CODES_CYAN        1
#define HP_SMART_CARD_INK_COLOR_CODES_MAGENTA     2
#define HP_SMART_CARD_INK_COLOR_CODES_YELLOW      3
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_A   4
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_B   5
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIAL_C   6
#define HP_SMART_CARD_INK_COLOR_CODES_TBD         7

// HPSCS_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
// HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_UNDEFINED       0
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_PHOTOK          2
#define HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC_TBD             7

// HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
// HPSCS_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
#define HP_SMART_CARD_INK_FAMILY_MEMBER_UNDEFINED       0
#define HP_SMART_CARD_INK_FAMILY_MEMBER_BASE            1
#define HP_SMART_CARD_INK_FAMILY_MEMBER_TEST            255

// HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
// HPSCS_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
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

// HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
// HPSCS_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
#define HP_SMART_CARD_INK_ID_REVISION_UNDEFINED       0
#define HP_SMART_CARD_INK_ID_REVISION_INK             1
#define HP_SMART_CARD_INK_ID_REVISION_TEST            255

// HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
// HPSCS_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
#define HP_SMART_CARD_INK_CARTRIDGE_NO_DISTINCTION       0

// HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
// HPSCS_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
#define HP_SMART_CARD_INK_KEY_SIZE_UNKNOWN       0
#define HP_SMART_CARD_INK_KEY_SIZE_NORMAL        5
#define HP_SMART_CARD_INK_KEY_SIZE_XL            6
#define HP_SMART_CARD_INK_KEY_SIZE_BULK          7

// HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
// HPSCS_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
#define HP_SMART_CARD_COUNTRY_US       0        // United States
#define HP_SMART_CARD_COUNTRY_SG       1        // Singapore
#define HP_SMART_CARD_COUNTRY_ES       2        // Spain
#define HP_SMART_CARD_COUNTRY_MX       3        // Mexico
#define HP_SMART_CARD_COUNTRY_MY       4        // Malaysia
#define HP_SMART_CARD_COUNTRY_CN       5        // China
#define HP_SMART_CARD_COUNTRY_TW       6        // Taiwan
#define HP_SMART_CARD_COUNTRY_DE       32       // Germany
#define HP_SMART_CARD_COUNTRY_UNKNOWN  255      // Unknown

// HP_SMART_CARD_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
// HPSCS_INK_OUT_OF_INK_BIT (Tag 9 - Out of Ink bit - 1, 7, 1)
#define HP_SMART_CARD_INK_ILG_OUT_OF_INK_UNREACH       0
#define HP_SMART_CARD_INK_ILG_OUT_OF_INK_REACHED       1

// HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
// HPSCS_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
#define HP_SMART_CARD_INK_FIRST_FAILURE_CODE_NOEEROR       0

// HP_SMART_CARD_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
// HPSCS_INK_ALTERED_SUPPLY (Tag 9 - Altered Supply - 26, 0, 1)
#define HP_SMART_CARD_INK_ALTERED_SUPPLY_NOT_EXCEEDED       0
#define HP_SMART_CARD_INK_ALTERED_SUPPLY_EXCEEDED           1

// HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
// HPSCS_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
#define HP_SMART_CARD_INK_USER_NOT_ACKNOWLEDGED       0
#define HP_SMART_CARD_INK_USER_ACKNOWLEDGED           1

// HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
// HPSCS_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
#define HP_SMART_CARD_INK_USER_ACK_NOT_EXPIRED       0
#define HP_SMART_CARD_INK_USER_ACK_EXPIRED           1

// HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
// HPSCS_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
#define HP_SMART_CARD_INK_FAULTY_NOT_FATAL       0
#define HP_SMART_CARD_INK_FAULTY_FATAL           1

// HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
// HPSCS_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
#define HP_SMART_CARD_INK_LAST_FAILURE_CODE_NOEEROR       0






#endif //PRINTER_AS_GIT_SC_COMMON_MEM_ACCESS_H
