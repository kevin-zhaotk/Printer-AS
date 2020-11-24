//
// Created by kevin on 2019/2/17.
//

#include <stdio.h>
#include <jni.h>
#include <src/sc_ink_mem_access.h>
#include <src/sc_common_mem_access.h>
#include <src/sc_supply_mem_access.h>
#include <malloc.h>
#include <drivers/internal_ifc/sc_gpio_driver.h>

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

#define HP_PRINT_CARTRIDGE                      11
#define HP_BULK_CARTRIDGE                       12
#define LEVEL_SENSOR                            21

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
#define SC_CHECKSUM_FAILED                      400

#define MAX_BAG_INK_VOLUME                      4000
#define MAX_PEN_INK_VOLUME                      300
#define INK_VOLUME_PER_CENTAGE                  (MAX_BAG_INK_VOLUME / 100)

#define DATA_SEPERATER                          100000      // 这之上是墨盒的减记次数（减记300次），这之下是墨盒/墨袋的减锁次数(MAX_INK_VOLUME)，

#define VERSION_CODE                            "1.0.343"

HP_SMART_CARD_result_t (*inkILGWriteFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit) = {
        inkWriteTag9ILGBit01To25,
        inkWriteTag9ILGBit26To50,
        inkWriteTag9ILGBit51To75,
        inkWriteTag9ILGBit76To100
};

HP_SMART_CARD_result_t (*inkILGReadFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit) = {
        inkReadTag9ILGBit01To25,
        inkReadTag9ILGBit26To50,
        inkReadTag9ILGBit51To75,
        inkReadTag9ILGBit76To100
};

HP_SMART_CARD_result_t (*supplyILGWriteFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t ilg_bit) = {
        supplyWriteTag9ILGBit01To25,
        supplyWriteTag9ILGBit26To50,
        supplyWriteTag9ILGBit51To75,
        supplyWriteTag9ILGBit76To100
};

HP_SMART_CARD_result_t (*supplyILGReadFunc[4])(HP_SMART_CARD_device_id_t cardId, uint32_t *ilg_bit) = {
        supplyReadTag9ILGBit01To25,
        supplyReadTag9ILGBit26To50,
        supplyReadTag9ILGBit51To75,
        supplyReadTag9ILGBit76To100
};

static void adjustLocalInkValue(jint card);

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

JNIEXPORT jint JNICALL Java_com_Smartcard_shutdown(JNIEnv *env, jclass arg) {
    LOGI("Shutting down smart card library....\n");

    LIB_HP_SMART_CARD_shutdown();

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_exist(JNIEnv *env, jclass arg) {
    LOGI("Checking smart card existence....\n");

    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    LIB_HP_SMART_CARD_init();

    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return SC_INIT_HOST_CARD_NOT_PRESENT;
    }
    return SC_SUCCESS;
}

/*
 * 初始化HP智能卡设备，包括HOST卡，COMPONENT卡以及LEVEL
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_init(JNIEnv *env, jclass arg) {
    LOGI("Initializing smart card library....%s\n", VERSION_CODE);

    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    // Register for assert callback
//    LIB_HP_SMART_CARD_register_assert_callback(assert_handler);

    // Register for cache monitor callack
//    LIB_HP_SMART_CARD_register_cache_monitor_callback(cache_monitor_failure_handler);

//    LIB_HP_SMART_CARD_set_log_depth(0);

    // Initialise the library
    LIB_HP_SMART_CARD_init();

    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        return SC_INIT_HOST_CARD_NOT_PRESENT;
    }

    // Initialize Smart Card 0, this should be a print cartridge
    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_0)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_0);
        return SC_INIT_PRNT_CTRG_NOT_PRESENT;
    }

    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_0)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_ID_0);
        return SC_INIT_PRNT_CTRG_INIT_FAILED;
    }

    uint8_t family_id;
//    HP_SMART_CARD_result_t r;
    if (HP_SMART_CARD_OK == readTag0FamilyID(HP_SMART_CARD_DEVICE_ID_0, &family_id)) {
        LOGD(">>> Tag0 Family ID of Device %d -> %d\n", HP_SMART_CARD_DEVICE_ID_0, family_id);
        if(family_id != HP_SMART_CARD_INK_FAMILY_ID) return SC_INIT_PRNT_CTRG_FID_NOT_MATCH;
    } else {
        return SC_PRINT_CTRG_ACCESS_FAILED;
    }

    // Initialize Smart Card 1, this should be a bulk cartridge
    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_ID_1)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_ID_1);
        return SC_INIT_BULK_CTRG_NOT_PRESENT;
    }

    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_ID_1)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_ID_1);
        return SC_INIT_BULK_CTRG_INIT_FAILED;
    }

    if (HP_SMART_CARD_OK == readTag0FamilyID(HP_SMART_CARD_DEVICE_ID_1, &family_id)) {
        LOGD(">>> Tag0 Family ID of Device %d -> %d\n", HP_SMART_CARD_DEVICE_ID_1, family_id);
        if(family_id != HPSCS_FAMILY_ID) return SC_INIT_BULK_CTRG_FID_NOT_MATCH;
    } else {
        return SC_BULK_CTRG_ACCESS_FAILED;
    }

    adjustLocalInkValue(HP_SMART_CARD_DEVICE_ID_0);
    adjustLocalInkValue(HP_SMART_CARD_DEVICE_ID_1);

// 初始化的时候将墨盒和墨袋的计数器清零（为了测试）
//    inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, 0);
//    supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, 0);

    SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL);

    uint16_t config;

    if(LEVEL_I2C_OK != readConfig(&config)) {
        return SC_LEVEL_CENSOR_ACCESS_FAILED;
    }
    LOGD(">>> Read config: 0x%04X", config);

    config &= CONFIG_ACTIVE_MODE_ENABLE;                // Set to Active mode
    if(LEVEL_I2C_OK != writeConfig(&config)) {
        return SC_LEVEL_CENSOR_ACCESS_FAILED;
    }
    LOGD(">>> Write config: 0x%04X", config);

    SC_GPIO_ADAPTER_select_38_xlater(I2C_BUS_HANGUP);

    return SC_SUCCESS;
}

static uint32_t calculateCheckSum(jint clientUniqueCode) {
    uint16_t manu_year = 0;
    supplyReadTag1ManufactureYear(HP_SMART_CARD_DEVICE_ID_1, &manu_year);
//    LOGD(">>>> HPSCS_SN_MFG_YEAR = %u", manu_year);

    uint8_t manu_woy = 0;
    supplyReadTag1ManufactureWeekOfYear(HP_SMART_CARD_DEVICE_ID_1, &manu_woy);
//    LOGD(">>>> HPSCS_SN_WEEK = %u", manu_woy);

    uint8_t manu_dow = 0;
    supplyReadTag1ManufactureDayOfWeek(HP_SMART_CARD_DEVICE_ID_1, &manu_dow);
//    LOGD(">>>> HPSCS_SN_DOW = %u", manu_dow);

    uint8_t manu_hod = 0;
    supplyReadTag1ManufactureHourOfDay(HP_SMART_CARD_DEVICE_ID_1, &manu_hod);
//    LOGD(">>>> HPSCS_SN_HOD = %u", manu_hod);

    uint8_t manu_moh = 0;
    supplyReadTag1ManufactureMinuteOfHour(HP_SMART_CARD_DEVICE_ID_1, &manu_moh);
//    LOGD(">>>> HPSCS_SN_MOH = %u", manu_moh);

    uint8_t manu_som = 0;
    supplyReadTag1ManufactureSecondOfMinute(HP_SMART_CARD_DEVICE_ID_1, &manu_som);
//    LOGD(">>>> HPSCS_SN_SOM = %u", manu_som);

    uint16_t fill_year = 0;
    supplyReadTag3CartridgeFillYear(HP_SMART_CARD_DEVICE_ID_1, &fill_year);
//    LOGD(">>>> HPSCS_FILL_YEAR = %u", fill_year);

    uint8_t fill_woy = 0;
    supplyReadTag3CartridgeFillWeekOfYear(HP_SMART_CARD_DEVICE_ID_1, &fill_woy);
//    LOGD(">>>> HPSCS_FILL_WEEK = %u", fill_woy);

    uint8_t fill_dow = 0;
    supplyReadTag3CartridgeFillDayOfWeek(HP_SMART_CARD_DEVICE_ID_1, &fill_dow);
//    LOGD(">>>> HPSCS_FILL_DOW = %u", fill_dow);

    uint8_t fill_hod = 0;
    supplyReadTag3CartridgeFillHourOfDay(HP_SMART_CARD_DEVICE_ID_1, &fill_hod);
//    LOGD(">>>> HPSCS_FILL_HOD = %u", fill_hod);

    uint8_t fill_moh = 0;
    supplyReadTag3CartridgeFillMinuteOfHour(HP_SMART_CARD_DEVICE_ID_1, &fill_moh);
//    LOGD(">>>> HPSCS_FILL_MOH = %u", fill_moh);

    uint8_t fill_som = 0;
    supplyReadTag3CartridgeFillSecondOfMinute(HP_SMART_CARD_DEVICE_ID_1, &fill_som);
//    LOGD(">>>> HPSCS_FILL_SOM = %u", fill_som);

    uint32_t sum = clientUniqueCode;

    sum += manu_year;
    sum += manu_woy;
    sum += manu_dow;
    sum += manu_hod;
    sum += manu_moh;
    sum += manu_som;

    sum *= 10000;

    sum += fill_year;
    sum += fill_woy;
    sum += fill_dow;
    sum += fill_hod;
    sum += fill_moh;
    sum += fill_som;

    uint32_t par = sum % 47;
    uint32_t check_sum = par * 100000000 + sum;
    check_sum ^= 0x55555555;
//    LOGD(">>>> CHECK_SUM = 0x%08X", check_sum);

    return check_sum;
}

/**
 * 写入验证码
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_writeCheckSum(JNIEnv *env, jclass arg, jint card, jint clientUniqueCode) {
    uint32_t check_sum = calculateCheckSum(clientUniqueCode);

    if(HP_PRINT_CARTRIDGE == card) {
        return inkWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_0, check_sum);
    } else if(HP_BULK_CARTRIDGE == card) {
        return supplyWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_1, check_sum);
    }
    return SC_CHECKSUM_FAILED;
}

/**
 * 读取验证码
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_checkSum(JNIEnv *env, jclass arg, jint card, jint clientUniqueCode) {
    uint32_t check_sum = calculateCheckSum(clientUniqueCode);

    uint32_t read_check_sum = -1;
    if(HP_PRINT_CARTRIDGE == card) {
        if (HP_SMART_CARD_OK != inkReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_0, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    } else if(HP_BULK_CARTRIDGE == card) {
        if (HP_SMART_CARD_OK != supplyReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_ID_1, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    }

//    LOGD(">>>> READ CHECK_SUM = 0x%08X", read_check_sum);

    if(check_sum != read_check_sum) {
        return SC_CHECKSUM_FAILED;
    }

    return SC_SUCCESS;
}


JNIEXPORT jint JNICALL Java_com_Smartcard_checkConsistency(JNIEnv *env, jclass arg) {
    uint8_t ink_designator, supply_designator;
    uint8_t ink_formulator_id, supply_formulator_id;
    uint8_t ink_family, supply_ink_family;
    uint8_t ink_color_code, supply_ink_color_code;
    uint8_t ink_family_member, supply_ink_family_member;

    if (HP_SMART_CARD_OK != inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_0, &ink_designator) ||
        HP_SMART_CARD_OK != inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_0, &ink_formulator_id) ||
        HP_SMART_CARD_OK != inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_0, &ink_family) ||
        HP_SMART_CARD_OK != inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_0, &ink_color_code) ||
        HP_SMART_CARD_OK != inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_0, &ink_family_member)) {
        return SC_PRINT_CTRG_ACCESS_FAILED;
    }

    if (HP_SMART_CARD_OK != supplyReadTag1OEMInkDesignator(HP_SMART_CARD_DEVICE_ID_1, &supply_designator) ||
        HP_SMART_CARD_OK != supplyReadTag4FormulatorID(HP_SMART_CARD_DEVICE_ID_1, &supply_formulator_id) ||
        HP_SMART_CARD_OK != supplyReadTag4InkFamily(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family) ||
        HP_SMART_CARD_OK != supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_color_code) ||
        HP_SMART_CARD_OK != supplyReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_ID_1, &supply_ink_family_member)) {
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

    supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, &out_of_ink);

    uint32_t value;
    char buf[4][26];
    for(int i=0; i<4; i++) {
        value = 0x00000000;
        supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_1, &value);
        toBinaryString(buf[i], value);
    }

    uint32_t x = 0;
    if (HP_SMART_CARD_OK != supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x)) {
        x = 0;
    }

    float density0, weight0, density1, weight1;
    int drop_volume0, drop_volume1;

    inkReadTag4Density(HP_SMART_CARD_DEVICE_ID_0, &density0);
    inkReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_ID_0, &weight0);
    inkReadTag5DropVolume(HP_SMART_CARD_DEVICE_ID_0, &drop_volume0);

    supplyReadTag4Density(HP_SMART_CARD_DEVICE_ID_1, &density1);
    supplyReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_ID_1, &weight1);

    sprintf(strTemp, "legal:{dev0:{dsg:%d,fml:%d,fam:%d,clr:%d,mem:%d,ds:%f,wt:%f,dv:%d},\ndev1:{dsg:%d,fml:%d,fam:%d,clr:%d,mem:%d,ds:%f,wt:%f}}\nOIB:%d\nILG:%s-%s-%s-%s\nCount:%d",
        ink_designator, ink_formulator_id, ink_family, ink_color_code, ink_family_member, density0, weight0, drop_volume0,
        supply_designator, supply_formulator_id, supply_ink_family, supply_ink_color_code, supply_ink_family_member, density1, weight1,
        out_of_ink,
        buf[3], buf[2], buf[1], buf[0], x
    );

    return (*env)->NewStringUTF(env, strTemp);
}

JNIEXPORT jint JNICALL Java_com_Smartcard_checkOIB(JNIEnv *env, jclass arg, jint card) {
    uint8_t out_of_ink = 1;

    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;
    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_0, &out_of_ink);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, &out_of_ink);
    }

    if (HP_SMART_CARD_OK != ret) {
        out_of_ink = 1;
    }

    return out_of_ink;
}

static void adjustLocalInkValue(jint card) {
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;
    uint32_t ilg = 100;

    for(int i=3; i>=0; i--) {
        uint32_t tmp_value = 0x00000000;
        if(HP_PRINT_CARTRIDGE == card) {
            ret = inkILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_0, &tmp_value);
        } else if(HP_BULK_CARTRIDGE == card) {
            ret = supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_1, &tmp_value);
        }

        if(HP_SMART_CARD_OK != ret) {
            tmp_value = 0x00000000;
        }

        if(!tmp_value) {
            ilg -= 25;
            continue;
        }

        uint32_t mask = 0x01000000;
        for(int i=0; i<25; i++) {
            if(mask & tmp_value) {
                break;
            }
            ilg--;
            mask >>= 1;
        }
        break;
    }

    uint32_t x = 0;
    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x);
    }
    if (HP_SMART_CARD_OK != ret) {
        x = 0;
    }

    // 100000以上给墨盒的200次减记留位，虽然在墨袋的时候不需要，为了处理统一也留。
    // 目的是为了在正式版本里面墨袋减记两次，墨盒减记一次。如果是使用墨盒替代墨袋做实验，那就在模拟墨袋的次数减慢后这个墨盒就已经不可用了，没有影响

    uint32_t value = x % DATA_SEPERATER;
    value = ilg * INK_VOLUME_PER_CENTAGE + value % INK_VOLUME_PER_CENTAGE;
    value += (int)(x / DATA_SEPERATER) * DATA_SEPERATER;

    if(HP_PRINT_CARTRIDGE == card) {
        inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, value);
    } else if(HP_BULK_CARTRIDGE == card) {
        supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, value);
    }
}

JNIEXPORT jint JNICALL Java_com_Smartcard_getLocalInk(JNIEnv *env, jclass arg, jint card) {
// 该判断另外逻辑处理，本函数如实返回读数
//    if(Java_com_Smartcard_checkOIB(env, arg, card) == 1) {
//        return 0;
//    }

    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    uint32_t x = MAX_BAG_INK_VOLUME;
    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x);
    }

    uint32_t y = MAX_PEN_INK_VOLUME;
    ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &y);

    uint32_t value = (y / DATA_SEPERATER) * DATA_SEPERATER + x % DATA_SEPERATER;

    LOGD(">>> Ink Level = %d", value);

    return value;
}

static void writeILG(jint card, int percent) {
    uint32_t value;
    char buf[4][26];

    if(percent < 0 || percent >= 100) return;

    for(int i=0; i<4; i++) {
        value = 0x00000000;
        if(HP_PRINT_CARTRIDGE == card) {
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_0, &value);
        } else if(HP_BULK_CARTRIDGE == card) {
            supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_ID_1, &value);
        }

        // percent/25选择ilg(0:1-25; 1:26-50; 2:51:75; 3:76-100)
        // percent%25选择ilg的写入位(1%, 26%, 51%, 76%在第0位)
        if(i == percent / 25) {
            value |= (0x00000001 << (percent % 25));
            if(HP_PRINT_CARTRIDGE == card) {
                inkILGWriteFunc[i](HP_SMART_CARD_DEVICE_ID_0, value);
            } else if(HP_BULK_CARTRIDGE == card) {
                supplyILGWriteFunc[i](HP_SMART_CARD_DEVICE_ID_1, value);
            }
        }
        toBinaryString(buf[i], value);
    }

    LOGD(">>> ILG(#%d) = %s-%s-%s-%s", card, buf[3], buf[2], buf[1], buf[0]);
}

JNIEXPORT jint JNICALL Java_com_Smartcard_downLocal(JNIEnv *env, jclass arg, jint card) {
    uint32_t x = 0;
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, &x);
    }

    if (HP_SMART_CARD_OK != ret) {
        if(HP_PRINT_CARTRIDGE == card) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        } else if(HP_BULK_CARTRIDGE == card) {
            return SC_BULK_CTRG_ACCESS_FAILED;
        }
    }

    int p1, p2;
    p1 = (x % DATA_SEPERATER) / INK_VOLUME_PER_CENTAGE;
    x++;
    p2 = (x % DATA_SEPERATER) / INK_VOLUME_PER_CENTAGE;

    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, x);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_1, x);
    }

    if (HP_SMART_CARD_OK != ret) {
        if(HP_PRINT_CARTRIDGE == card) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        } else if(HP_BULK_CARTRIDGE == card) {
            return SC_BULK_CTRG_ACCESS_FAILED;
        }
    }

    if(p1 != p2) {
        writeILG(card, p1);

        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, &x);
        if (HP_SMART_CARD_OK != ret) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        }

        uint32_t y = x / DATA_SEPERATER;
        y++;
        x = (y * DATA_SEPERATER) + (x % DATA_SEPERATER);

        inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_ID_0, x);
        writeILG(HP_SMART_CARD_DEVICE_ID_0, (y/3 - 1));
        if(100 <= y/3) {
            LOGD(">>> Print Cartridge OIB");
            inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_0, 1);
        }

        if(100 <= p2) {
            LOGD(">>> OIB");
            if(HP_PRINT_CARTRIDGE == card) {
                inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_0, 1);
            } else if(HP_BULK_CARTRIDGE == card) {
                supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, 1);
            }
        }
    }

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_writeOIB(JNIEnv *env, jclass arg, jint card) {
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    LOGD(">>> Write OIB");
    if(HP_PRINT_CARTRIDGE == card) {
        ret = inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_0, 1);
    } else if(HP_BULK_CARTRIDGE == card) {
        ret = supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_ID_1, 1);
    }

    return ret;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_readLevel(JNIEnv *env, jclass arg, jint card) {
    if(LEVEL_SENSOR != card) return 0;

    SC_GPIO_ADAPTER_select_38_xlater(SELECT_LEVEL);

    uint32_t chData;

    if(LEVEL_I2C_OK != readChannelData0(&chData)) {
        chData = 0;
    }

    SC_GPIO_ADAPTER_select_38_xlater(I2C_BUS_HANGUP);

    LOGD(">>> Level data read: 0x%08X", chData);

    return chData;
}

/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"exist",					"()I",	                    (void *)Java_com_Smartcard_exist},
        {"init",					"()I",	                    (void *)Java_com_Smartcard_init},
        {"writeCheckSum",	        "(II)I",					(void *)Java_com_Smartcard_writeCheckSum},
        {"checkSum",	            "(II)I",					(void *)Java_com_Smartcard_checkSum},
        {"checkConsistency",	    "()I",						(void *)Java_com_Smartcard_checkConsistency},
        {"readConsistency",	        "()Ljava/lang/String;",		(void *)Java_com_Smartcard_readConsistency},
        {"checkOIB",		        "(I)I",						(void *)Java_com_Smartcard_checkOIB},
        {"getLocalInk",		        "(I)I",						(void *)Java_com_Smartcard_getLocalInk},
        {"downLocal",		        "(I)I",						(void *)Java_com_Smartcard_downLocal},
        {"writeOIB",		        "(I)I",						(void *)Java_com_Smartcard_writeOIB},
        {"readLevel",		        "(I)I",						(void *)Java_com_Smartcard_readLevel},
        {"shutdown",				"()I",	                    (void *)Java_com_Smartcard_shutdown},
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

    LOGI("SmartCard.so %s Loaded.", VERSION_CODE);

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
