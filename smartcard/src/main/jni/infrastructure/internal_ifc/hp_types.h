/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2016 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_TYPES_H_
#define _HP_TYPES_H_

#include <string.h>

#ifdef _MSC_VER
/* stdint.h doesn't seem to be there on MSVC */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;

typedef signed char         int8_t;
typedef short int           int16_t;
typedef int                 int32_t;
#else
#include <stdint.h>
#endif

#define TRUE            1
#define FALSE           0

#define FEATURE_ON      1
#define FEATURE_OFF     0

typedef enum
{
    HP_SMART_CARD_FALSE = 0,
    HP_SMART_CARD_TRUE  = 1,
} HP_SMART_CARD_bool_t;
#endif /* _HP_TYPES_H_ */
