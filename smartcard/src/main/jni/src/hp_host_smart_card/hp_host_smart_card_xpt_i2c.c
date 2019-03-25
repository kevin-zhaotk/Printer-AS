/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include "hp_host_smart_card.h"
#include "hp_host_smart_card_chip.h"
#include "hp_assert.h"

////////////////////////////////////////////
// Typedefs
////////////////////////////////////////////
typedef struct hp_host_smart_card_i2c_xpt_st
{
    hp_host_smart_card_xpt_t xpt;
} i2c_xpt_t;

////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////
static HP_SMART_CARD_i2c_result_t _i2c_cmd_rsp(hp_host_smart_card_xpt_t *xpt_p, host_smart_card_cmd_rsp_t *cmd_rsp_p);

////////////////////////////////////////////
// Static variables
////////////////////////////////////////////
static hp_host_smart_card_xpt_vtbl_t    _i2c_xpt_vtbl = { _i2c_cmd_rsp };
static i2c_xpt_t                        _i2c_xpt;

#define THIS    hp_host_smart_card_xpt_t * xpt_p

static HP_SMART_CARD_i2c_result_t _i2c_cmd_rsp(THIS, host_smart_card_cmd_rsp_t *cmd_rsp_p)
{
    HP_ASSERT(&_i2c_xpt_vtbl == xpt_p->vtbl_p);
    return HP_HOST_SMART_CARD_chip_cmd_rsp(cmd_rsp_p);
}

#undef THIS

void hp_host_smart_card_xpt_i2c_init(void)
{
    _i2c_xpt.xpt.vtbl_p = &_i2c_xpt_vtbl;
    hp_host_smart_card_set_xpt(&_i2c_xpt.xpt);
}

