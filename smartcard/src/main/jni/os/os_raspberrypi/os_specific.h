/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2017 HP Development Company, L.P.
   Made in U.S.A.
 */

/* This file contains OS specific definitions and macros for Raspberry Pi
 */

#ifndef _OS_SPECIFIC_H_
#define _OS_SPECIFIC_H_

#include <unistd.h>
#include "os_posix.h"

#define HP_USLEEP(micro_seconds)    usleep(micro_seconds)

//#define HP_OS_PRINT(buf)			fprintf(stderr, "%s", buf)
#define HP_OS_PRINT(buf)            printf("%s", buf)

#define PTHREAD_MUTEX_RECURSIVE    PTHREAD_MUTEX_RECURSIVE_NP
#endif /* _OS_SPECIFIC_H_ */
