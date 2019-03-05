/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef SINGLE_THREADED
#include <pthread.h>
#include <semaphore.h>
#endif

#include "hp_os_service.h"
#include "hp_assert.h"
#include "hp_types.h"

#ifndef SINGLE_THREADED

/* REVISIT:
 * 1. Need to check return values of all the calls.
 * 2. Add debug message during failure.
 */
int HP_mutex_init(HP_MUTEX_ID *mutex_id)
{
    pthread_mutexattr_init(&mutex_id->attr);
    pthread_mutexattr_settype(&mutex_id->attr, PTHREAD_MUTEX_RECURSIVE);

    return pthread_mutex_init(&mutex_id->mutex, &mutex_id->attr);
}

int HP_mutex_lock(HP_MUTEX_ID *mutex_id)
{
    return pthread_mutex_lock(&mutex_id->mutex);
}

int HP_mutex_trylock(HP_MUTEX_ID *mutex_id)
{
    return pthread_mutex_trylock(&mutex_id->mutex);
}

int HP_mutex_unlock(HP_MUTEX_ID *mutex_id)
{
    return pthread_mutex_unlock(&mutex_id->mutex);
}

int HP_mutex_destroy(HP_MUTEX_ID *mutex_id)
{
    return pthread_mutex_destroy(&mutex_id->mutex);
}

int HP_sem_init(HP_SEM_ID *sem_id, uint32_t initial_value)
{
#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
    return sem_init(sem_id, 0, initial_value);
#else
    sem_t           *id;
    static uint32_t counter = 0;
    char            name[16];

    snprintf(name, 16, "sem_id_%d", counter++);
    sem_unlink(name);
    id = sem_open(name, O_CREAT, 777, initial_value);
    if (SEM_FAILED == id)
    {
        return -1;
    }

    *sem_id = id;
    return 0;
#endif
}

int HP_sem_wait(HP_SEM_ID *sem_id)
{
#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
    return sem_wait(sem_id);
#else
    return sem_wait(*sem_id);
#endif
}

int HP_sem_trywait(HP_SEM_ID *sem_id)
{
#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
    return sem_trywait(sem_id);
#else
    int ret;
    ret = sem_trywait(*sem_id);
    return ret;
#endif
}

int HP_sem_post(HP_SEM_ID *sem_id)
{
#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
    return sem_post(sem_id);
#else
    return sem_post(*sem_id);
#endif
}

int HP_sem_destroy(HP_SEM_ID *sem_id)
{
#if (USE_NAMED_SEMAPHORE == FEATURE_OFF)
    return sem_destroy(sem_id);
#else
    return sem_destroy(*sem_id);
#endif
}

int HP_thread_create(HP_thread_t *thread, void *(*start_routine)(void*), void *arg)
{
    pthread_create(thread,
                   NULL,
                   start_routine,
                   arg);
    return 0;
}

int HP_thread_cancel(HP_thread_t thread)
{
    pthread_kill(thread, SIGUSR1);
    return 0;
}

#else /* SINGLE_THREADED */

int HP_mutex_init(HP_MUTEX_ID *mutex_id)
{
    return 0;
}

int HP_mutex_lock(HP_MUTEX_ID *mutex_id)
{
    return 0;
}

int HP_mutex_trylock(HP_MUTEX_ID *mutex_id)
{
    return 0;
}

int HP_mutex_unlock(HP_MUTEX_ID *mutex_id)
{
    return 0;
}

int HP_mutex_destroy(HP_MUTEX_ID *mutex_id)
{
    return 0;
}

int HP_sem_init(HP_SEM_ID *sem_id, uint32_t initial_value)
{
    return 0;
}

int HP_sem_wait(HP_SEM_ID *sem_id)
{
    return 0;
}

int HP_sem_trywait(HP_SEM_ID *sem_id)
{
    return 0;
}

int HP_sem_post(HP_SEM_ID *sem_id)
{
    return 0;
}

int HP_sem_destroy(HP_SEM_ID *sem_id)
{
    return 0;
}

int HP_thread_create(HP_thread_t *thread, void *(*start_routine)(void*), void *arg)
{
    return 0;
}

int HP_thread_cancel(HP_thread_t thread)
{
    return 0;
}
#endif