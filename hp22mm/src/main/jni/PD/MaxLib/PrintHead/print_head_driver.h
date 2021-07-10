/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/



/** \file print_head_driver.h
 * Interface APIs to manage the Printhead Driver.
 */
#ifndef __PRINT_HEAD_DRIVER_H__
#define __PRINT_HEAD_DRIVER_H__

#include "lock.h" 
#include "service.h"

/* Print Head Controller handle structure */
typedef struct
{
    bool is_initialized;
    //Lock_t lock;    
} PDHandle_t;

/* Array of print head control handle structure */
extern PDHandle_t _pd_handle[NUM_BLUR_INSTANCES];

//#define MUTEX_LOCKINSTANCE_LOCKID(instance)  (&(_pd_handle[instance].lock))

extern bool _is_lib_initialized;

#endif
