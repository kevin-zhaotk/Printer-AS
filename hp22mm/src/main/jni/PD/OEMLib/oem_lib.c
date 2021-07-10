/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include "lock.h"
#include "uart_ifc.h"
#include "max_common_types.h"
#include "debug_log.h"

static Lock_t   _shared_lock;
static bool     _is_lib_initialized = false;

void oem_lib_init() {
    LOGI("Enter %s", __FUNCTION__);

    /* If the library is already initialize, just return */
    if (true == _is_lib_initialized) return;

    /* Initialize mutex locks.
     * In the case of Max dev kit there is only one
     * physical UART on Raspberry Pi. This means that
     * we can have only one command active at any point in time.
     */
    mutex_init(&_shared_lock);

    /* Initialize UART lib */
    if(UART_OK != uart_lib_init()) return;

    _is_lib_initialized = true;

    LOGI("%s success", __FUNCTION__);
}

void oem_lib_shutdown() {
    LOGI("Enter %s", __FUNCTION__);

    /* If the library is already shutdown, just return */
    if (false == _is_lib_initialized) return;

    _is_lib_initialized = false;

    /* shutdown uart */
    if(UART_OK != uart_lib_shutdown()) return;

    /* destroy the mutex */
    mutex_destroy(&_shared_lock);

    LOGI("%s success", __FUNCTION__);
}

void oem_init(int instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return;
    }

    /* Initialize UART instance */
    if(UART_OK != uart_init(instance)) return;

    LOGI("%s success", __FUNCTION__);
}

void oem_shutdown(int instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return;
    }

    /* shutdown uart (will check to make sure uart is initialized) */
    if(UART_OK != uart_shutdown(instance)) return;

    LOGI("%s success", __FUNCTION__);
}

void oem_lock(int instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return;
    }

    mutex_lock(&_shared_lock);

    LOGI("%s success", __FUNCTION__);
}

void oem_unlock(int instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return;
    }

    mutex_unlock(&_shared_lock);

    LOGI("%s success", __FUNCTION__);
}



