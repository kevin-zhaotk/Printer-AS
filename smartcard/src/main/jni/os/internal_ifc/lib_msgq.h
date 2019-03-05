/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

/***************************************************************//**
 * \file lib_msgq.h
 *
 * \brief Message Queue Routines
 *
 * These commands can be used to send inter-thread communications.
 ******************************************************************/
#ifndef LIB_MSGQ_H
#define LIB_MSGQ_H

#include "hp_smart_card.h"
#include "os_specific.h"


/**
 * \brief Flush Queue - Create
 *
 * Creates the flush queue.
 *
 */
extern void flushq_create(void);


/**
 * \brief Flush Queue - Queue Device
 *
 * Mark a device as needing flushed AND trigger background thread to flush.
 *
 */
extern void flushq_queue_device(int device);


/**
 * \brief Flush Queue - Trigger Flush
 *
 * Trigger background thread to flush.
 *
 */
extern void flushq_trigger_flush(void);


/**
 * \brief Flush Queue - Get Device Bits
 *
 * Get bit mask of devices needing flushed. Does NOT clear any bits.
 *
 */
extern uint32_t flushq_get_device_bits(void);


/**
 * \brief Flush Queue - Clear Device Bit
 *
 * Clear a single device bit from the bitmask of devices needing flushed.
 *
 */
extern void flushq_clear_device_bit(int device);


/**
 * \brief Flush Queue - Wait To Flush
 *
 * EXCLUSIVE USE: _cache_monitor
 * Wait for signal that flush is needed (NOTE: consumes the signal).
 *
 */
extern int flushq_wait_to_flush(void);


/**
 * \brief Flush Queue - Set Last Result
 *
 * EXCLUSIVE USE: _flush_device and _flush_all_pending (for COMBINED)
 * Set the last result from flush of a device.
 * NOTE: if device <0, will set the COMBINED result
 *
 */
extern void flushq_set_last_result(int device, HP_SMART_CARD_result_t result);


/**
 * \brief Flush Queue - Signal All Waiters
 *
 * EXCLUSIVE USE: _cache_monitor
 * Signal all waiters that cach flush is complete.
 *
 */
extern void flushq_signal_all_waiters(void);


/**
 * \brief Flush Queue - Will Wait For Flush Complete
 *
 * Wait for signal that flush is needed (NOTE: consumes the signal).
 *
 */
extern void flushq_will_wait_for_flush_complete(void);


/**
 * \brief Flush Queue - Wait For Flush Complete
 *
 * Wait for signal that flush is complete AND returns last flush result for a device.
 * NOTE: If device <0 will return a COMBINED result of last flush.
 *
 */
extern HP_SMART_CARD_result_t flushq_wait_for_flush_complete(int device);
#endif // LIB_MSGQ_H

