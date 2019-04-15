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
    int ret=0;
    HP_SMART_CARD_gpio_init();
    HP_SMART_CARD_i2c_init();

    printf("Initializing smart card library....\n");

    // Register for assert callback
    LIB_HP_SMART_CARD_register_assert_callback(assert_handler);

    // Register for cache monitor callack
    LIB_HP_SMART_CARD_register_cache_monitor_callback(cache_monitor_failure_handler);

    // Initialise the library
    LIB_HP_SMART_CARD_init();
    return ret;
}

JNIEXPORT jint JNICALL Java_com_Smartcard_close
        (JNIEnv *env, jclass arg, jint fd)
{
    int ret=0;
    return ret;
}
/**
 * RTC操作jni接口
 */
static JNINativeMethod gMethods[] = {
        {"open",					"(Ljava/lang/String;)I",	(void *)Java_com_Smartcard_init},
        {"close",					"(I)I",						(void *)Java_com_Smartcard_close},
};

/**
 * 注册RTC操作的JNI方法
 */
int register_com_smartcard(JNIEnv* env) {
    const char* kClassPathName = "com/industry/printer/hardware/RTCDevice";
    jclass clazz = (*env)->FindClass(env, kClassPathName);
    if(clazz == NULL) {
        return JNI_FALSE;
    }
    return (*env)->RegisterNatives(env, clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0]));
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

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

