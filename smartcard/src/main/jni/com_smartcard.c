//
// Created by kevin on 2019/2/17.
//

#include <stdlib.h>
#include <jni.h>
#include <src/sc_ink_mem_access.h>
#include <src/sc_common_mem_access.h>
#include <unistd.h>
#include "hp_smart_card.h"

#include "hp_host_smart_card.h"
#include "drivers/internal_ifc/hp_smart_card_gpio_ifc.h"
#include "common_log.h"
#include "com_smartcard.h"
#include "drivers/internal_ifc/sc_gpio_adapter.h"
#include "src/level_memory_access.h"

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
    printf("Smartcard Main: HP_ASSERT Failed\n");
    printf("%s\n", error_str);
    printf("=========================================\n");
}

void cache_monitor_failure_handler(HP_SMART_CARD_device_id_t dev_id,
                                   HP_SMART_CARD_result_t result)
{
    printf("=========================================\n");
    printf("Smartcard Main: Cache monitor failure\n");
    printf("Device Id = %d, ", dev_id);
    print_returns(result);
    printf("=========================================\n");
}

JNIEXPORT jint JNICALL Java_com_Smartcard_init
        (JNIEnv *env, jclass arg) {
    int ret = 0;
    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    LOGI("Initializing smart card library....\n");

    // Register for assert callback
    LIB_HP_SMART_CARD_register_assert_callback(assert_handler);

    // Register for cache monitor callack
    LIB_HP_SMART_CARD_register_cache_monitor_callback(cache_monitor_failure_handler);

    // Initialise the library
    LIB_HP_SMART_CARD_init();

// Added by H.M.Wang 2019-10-17
    if (LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return -1;
    }

//    for (int i = 0; i < HP_SMART_CARD_DEVICE_NUM_DEVICES; i++) {
    if (LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_0) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_0);
        return -1;
    }
    LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_0);
//    }
// End of Added by H.M.Wang 2019-10-17
/*
    uint8_t chip_tag;
    HP_SMART_CARD_result_t r = readTag0ChipTag(HP_SMART_CARD_DEVICE_ID_0, &chip_tag);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Chip Tag = %d\n", chip_tag);
    else
        return -1;
*/
    uint8_t family_id;
    HP_SMART_CARD_result_t r = readTag0FamilyID(HP_SMART_CARD_DEVICE_ID_0, &family_id);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag0 Family ID -> %d\n", family_id);
    } else {
        return -1;
    }
/*
    uint8_t approved;
    r = readTag0ApprovedOEMBit(HP_SMART_CARD_DEVICE_ID_0, &approved);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Approved OEM = %d\n", approved);
    else
        return -1;

    uint8_t oem_id;
    r = readTag0OEMID(HP_SMART_CARD_DEVICE_ID_0, &oem_id);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> OEM ID = %d\n", oem_id);
    else
        return -1;

    uint8_t addr_pos;
    r = readTag0AddressPosition(HP_SMART_CARD_DEVICE_ID_0, &addr_pos);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Address Position = %d\n", addr_pos);
    else
        return -1;

    uint8_t major;
    r = readTag0TemplateVersionMajor(HP_SMART_CARD_DEVICE_ID_0, &major);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Template Version(Major) = %d\n", major);
    else
        return -1;

    uint8_t minor;
    r = readTag0TemplateVersionMinor(HP_SMART_CARD_DEVICE_ID_0, &minor);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Template Version(Minor) = %d\n", minor);
    else
        return -1;

    r = inkWriteTag10OEMRWORField1(HP_SMART_CARD_DEVICE_ID_0, 0x10100101);
    if (HP_SMART_CARD_OK != r)
        return -1;

    r = inkWriteTag10OEMRWORField2(HP_SMART_CARD_DEVICE_ID_0, 0x01011010);
    if (HP_SMART_CARD_OK != r)
        return -1;

    r = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, 0xffff0000);
    if (HP_SMART_CARD_OK != r)
        return -1;

    r = inkWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_0, 0x0000ffff);
    if (HP_SMART_CARD_OK != r)
        return -1;

    uint32_t x;
    r = inkReadTag10OEMRWORField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Tag10 OEM Field 1 = 0x%08x\n", x);
    else
        return -1;

    r = inkReadTag10OEMRWORField2(HP_SMART_CARD_DEVICE_ID_0, &x);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Tag10 OEM Field 2 = 0x%08x\n", x);
    else
        return -1;

    r = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Tag12 OEM Field 1 = 0x%08x\n", x);
    else
        return -1;

    r = inkReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_0, &x);
    if (HP_SMART_CARD_OK == r)
        LOGD(">>>> Tag12 OEM Field 2 = 0x%08x\n", x);
    else
        return -1;

    r = inkWriteTag11MRUPlatformID(HP_SMART_CARD_DEVICE_ID_0, (unsigned char *)"abcdefgh1234");
    if (HP_SMART_CARD_OK != r)
        return -1;
*/

    uint16_t y;
    r = inkWriteTag11CartridgeMRUYear(HP_SMART_CARD_DEVICE_ID_0, 2020);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag11 Cartridge MRU Year <- %d\n", 2020);
    } else
        return -1;

    r = inkReadTag11CartridgeMRUYear(HP_SMART_CARD_DEVICE_ID_0, &y);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag11 Cartridge MRU Year -> %d\n", y);
    } else
        return -1;

    uint32_t x;

    r = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, 0xffff0000);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag12 OEM Field 1 <- 0x%08x\n", 0xffff0000);
    } else
        return -1;

    r = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag12 OEM Field 1 -> 0x%08x\n", x);
    } else
        return -1;

    r = inkReadTag61stInstallYear(HP_SMART_CARD_DEVICE_ID_0, &y);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag6 1st Installed Year -> %d\n", y);
    } else {
        return -1;
    }

    uint8_t w;
    r = inkReadTag61stInstallWeekOfYear(HP_SMART_CARD_DEVICE_ID_0, &w);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag6 1st Installed Week -> %d\n", w);
    } else {
        return -1;
    }

    r = inkReadTag13ExtendedOEMID(HP_SMART_CARD_DEVICE_ID_0, &w);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag13 Extended OEM ID -> 0x%d\n", w);
    } else {
        return -1;
    }

    r = inkWriteTag11MRUPlatformID(HP_SMART_CARD_DEVICE_ID_0, (unsigned char *)"123456789ABC---");
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag11 MRU Platform ID <- \"123456789ABC---\"\n");
    } else {
        return -1;
    }

    unsigned char buf[13] = {0x00};
    r = inkReadTag11MRUPlatformID(HP_SMART_CARD_DEVICE_ID_0, buf);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag11 MRU Platform ID -> \"%s\"\n", buf);
    } else {
        return -1;
    }

    memset(buf, 0x00, 13);
    r = inkReadTag8HPTrademark(HP_SMART_CARD_DEVICE_ID_0, buf);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>>> Tag8 Trademark -> \"%s\"\n", buf);
    } else {
        return -1;
    }

/*
    SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL_1);

    uint16_t config;
    ret = readConfig(&config);
    LOGD(">>> Return: %d, Config: 0x%04X", ret, config);

    config &= CONFIG_ACTIVE_MODE_ENABLE;
    ret = writeConfig(&config);

    ret = readConfig(&config);
    LOGD(">>> Return: %d, Config: 0x%04X", ret, config);

    uint16_t mfgID;
    ret = readManufactureID(&mfgID);
    LOGD(">>> Return: %d, MfgID: 0x%04X", ret, mfgID);

    uint16_t devID;
    ret = readDeviceID(&devID);
    LOGD(">>> Return: %d, DevID: 0x%04X", ret, devID);

    uint32_t chData;
    ret = readChannelData0(&chData);
    LOGD(">>> Return: %d, Data0: 0x%08X", ret, chData);
*/
    LIB_HP_SMART_CARD_shutdown();
    printf(">>> LIB_HP_SMART_CARD_shutdown() complete\n");

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
/*
    SC_I2C_DRIVER_RESULT ret = SC_I2C_DRIVER_open(0x01, 0x2b);

    if(ret != SC_I2C_DRIVER_RESULT_OK) return 0;

    jint data;
    ret = SC_I2C_DRIVER_read(4, 0, (uint8_t *)data);

    if(ret == SC_I2C_DRIVER_RESULT_OK) {
        return data;
    } else {
        return 0;
    }
*/
    return 0;
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
//            cardId = HP_SMART_CARD_DEVICE_ID_1;
            break;
        case COMPONET_SMART_CARD_3:
//            cardId = HP_SMART_CARD_DEVICE_ID_2;
            break;
        case COMPONET_SMART_CARD_4:
//            cardId = HP_SMART_CARD_DEVICE_ID_3;
            break;
    }

    if(cardId == -1 || LIB_HP_SMART_CARD_device_present(cardId) != HP_SMART_CARD_OK) {
        return -1;
    }

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

    LOGI("SmartCard.so 1.0.160 Loaded.");

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

