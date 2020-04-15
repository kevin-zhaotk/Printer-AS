//
// Created by kevin on 2019/2/17.
//

#include <stdlib.h>
#include <jni.h>
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
        (JNIEnv *env, jclass arg)
{
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
    if(LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return -1;
    }

//    for (int i = 0; i < HP_SMART_CARD_DEVICE_NUM_DEVICES; i++) {
        if(LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_0) != HP_SMART_CARD_OK) {
            LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_0);
            return -1;
        }
        LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_0);
//    }
// End of Added by H.M.Wang 2019-10-17

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

    LOGI("SmartCard.so 1.0.136 Loaded.");

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

