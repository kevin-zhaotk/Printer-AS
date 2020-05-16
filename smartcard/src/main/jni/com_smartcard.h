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
#define HP_PRINT_CARTRIDGE      11
#define HP_BULK_CARTRIDGE       12
#define LEVEL_SENSOR            21
/* Added by H.M.Wang 2019-10-17 end */

/*
 * 初始化HP智能卡设备，包括HOST卡，COMPONENT卡以及LEVEL
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_init(JNIEnv *env, jclass arg);

/**
 * 检查墨袋参数一致性
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_chechConsistency(JNIEnv *env, jclass arg, jint card);

/**
 * 检查墨袋是否已经用完
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_checkOIB(JNIEnv *env, jclass arg, jint card);

/**
 * 获取剩余墨量数据
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_getLocalInk(JNIEnv *env, jclass arg, jint card);

JNIEXPORT jint JNICALL Java_com_Smartcard_downLocal(JNIEnv *env, jclass arg, jint card);

JNIEXPORT jint JNICALL Java_com_Smartcard_readLevel(JNIEnv *env, jclass arg, jint card);


#ifdef __cplusplus
}
#endif
#endif //_PRINTER_AS_COM_SMARTCARD_H
