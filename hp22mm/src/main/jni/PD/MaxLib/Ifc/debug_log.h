/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file debug_log.h
 * APIs to manage the C library debug messages.
 */
 
#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#include <jni.h>
#include <android/log.h>
#include "max_common_types.h"

char *getLogTag(const char* file, int line);

#define LOG_TAG "hp22mm"

//#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, getLogTag(__FILE__, __LINE__), __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, getLogTag(__FILE__, __LINE__), __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, getLogTag(__FILE__, __LINE__), __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
//#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, getLogTag(__FILE__, __LINE__), __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
//#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, getLogTag(__FILE__, __LINE__), __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

int toHexString(const uint8_t *src, char *dst, int len, char div);

/**
 * @brief Debug masks to filter the debug messages.
 */

//typedef enum
//{
//    DEBUG_LEVEL_ERROR               = 0x0001, /**< Errors */
//    DEBUG_LEVEL_WARNING             = 0x0002, /**< Warnings */
//    DEBUG_LEVEL_DEBUG               = 0x0004, /**< Log micro controller protocol information */
//    DEBUG_LEVEL_API                 = 0x0008, /**< API layer logging */
//    DEBUG_LEVEL_TRACE               = 0x0010, /**< Additional debug messages. Verbose mode */
//} DEBUG_level_t;


//#define DEFAULT_DEBUG_LEVEL     (DEBUG_LEVEL_ERROR)

/**
 * This function can be invoked to log the message.
 * @param module_name Name of the module emitting this message.
 * @param level       Debug level of this message. Message will be logged only if this level is enabled.
 * @param format      Standard C printf() format specifier.
 */
//void DEBUG_LOG(const char *module_name, DEBUG_level_t level, const char *format, ...);

/**
 * This function can be invoked to log a buffer.
 * @param module_name Name of the module emitting this message.
 * @param level       Debug level of this message. Message will be logged only if this level is enabled.
 * @param buf_name    Name of the buffer.
 * @param buf         Pointer to buffer containing the data to be logged.
 * @param buf_size    size of the buf in bytes.
 */
/*
void DEBUG_UINT8_BUFFER_LOG(const char      *module_name,
                            DEBUG_level_t   level,
                            const char      *buf_name,
                            uint8_t         *buf,
                            size_t          buf_size);
*/
/**
 * This function returns the number of log levels.
 */
int32_t get_num_log_levels();

/**
 * Enables a specific log level.
 * @param level     Log level to be enabled. To enable multiple log levels in 
 *                  one call a bitwise OR of the levels can be sent to this argument.
 */
//void enable_log_level(int32_t level);

/**
 * Disables a specific log level.
 * @param level     Log level to be enabled. To disable multiple log levels in 
 *                  one call a bitwise OR of the levels can be sent to this argument.
 */
//void disable_log_level(int32_t level);

/**
 * Returns if the given log level is enabled.
 * @param level     Log level.
 */
//int32_t is_log_level_enabled(int32_t level);

/**
 * Returns description of the given log level.
 * @param level     Log level.
 */
//char* get_log_level_description(int32_t level);

#endif /* __DEBUG_LOG_H__ */
