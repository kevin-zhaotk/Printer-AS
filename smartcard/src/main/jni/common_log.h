#include <jni.h>
#include <android/log.h>

#ifndef _COMMON_LOG
#define _COMMON_LOG

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_TAG
#define LOG_TAG "SmartCard"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
/*
char *__common_log_to_hex_string(const char *src, char *tgt, size_t len) {
    memset(tgt, 0x00, len);

    char s[16];
    for(int i=0; i<len; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "%02X", src[i]);
        } else {
            sprintf(s, " %02X", src[i]);
        }
        strcat(tgt, s);
    }
    return tgt;
}
*/
#ifdef __cplusplus
}
#endif
#endif
