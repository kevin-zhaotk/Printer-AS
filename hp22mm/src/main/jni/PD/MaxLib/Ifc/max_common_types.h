/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file max_common_types.h
 * Definitions of common data types and configuration macros.
 */
 
#ifndef __MAX_COMMON_TYPES_H__
#define __MAX_COMMON_TYPES_H__

#include "oem_common_types.h"

//#define TRUE  1
//#define FALSE 0

#define FEATURE_ON 	1
#define FEATURE_OFF 0

#define DEBUG_BUILD     0
#define RELEASE_BUILD   1

#ifndef BUILD
#define BUILD RELEASE_BUILD
#endif

#define MAX_ASSERT(expr)    OEM_ASSERT(expr) /**< Define what the asserts should do in OEM environment. */
#define CONSOLE_PRINT(m)    OEM_CONSOLE_PRINT(m) /**< Define how to log the debug messages in OEM environment.*/


/*! \struct PlatformInfo
 *  Platform Information
 */
typedef struct
{
    char        model[12];      /**< Platform model string. */
    uint16_t    mfg_year;       /**< Platform manufacture year (e.g. 2018). */
    uint8_t     mfg_woy;        /**< Platform manufacture week of year (1-52). */
    uint8_t     mfg_country;    /**< Platform manufacture. */
    uint8_t     mfg_rev_major;  /**< Platform firmware major revision. */
    uint8_t     mfg_rev_minor;  /**< Platform firmware minor revision. */
    uint8_t     orientation;    /**< Printhead: 0=Vertical, 1=Horizontal. Supply: (not used) */
}PlatformInfo_t;


#endif /* __MAX_COMMON_TYPES_H__ */

