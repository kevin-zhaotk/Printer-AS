/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/
/** \file oem_common_types.h
 * Basic data types and configurations specific to OEM environment.
 */
 
#ifndef __OEM_COMMON_TYPES_H__
#define __OEM_COMMON_TYPES_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#define NUM_BLUR_INSTANCES  2   /**< Number of Printhead Driver instances. Default is "1" */
#define NUM_IDS_INSTANCES   2   /**< Number of IDS instances. Default is "1" */

/**< Map what the assert should do. 
     1) Display the stack trace, 
     2) Call Linux assert */
//void display_stack_trace(void);
//#define OEM_ASSERT(expr) { if(!(expr)){ display_stack_trace(); } assert(expr); }
#define OEM_ASSERT(expr) assert(expr)


#define OEM_CONSOLE_PRINT(m) fprintf(stderr, "%s", m) /**< Map OEM_CONSOLE_PRINT to appropriate output device.
                                                            This example routes the debug messages to Standard Error stream */

#endif /* __OEM_COMMON_TYPES_H__ */
