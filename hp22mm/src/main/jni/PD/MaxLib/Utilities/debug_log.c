/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/*****************************************************************************
* File : debug_log.c
* -----------------------------------------------------------------------------
* Description: Implementation of debug log service
*
*****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "debug_log.h"
#include "oem_time.h"

/*
#define BUFF_SIZE 256

static DEBUG_level_t _current_debug_level = DEFAULT_DEBUG_LEVEL;

void DEBUG_LOG( const char      *module_name, 
                DEBUG_level_t   level, 
                const char      *format, ...)
{
    va_list args;
    char    buf[BUFF_SIZE];

    if((level == DEBUG_LEVEL_ERROR) || (level & _current_debug_level))
    {
        OemTime_t t;
        oem_get_time(&t);
            
        snprintf(buf, BUFF_SIZE, "%02d:%02d:%02d.%03d %s: ", t.hour, t.minutes, t.seconds,
                                                         t.msec, module_name);

        va_start(args, format);
        vsnprintf(buf + strlen(buf), BUFF_SIZE - strlen(buf), format, args);
        va_end(args);
    	
	    CONSOLE_PRINT(buf);
    }

    snprintf(buf, BUFF_SIZE, ":%s: ", module_name);
    va_start(args, format);
    vsnprintf(buf + strlen(buf), BUFF_SIZE - strlen(buf), format, args);
    va_end(args);

    switch (level) {
        case DEBUG_LEVEL_ERROR:
            LOGE("%s", buf);
            break;
        case DEBUG_LEVEL_WARNING:
            LOGW("%s", buf);
            break;
        case DEBUG_LEVEL_DEBUG:
            LOGD("%s", buf);
            break;
        case DEBUG_LEVEL_API:
        case DEBUG_LEVEL_TRACE:
            LOGI("%s", buf);
            break;
    }
}

void DEBUG_UINT8_BUFFER_LOG(const char      *module_name,  
                            DEBUG_level_t   level, 
                            const char      *buf_name,
                            uint8_t         *buf, 
                            size_t          buf_size)
{
    if((level == DEBUG_LEVEL_ERROR) || (level & _current_debug_level))
    {
        fprintf(stderr, "%s: %s: ", module_name, buf_name);
        int i = 0;
        for(i = 0; i < buf_size; ++i)
        {
            fprintf(stderr, "%d ", buf[i]);
        }
        fprintf(stderr, "\n");
    }
    
}

void set_log_mask(DEBUG_level_t level)
{
    _current_debug_level |= level;
}

void clear_log_mask(DEBUG_level_t level)
{
    _current_debug_level &= ~level;
}

typedef struct 
{
	int32_t level;
	char    *description;
} LogLevelStruct;

static LogLevelStruct log_level_struct[] = 
{ 
    { 1, "Error"},
    { 2, "Warning"},
	{ 3, "Protocol"},
    { 4, "API"},
    { 5, "Trace"},
};

int32_t get_num_log_levels()
{
	return sizeof(log_level_struct)/sizeof(LogLevelStruct);
}

void enable_log_level(int32_t level)
{
	_current_debug_level |= 1 << (level-1);
}

void disable_log_level(int32_t level)
{
	_current_debug_level &= ~(1 << (level-1));
}

int32_t is_log_level_enabled(int32_t level)
{
	return (_current_debug_level & (1 << (level-1))) ? 1 : 0;
}

char* get_log_level_description(int32_t level)
{
	return log_level_struct[level-1].description;
}
*/
#define BUFF_SIZE 256
char    buf[BUFF_SIZE];

char *getLogTag(const char* file, int line) {
    memset(buf, 0x00, BUFF_SIZE);
    snprintf(buf, BUFF_SIZE, "%s:%0s(%d): ", LOG_TAG, file, line);
    return buf;
}

int toHexString(const uint8_t *src, char *dst, int len, char div) {
    if(!src) return -1;
    if(!dst) return -1;

    char s[16];
    for(int i=0; i<len; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "0x%02X", src[i]);
        } else {
            sprintf(s, "%c0x%02X", div, src[i]);
        }
        strcat(dst, s);
    }

    return len;
}

