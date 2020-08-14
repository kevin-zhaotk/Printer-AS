//
// Created by kevin on 2019/2/17.
//

#include <stdio.h>
#include <jni.h>
#include <src/sc_ink_mem_access.h>
#include <src/sc_common_mem_access.h>
#include <src/sc_supply_mem_access.h>
#include <malloc.h>

#include "hp_host_smart_card.h"
#include "drivers/internal_ifc/hp_smart_card_gpio_ifc.h"
#include "common_log.h"
#include "com_smartcard.h"
#include "drivers/internal_ifc/sc_gpio_adapter.h"
#include "src/level_memory_access.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SC_SUCCESS                              0
#define SC_INIT_HOST_CARD_NOT_PRESENT           100
#define SC_INIT_PRNT_CTRG_NOT_PRESENT           110
#define SC_INIT_BULK_CTRG_NOT_PRESENT           111
#define SC_INIT_PRNT_CTRG_INIT_FAILED           120
#define SC_INIT_BULK_CTRG_INIT_FAILED           121
#define SC_INIT_PRNT_CTRG_FID_NOT_MATCH         130
#define SC_INIT_BULK_CTRG_FID_NOT_MATCH         131
#define SC_PRINT_CTRG_ACCESS_FAILED             200
#define SC_BULK_CTRG_ACCESS_FAILED              201
#define SC_LEVEL_CENSOR_ACCESS_FAILED           202
#define SC_CONSISTENCY_FAILED                   300

#define MAX_INK_VOLUME                          4700
#define INK_VOLUME_PER_CENTAGE                  (MAX_INK_VOLUME / 100)

HP_SMART_CARD_result_t (*ILGWriteFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit) = {
        supplyWriteTag9ILGBit01To25,
        supplyWriteTag9ILGBit26To50,
        supplyWriteTag9ILGBit51To75,
        supplyWriteTag9ILGBit76To100
};

HP_SMART_CARD_result_t (*ILGReadFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit) = {
        supplyReadTag9ILGBit01To25,
        supplyReadTag9ILGBit26To50,
        supplyReadTag9ILGBit51To75,
        supplyReadTag9ILGBit76To100
};

static char* toBinaryString(char* dst, uint32_t src) {
    uint32_t mask = 0x01000000;
    for(int i=0; i<25; i++) {
        if(mask & src) {
            dst[i] = '1';
        } else {
            dst[i] = '0';
        }
        mask >>= 1;
    }
    dst[25] = 0x00;
    return dst;
}

void print_returns(HP_SMART_CARD_result_t result)
{
    HW_SMART_CARD_status_t status = LIB_HP_SMART_CARD_last_status();
    LOGD("Result = %s (%d)  Status = %s (%d)\n",
           LIB_HP_SMART_CARD_result_string(result), result,
           LIB_HP_SMART_CARD_status_string(status), status);
}

void assert_handler(const char *error_str)
{
    LOGD("=========================================\n");
    LOGD("Smartcard Main: HP_ASSERT Failed\n");
    LOGD("%s\n", error_str);
    LOGD("=========================================\n");
}

void cache_monitor_failure_handler(HP_SMART_CARD_device_id_t dev_id,
                                   HP_SMART_CARD_result_t result)
{
    LOGD("=========================================\n");
    LOGD("Smartcard Main: Cache monitor failure\n");
    LOGD("Device Id = %d, ", dev_id);
    print_returns(result);
    LOGD("=========================================\n");
}

/*
 * 初始化HP智能卡设备，包括HOST卡，COMPONENT卡以及LEVEL
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_init(JNIEnv *env, jclass arg) {

    LOGI("Initializing smart card library....\n");

//    LIB_HP_SMART_CARD_shutdown();

    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    // Register for assert callback
//    LIB_HP_SMART_CARD_register_assert_callback(assert_handler);

    // Register for cache monitor callack
//    LIB_HP_SMART_CARD_register_cache_monitor_callback(cache_monitor_failure_handler);

//    LIB_HP_SMART_CARD_set_log_depth(0);

    // Initialise the library
    LIB_HP_SMART_CARD_init();

    if (LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return SC_INIT_HOST_CARD_NOT_PRESENT;
    }

    // Initialize Smart Card 0, this should be a print cartridge
    if (LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_0) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_0);
        return SC_INIT_PRNT_CTRG_NOT_PRESENT;
    }

    if (LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_0) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_ID_0);
        return SC_INIT_PRNT_CTRG_INIT_FAILED;
    }

    uint8_t family_id;
    HP_SMART_CARD_result_t r;

    r = readTag0FamilyID(HP_SMART_CARD_DEVICE_ID_0, &family_id);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>> Tag0 Family ID of Device %d -> %d\n", HP_SMART_CARD_DEVICE_ID_0, family_id);
        if(family_id != HP_SMART_CARD_INK_FAMILY_ID) return SC_INIT_PRNT_CTRG_FID_NOT_MATCH;
    } else {
        return SC_PRINT_CTRG_ACCESS_FAILED;
    }

    // Initialize Smart Card 1, this should be a bulk cartridge
    if (LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_1) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_1);
        return SC_INIT_BULK_CTRG_NOT_PRESENT;
    }

    if (LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_1) != HP_SMART_CARD_OK) {
        LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_ID_1);
        return SC_INIT_BULK_CTRG_INIT_FAILED;
    }

    r = readTag0FamilyID(HP_SMART_CARD_DEVICE_ID_1, &family_id);
    if (HP_SMART_CARD_OK == r) {
        LOGD(">>> Tag0 Family ID of Device %d -> %d\n", HP_SMART_CARD_DEVICE_ID_1, family_id);
        if(family_id != HPSCS_FAMILY_ID) return SC_INIT_BULK_CTRG_FID_NOT_MATCH;
    } else {
        return SC_BULK_CTRG_ACCESS_FAILED;
    }

    SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL);

    int ret;
    uint16_t config;

    ret = readConfig(&config);
    if(LEVEL_I2C_OK != ret) {
        return SC_LEVEL_CENSOR_ACCESS_FAILED;
    }
    LOGD(">>> Read: %d, Config: 0x%04X", ret, config);

    config &= CONFIG_ACTIVE_MODE_ENABLE;                // Set to Active mode
    ret = writeConfig(&config);
    if(LEVEL_I2C_OK != ret) {
        return SC_LEVEL_CENSOR_ACCESS_FAILED;
    }

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_checkConsistency(JNIEnv *env, jclass arg) {
    HP_SMART_CARD_result_t r = HP_SMART_CARD_OK;

////////////////////////    return SC_SUCCESS;

    uint8_t ink_designator, supply_designator;
    uint8_t ink_formulator_id, supply_formulator_id;
    uint8_t ink_family, supply_ink_family;
    uint8_t ink_color_code, supply_ink_color_code;
    uint8_t ink_family_member, supply_ink_family_member;

    r |= inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_0, &ink_designator);
    r |= inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_0, &ink_formulator_id);
    r |= inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_0, &ink_family);
    r |= inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_0, &ink_color_code);
    r |= inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_0, &ink_family_member);

    if (HP_SMART_CARD_OK != r) {
        return SC_PRINT_CTRG_ACCESS_FAILED;
    }

    r |= supplyReadTag1OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_1, &supply_designator);
    r |= supplyReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_1, &supply_formulator_id);
    r |= supplyReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family);
    r |= supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_color_code);
    r |= supplyReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family_member);

    if (HP_SMART_CARD_OK != r) {
        return SC_BULK_CTRG_ACCESS_FAILED;
    }

    if(ink_designator != supply_designator ||
       ink_formulator_id != supply_formulator_id ||
       ink_family != supply_ink_family ||
       ink_color_code != supply_ink_color_code ||
       ink_family_member != supply_ink_family_member) {
        return SC_CONSISTENCY_FAILED;
    }

    return SC_SUCCESS;
}

JNIEXPORT jstring JNICALL Java_com_Smartcard_readConsistency(JNIEnv *env, jclass arg) {
    char strTemp[1024];

    uint8_t ink_designator, supply_designator;
    uint8_t ink_formulator_id, supply_formulator_id;
    uint8_t ink_family, supply_ink_family;
    uint8_t ink_color_code, supply_ink_color_code;
    uint8_t ink_family_member, supply_ink_family_member;
    uint8_t out_of_ink;

    inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_0, &ink_designator);
    inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_0, &ink_formulator_id);
    inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_0, &ink_family);
    inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_0, &ink_color_code);
    inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_0, &ink_family_member);

    supplyReadTag1OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_1, &supply_designator);
    supplyReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_1, &supply_formulator_id);
    supplyReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family);
    supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_color_code);
    supplyReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family_member);

    HP_SMART_CARD_result_t r = supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, &out_of_ink);

    uint32_t value;
    char buf[4][26];
    for(int i=0; i<4; i++) {
        value = 0x00000000;
        ILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_1, &value);
        toBinaryString(buf[i], value);
    }

    sprintf(strTemp, "legal:{dev0:{dsg:%d,fml:%d,fam:%d,clr:%d,mem:%d},\ndev1:{dsg:%d,fml:%d,fam:%d,clr:%d,mem:%d}}\nOIB:%d\nILG:%s-%s-%s-%s",
        ink_designator, ink_formulator_id, ink_family, ink_color_code, ink_family_member,
        supply_designator, supply_formulator_id, supply_ink_family, supply_ink_color_code, supply_ink_family_member,
        out_of_ink,
        buf[3], buf[2], buf[1], buf[0]
    );

    return (*env)->NewStringUTF(env, strTemp);
}

JNIEXPORT jint JNICALL Java_com_Smartcard_chechOIB(JNIEnv *env, jclass arg, jint card) {
////////////////////////    return 0;

    uint8_t out_of_ink = 1;

    HP_SMART_CARD_result_t r = supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, &out_of_ink);

    if (HP_SMART_CARD_OK != r) {
        out_of_ink = 1;
    }

    return out_of_ink;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_getLocalInk(JNIEnv *env, jclass arg, jint card) {
////////////////////////    return 2000;

    uint32_t x = 0;

    HP_SMART_CARD_result_t r = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x);
    if (HP_SMART_CARD_OK != r) {
        return 0;
    }

    if(x == 0) {
        uint8_t out_of_ink;
        r = supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, &out_of_ink);

        if (HP_SMART_CARD_OK != r || out_of_ink == 1) {
            return 0;
        }

        r = supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, MAX_INK_VOLUME);
        if (HP_SMART_CARD_OK != r) {
            return 0;
        }
    }

    LOGD(">>> Ink Level = %d", x);

    return x;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_downLocal(JNIEnv *env, jclass arg, jint card) {
////////////////////////    return SC_SUCCESS;

    uint32_t x = 0;

    HP_SMART_CARD_result_t r = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x);
    if (HP_SMART_CARD_OK != r) {
        return SC_BULK_CTRG_ACCESS_FAILED;
    }

    int p1, p2;
    p1 = (MAX_INK_VOLUME - x) / INK_VOLUME_PER_CENTAGE;
    x--;
    p2 = (MAX_INK_VOLUME - x) / INK_VOLUME_PER_CENTAGE;

    r = supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, x);
    if (HP_SMART_CARD_OK != r) {
        return SC_BULK_CTRG_ACCESS_FAILED;
    }

    if(p1 != p2) {
        uint32_t value;
        char buf[4][26];

        for(int i=0; i<4; i++) {
            value = 0x00000000;
            ILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_1, &value);
            if(i == p1 / 25) {
                value |= (0x00000001 << (p1 % 25));
                ILGWriteFunc[i](HP_SMART_CARD_DEVICE_ID_1, value);
            }
            toBinaryString(buf[i], value);
        }

        LOGD(">>> ILG = %s-%s-%s-%s", buf[3], buf[2], buf[1], buf[0]);

        if(100 == p2) {
            LOGD(">>> OIB");
            supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, 1);
        }
    }

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_readLevel(JNIEnv *env, jclass arg, jint card) {
////////////////////////    return 0x77777777;

    int ret;

//    LOGD(">>> Enter readLevel(). Start 3 ioctls");
    SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL);
//    LOGD(">>> Finish 3 ioctls");

    uint32_t chData;
    ret = readChannelData0(&chData);
    LOGD(">>> Return: %d, Data0: 0x%08X", ret, chData);

    if(LEVEL_I2C_OK != ret) {
        chData = 0;
    }

    ///////////////////////////////////////////////
//    struct timeval t;
//    gettimeofday(&t, NULL);
//    chData = t.tv_usec % 1000;
    ///////////////////////////////////////////////

    return chData;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_close(JNIEnv *env, jclass arg) {
    LIB_HP_SMART_CARD_shutdown();
    LOGD(">>> LIB_HP_SMART_CARD_shutdown() complete\n");
    return SC_SUCCESS;
}

/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"init",					"()I",	                    (void *)Java_com_Smartcard_init},
        {"close",					"()I",						(void *)Java_com_Smartcard_close},
        {"checkConsistency",	    "()I",						(void *)Java_com_Smartcard_checkConsistency},
        {"readConsistency",	        "()Ljava/lang/String;",		(void *)Java_com_Smartcard_readConsistency},
        {"chechOIB",		        "(I)I",						(void *)Java_com_Smartcard_chechOIB},
        {"getLocalInk",		        "(I)I",						(void *)Java_com_Smartcard_getLocalInk},
        {"downLocal",		        "(I)I",						(void *)Java_com_Smartcard_downLocal},
        {"readLevel",		        "(I)I",						(void *)Java_com_Smartcard_readLevel},
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

    LOGI("SmartCard.so 1.0.269 Loaded.");

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

#ifdef __cplusplus
}
#endif
