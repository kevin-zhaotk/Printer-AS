/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/** \file file.h
 * Wrapper over the file system on OEM environment.
 */


#ifndef __LOCK_H__
#define __LOCK_H__

#include "max_common_types.h"
#include <pthread.h>



typedef struct Mutex_Lock_t
{
    
    pthread_mutex_t lock_Id;
    
}Lock_t;

void mutex_init(void *);
void mutex_lock(void *);
void mutex_unlock(void *);
void mutex_destroy(void *);

#endif
