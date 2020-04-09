/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_utils.c
//-----------------------------------------------------------------------------
// Description: Implementation of HP debug log service
//
//*****************************************************************************

#include <stdio.h>
#include <stdarg.h>

#include "hp_debug_log_internal.h"
#include "hp_os_service.h"

HP_SMART_CARD_bool_t IS_LITTLE_ENDIAN()
{
    unsigned int    i   = 0xaabbccdd;
    unsigned char   *ch = (unsigned char *) &i;

    if (*ch == 0xdd)
    {
        return HP_SMART_CARD_TRUE;
    }
    else
    {
        return HP_SMART_CARD_FALSE;
    }
}

static void leftShift(unsigned char *dataStream, int length)
{
    int             i;
    unsigned char   temp;

    dataStream[0] <<= 1;                                //Left Shift MSB by 1
    for (i = 1; i < (length); i++)
    {
        temp                = dataStream[i];
        dataStream[i]       <<= 1;
        temp                >>= 7;
        dataStream[i - 1]   |= temp;
    }
}

void shift_right(unsigned char *object, size_t size_of_object, size_t num_bits_to_shift)
{
    unsigned char   *byte;
    int             i;
    size_t          size;

    for (i = 0; i < (int) num_bits_to_shift; i++)
        for (byte = object, size = size_of_object; size; )
        {
            unsigned char bit = 0;
            bit = 0;

            if (size > 1)
            {
                bit = (byte[size - 2] & (1 << 0)) ? 1 : 0;
            }
            byte[size - 1]  >>= 1;
            byte[size - 1]  |= (bit) << 7;
            size--;
        }
}

void shift_left(unsigned char *object, size_t size_of_object, size_t num_bits_to_shift)
{
    int i;

    for (i = 0; i < (int) num_bits_to_shift; i++)
    {
        leftShift(object, size_of_object);
    }
}

// Added by H.M.Wang 2020-1-21
int toHexString(const uint8_t *src, char *dst, int len, char div) {
    if(!src) return -1;
    if(!dst) return -1;

    memset(dst, 0x00, sizeof(dst));

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
//    strcat(dst, 0x00);
    return 0;
}
