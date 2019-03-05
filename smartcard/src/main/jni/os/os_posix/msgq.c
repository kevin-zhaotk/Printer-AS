/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

/***************************************************************//**
 *  \file msgq.c
 *
 *  \brief Message Queue component
 *
 *  OS agnostic message queue
 *  2015/09/10 - message queue replaced with cache flush specific
 *               implementation which is not really a queue.
 ******************************************************************/
#include <stdlib.h>
#include <string.h>

#include "hp_types.h"
#include "hp_os_service.h"
#include "hp_assert.h"
#include "lib_msgq.h"


static HP_MUTEX_ID              _flushq_access;             // Lock access to cache flush values
static HP_SEM_ID                _flushq_need_flush;         // Counting sempahore, signaled when flush needed
static int                      _signal_count;              // number of pending signals for flush
static HP_SEM_ID                _flushq_flush_complete;     // Counting sempahore, signaled when flush complete
static int                      _wait_count;                // number of threads waiting for signal

static uint32_t                 _cache_flush_devices;
static HP_SMART_CARD_result_t   _last_flush_result[HP_SMART_CARD_DEVICE_NUM_DEVICES];
static HP_SMART_CARD_result_t   _last_combined_flush_result;


/*******************************************************************************/
void flushq_create()
{
    int i;
    for (i = 0; i < HP_SMART_CARD_DEVICE_NUM_DEVICES; i++)
    {
        _last_flush_result[i] = HP_SMART_CARD_OK;
    }
    _last_combined_flush_result = HP_SMART_CARD_OK;
    _signal_count               = 0;
    _wait_count                 = 0;
    _cache_flush_devices        = 0;
    HP_sem_init(&_flushq_flush_complete, 0);
    HP_sem_init(&_flushq_need_flush, 0);
    HP_mutex_init(&_flushq_access);
}


/*******************************************************************************/
void flushq_queue_device(int device)
{
    // validate the device id (only 32 devices are supported)
    if (device < 0 || device > 31)
        return;

    // LOCK while changing values
    HP_mutex_lock(&_flushq_access);
    {
        // OR in device bit
        _cache_flush_devices |= (0x1 << device);
    }
    HP_mutex_unlock(&_flushq_access);

    // Trigger a flush
    flushq_trigger_flush();
}


//*****************************************************************************
void flushq_trigger_flush()
{
    HP_mutex_lock(&_flushq_access);
    HP_sem_post(&_flushq_need_flush);   // Signal that flush is needed
    _signal_count++;                    // (not needed when single-threaded, but does no harm)
    HP_mutex_unlock(&_flushq_access);
}


/*******************************************************************************/
uint32_t flushq_get_device_bits()
{
    uint32_t bits;

    // LOCK while changing values
    HP_mutex_lock(&_flushq_access);
    {
        // copy values and clear the signal
        bits = _cache_flush_devices;
    }
    HP_mutex_unlock(&_flushq_access);

    return bits;
}


/*******************************************************************************/
void flushq_clear_device_bit(int device)
{
    uint32_t mask = ~(0x1 << device);

    // LOCK while changing values
    HP_mutex_lock(&_flushq_access);
    {
        _cache_flush_devices &= mask;                                   // clear the bit for this device
    }
    HP_mutex_unlock(&_flushq_access);
}


/*******************************************************************************/
int flushq_wait_to_flush()
{
    int report_count    = 0;
    int signals         = 0;

    // wait for signal
    HP_sem_wait(&_flushq_need_flush);

    // get number of signals pending and clear the count
    HP_mutex_lock(&_flushq_access);
    {
        signals         = _signal_count;
        _signal_count   = 0;
    }
    HP_mutex_unlock(&_flushq_access);

    report_count = signals;
    signals--;          // one signal already consumed

    // consume additional signals to avoid extra triggers
    while (signals > 0)
    {
        HP_sem_wait(&_flushq_need_flush);
        signals--;
    }
    return report_count;
}


/*******************************************************************************/
void flushq_set_last_result(int device, HP_SMART_CARD_result_t result)
{
    if (device < 0)
        _last_combined_flush_result = result;
    else
        _last_flush_result[device] = result;
}


/*******************************************************************************/
void flushq_signal_all_waiters()
{
    // LOCK while changing values
    HP_mutex_lock(&_flushq_access);
    {
        while (_wait_count > 0)
        {
            HP_sem_post(&_flushq_flush_complete);
            _wait_count--;
        }
    }
    HP_mutex_unlock(&_flushq_access);
}


/*******************************************************************************/
void flushq_will_wait_for_flush_complete()
{
    // LOCK while changing values
    HP_mutex_lock(&_flushq_access);
    {
        _wait_count++;
    }
    HP_mutex_unlock(&_flushq_access);
}


/*******************************************************************************/
HP_SMART_CARD_result_t flushq_wait_for_flush_complete(int device)
{
    HP_sem_wait(&_flushq_flush_complete);
    if (device >= HP_SMART_CARD_DEVICE_NUM_DEVICES)
        return HP_SMART_CARD_INVALID_DEVICE_ID;
    if (device < 0)
        return _last_combined_flush_result;
    return _last_flush_result[device];
}



