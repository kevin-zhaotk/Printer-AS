//
// Created by kevin on 2019/2/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <jni.h>

#include "hp_smart_card.h"
#include "hp_debug_log_internal.h"
#include "hp_assert.h"
#include "hp_generic_macros.h"

#include "hp_host_smart_card_ifc.h"
#include "hp_host_smart_card.h"
#include "drivers/internal_ifc/hp_smart_card_gpio_ifc.h"
#include "drivers/internal_ifc/hp_smart_card_i2c_ifc.h"
#include "common_log.h"
#include "com_smartcard.h"
#include "drivers/internal_ifc/sc_gpio_adapter.h"
#include "drivers/internal_ifc/sc_i2c_driver.h"

void print_returns(HP_SMART_CARD_result_t result)
{
    HW_SMART_CARD_status_t status = LIB_HP_SMART_CARD_last_status();
    printf("Result = %s (%d)  Status = %s (%d)\n",
           LIB_HP_SMART_CARD_result_string(result), result,
           LIB_HP_SMART_CARD_status_string(status), status);
}

void assert_handler(const char *error_str)
{
    printf("=========================================\n");
    printf("Test Main: HP_ASSERT Failed\n");
    printf("%s\n", error_str);
    printf("=========================================\n");
}

void cache_monitor_failure_handler(HP_SMART_CARD_device_id_t dev_id,
                                   HP_SMART_CARD_result_t result)
{
    printf("=========================================\n");
    printf("Test Main: Cache monitor failure\n");
    printf("Device Id = %d, ", dev_id);
    print_returns(result);
    printf("=========================================\n");
}

JNIEXPORT jint JNICALL Java_com_Smartcard_init
        (JNIEnv *env, jclass arg)
{
    int ret = 0;
    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    LOGI("Initializing smart card library....\n");

/*
// 测试I2C访问是否能够正常实施
    SC_I2C_DRIVER_RESULT ret1 = SC_I2C_DRIVER_open(0x01, 0x68);
    SC_I2C_DRIVER_write(0x0f,"0x55");
    uint8_t result[1024];

    SC_I2C_DRIVER_read(1, 0x0f, &result);

    LOGD("I2C Read: %s", result);
*/

    // Register for assert callback
    LIB_HP_SMART_CARD_register_assert_callback(assert_handler);

    // Register for cache monitor callack
    LIB_HP_SMART_CARD_register_cache_monitor_callback(cache_monitor_failure_handler);

    // Initialise the library
    LIB_HP_SMART_CARD_init();

    // Command sent [1,0xF0]
    // Command sent Command sent [0x0,0x80,0x06,0x48,0x65,0x6C,0x6C,0x6F,0x00]
    // Command sent [1,0xF0]
    // Command sent [3,0x80]

/* Added by H.M.Wang 2019-10-17 */
    if(LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return -1;
    }

    for (int i = 0; i < HP_SMART_CARD_DEVICE_NUM_DEVICES; i++) {
        if(LIB_HP_SMART_CARD_device_present(i) == HP_SMART_CARD_OK) {
            LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_0);
        } else {
            LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", i);
        }
    }
/* Added by H.M.Wang 2019-10-17 end */

    return ret;


/* JNI Level access
    if(select_item == COMPONET_LEVEL_1) {
        HP_SMART_CARD_gpio_select(SELECT_LEVEL_1);               // Select Level 0
    } else if(select_item == COMPONET_LEVEL_2) {
        HP_SMART_CARD_gpio_select(SELECT_LEVEL_2);               // Select Level 2
    }
*/
/* JNI Card access
    read all HP_SMART_CARD_product_fields items in hp_smart_card_config.h
    result = LIB_HP_SMART_CARD_read_field(HP_SMART_CARD_DEVICE_ID_0,
                                          HP_SMART_CARD_FAMILY_ID,
                                          sizeof(family_id),
                                          &family_id);
*/
}

JNIEXPORT jint JNICALL Java_com_Smartcard_close
        (JNIEnv *env, jclass arg)
{
    int ret=0;
    return ret;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_getLevelData
        (JNIEnv *env, jclass arg, jint card) {

    switch (card) {
        case COMPONET_LEVEL_1:
            SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL_1);
            break;
        case COMPONET_LEVEL_2:
            SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL_2);
            break;
        case COMPONET_LEVEL_3:
//            _gpio_select(SELECT_LEVEL_3);
            break;
        case COMPONET_LEVEL_4:
//            _gpio_select(SELECT_LEVEL_4);
            break;
    }

    SC_I2C_DRIVER_RESULT ret = SC_I2C_DRIVER_open(0x01, 0x2b);

    if(ret != SC_I2C_DRIVER_RESULT_OK) return 0;

    jint data;
    ret = SC_I2C_DRIVER_read(4, 0, (uint8_t *)data);

    if(ret == SC_I2C_DRIVER_RESULT_OK) {
        return data;
    } else {
        return 0;
    }
}

/* Added by H.M.Wang 2019-10-19 */
JNIEXPORT jint JNICALL Java_com_Smartcard_getSmartCardData
        (JNIEnv *env, jclass arg, jint card) {

    int cardId = -1;
    switch (card) {
        case COMPONET_SMART_CARD_1:
            cardId = HP_SMART_CARD_DEVICE_ID_0;
            break;
        case COMPONET_SMART_CARD_2:
            cardId = HP_SMART_CARD_DEVICE_ID_1;
            break;
        case COMPONET_SMART_CARD_3:
            cardId = HP_SMART_CARD_DEVICE_ID_2;
            break;
        case COMPONET_SMART_CARD_4:
            cardId = HP_SMART_CARD_DEVICE_ID_3;
            break;
    }

    if(cardId == -1 || LIB_HP_SMART_CARD_device_present(cardId) != HP_SMART_CARD_OK) {
        return -1;
    }

    HP_SMART_CARD_result_t result;

    ////////////////////////////////// TAG 0 ////////////////////////////////////////////////

    // Read HP_SMART_CARD_CHIP_TAG (Tag 0 - Chip tag - 0, 7, 4)
    uint8_t chip_tag;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_CHIP_TAG,
                                          sizeof(chip_tag),
                                          &chip_tag);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_CHIP_TAG = %d", chip_tag);
    } else {
        LOGE("Failed to read HP_SMART_CARD_CHIP_TAG");
    }

    // Read HP_SMART_CARD_RESERVED_1 (Tag 0 - Reserved - 0, 3, 1)
    uint8_t reserved_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_RESERVED_1,
                                          sizeof(reserved_1),
                                          &reserved_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_RESERVED_1 = %d", reserved_1);
    } else {
        LOGE("Failed to read HP_SMART_CARD_RESERVED_1");
    }

    // Read HP_SMART_CARD_FAMILY_ID (Tag 0 - Family ID - 0, 2, 5)
    uint8_t family_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_FAMILY_ID,
                                          sizeof(family_id),
                                          &family_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_FAMILY_ID = %d", family_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_FAMILY_ID");
    }

    // Read HP_SMART_CARD_APPROVED_HP_OEM_BIT (Tag 0 - Approved HP OEM - 1, 5, 1)
    uint8_t approved_hp_oem;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_APPROVED_HP_OEM_BIT,
                                          sizeof(approved_hp_oem),
                                          &approved_hp_oem);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_APPROVED_HP_OEM_BIT = %d", approved_hp_oem);
    } else {
        LOGE("Failed to read HP_SMART_CARD_APPROVED_HP_OEM_BIT");
    }

    // Read HP_SMART_CARD_OEM_ID (Tag 0 - OEM ID - 1, 4, 5)
    uint8_t oem_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_OEM_ID,
                                          sizeof(oem_id),
                                          &oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_OEM_ID = %d", oem_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_OEM_ID");
    }

    // Read HP_SMART_CARD_ADDRESS_POSITION (Tag 0 - Address Position - 2, 7, 8)
    uint8_t address_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_ADDRESS_POSITION,
                                          sizeof(address_position),
                                          &address_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_ADDRESS_POSITION = %d", address_position);
    } else {
        LOGE("Failed to read HP_SMART_CARD_ADDRESS_POSITION");
    }

    // Read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR (Tag 0 - Template Version Major - 3, 7, 4)
    uint8_t temp_ver_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MAJOR,
                                          sizeof(temp_ver_major),
                                          &temp_ver_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_TEMPLATE_VERSION_MAJOR = %d", temp_ver_major);
    } else {
        LOGE("Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MAJOR");
    }

    // Read HP_SMART_CARD_TEMPLATE_VERSION_MINOR (Tag 0 - Template Version Minor - 3, 3, 4)
    uint8_t temp_ver_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_TEMPLATE_VERSION_MINOR,
                                          sizeof(temp_ver_minor),
                                          &temp_ver_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_TEMPLATE_VERSION_MINOR = %d", temp_ver_minor);
    } else {
        LOGE("Failed to read HP_SMART_CARD_TEMPLATE_VERSION_MINOR");
    }

    // Skipped (Tag 0 - Tag Encryption Mask - 4, 7, 16)

    // Read HP_SMART_CARD_MEMORY_PARTITION_REV (Tag 0 - Perso Revision - 6, 7, 4)
    uint8_t perso_revision;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_MEMORY_PARTITION_REV,
                                          sizeof(perso_revision),
                                          &perso_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_MEMORY_PARTITION_REV = %d", perso_revision);
    } else {
        LOGE("Failed to read HP_SMART_CARD_MEMORY_PARTITION_REV");
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
        LOGD("HP_SMART_CARD_INK_UD0_FUSE = %d", ud0_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD0_FUSE");
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
        LOGD("HP_SMART_CARD_INK_MANUFACTURE_ID = %d", manufacture_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MANUFACTURE_ID");
    }

    // Read HP_SMART_CARD_INK_MANUFACTURE_LINE (Tag 1 - Dry Cartridge SN (Serial Number) Manufacture Line - 3, 7, 5)
    uint8_t manufacture_line;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_LINE,
                                          sizeof(manufacture_line),
                                          &manufacture_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MANUFACTURE_LINE = %d", manufacture_line);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MANUFACTURE_LINE");
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
        LOGD("HP_SMART_CARD_INK_MANUFACTURE_YEAR = %d", manufacture_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MANUFACTURE_YEAR");
    }

    // Read HP_SMART_CARD_INK_MANUFACTURE_WEEK (Tag 1 - Dry Cartridge SN (Serial Number) Week of Year - 4, 5, 6)
    uint8_t manufacture_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MANUFACTURE_WEEK,
                                          sizeof(manufacture_woy),
                                          &manufacture_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MANUFACTURE_WEEK = %d", manufacture_woy);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MANUFACTURE_WEEK");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_DOW (Tag 1 - Dry Cartridge SN (Serial Number) Day of Week - 5, 7, 3)
    uint8_t manufacture_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_DOW,
                                          sizeof(manufacture_dow),
                                          &manufacture_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SERIAL_NUM_DOW = %d", manufacture_dow);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SERIAL_NUM_DOW");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_HOD (Tag 1 - Dry Cartridge SN (Serial Number) Hour of Day - 5, 4, 5)
    uint8_t manufacture_hod;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_HOD,
                                          sizeof(manufacture_hod),
                                          &manufacture_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SERIAL_NUM_HOD = %d", manufacture_hod);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SERIAL_NUM_HOD");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_MOH (Tag 1 - Dry Cartridge SN (Serial Number) Minute of Hour - 6, 7, 6)
    uint8_t manufacture_moh;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_MOH,
                                          sizeof(manufacture_moh),
                                          &manufacture_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SERIAL_NUM_MOH = %d", manufacture_moh);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SERIAL_NUM_MOH");
    }

    // Read HP_SMART_CARD_INK_SERIAL_NUM_SOM (Tag 1 - Dry Cartridge SN (Serial Number) Sec. of Minute - 6, 1, 6)
    uint8_t manufacture_som;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SERIAL_NUM_SOM,
                                          sizeof(manufacture_som),
                                          &manufacture_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SERIAL_NUM_SOM = %d", manufacture_som);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SERIAL_NUM_SOM");
    }

    // Read HP_SMART_CARD_INK_DRY_PROCESS_POSITION (Tag 1 - Dry Cartridge SN (Serial Number) Process Position - 7, 3, 3)
    uint8_t process_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DRY_PROCESS_POSITION,
                                          sizeof(process_position),
                                          &process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_DRY_PROCESS_POSITION = %d", process_position);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_DRY_PROCESS_POSITION");
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
        LOGD("HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME = %f", max_usable_cartridge_volume * 0.1 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MAX_USABLE_CARTRIDGE_VOLUME");
    }

    // Read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION (Tag 1 - Printer Lockdown Partition - 9, 0, 5)
    uint8_t  lock_down_partition;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION,
                                          sizeof(lock_down_partition),
                                          &lock_down_partition);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION = %d", lock_down_partition);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_PRINTER_LOCK_DOWN_PARTITION");
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
        LOGD("HP_SMART_CARD_INK_TSR = %d", ink_tsr * 1 + 250);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TSR");
    }

    // Read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT (Tag 2 - TSR Thermal Coefficient(TCR) - 1, 0, 8)
    // Bulk cartridge identifier
    //    0 = single use cartridge
    //    1 = bulk ink cartridge
    uint8_t ink_tcr;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT,
                                          sizeof(ink_tcr),
                                          &ink_tcr);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT = %d", ink_tcr * 5 + 2900);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TSR_THERMAL_COEFFICIENT");
    }

    // Read HP_SMART_CARD_INK_BULK_CTRDG (Tag 2 - TSR Thermal Coefficient(TCR) - 2, 0, 1)
    uint8_t bulk_ctrdg;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_BULK_CTRDG,
                                          sizeof(bulk_ctrdg),
                                          &bulk_ctrdg);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_BULK_CTRDG = %d", bulk_ctrdg);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_BULK_CTRDG");
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
        LOGD("HP_SMART_CARD_INK_UD1_FUSE = %d", ud1_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD1_FUSE");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID (Tag 3 - Cartridge Fill SN (Serial Number) Site ID - 1, 6, 8)
    uint8_t site_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID,
                                          sizeof(site_id),
                                          &site_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID = %d", site_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SITE_ID");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE (Tag 3 - Cartridge Fill SN (Serial Number) Line - 2, 6, 5)
    uint8_t fill_line;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE,
                                          sizeof(fill_line),
                                          &fill_line);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE = %d", fill_line);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_LINE");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR (Tag 3 - Cartridge Fill SN (Serial Number) Year - 2, 1, 5)
    // Year that cartridge was filled m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t fill_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR,
                                          sizeof(fill_year),
                                          &fill_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR = %d", fill_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_YEAR");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY (Tag 3 - Cartridge Fill SN (Serial Number) Week of Year - 3, 4, 6)
    uint8_t fill_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY,
                                          sizeof(fill_woy),
                                          &fill_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY = %d", fill_year);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_WOY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW (Tag 3 - Cartridge Fill SN (Serial Number) Day of Week - 4, 6, 3)
    uint8_t fill_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW,
                                          sizeof(fill_dow),
                                          &fill_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW = %d", fill_dow);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_DOW");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD (Tag 3 - Cartridge Fill SN (Serial Number) Hour of Day - 4, 3, 5)
    uint8_t fill_hod;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD,
                                          sizeof(fill_hod),
                                          &fill_hod);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD = %d", fill_hod);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_HOD");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH (Tag 3 - Cartridge Fill SN (Serial Number) Minute of Hour - 5, 6, 6)
    uint8_t fill_moh;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH,
                                          sizeof(fill_moh),
                                          &fill_moh);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH = %d", fill_moh);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_MOH");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM (Tag 3 - Cartridge Fill SN (Serial Number) Second of Minute - 5, 0, 6)
    uint8_t fill_som;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM,
                                          sizeof(fill_som),
                                          &fill_som);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM = %d", fill_som);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_SOM");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION (Tag 3 - Cartridge Fill SN (Serial Number) Process Position - 6, 2, 3)
    uint8_t fill_process_position;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION,
                                          sizeof(fill_process_position),
                                          &fill_process_position);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION = %d", fill_process_position);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_FILL_PROCESS_POSITION");
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
        LOGD("HP_SMART_CARD_INK_FORMULATOR_ID = %d", formulator_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FORMULATOR_ID");
    }

    // Read HP_SMART_CARD_INK_INK_FAMILY (Tag 4 - Ink Family - 2, 7, 8)
    uint8_t ink_family;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY,
                                          sizeof(ink_family),
                                          &ink_family);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INK_FAMILY = %d", ink_family);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INK_FAMILY");
    }

    // Read HP_SMART_CARD_INK_COLOR_CODES_GENERAL (Tag 4 - Color Codes – General - 3, 7, 3)
    uint8_t ink_color_code_general;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_GENERAL,
                                          sizeof(ink_color_code_general),
                                          &ink_color_code_general);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_COLOR_CODES_GENERAL = %d", ink_color_code_general);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_COLOR_CODES_GENERAL");
    }

    // Read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC (Tag 4 - Color Codes – Specific - 3, 4, 3)
    uint8_t ink_color_code_specific;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC,
                                          sizeof(ink_color_code_specific),
                                          &ink_color_code_specific);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC = %d", ink_color_code_specific);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_COLOR_CODES_SPECIFIC");
    }

    // Read HP_SMART_CARD_INK_INK_FAMILY_MEMBER (Tag 4 - Ink Family Member - 3, 1, 8)
    uint8_t ink_family_member;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_FAMILY_MEMBER,
                                          sizeof(ink_family_member),
                                          &ink_family_member);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INK_FAMILY_MEMBER = %d", ink_family_member);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INK_FAMILY_MEMBER");
    }

    // Read HP_SMART_CARD_INK_INK_ID_NUM (Tag 4 - Ink ID Number - 4, 1, 12)
    uint16_t ink_id_num;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_ID_NUM,
                                          sizeof(ink_id_num),
                                          &ink_id_num);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INK_ID_NUM = %d", ink_id_num);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INK_ID_NUM");
    }

    // Read HP_SMART_CARD_INK_INK_REVISION (Tag 4 - Ink Revision - 6, 5, 8)
    uint8_t ink_revision;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_REVISION,
                                          sizeof(ink_revision),
                                          &ink_revision);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INK_REVISION = %d", ink_revision);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INK_REVISION");
    }

    // Read HP_SMART_CARD_INK_DENSITY (Tag 4 - Ink Density - 7, 5, 10)
    uint16_t ink_density;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DENSITY,
                                          sizeof(ink_density),
                                          &ink_density);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_DENSITY = %d", ink_density);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_DENSITY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION (Tag 4 - Cartridge Distinction - 8, 3, 5)
    uint8_t ink_cartridge_distinction;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION,
                                          sizeof(ink_cartridge_distinction),
                                          &ink_cartridge_distinction);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION = %d", ink_cartridge_distinction);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_DISTINCTION");
    }

    // Read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR (Tag 4 - Supply Key Size Descriptor - 9, 6, 3)
    uint8_t ink_supply_keying_descriptor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR,
                                          sizeof(ink_supply_keying_descriptor),
                                          &ink_supply_keying_descriptor);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR = %d", ink_cartridge_distinction);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SUPPLY_KEYING_DESCRIPTOR");
    }

    // Read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS (Tag 4 - Shelf Life (weeks) - 9, 3, 8)
    uint8_t ink_shelf_life_weeks;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_WEEKS,
                                          sizeof(ink_shelf_life_weeks),
                                          &ink_shelf_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SHELF_LIFE_WEEKS = %d", ink_shelf_life_weeks);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SHELF_LIFE_WEEKS");
    }

    // Read HP_SMART_CARD_INK_SHELF_LIFE_DAYS (Tag 4 - Shelf Life (days) - 10, 3, 3)
    uint8_t ink_shelf_life_days;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_SHELF_LIFE_DAYS,
                                          sizeof(ink_shelf_life_days),
                                          &ink_shelf_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_SHELF_LIFE_DAYS = %d", ink_shelf_life_days);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_SHELF_LIFE_DAYS");
    }

    // Read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS (Tag 4 - Installed Life (weeks) - 10, 0, 8)
    uint8_t ink_installed_life_weeks;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS,
                                          sizeof(ink_installed_life_weeks),
                                          &ink_installed_life_weeks);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS = %d", ink_installed_life_weeks);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_WEEKS");
    }

    // Read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS (Tag 4 - Installed Life (days) - 11, 0, 3)
    uint8_t ink_installed_life_days;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS,
                                          sizeof(ink_installed_life_days),
                                          &ink_installed_life_days);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS = %d", ink_installed_life_days);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INSTALLED_LIFE_DAYS");
    }

    // Read HP_SMART_CARD_INK_USABLE_INK_WEIGHT (Tag 4 - Usable Ink Weight - 12, 5, 16)
    // m = 0.1 b = 0 range = 0 – 6553.5 in equation y = mx + b
    uint16_t ink_usable_ink_weight;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USABLE_INK_WEIGHT,
                                          sizeof(ink_usable_ink_weight),
                                          &ink_usable_ink_weight);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_USABLE_INK_WEIGHT = %f", ink_usable_ink_weight * 0.1 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_USABLE_INK_WEIGHT");
    }

    // Read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL (Tag 4 - Altered Supply Notification Level - 14, 5, 7)
    // Percentage Value: 10 means 10%
    uint8_t ink_notification_level;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL,
                                          sizeof(ink_notification_level),
                                          &ink_notification_level);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL = %d", ink_notification_level);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY_NOTIFICATION_LEVEL");
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
        LOGD("HP_SMART_CARD_INK_FIRING_FREQ = %f", firing_freq * 0.2 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRING_FREQ");
    }

    // Read HP_SMART_CARD_INK_PULSE_WIDTH (Tag 5 - Pulse Width (Tpw) - 1, 0, 8)
    // Firing pulse width, based on the filled ink type. m = 0.01 b = 1.5, range = 1.50 – 4.05 in equation y = mx + b
    uint8_t pulse_width;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WIDTH,
                                          sizeof(pulse_width),
                                          &pulse_width);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_PULSE_WIDTH = %f", pulse_width * 0.1 + 1.5);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_PULSE_WIDTH");
    }

    // Read HP_SMART_CARD_INK_FIRING_VOLTAGE (Tag 5 - Firing Voltage - 2, 0, 7)
    // Firing voltage required by the cartridge, based on the filled ink type. m = 0.1, b = 5.0, range = 5.0 – 17.7 y = mx + b
    uint8_t firing_voltage;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRING_VOLTAGE,
                                          sizeof(firing_voltage),
                                          &firing_voltage);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRING_VOLTAGE = %f", firing_voltage * 0.1 + 5.0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRING_VOLTAGE");
    }

    // Read HP_SMART_CARD_INK_TURN_ON_ENERGY (Tag 5 - Turn on Energy (TOE) - 3, 1, 8)
    // Turn on energy for a firing resistor, based on the filled ink type. m = 0.02, b = 2.0, range = 2.0 – 7.1 y = mx + b
    uint8_t turn_on_energy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TURN_ON_ENERGY,
                                          sizeof(turn_on_energy),
                                          &turn_on_energy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TURN_ON_ENERGY = %f", turn_on_energy * 0.02 + 2.0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TURN_ON_ENERGY");
    }

    // Read HP_SMART_CARD_INK_PULSE_WARMING_TEMP (Tag 5 - Pulse Warming Temperature - 4, 1, 7)
    // Pulse warming temperature required for optimal firing for this cartridge, based on the filled ink type. m = 1, b = 0, range = 0 – 127 y = mx + b
    uint8_t pulse_warming_temp;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_PULSE_WARMING_TEMP,
                                          sizeof(pulse_warming_temp),
                                          &pulse_warming_temp);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_PULSE_WARMING_TEMP = %d", pulse_warming_temp * 1 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_PULSE_WARMING_TEMP");
    }

    // Read HP_SMART_CARD_INK_MAX_TEMP (Tag 5 - Maximum Temperature - 5, 2, 7)
    // Maximum head temperature for this cartridge, based on the filled ink type. m = 1, b = 0, range = 0 – 127 y = mx + b
    uint8_t max_temp;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MAX_TEMP,
                                          sizeof(max_temp),
                                          &max_temp);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MAX_TEMP = %d", max_temp * 1 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MAX_TEMP");
    }

    // Read HP_SMART_CARD_INK_DROP_VOLUME (Tag 5 - Drop Volume - 6, 3, 6)
    // Ink drop volume, based on filled ink type firing parameters. m = 1, b = 0, range = 0 – 63 y = mx + b
    uint8_t drop_volume;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_DROP_VOLUME,
                                          sizeof(drop_volume),
                                          &drop_volume);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_DROP_VOLUME = %d", drop_volume * 1 + 0);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_DROP_VOLUME");
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
        LOGD("HP_SMART_CARD_INK_WRITE_PROTECT = %d", write_protect);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_WRITE_PROTECT");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 6 - 1st Platform ID - 12 Char - 1, 6, 72)
    unsigned char first_platform_id[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,
                                           12,
                                           first_platform_id);
    first_platform_id[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR = %s", first_platform_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_ID_CHAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR (Tag 6 - 1st Platform Mfg. Year - 10, 6, 5)
    // The first install printer platform manufacture date - year. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t mfg_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR,
                                          sizeof(mfg_year),
                                          &mfg_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR = %d", mfg_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_YEAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY (Tag 6 - 1st Platform Mfg. Week of Year - 10, 1, 6)
    uint8_t mfg_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY,
                                          sizeof(mfg_woy),
                                          &mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY = %d", mfg_woy);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_WOY");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY (Tag 6 - 1st Platform Mfg. Country - 11, 3, 6)
    uint8_t mfg_country;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY,
                                          sizeof(mfg_country),
                                          &mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY = %d", mfg_country);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_MFG_COUNTRY");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION (Tag 6 - 1st Platform F/W Revision major - 12, 5, 4)
    uint8_t fw_revision_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION,
                                          sizeof(fw_revision_major),
                                          &fw_revision_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION = %d", fw_revision_major);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MAJOR_REVISION");
    }

    // Read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION (Tag 6 - 1st Platform F/W Revision minor - 12, 1, 4)
    uint8_t fw_revision_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION,
                                          sizeof(fw_revision_minor),
                                          &fw_revision_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION = %d", fw_revision_minor);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_PLATFORM_FW_MINOR_REVISION");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT (Tag 6 - 1st Install Cartridge Count - 13, 5, 12)
    uint16_t first_install_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT,
                                          sizeof(first_install_count),
                                          &first_install_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT = %d", first_install_count);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_CARTRIDGE_COUNT");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR (Tag 6 - Cartridge 1st Install Year - 14, 1, 5)
    // The year of the first installation of this cartridge into this printer platform. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b
    uint8_t first_install_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_YEAR,
                                          sizeof(first_install_year),
                                          &first_install_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_INSTALL_YEAR = %d", first_install_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_YEAR");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_WOY (Tag 6 - Cartridge 1st Install Week of Year - 15, 4, 6)
    uint8_t first_install_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_WOY,
                                          sizeof(first_install_woy),
                                          &first_install_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_INSTALL_WOY = %d", first_install_woy);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_WOY");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_DOW (Tag 6 - Cartridge 1st Install Day of Week - 16, 6, 3)
    uint8_t first_install_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_DOW,
                                          sizeof(first_install_dow),
                                          &first_install_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_INSTALL_DOW = %d", first_install_dow);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_DOW");
    }

    // Read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION (Tag 6 - Ink Level Gauge Resolution - 16, 3, 7)
    uint8_t gauge_resolution;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION,
                                          sizeof(gauge_resolution),
                                          &gauge_resolution);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION = %d", gauge_resolution);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INK_LEVEL_GAUGE_RESOLUTION");
    }

    // Read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD (Tag 6 - OEM Defined - 17, 4, 21)
    uint32_t oem_field;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD,
                                          sizeof(oem_field),
                                          &oem_field);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD = %d", oem_field);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_INSTALL_OEM_FIELD");
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
        LOGD("HP_SMART_CARD_INK_UD3_FUSE = %d", ud3_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD3_FUSE");
    }

    // Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 (Tag 7 - UD3 Fuse - 1, 6, 31)
    uint32_t oem_defined_filed_1;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1,
                                          sizeof(oem_defined_filed_1),
                                          &oem_defined_filed_1);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1 = %d", oem_defined_filed_1);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 (Tag 7 - UD3 Fuse - 5, 7, 32)
    uint32_t oem_defined_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2,
                                          sizeof(oem_defined_filed_2),
                                          &oem_defined_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2 = %d", oem_defined_filed_2);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_DEFINED_FIELD_2");
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
        LOGD("HP_SMART_CARD_INK_HP_TRADEMARK_CHAR = %s", first_platform_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_HP_TRADEMARK_CHAR");
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
        LOGD("HP_SMART_CARD_INK_UD4_FUSE = %d", ud4_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD4_FUSE");
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
        LOGD("HP_SMART_CARD_INK_OUT_OF_INK_BIT = %d", out_of_ink);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OUT_OF_INK_BIT");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_01_25 (Tag 9 - ILG Bits 1-25 - 1, 6, 25)
    // Ink level gauge bits 1-25 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_25_01;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_01_25,
                                          sizeof(ilg_bit_25_01),
                                          &ilg_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_ILG_BIT_01_25 = %d", ilg_bit_25_01);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ILG_BIT_01_25");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_26_50 (Tag 9 - ILG Bits 26-50 - 4, 5, 25)
    // Ink level gauge bits 26-50 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_50_26;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_26_50,
                                          sizeof(ilg_bit_50_26),
                                          &ilg_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_ILG_BIT_26_50 = %d", ilg_bit_50_26);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ILG_BIT_26_50");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_51_75 (Tag 9 - ILG Bits 51-75 - 7, 4, 25)
    // Ink level gauge bits 51-75 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_75_51;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_51_75,
                                          sizeof(ilg_bit_75_51),
                                          &ilg_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_ILG_BIT_51_75 = %d", ilg_bit_75_51);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ILG_BIT_51_75");
    }

    // Read HP_SMART_CARD_INK_ILG_BIT_76_100 (Tag 9 - ILG Bits 76-100 - 10, 3, 25)
    // Ink level gauge bits 76-100 of a 100-bit field. This enables up to 1% resolution.
    uint32_t ilg_bit_100_76;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_ILG_BIT_76_100,
                                          sizeof(ilg_bit_100_76),
                                          &ilg_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_ILG_BIT_76_100 = %d", ilg_bit_100_76);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ILG_BIT_76_100");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_01_25 (Tag 9 - TIUG Bits 1-25 - 13, 2, 25)
    // Total Ink Usage Gauge bits 1 - 25 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_25_01;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_01_25,
                                          sizeof(tiug_bit_25_01),
                                          &tiug_bit_25_01);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TUG_BIT_01_25 = %d", tiug_bit_25_01);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TUG_BIT_01_25");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_26_50 (Tag 9 - TIUG Bits 26-50 - 16, 1, 25)
    // Total Ink Usage Gauge bits 26 - 50 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_50_26;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_26_50,
                                          sizeof(tiug_bit_50_26),
                                          &tiug_bit_50_26);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TUG_BIT_26_50 = %d", tiug_bit_50_26);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TUG_BIT_26_50");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_51_75 (Tag 9 - TIUG Bits 51-75 - 19, 0, 25)
    // Total Ink Usage Gauge bits 51 - 75 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_75_51;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_51_75,
                                          sizeof(tiug_bit_75_51),
                                          &tiug_bit_75_51);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TUG_BIT_51_75 = %d", tiug_bit_75_51);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TUG_BIT_51_75");
    }

    // Read HP_SMART_CARD_INK_TUG_BIT_76_100 (Tag 9 - TIUG Bits 76-100 - 19, 0, 25)
    // Total Ink Usage Gauge bits 76 - 100 of a 100-bit field. This enables up to 1% resolution.
    uint32_t tiug_bit_100_76;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_TUG_BIT_76_100,
                                          sizeof(tiug_bit_100_76),
                                          &tiug_bit_100_76);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_TUG_BIT_76_100 = %d", tiug_bit_100_76);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_TUG_BIT_76_100");
    }

    // Read HP_SMART_CARD_INK_FIRST_FAILURE_CODE (Tag 9 - First Failure Code - 26, 6, 6)
    uint8_t first_failure_code;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FIRST_FAILURE_CODE,
                                          sizeof(first_failure_code),
                                          &first_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FIRST_FAILURE_CODE = %d", first_failure_code);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FIRST_FAILURE_CODE");
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
        LOGD("HP_SMART_CARD_INK_ALTERED_SUPPLY = %d", altered_supply);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_ALTERED_SUPPLY");
    }

    // Read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK (Tag 9 - User Acknowledge Altered Supply - 27, 7, 1)
    uint8_t user_ack_altered_supply;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK,
                                          sizeof(user_ack_altered_supply),
                                          &user_ack_altered_supply);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK = %d", user_ack_altered_supply);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_USER_ACK_NON_ORG_INK");
    }

    // Read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK (Tag 9 - User Acknowledge Expired Ink - 27, 6, 1)
    uint8_t user_ack_expired_ink;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK,
                                          sizeof(user_ack_expired_ink),
                                          &user_ack_expired_ink);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK = %d", user_ack_expired_ink);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_USER_ACK_EXPIRED_INK");
    }

    // Read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY (Tag 9 - Faulty Replace Immediately - 27, 5, 1)
    uint8_t faulty_replace_immediately;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY,
                                          sizeof(faulty_replace_immediately),
                                          &faulty_replace_immediately);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY = %d", faulty_replace_immediately);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_FAULTY_REPLACE_IMMEDIATELY");
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
        LOGD("HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1 = %d", oem_rw_or_filed_1);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 (Tag 10 - OEM defined RW_OR Field 2 - 5, 7, 32)
    uint32_t oem_rw_or_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2,
                                          sizeof(oem_rw_or_filed_2),
                                          &oem_rw_or_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2 = %d", oem_rw_or_filed_2);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_RW_OR_FIELD_2");
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
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR = %d", cartridge_mru_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_YEAR");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY (Tag 11 - Cartridge MRU Week of Year - 1, 2, 6)
    uint8_t cartridge_mru_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY,
                                          sizeof(cartridge_mru_woy),
                                          &cartridge_mru_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY = %d", cartridge_mru_woy);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_WOY");
    }

    // Read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW (Tag 11 - Cartridge MRU Day of Week - 2, 4, 3)
    uint8_t cartridge_mru_dow;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW,
                                          sizeof(cartridge_mru_dow),
                                          &cartridge_mru_dow);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW = %d", cartridge_mru_dow);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_CARTRIDGE_MRU_DOW");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR (Tag 6 - MRU Platform ID - 12 Char - 2, 1, 72)
    unsigned char mru_platform_id[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR_1,
                                           12,
                                           mru_platform_id);
    mru_platform_id[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR = %s", mru_platform_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_ID_CHAR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR (Tag 11 - MRU Platform Mfg Year - 11, 1, 5)
    // The most recent use (MRU) printer platform manufacture date - year. m = 1 b = 2006 range = 2006 – 2037 in equation y = mx + b.
    uint8_t mru_platform_mfg_year;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR,
                                          sizeof(mru_platform_mfg_year),
                                          &mru_platform_mfg_year);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR = %d", mru_platform_mfg_year * 1 + 2006);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_YEAR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY (Tag 11 - MRU Platform Mfg Week of Year - 12, 4, 6)
    uint8_t mru_platform_mfg_woy;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY,
                                          sizeof(mru_platform_mfg_woy),
                                          &mru_platform_mfg_woy);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY = %d", mru_platform_mfg_woy);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_WOY");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY (Tag 11 - MRU Platform Mfg Country - 13, 6, 6)
    uint8_t mru_platform_mfg_country;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY,
                                          sizeof(mru_platform_mfg_country),
                                          &mru_platform_mfg_country);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY = %d", mru_platform_mfg_country);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_MFG_COUNTRY");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR (Tag 11 - MRU Platform F/W Revision major - 13, 0, 4)
    uint8_t mru_platform_fw_rev_major;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR,
                                          sizeof(mru_platform_fw_rev_major),
                                          &mru_platform_fw_rev_major);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR = %d", mru_platform_fw_rev_major);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MAJOR");
    }

    // Read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR (Tag 11 - MRU Platform F/W Revision minor - 14, 4, 4)
    uint8_t mru_platform_fw_rev_minor;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR,
                                          sizeof(mru_platform_fw_rev_minor),
                                          &mru_platform_fw_rev_minor);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR = %d", mru_platform_fw_rev_minor);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MRU_PLATFORM_FW_REV_MINOR");
    }

    // Read HP_SMART_CARD_INK_INSERTION_COUNT (Tag 11 - Cartridge Insertion Count - 14, 0, 6)
    uint8_t insertion_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_INSERTION_COUNT,
                                          sizeof(insertion_count),
                                          &insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_INSERTION_COUNT = %d", insertion_count);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_INSERTION_COUNT");
    }

    // Read HP_SMART_CARD_INK_STALL_INSERTION_COUNT (Tag 11 - Stall Insertion Count - 15, 2, 16)
    uint16_t stall_insertion_count;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_STALL_INSERTION_COUNT,
                                          sizeof(stall_insertion_count),
                                          &stall_insertion_count);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_STALL_INSERTION_COUNT = %d", stall_insertion_count);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_STALL_INSERTION_COUNT");
    }

    // Read HP_SMART_CARD_INK_LAST_FAILURE_CODE (Tag 11 - Last Failure Code - 17, 2, 6)
    uint8_t last_failure_code;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_LAST_FAILURE_CODE,
                                          sizeof(last_failure_code),
                                          &last_failure_code);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_LAST_FAILURE_CODE = %d", last_failure_code);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_LAST_FAILURE_CODE");
    }

    // Read HP_SMART_CARD_INK_USER_REPORTED_STATUS (Tag 11 - Last Failure Code - 18, 4, 4)
    uint8_t last_user_reported_status;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_USER_REPORTED_STATUS,
                                          sizeof(last_user_reported_status),
                                          &last_user_reported_status);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_USER_REPORTED_STATUS = %d", last_user_reported_status);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_USER_REPORTED_STATUS");
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
        LOGD("HP_SMART_CARD_INK_MARKETING_DATA_REVISION = %d", marketing_data_revision);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_MARKETING_DATA_REVISION");
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
        LOGD("HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1 = %d", oem_rw_filed_1);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_1");
    }

    // Read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 (Tag 12 - OEM defined RW Field 1 - 5, 7, 32)
    uint32_t oem_rw_filed_2;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2,
                                          sizeof(oem_rw_filed_2),
                                          &oem_rw_filed_2);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2 = %d", oem_rw_filed_2);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_OEM_DEF_RW_FIELD_2");
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
        LOGD("HP_SMART_CARD_INK_UD7_FUSE = %d", ud7_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD7_FUSE");
    }

    // Read HP_SMART_CARD_INK_EXTENDED_OEM_ID (Tag 13 - Extended OEM ID - 1, 6, 8)
    uint8_t extended_oem_id;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_EXTENDED_OEM_ID,
                                          sizeof(extended_oem_id),
                                          &extended_oem_id);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_EXTENDED_OEM_ID = %d", extended_oem_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_EXTENDED_OEM_ID");
    }

    // Read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR (Tag 13 - HP / OEM Ink Designator - 2, 6, 1)
    uint8_t oem_ink_designator;
    result = LIB_HP_SMART_CARD_read_field(cardId,
                                          (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR,
                                          sizeof(oem_ink_designator),
                                          &oem_ink_designator);
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR = %d", oem_ink_designator);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_HP_OEM_INK_DESIGNATOR");
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
        LOGD("HP_SMART_CARD_INK_REGIONALIZATION_ID = %d", ink_regionalization_id);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_REGIONALIZATION_ID");
    }

    // Read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR (Tag 14 - Cartridge Reorder P/ N - 12 Char - 2, 7, 72)
    unsigned char reorder_part_num[32];
    result = LIB_HP_SMART_CARD_read_string(cardId,
                                           (HP_SMART_CARD_field_t)HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR_1,
                                           12,
                                           reorder_part_num);
    reorder_part_num[12] = '\0';
    if (HP_SMART_CARD_OK == result) {
        LOGD("HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR = %s", reorder_part_num);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_REORDER_PART_NUM_CHAR");
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
        LOGD("HP_SMART_CARD_INK_UD8_FUSE = %d", ud8_fuse);
    } else {
        LOGE("Failed to read HP_SMART_CARD_INK_UD8_FUSE");
    }

    // Skipped (Tag 14 - Blank - 11, 6, 15)
    // Skipped (Tag 14 - HP reserved - 13, 7, 224)
    // Skipped (Tag 14 - Parity Byte - 41, 7, 8)
    // Skipped (Tag 14 - Parity Byte2 - 42, 7, 8)

    return 0;
}

/* Added by H.M.Wang 2019-10-19 end */

/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"open",					"()I",	                    (void *)Java_com_Smartcard_init},
        {"close",					"()I",						(void *)Java_com_Smartcard_close},
        {"getSmartCardData",		"(I)I",						(void *)Java_com_Smartcard_getSmartCardData},
        {"getLevelData",		    "(I)I",						(void *)Java_com_Smartcard_getLevelData},
};

/**
 * 注册RTC操作的JNI方法
 */
int register_com_smartcard(JNIEnv* env) {
    const char* kClassPathName = "com/industry/printer/hardware/SmartCard";
    jclass clazz = (*env)->FindClass(env, kClassPathName);
    if(clazz == NULL) {
        return JNI_FALSE;
    }
    return (*env)->RegisterNatives(env, clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0]));
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    LOGI("SmartCard.so 1.0.36 Loaded.");

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        //__android_log_print(ANDROID_LOG_INFO, JNI_TAG,"ERROR: GetEnv failed\n");
        goto fail;
    }

    if (register_com_smartcard(env) < 0) {
        goto fail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    fail:
    return result;
}

