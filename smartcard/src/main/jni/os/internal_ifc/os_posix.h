/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _OS_POSIX_H_
#define _OS_POSIX_H_

#ifndef SINGLE_THREADED

#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    pthread_mutex_t     mutex;
    pthread_mutexattr_t attr;
} HP_MUTEX_ID;

#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
typedef sem_t       HP_SEM_ID;
#else
typedef sem_t *     HP_SEM_ID;
#endif

typedef pthread_t   HP_thread_t;

#else

typedef struct
{
    void* mutex;
    void* attr;
} HP_MUTEX_ID;
typedef void*   HP_SEM_ID;
typedef int     HP_thread_t;
#endif
#endif /* _OS_POSIX_H_ */
