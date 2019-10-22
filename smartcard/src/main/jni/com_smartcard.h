//
// Created by kevin on 2019/2/17.
//

#include <jni.h>

#ifndef _PRINTER_AS_COM_SMARTCARD_H
#define _PRINTER_AS_COM_SMARTCARD_H
#ifdef __cplusplus
extern "C" {
#endif

/* Added by H.M.Wang 2019-10-17 */
#define COMPONET_SMART_CARD_1   11
#define COMPONET_SMART_CARD_2   12
#define COMPONET_SMART_CARD_3   13
#define COMPONET_SMART_CARD_4   14

#define COMPONET_LEVEL_1        21
#define COMPONET_LEVEL_2        22
#define COMPONET_LEVEL_3        23
#define COMPONET_LEVEL_4        24
/* Added by H.M.Wang 2019-10-17 end */

/**************************************
 * RTC操作接口
 **************************************/
/**
 * 打开RTC设备
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_init
    (JNIEnv *env, jclass arg);

/**
 * 关闭RTC设备
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_close
    (JNIEnv *env, jclass arg);

JNIEXPORT jint JNICALL Java_com_Smartcard_getSmartCardData
        (JNIEnv *env, jclass arg, jint card);

JNIEXPORT jint JNICALL Java_com_Smartcard_getLevelData
        (JNIEnv *env, jclass arg, jint card);


#ifdef __cplusplus
}
#endif
#endif //_PRINTER_AS_COM_SMARTCARD_H
