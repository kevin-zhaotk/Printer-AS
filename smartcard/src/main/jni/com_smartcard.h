//
// Created by kevin on 2019/2/17.
//

#ifndef PRINTER_AS_COM_SMARTCARD_H
#define PRINTER_AS_COM_SMARTCARD_H

/**************************************
 * RTC操作接口
 **************************************/
/**
 * 打开RTC设备
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_open
        (JNIEnv *env, jclass arg, jstring dev);

/**
 * 关闭RTC设备
 */
JNIEXPORT jint JNICALL Java_com_Smartcard_close
        (JNIEnv *env, jclass arg, jint fd);

#endif //PRINTER_AS_COM_SMARTCARD_H
