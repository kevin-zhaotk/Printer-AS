/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2016 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_debug_log.c
//-----------------------------------------------------------------------------
// Description: Implementation of HP debug log service
//
//*****************************************************************************

// for Windows, removes warning about use of vsnprintf
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>

#include "hp_debug_log_internal.h"
#include "hp_os_service.h"
#include "common_log.h"

#define BUFF_SIZE    256

static uint8_t  _current_debug_level    = (uint8_t) HP_DEBUG_LEVEL_DEFAULT;
static uint8_t  _current_log_depth      = 0;

void HP_DEBUG_printf(const char *module_id, unsigned char level, unsigned char loglevel, const char *format, ...)
{
    va_list args;
    char    buf[BUFF_SIZE];
    int     len;
    int     i;
    int     indent;

    // NOTE: If module_id is NULL, will be appended without indenting
    if (module_id == NULL)
    {
        buf[0] = '\0';                                                                                                  // need to terminate for strlen below
    }
    else
    {
        // depending on log level, add indenting
        switch (loglevel)
        {
        case 0:
        case 1:
            indent = 0;
            break;
        case 2:
        case 3:
        case 4:
            indent = 4;
            break;
        case 5:
            indent = 8;
            break;
        default:
            indent = 12;
            break;
        }
        for (i = 0; i < indent && i < (BUFF_SIZE - 1); i++)
            buf[i] = ' ';

        // Copy the module id in to the buffer after indenting
        snprintf((buf + indent), (BUFF_SIZE - indent), "%s ", module_id);
    }

    /* Verify if this level is enabled */
    if ((loglevel <= _current_log_depth) ||
        (level == HP_DBG_LEVEL_ERROR) ||
        (level & _current_debug_level))
    {
        va_start(args, format);
        len = vsnprintf(buf + strlen(buf), BUFF_SIZE - strlen(buf), format, args);
        va_end(args);

        HP_OS_PRINT(buf);

/* Added by H.M.Wang 2019-10-18 */
        switch(level) {
            case HP_DBG_LEVEL_ERROR:
                LOGE("%s", buf);
                break;
            case HP_DBG_LEVEL_AUTH_INFO:
            case HP_DBG_LEVEL_HSCC_INFO:
                LOGI("%s", buf);
                break;
            default:
                LOGD("%s", buf);
                break;
        }
/* Added by H.M.Wang 2019-10-18 end */
    }
}

void LIB_HP_SMART_CARD_set_log_mask(HP_DEBUG_level_t category)
{
    _current_debug_level |= category;
}

void LIB_HP_SMART_CARD_clear_log_mask(HP_DEBUG_level_t category)
{
    _current_debug_level &= ~category;
}

void LIB_HP_SMART_CARD_set_log_depth(uint8_t depth)
{
    _current_log_depth = depth;
}
