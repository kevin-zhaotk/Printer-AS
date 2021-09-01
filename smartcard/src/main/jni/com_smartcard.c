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
#include <unistd.h>
#include <pthread.h>

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
#define SC_PRINT_CTRG_ACCESS_FAILED             200
#define SC_BULK_CTRG_ACCESS_FAILED              201
#define SC_LEVEL_CENSOR_ACCESS_FAILED           202
#define SC_CONSISTENCY_FAILED                   300
#define SC_CHECKSUM_FAILED                      400

#define PEN_VS_BAG_RATIO                        3
#define MAX_BAG_INK_VOLUME_MAXIMUM              4700
#define MAX_BAG_INK_VOLUME_MINIMUM              3150
#define MAX_PEN_INK_VOLUME                      MAX_BAG_INK_VOLUME_MINIMUM * PEN_VS_BAG_RATIO
#define INK_VOL_OF_BAG_PERCENTAGE               (MAX_BAG_INK_VOLUME_MINIMUM / 100)
#define INK_VOL_OF_PEN_PERCENTAGE               (MAX_PEN_INK_VOLUME / 100)

static int MaxBagInkVolume                      = MAX_BAG_INK_VOLUME_MINIMUM;
static int MaxPenInkVolume                      = MAX_PEN_INK_VOLUME;
static int InkVolOfBagPercentage                = INK_VOL_OF_BAG_PERCENTAGE;
static int InkVolOfPenPercentage                = INK_VOL_OF_PEN_PERCENTAGE;

static pthread_mutex_t mutex;

//#define DATA_SEPERATER                          100000      // 这之上是墨盒的减记次数（减记300次），这之下是墨盒/墨袋的减锁次数(MAX_INK_VOLUME)，

#define VERSION_CODE                            "1.0.373"

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

    pthread_mutex_destroy(&mutex);

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

    if (pthread_mutex_init(&mutex, NULL) != 0){
        return -1;
    }

    pthread_mutex_lock(&mutex);

    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    LIB_HP_SMART_CARD_init();

    if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST)) {
        LOGE(">>> LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_HOST): NOT PRESENT.  ");
        pthread_mutex_unlock(&mutex);
        return SC_INIT_HOST_CARD_NOT_PRESENT;
    }

    pthread_mutex_unlock(&mutex);

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_init_comp(JNIEnv *env, jclass arg, jint card ) {
    LOGI("Initializing smart card component...%d\n", card);

    pthread_mutex_lock(&mutex);

    if(CARD_SELECT_PEN1 == card) {
        // Initialize Smart Card 0, this should be a print cartridge
        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_PEN1)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_PEN1);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_PRNT_CTRG_NOT_PRESENT;
        }

        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_PEN1)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_PEN1);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_PRNT_CTRG_INIT_FAILED;
        }
        adjustLocalInkValue(HP_SMART_CARD_DEVICE_PEN1);
    } else if(CARD_SELECT_PEN2 == card) {
        // Initialize Smart Card 0, this should be a print cartridge
        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_PEN2)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_PEN2);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_PRNT_CTRG_NOT_PRESENT;
        }

        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_PEN2)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_PEN2);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_PRNT_CTRG_INIT_FAILED;
        }
        adjustLocalInkValue(HP_SMART_CARD_DEVICE_PEN2);
    } else if(CARD_SELECT_BULK1 == card || CARD_SELECT_BULKX == card) {
        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_present(HP_SMART_CARD_DEVICE_BULK1)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_present(%d): NOT PRESENT.  ", HP_SMART_CARD_DEVICE_BULK1);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_BULK_CTRG_NOT_PRESENT;
        }

        if (HP_SMART_CARD_OK != LIB_HP_SMART_CARD_device_init(HP_SMART_CARD_DEVICE_BULK1)) {
            LOGE(">>> LIB_HP_SMART_CARD_device_init(%d): Initialization Failed.  ", HP_SMART_CARD_DEVICE_BULK1);
            pthread_mutex_unlock(&mutex);
            return SC_INIT_BULK_CTRG_INIT_FAILED;
        }
        adjustLocalInkValue(HP_SMART_CARD_DEVICE_BULK1);
    } else if(SELECT_LEVEL1 == card) {
        SC_GPIO_ADAPTER_select_device(GPIO_DEVICE_PEN1);

        uint16_t config;

        if(LEVEL_I2C_OK != readConfig(&config)) {
            pthread_mutex_unlock(&mutex);
            return SC_LEVEL_CENSOR_ACCESS_FAILED;
        }
        LOGD(">>> Read config: 0x%04X", config);

        config &= CONFIG_ACTIVE_MODE_ENABLE;                // Set to Active mode
        if(LEVEL_I2C_OK != writeConfig(&config)) {
            pthread_mutex_unlock(&mutex);
            return SC_LEVEL_CENSOR_ACCESS_FAILED;
        }
        LOGD(">>> Write config: 0x%04X", config);
    } else if(SELECT_LEVEL2 == card) {
        SC_GPIO_ADAPTER_select_device(GPIO_DEVICE_PEN2);

        uint16_t config;

        if(LEVEL_I2C_OK != readConfig(&config)) {
            pthread_mutex_unlock(&mutex);
            return SC_LEVEL_CENSOR_ACCESS_FAILED;
        }
        LOGD(">>> Read config: 0x%04X", config);

        config &= CONFIG_ACTIVE_MODE_ENABLE;                // Set to Active mode
        if(LEVEL_I2C_OK != writeConfig(&config)) {
            pthread_mutex_unlock(&mutex);
            return SC_LEVEL_CENSOR_ACCESS_FAILED;
        }
        LOGD(">>> Write config: 0x%04X", config);
    }

    pthread_mutex_unlock(&mutex);

    return SC_SUCCESS;
}

static uint32_t calculateCheckSum(jint clientUniqueCode) {
    uint16_t manu_year = 0;
    supplyReadTag1ManufactureYear(HP_SMART_CARD_DEVICE_BULK1, &manu_year);
//    LOGD(">>>> HPSCS_SN_MFG_YEAR = %u", manu_year);

    uint8_t manu_woy = 0;
    supplyReadTag1ManufactureWeekOfYear(HP_SMART_CARD_DEVICE_BULK1, &manu_woy);
//    LOGD(">>>> HPSCS_SN_WEEK = %u", manu_woy);

    uint8_t manu_dow = 0;
    supplyReadTag1ManufactureDayOfWeek(HP_SMART_CARD_DEVICE_BULK1, &manu_dow);
//    LOGD(">>>> HPSCS_SN_DOW = %u", manu_dow);

    uint8_t manu_hod = 0;
    supplyReadTag1ManufactureHourOfDay(HP_SMART_CARD_DEVICE_BULK1, &manu_hod);
//    LOGD(">>>> HPSCS_SN_HOD = %u", manu_hod);

    uint8_t manu_moh = 0;
    supplyReadTag1ManufactureMinuteOfHour(HP_SMART_CARD_DEVICE_BULK1, &manu_moh);
//    LOGD(">>>> HPSCS_SN_MOH = %u", manu_moh);

    uint8_t manu_som = 0;
    supplyReadTag1ManufactureSecondOfMinute(HP_SMART_CARD_DEVICE_BULK1, &manu_som);
//    LOGD(">>>> HPSCS_SN_SOM = %u", manu_som);

    uint16_t fill_year = 0;
    supplyReadTag3CartridgeFillYear(HP_SMART_CARD_DEVICE_BULK1, &fill_year);
//    LOGD(">>>> HPSCS_FILL_YEAR = %u", fill_year);

    uint8_t fill_woy = 0;
    supplyReadTag3CartridgeFillWeekOfYear(HP_SMART_CARD_DEVICE_BULK1, &fill_woy);
//    LOGD(">>>> HPSCS_FILL_WEEK = %u", fill_woy);

    uint8_t fill_dow = 0;
    supplyReadTag3CartridgeFillDayOfWeek(HP_SMART_CARD_DEVICE_BULK1, &fill_dow);
//    LOGD(">>>> HPSCS_FILL_DOW = %u", fill_dow);

    uint8_t fill_hod = 0;
    supplyReadTag3CartridgeFillHourOfDay(HP_SMART_CARD_DEVICE_BULK1, &fill_hod);
//    LOGD(">>>> HPSCS_FILL_HOD = %u", fill_hod);

    uint8_t fill_moh = 0;
    supplyReadTag3CartridgeFillMinuteOfHour(HP_SMART_CARD_DEVICE_BULK1, &fill_moh);
//    LOGD(">>>> HPSCS_FILL_MOH = %u", fill_moh);

    uint8_t fill_som = 0;
    supplyReadTag3CartridgeFillSecondOfMinute(HP_SMART_CARD_DEVICE_BULK1, &fill_som);
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
    HP_SMART_CARD_result_t result = SC_CHECKSUM_FAILED;

    pthread_mutex_lock(&mutex);

    if(CARD_SELECT_PEN1 == card) {
        result = inkWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_PEN1, check_sum);
    } else if(CARD_SELECT_PEN2 == card) {
        result = inkWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_PEN2, check_sum);
    } else if(CARD_SELECT_BULK1 == card) {
        result = supplyWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_BULK1, check_sum);
    } else if(CARD_SELECT_BULKX == card) {
        result = inkWriteTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_BULK1, check_sum);
    }

    pthread_mutex_unlock(&mutex);

    return result;
}

/**
 * 读取验证码
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_checkSum(JNIEnv *env, jclass arg, jint card, jint clientUniqueCode) {
    uint32_t check_sum = calculateCheckSum(clientUniqueCode);

    uint32_t read_check_sum = -1;
    if(CARD_SELECT_PEN1 == card) {
        if (HP_SMART_CARD_OK != inkReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_PEN1, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    } else if(CARD_SELECT_PEN2 == card) {
        if (HP_SMART_CARD_OK != inkReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_PEN2, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    } else if(CARD_SELECT_BULK1 == card) {
        if (HP_SMART_CARD_OK != supplyReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_BULK1, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    } else if(CARD_SELECT_BULKX == card) {
        if (HP_SMART_CARD_OK != inkReadTag12OEMDefRWField2(HP_SMART_CARD_DEVICE_BULK1, &read_check_sum)) {
            return SC_CHECKSUM_FAILED;
        }
    }

//    LOGD(">>>> READ CHECK_SUM = 0x%08X", read_check_sum);

    if(check_sum != read_check_sum) {
        return SC_CHECKSUM_FAILED;
    }

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_checkConsistency(JNIEnv *env, jclass arg, jint card, jint supply) {
    uint8_t ink_designator, supply_designator;
    uint8_t ink_formulator_id, supply_formulator_id;
    uint8_t ink_family, supply_ink_family;
    uint8_t ink_color_code, supply_ink_color_code;
    uint8_t ink_family_member, supply_ink_family_member;

    if(CARD_SELECT_PEN1 == card) {
        if (HP_SMART_CARD_OK != inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_PEN1, &ink_designator) ||
            HP_SMART_CARD_OK != inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_PEN1, &ink_formulator_id) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_PEN1, &ink_family) ||
            HP_SMART_CARD_OK != inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_PEN1, &ink_color_code) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_PEN1, &ink_family_member)) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        }
    } else if(CARD_SELECT_PEN2 == card) {
        if (HP_SMART_CARD_OK != inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_PEN2, &ink_designator) ||
            HP_SMART_CARD_OK != inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_PEN2, &ink_formulator_id) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_PEN2, &ink_family) ||
            HP_SMART_CARD_OK != inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_PEN2, &ink_color_code) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_PEN2, &ink_family_member)) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        }
    } else {
        return SC_SUCCESS;
    }

    if(CARD_SELECT_BULK1 == supply) {
        if (HP_SMART_CARD_OK != supplyReadTag1OEMInkDesignator(HP_SMART_CARD_DEVICE_BULK1, &supply_designator) ||
            HP_SMART_CARD_OK != supplyReadTag4FormulatorID(HP_SMART_CARD_DEVICE_BULK1, &supply_formulator_id) ||
            HP_SMART_CARD_OK != supplyReadTag4InkFamily(HP_SMART_CARD_DEVICE_BULK1, &supply_ink_family) ||
            HP_SMART_CARD_OK != supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_BULK1, &supply_ink_color_code) ||
            HP_SMART_CARD_OK != supplyReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_BULK1, &supply_ink_family_member)) {
            return SC_BULK_CTRG_ACCESS_FAILED;
        }
    } else if(CARD_SELECT_BULKX == supply) {
        if (HP_SMART_CARD_OK != inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_BULK1, &ink_designator) ||
            HP_SMART_CARD_OK != inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_BULK1, &ink_formulator_id) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_BULK1, &ink_family) ||
            HP_SMART_CARD_OK != inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_BULK1, &ink_color_code) ||
            HP_SMART_CARD_OK != inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_BULK1, &ink_family_member)) {
            return SC_PRINT_CTRG_ACCESS_FAILED;
        }
    } else {
        return SC_SUCCESS;
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

JNIEXPORT int JNICALL Java_com_Smartcard_getMaxVolume(JNIEnv *env, jclass arg, jint card) {
    uint8_t drop_volume = 29;

    if(CARD_SELECT_PEN1 == card) {
        inkReadTag5DropVolume(HP_SMART_CARD_DEVICE_PEN1, &drop_volume);
    } else if(CARD_SELECT_PEN2 == card) {
        inkReadTag5DropVolume(HP_SMART_CARD_DEVICE_PEN2, &drop_volume);
    } else {
        return MaxBagInkVolume;
    }

    // bulk 计算
    // DV 29 对应3150
    // DV x=20-35 ：   3150*(29/X)
    // DV x < 20  or  X>35:    4700

    if(drop_volume < 20 || drop_volume > 35) {
        MaxBagInkVolume = MAX_BAG_INK_VOLUME_MAXIMUM;
    } else {
        MaxBagInkVolume = MAX_BAG_INK_VOLUME_MINIMUM * 29 / drop_volume;
    }

    MaxPenInkVolume                      = MaxBagInkVolume * PEN_VS_BAG_RATIO;
    InkVolOfBagPercentage                = MaxBagInkVolume / 100;
    InkVolOfPenPercentage                = MaxPenInkVolume / 100;

    return MaxBagInkVolume;
}

JNIEXPORT jstring JNICALL Java_com_Smartcard_readConsistency(JNIEnv *env, jclass arg, jint card) {
    char strTemp[1024];

    uint8_t ink_designator;
    uint8_t ink_formulator_id;
    uint8_t ink_family;
    uint8_t ink_color_code;
    uint8_t ink_family_member;
    uint8_t out_of_ink;
    float density, weight;
    uint8_t drop_volume;
    uint32_t x = 0;

    uint32_t value;
    char buf[4][26];
    char title[8];

    strcpy(title, "UNKNOWN");
    if(CARD_SELECT_PEN1 == card) {
        inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_PEN1, &ink_designator);
        inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_PEN1, &ink_formulator_id);
        inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_PEN1, &ink_family);
        inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_PEN1, &ink_color_code);
        inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_PEN1, &ink_family_member);
        inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN1, &out_of_ink);
        inkReadTag4Density(HP_SMART_CARD_DEVICE_PEN1, &density);
        inkReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_PEN1, &weight);
        inkReadTag5DropVolume(HP_SMART_CARD_DEVICE_PEN1, &drop_volume);
        for(int i=0; i<4; i++) {
            value = 0x00000000;
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN1, &value);
            toBinaryString(buf[i], value);
        }
        strcpy(title, "PEN1");
    } else if(CARD_SELECT_PEN2 == card) {
        inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_PEN2, &ink_designator);
        inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_PEN2, &ink_formulator_id);
        inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_PEN2, &ink_family);
        inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_PEN2, &ink_color_code);
        inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_PEN2, &ink_family_member);
        inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN2, &out_of_ink);
        inkReadTag4Density(HP_SMART_CARD_DEVICE_PEN2, &density);
        inkReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_PEN2, &weight);
        inkReadTag5DropVolume(HP_SMART_CARD_DEVICE_PEN2, &drop_volume);
        for(int i=0; i<4; i++) {
            value = 0x00000000;
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN2, &value);
            toBinaryString(buf[i], value);
        }
        strcpy(title, "PEN2");
    } else if(CARD_SELECT_BULK1 == card) {
        supplyReadTag1OEMInkDesignator(HP_SMART_CARD_DEVICE_BULK1, &ink_designator);
        supplyReadTag4FormulatorID(HP_SMART_CARD_DEVICE_BULK1, &ink_formulator_id);
        supplyReadTag4InkFamily(HP_SMART_CARD_DEVICE_BULK1, &ink_family);
        supplyReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_BULK1, &ink_color_code);
        supplyReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_BULK1, &ink_family_member);
        supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, &out_of_ink);
        supplyReadTag4Density(HP_SMART_CARD_DEVICE_BULK1, &density);
        supplyReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_BULK1, &weight);
        drop_volume = 0;
        for(int i=0; i<4; i++) {
            value = 0x00000000;
            supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &value);
            toBinaryString(buf[i], value);
        }
        if (HP_SMART_CARD_OK != supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x)) {
            x = 0;
        }
        strcpy(title, "BULK1");
    } else if(CARD_SELECT_BULKX == card) {
        inkReadTag13OEMInkDesignator(HP_SMART_CARD_DEVICE_BULK1, &ink_designator);
        inkReadTag4FormulatorID(HP_SMART_CARD_DEVICE_BULK1, &ink_formulator_id);
        inkReadTag4InkFamily(HP_SMART_CARD_DEVICE_BULK1, &ink_family);
        inkReadTag4ColorCodesGeneral(HP_SMART_CARD_DEVICE_BULK1, &ink_color_code);
        inkReadTag4InkFamilyMember(HP_SMART_CARD_DEVICE_BULK1, &ink_family_member);
        inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, &out_of_ink);
        inkReadTag4Density(HP_SMART_CARD_DEVICE_BULK1, &density);
        inkReadTag4UsableInkWeight(HP_SMART_CARD_DEVICE_BULK1, &weight);
        drop_volume = 0;
        for(int i=0; i<4; i++) {
            value = 0x00000000;
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &value);
            toBinaryString(buf[i], value);
        }
        strcpy(title, "BULKX");
    }

    sprintf(strTemp, "[%s]\ndsg:%d,fml:%d,fam:%d,clr:%d,mem:%d,ds:%f,wt:%f,dv:%d\nILG:%s-%s-%s-%s\nOIB:%d\nCount:%d",
        title, ink_designator, ink_formulator_id, ink_family, ink_color_code, ink_family_member, density, weight, drop_volume,buf[3], buf[2], buf[1], buf[0],out_of_ink,x
    );

    jstring result = (*env)->NewStringUTF(env, strTemp);
//    (*env)->ReleaseStringUTFChars(env, result, strTemp);

    return result;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_checkOIB(JNIEnv *env, jclass arg, jint card) {
    uint8_t out_of_ink = 1;

    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;
    if(CARD_SELECT_PEN1 == card) {
        ret = inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN1, &out_of_ink);
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN2, &out_of_ink);
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, &out_of_ink);
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkReadTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, &out_of_ink);
    }

    if (HP_SMART_CARD_OK != ret) {
        LOGE(">>> checkOIB access error(#%d)!", card);
        out_of_ink = 1;
    }

    LOGD(">>> checkOIB(#%d) = %d", card, out_of_ink);

    return out_of_ink;
}

// H.M.Wang 2021-8-9 修改根据ILG调整OEM数值的算法，由原来的从高位开始检测改为从低位开始检测，如果低位为1则增加一个百分比，检查紧邻高位，一次类推，遇到为0则退出。访问错误时不进行根据ILG对OEM值的修改
static void adjustLocalInkValue(jint card) {
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;
    uint32_t ilg = 0;
    uint8_t quit = 0;

    for(int i=0; i<4; i++) {
        uint32_t tmp_value = 0x00000000;
        if(CARD_SELECT_PEN1 == card) {
            ret = inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN1, &tmp_value);
        } else if(CARD_SELECT_PEN2 == card) {
            ret = inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN2, &tmp_value);
        } else if(CARD_SELECT_BULK1 == card) {
            ret = supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &tmp_value);
        } else if(CARD_SELECT_BULKX == card) {
            ret = inkILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &tmp_value);
        }

        if(HP_SMART_CARD_OK != ret) {
            break;
        }

        uint32_t mask = 0x00000001;
        for(int i=0; i<25; i++) {
            if(mask & tmp_value) {
                ilg++;
                mask <<= 1;
            } else {
                quit = 1;
                break;
            }
        }
        if(quit) break;
    }

    int vol_percentage = 1;
    uint32_t x = 0;
    if(CARD_SELECT_PEN1 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN1, &x);
        vol_percentage = InkVolOfPenPercentage;
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN2, &x);
        vol_percentage = InkVolOfPenPercentage;
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
        vol_percentage = InkVolOfBagPercentage;
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
        vol_percentage = InkVolOfBagPercentage;
    }
    if (HP_SMART_CARD_OK != ret) {
        x = 0;
    }

    x = (quit == 1 ? ilg * vol_percentage + x % vol_percentage : x);

    if(CARD_SELECT_PEN1 == card) {
        inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN1, x);
    } else if(CARD_SELECT_PEN2 == card) {
        inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN2, x);
    } else if(CARD_SELECT_BULK1 == card) {
        supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, x);
    } else if(CARD_SELECT_BULKX == card) {
        inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, x);
    }
}
// End of H.M.Wang 2021-8-9 修改根据ILG调整OEM数值的算法，由原来的从高位开始检测改为从低位开始检测，如果低位为1则增加一个百分比，检查紧邻高位，一次类推，遇到为0则退出。访问错误时不进行根据ILG对OEM值的修改

JNIEXPORT jint JNICALL Java_com_Smartcard_getLocalInk(JNIEnv *env, jclass arg, jint card) {
// 该判断另外逻辑处理，本函数如实返回读数
//    if(Java_com_Smartcard_checkOIB(env, arg, card) == 1) {
//        return 0;
//    }

    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    uint32_t x = -1;
    if(CARD_SELECT_PEN1 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN1, &x);
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN2, &x);
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
    }

    if (HP_SMART_CARD_OK != ret) {
        LOGE(">>> getLocalInk access error(#%d)!", card);
    }
    LOGD(">>> Ink Level(#%d) = %d", card, x);

    return x;
}

static void writeILG(jint card, int percent) {
    uint32_t value;
    char buf[4][26];

    if(percent < 0 || percent >= 100) return;

    for(int i=0; i<4; i++) {
        value = 0x00000000;
        if(CARD_SELECT_PEN1 == card) {
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN1, &value);
        } else if(CARD_SELECT_PEN2 == card) {
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_PEN2, &value);
        } else if(CARD_SELECT_BULK1 == card) {
            supplyILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &value);
        } else if(CARD_SELECT_BULKX == card) {
            inkILGReadFunc[i](HP_SMART_CARD_DEVICE_BULK1, &value);
        }

        // percent/25选择ilg(0:1-25; 1:26-50; 2:51:75; 3:76-100)
        // percent%25选择ilg的写入位(1%, 26%, 51%, 76%在第0位)
        if(i == percent / 25) {
            value |= (0x00000001 << (percent % 25));
            if(CARD_SELECT_PEN1 == card) {
                inkILGWriteFunc[i](HP_SMART_CARD_DEVICE_PEN1, value);
            } else if(CARD_SELECT_PEN2 == card) {
                inkILGWriteFunc[i](HP_SMART_CARD_DEVICE_PEN2, value);
            } else if(CARD_SELECT_BULK1 == card) {
                supplyILGWriteFunc[i](HP_SMART_CARD_DEVICE_BULK1, value);
            } else if(CARD_SELECT_BULKX == card) {
                inkILGWriteFunc[i](HP_SMART_CARD_DEVICE_BULK1, value);
            }
        }
        toBinaryString(buf[i], value);
    }

    LOGD(">>> ILG(#%d) = %s-%s-%s-%s", card, buf[3], buf[2], buf[1], buf[0]);
}

JNIEXPORT jint JNICALL Java_com_Smartcard_downLocal(JNIEnv *env, jclass arg, jint card) {
    uint32_t x = 0;
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    pthread_mutex_lock(&mutex);

    int vol_percentage = 1;

    if(CARD_SELECT_PEN1 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN1, &x);
        vol_percentage *= InkVolOfPenPercentage;
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN2, &x);
        vol_percentage *= InkVolOfPenPercentage;
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
        vol_percentage *= InkVolOfBagPercentage;
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkReadTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, &x);
        vol_percentage *= InkVolOfBagPercentage;
    }

    if (HP_SMART_CARD_OK != ret) {
        if(CARD_SELECT_PEN1 == card || CARD_SELECT_PEN2 == card) {
            pthread_mutex_unlock(&mutex);
            return SC_PRINT_CTRG_ACCESS_FAILED;
        } else if(CARD_SELECT_BULK1 == card || CARD_SELECT_BULKX == card) {
            pthread_mutex_unlock(&mutex);
            return SC_BULK_CTRG_ACCESS_FAILED;
        }
    }

    int p1, p2;
    p1 = x / vol_percentage;
    x++;
    p2 = x / vol_percentage;

    if(CARD_SELECT_PEN1 == card) {
        ret = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN1, x);
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_PEN2, x);
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, x);
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkWriteTag12OEMDefRWField1(HP_SMART_CARD_DEVICE_BULK1, x);
    }

    if (HP_SMART_CARD_OK != ret) {
        if(CARD_SELECT_PEN1 == card || CARD_SELECT_PEN2 == card) {
            pthread_mutex_unlock(&mutex);
            return SC_PRINT_CTRG_ACCESS_FAILED;
        } else if(CARD_SELECT_BULK1 == card || CARD_SELECT_BULKX == card) {
            pthread_mutex_unlock(&mutex);
            return SC_BULK_CTRG_ACCESS_FAILED;
        }
    }

    LOGD(">>> downLocal(#%d) -> %d", card, x);

    if(p1 != p2) {
        writeILG(card, p1);

        if(100 <= p2) {
            LOGD(">>> OIB(#%d)", card);
            if(CARD_SELECT_PEN1 == card) {
                inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN1, 1);
            } else if(CARD_SELECT_PEN2 == card) {
                inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN2, 1);
            } else if(CARD_SELECT_BULK1 == card) {
                supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, 1);
            } else if(CARD_SELECT_BULKX == card) {
                inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, 1);
            }
        }
    }

    pthread_mutex_unlock(&mutex);

    return SC_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_writeOIB(JNIEnv *env, jclass arg, jint card) {
    HP_SMART_CARD_result_t ret = HP_SMART_CARD_ERROR;

    pthread_mutex_lock(&mutex);

    LOGD(">>> Write OIB(#%d)", card);

    if(CARD_SELECT_PEN1 == card) {
        ret = inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN1, 1);
    } else if(CARD_SELECT_PEN2 == card) {
        ret = inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_PEN2, 1);
    } else if(CARD_SELECT_BULK1 == card) {
        ret = supplyWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, 1);
    } else if(CARD_SELECT_BULKX == card) {
        ret = inkWriteTag9ILGOutOfInkBit(HP_SMART_CARD_DEVICE_BULK1, 1);
    }

    pthread_mutex_unlock(&mutex);

    return ret;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_readLevel(JNIEnv *env, jclass arg, jint card) {
    LOGD(">>> Read Level(#%d)", card);

    pthread_mutex_lock(&mutex);

    if(SELECT_LEVEL1 == card) {
        SC_GPIO_ADAPTER_select_device(GPIO_DEVICE_PEN1);
    } else if(SELECT_LEVEL2 == card) {
        SC_GPIO_ADAPTER_select_device(GPIO_DEVICE_PEN2);
    } else {
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    uint32_t chData;

    if(LEVEL_I2C_OK != readChannelData0(&chData)) {
        chData = 0;
    }

    LOGD(">>> Level data read: 0x%08X", chData);

    pthread_mutex_unlock(&mutex);

    return chData;
}

/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"exist",					"()I",	                    (void *)Java_com_Smartcard_exist},
        {"init",					"()I",	                    (void *)Java_com_Smartcard_init},
        {"initComponent",			"(I)I",	                    (void *)Java_com_Smartcard_init_comp},
        {"writeCheckSum",	        "(II)I",					(void *)Java_com_Smartcard_writeCheckSum},
        {"checkSum",	            "(II)I",					(void *)Java_com_Smartcard_checkSum},
        {"checkConsistency",	    "(II)I",					(void *)Java_com_Smartcard_checkConsistency},
        {"getMaxVolume",	        "(I)I",						(void *)Java_com_Smartcard_getMaxVolume},
        {"readConsistency",	        "(I)Ljava/lang/String;",	(void *)Java_com_Smartcard_readConsistency},
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
