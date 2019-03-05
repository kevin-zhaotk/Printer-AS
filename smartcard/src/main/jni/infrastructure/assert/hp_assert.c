/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_assert.c
//-----------------------------------------------------------------------------
// Description: Implementation of HP assert callback.
//
//*****************************************************************************

#include <stdio.h>

#include "hp_types.h"
#include "hp_smart_card.h"
#include "hp_debug_log_internal.h"
#include "hp_os_service.h"

void (*_assert_callback_func)(const char *error_str) = NULL;

void LIB_HP_SMART_CARD_register_assert_callback(void (*assert_callback_func)(const char *error_str))
{
    // OK to set callback to NULL
    _assert_callback_func = assert_callback_func;
}

void HP_ASSERT_impl(const char *file, uint32_t line)
{
    uint8_t buf[128];

    HP_DEBUG_printf("##################\nHP_ASSERT",
                    HP_DBG_LEVEL_ERROR, 0,
                    "HP Assertion failed.\n File: %s\nLine:%d\n##################\n",
                    file, line);

    snprintf((char *) buf, 128, "File:%s, Line:%d", file, line);

    /* If callback is registered, invoke callback */
    if (NULL != _assert_callback_func)
    {
        _assert_callback_func((const char *) buf);
    }
}
