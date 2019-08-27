/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <android/log.h>
/* Header for class com_industry_printer_data_NativeGraphicJni */

#ifndef _Included_com_industry_printer_data_NativeGraphicJni
#define _Included_com_industry_printer_data_NativeGraphicJni
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_TAG
#define LOG_TAG "NativeGraphicJni"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

/* Class:     com_industry_printer_data_NativeGraphicJni
* Method:    ShiftImage
* Signature: ([IIIIII)[I
*/
JNIEXPORT jintArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_ShiftImage
        (JNIEnv *, jclass, jintArray, jint, jint, jint, jint, jint);

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    Binarization
 * Signature: ([IIIII)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_Binarize
  (JNIEnv *, jclass, jintArray, jint, jint, jint, jint);

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetDots
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetDots
  (JNIEnv *, jclass);

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetBgBuffer
 * Signature: ([{IIIIZII})[C
 */
/*JNIEXPORT jcharArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetBgBuffer
  (JNIEnv *env, jclass thiz, jbyteArray src, jint length, jint bytesFeed, jint bytesPerHFeed, jint bytesPerH, jboolean isInchType, jint column, jint type);*/

/*
 * Class:     com_industry_printer_data_NativeGraphicJni
 * Method:    GetBgBuffer
 * Signature: ([BIIIIII})[C
 */
JNIEXPORT jcharArray JNICALL Java_com_industry_printer_data_NativeGraphicJni_GetBgBuffer
        (JNIEnv *, jclass, jbyteArray, jint, jint, jint, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
