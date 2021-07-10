//
// Created by kevin on 2019/2/17.
//

#include <jni.h>

#ifndef _PRINTER_AS_COM_HP22MM_H
#define _PRINTER_AS_COM_HP22MM_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * 初始化HP智能卡设备和HOST卡
 */
JNIEXPORT jint JNICALL Java_com_hp22mm_init(JNIEnv *env, jclass arg);

#ifdef __cplusplus
}
#endif
#endif //_PRINTER_AS_COM_HP22MM_H
