/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HW_SMART_CARD_XPT_H_
#define _HW_SMART_CARD_XPT_H_

#include "hp_types.h"
#include "../../drivers/internal_ifc/hp_smart_card_i2c_ifc.h"

typedef struct hw_smart_card_xpt_st
{
    struct hw_smart_card_xpt_vtbl_st *vtbl_p;
} hw_smart_card_xpt_t;

// for Windows, THIS would cause a warning
#define SCXPT_THIS    hw_smart_card_xpt_t *

typedef struct hw_smart_card_xpt_vtbl_st
{
    HP_SMART_CARD_i2c_result_t (*read)(SCXPT_THIS, uint8_t addr, uint8_t *rcv_buf_p, int rcv_buf_len);
    HP_SMART_CARD_i2c_result_t (*write)(SCXPT_THIS, uint8_t addr, uint8_t *snd_buf_p, int snd_buf_len);
} hw_smart_card_xpt_vtbl_t;

#undef SCXPT_THIS
#endif /* _HW_SMART_CARD_XPT_H_ */
