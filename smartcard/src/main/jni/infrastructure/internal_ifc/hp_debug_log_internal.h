/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_debug_log_internal.h
//-----------------------------------------------------------------------------
// Description: Interface definition for HP debug log service
//
//*****************************************************************************
#ifndef _HP_DEBUG_LOG_INTERNAL_H_
#define _HP_DEBUG_LOG_INTERNAL_H_

#include "hp_smart_card.h"

void HP_DEBUG_printf(const char *module_id, unsigned char level, unsigned char loglevel, const char *format, ...);
HP_SMART_CARD_bool_t IS_LITTLE_ENDIAN();
void shift_right(unsigned char *object, size_t size_of_object, size_t num_bits_to_shift);
void shift_left(unsigned char *object, size_t size_of_object, size_t num_bits_to_shift);
char *toHexString(char *src, int len);
#endif /* _HP_DEBUG_LOG_INTERNAL_H_ */
