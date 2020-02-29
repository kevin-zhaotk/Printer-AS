#include <stdio.h>
#include <string.h>
#include "common_log.h"

#define LOG_TAG "SmartCard"

void __log_print(android_LogPriority lvl, const char *file, uint32_t line, ...) {
    char buf[1024];

    char *p = strrchr(file, '/');

    if(NULL != p) {
        sprintf(buf, "%s: %s:%d", LOG_TAG, ++p, line);
    } else {
        sprintf(buf, "%s: %s:%d", LOG_TAG, file, line);
    }

    __android_log_print(lvl, buf, __VA_ARGS__);
}
