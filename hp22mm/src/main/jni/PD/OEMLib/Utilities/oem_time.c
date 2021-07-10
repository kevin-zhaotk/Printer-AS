/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include "oem_time.h"

/* 
 * Returns the current time.
 * Caller need to allocate memory for OemTime_t.
 * OEM need to implement this function.
 */
void oem_get_time(OemTime_t *t)
{
    struct timeval tv;
    struct tm *tm;
    
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    
    t->hour = tm->tm_hour;
    t->minutes = tm->tm_min;
    t->seconds = tm->tm_sec;
    t->msec = (int) (tv.tv_usec / 1000);
    
    return;
}