/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include "hp_smart_card_config.h"
#include "../../drivers/internal_ifc/hp_smart_card_i2c_ifc.h"
#include "hw_smart_card_xpt.h"
#include "hw_smart_card.h"
#include "hp_host_smart_card.h"

#include "hp_generic_macros.h"

typedef struct hw_smart_card_i2c_xpt_st
{
    hw_smart_card_xpt_t         xpt;
    HP_SMART_CARD_device_id_t   device;
} i2c_xpt_t;

#define THIS    hw_smart_card_xpt_t * xpt_p

static HP_SMART_CARD_i2c_result_t _i2c_read(THIS, uint8_t addr, uint8_t *rcv_buf_p, int rcv_buf_len)
{
    i2c_xpt_t *this_p = IMPL(i2c_xpt_t, xpt, xpt_p);
    return HP_SMART_CARD_i2c_read(this_p->device, addr, rcv_buf_p, rcv_buf_len);
}

static HP_SMART_CARD_i2c_result_t _i2c_write(THIS, uint8_t addr, uint8_t *snd_buf_p, int snd_buf_len)
{
    i2c_xpt_t *this_p = IMPL(i2c_xpt_t, xpt, xpt_p);
    return HP_SMART_CARD_i2c_write(this_p->device, addr, snd_buf_p, snd_buf_len);
}

#undef THIS

static hw_smart_card_xpt_vtbl_t _i2c_xpt_vtbl = {
    _i2c_read,
    _i2c_write,
};

static i2c_xpt_t                _i2c_xpt[HW_SMART_CARD_NUM_DEVICES];

void hw_smart_card_xpt_i2c_init(void)
{
    int i;
    for (i = 0; i < HW_SMART_CARD_NUM_DEVICES; i++)
    {
        _i2c_xpt[i].device      = i;
        _i2c_xpt[i].xpt.vtbl_p  = &_i2c_xpt_vtbl;
        hw_smart_card_set_xpt(i, &_i2c_xpt[i].xpt);
    }
}
