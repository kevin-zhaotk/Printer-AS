#include <jni.h>
#include <android/log.h>

#ifndef _COMMON_LOG
#define _COMMON_LOG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef LOG_TAG
#define LOG_TAG "SmartCard"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define DEBUG_BUFFER_MAX 512
/*
#define LOG_TAG_CREATE \
            char buffer[DEBUG_BUFFER_MAX+1]={0}; \
            char *p = strrchr(__FILE__, '/'); \
            snprintf(buffer, DEBUG_BUFFER_MAX, "%s(%d)\n", (p == NULL ? __FILE__ : p+1), __LINE__);

#define LOGV(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__); \
        }

#define LOGD(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__); \
        }

#define LOGI(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__); \
        }

#define LOGW(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__); \
        }

#define LOGE(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__); \
        }

#define LOGF(...) { \
            LOG_TAG_CREATE; \
            __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__); \
        }
*/
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _COMMON_LOG