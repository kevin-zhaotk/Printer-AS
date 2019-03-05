/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_ASSERT_H_
#define _HP_ASSERT_H_

#include "hp_smart_card.h"

extern void HP_ASSERT_impl(const char *file, uint32_t line);

#ifdef NDEBUG
/* Don't do anything if NDEBUG is enabled */
#define HP_ASSERT(expr)
#else
/* This macro just displays the message */
#define HP_ASSERT(expr)    ((expr) ? (void) 0 : HP_ASSERT_impl(__FILE__, __LINE__))
#endif
#endif /* _HP_ASSERT_H_ */
