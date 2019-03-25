/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_HOST_SMART_CARD_XPT_H_
#define _HP_HOST_SMART_CARD_XPT_H_

#include "hp_host_smart_card_chip.h"

void hp_host_smart_card_xpt_i2c_init(void);

typedef struct hp_host_smart_card_xpt_st
{
    struct hp_host_smart_card_xpt_vtbl_st *vtbl_p;
} hp_host_smart_card_xpt_t;

// for Windows, THIS would cause a warning
#define HSCXPT_THIS    hp_host_smart_card_xpt_t *

typedef struct hp_host_smart_card_xpt_vtbl_st
{
    HP_SMART_CARD_i2c_result_t (*cmd_rsp)(HSCXPT_THIS, host_smart_card_cmd_rsp_t *cmd_rsp_p);
} hp_host_smart_card_xpt_vtbl_t;

#undef HSCXPT_THIS
#endif /* _HP_HOST_SMART_CARD_XPT_H_ */
