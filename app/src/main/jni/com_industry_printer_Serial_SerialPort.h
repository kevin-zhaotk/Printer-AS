/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <android/log.h>
/* Header for class com_tongfang_opticalmagnetic_SerialPort */

#ifndef _Included_com_industry_printer_Serial_SerialPort
#define _Included_com_industry_printer_Serial_SerialPort
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_TAG
#define LOG_TAG "SerialPort(Native)"
#endif
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;I)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_industry_printer_Serial_SerialPort_openStream
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;I)I;
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_openSerial
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    stop
 * Signature: ()V
 */

JNIEXPORT void JNICALL Java_com_industry_printer_Serial_SerialPort_stop
        (JNIEnv *, jobject);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    close
 * Signature: (Ljava/io/FileDescriptor)I
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_closeStream
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    close
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_closeSerial
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    read
 * Signature: (I)[B
 */

JNIEXPORT jbyteArray JNICALL Java_com_industry_printer_Serial_SerialPort_read
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_industry_printer_Serial_SerialPort
 * Method:    write
 * Signature: (I[BI)I
 */

JNIEXPORT jint JNICALL Java_com_industry_printer_Serial_SerialPort_write
  (JNIEnv *, jobject, jint, jbyteArray, jint);

#ifdef __cplusplus
}
#endif
#endif
