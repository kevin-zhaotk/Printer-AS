/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#define _GNU_SOURCE
#include <pthread.h>

#include "lock.h"
#include "print_head_driver_ifc.h"

/* 
 * Initialize Mutex
 * Returns zero on success. 
 * Returns non-zero value on failure
 */
void mutex_init(void *lock)
{
    MAX_ASSERT(NULL != lock);
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    
    /*
    **Currently default values are set to attributes.To change the set values of attributes following built-in APIs can be used
    */ 
    #if 0
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_DEFAULT);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutexattr_setprotocol(&mattr,PTHREAD_PRIO_NONE);
    pthread_mutexattr_setrobust_np(&mattr, PTHREAD_MUTEX_STALLED_NP);
    #endif
    
    int mutex_result;
    mutex_result = pthread_mutex_init(&((*((Lock_t *)lock)).lock_Id),&mattr);
    
    MAX_ASSERT(mutex_result == 0);
    
}


void mutex_lock(void *lock)
{
    
    MAX_ASSERT(NULL != lock);
    int mutex_result;
    mutex_result = pthread_mutex_lock(&((*((Lock_t *)lock)).lock_Id));
    
    MAX_ASSERT(mutex_result == 0);
    
}

void mutex_unlock(void *lock)
{
    MAX_ASSERT(NULL != lock);
    int mutex_result;
    mutex_result = pthread_mutex_unlock(&((*((Lock_t *)lock)).lock_Id));
    
    MAX_ASSERT(mutex_result == 0);
    
}

void mutex_destroy(void *lock)
{
    MAX_ASSERT(NULL != lock);
    int mutex_result;    
    mutex_result = pthread_mutex_destroy(&((*((Lock_t *)lock)).lock_Id));
    
    MAX_ASSERT(mutex_result == 0);
    
}

