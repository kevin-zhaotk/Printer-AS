/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_OS_SERVICES_H_
#define _HP_OS_SERVICES_H_

#include "hp_types.h"
#include "lib_msgq.h"
#include "os_specific.h"

extern int HP_mutex_init(HP_MUTEX_ID *mutex_id);
extern int HP_mutex_lock(HP_MUTEX_ID *mutex_id);
extern int HP_mutex_trylock(HP_MUTEX_ID *mutex_id);
extern int HP_mutex_unlock(HP_MUTEX_ID *mutex_id);
extern int HP_mutex_destroy(HP_MUTEX_ID *mutex_id);

extern int HP_sem_init(HP_SEM_ID *sem_id, uint32_t initial_value);
extern int HP_sem_wait(HP_SEM_ID *sem_id);
extern int HP_sem_trywait(HP_SEM_ID *sem_id);
extern int HP_sem_post(HP_SEM_ID *sem_id);
extern int HP_sem_destroy(HP_SEM_ID *sem_id);

extern int HP_thread_create(HP_thread_t *thread, void *(*start_routine)(void*), void *arg);
extern int HP_thread_cancel(HP_thread_t thread);
#endif /* _HP_OS_SERVICES_H_ */
