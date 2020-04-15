/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include "hp_assert.h"
#include "hp_debug_log_internal.h"
#include "hp_generic_macros.h"
#include "hp_types.h"

#include "hp_host_smart_card_ifc.h"
#include "hw_smart_card.h"
#include "hw_smart_card_proto.h"
#include "carnac_types.h"

#define DBG_HW_SMART_CARD_ID    "HWSC"

HW_SMART_CARD_device_t      hw_smart_card_device[HW_SMART_CARD_NUM_DEVICES];
static void                 (*_stats_func_cb_p)(int device, HW_SMART_CARD_status_t err_type) = NULL;

static CARNAC_mk_entry_t    host_mkdb[HW_SMART_CARD_MAX_MKDB_LEN];
static int                  host_mkdb_count = 0;


CARNAC_mk_id_t
HW_SMART_CARD_find_host_device_mkid_match(CARNAC_mk_entry_t *host_mkids,
                                          int host_keyID_ct,
                                          HW_SMART_CARD_bkdb_entry_t *device_bkids,
                                          int device_keyID_ct)
{
    int             host_idx, device_idx;
    CARNAC_mk_id_t  retID = { { 0, 0 }, { 0 } };        // zero it out for no-match case.

    for (host_idx = 0; host_idx < host_keyID_ct; host_idx++)
    {
        for (device_idx = 0; device_idx < device_keyID_ct; device_idx++)
        {
            if ((0 == memcmp(host_mkids[host_idx].mk_id.rk_id,
                             device_bkids[device_idx].bk_id.mk_id.rk_id,
                             CARNAC_RK_ID_LEN)) &&
                (device_bkids[device_idx].bk_id.mk_id.mk_depth[0] >=
                 host_mkids[host_idx].mk_id.mk_depth[0]) &&
                (device_bkids[device_idx].bk_id.mk_id.mk_depth[0] <=
                 host_mkids[host_idx].max_depth))
            {
                return device_bkids[device_idx].bk_id.mk_id;
            }
        }
    }
    return retID;                         // if we don't find a match, return null.
}

void hw_smart_card_init(void)
{
    unsigned int i;

    hw_smart_card_xpt_i2c_init();
    hw_smart_card_proto_init();

    for (i = 0; i < ARRAY_SIZE(hw_smart_card_device); i++)
    {
        CRYPTO_blkcipher_t *blk_p = CRYPTO_aes_init(&hw_smart_card_device[i].sk.aes, NULL, 0);
        CRYPTO_cmac_init(&hw_smart_card_device[i].sk.cmac, blk_p);
        hw_smart_card_device[i].idx         = i;
        hw_smart_card_device[i].compare_ids = FALSE;
    }
}

void hw_smart_card_multikey_init()
{
    CARNAC_mk_entry_t           raw_keyIDs[HW_SMART_CARD_MAX_MKDB_LEN];
    int                         raw_keyID_count = HW_SMART_CARD_MAX_MKDB_LEN, src_idx, dst_idx;

    HP_HOST_SMART_CARD_return_t status;
    status = HP_HOST_SMART_CARD_list_mkdb(raw_keyIDs, &raw_keyID_count);
    HP_ASSERT(status == HP_HOST_SMART_CARD_ok_e);

    dst_idx = 0;
    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_AUTH_INFO, 3,
                    "Host:");
    for (src_idx = 0; src_idx < raw_keyID_count; src_idx++)
    {
        if (raw_keyIDs[src_idx].mk_id.rk_id[0] == 0x44)
        {
            host_mkdb[dst_idx++] = raw_keyIDs[src_idx];
            HP_DEBUG_printf(NULL,
                            HP_DBG_LEVEL_AUTH_INFO, 3,
                            " 0x%02x%02x", raw_keyIDs[src_idx].mk_id.rk_id[0], raw_keyIDs[src_idx].mk_id.rk_id[1]);
        }
    }
    HP_DEBUG_printf(NULL,
                    HP_DBG_LEVEL_AUTH_INFO, 3,
                    "\n");
    host_mkdb_count = dst_idx;
}

void hw_smart_card_set_xpt(int instance_id, hw_smart_card_xpt_t *xpt_p)
{
    hw_smart_card_device[instance_id].xpt_p     = xpt_p;
    hw_smart_card_device[instance_id].sk_valid  = FALSE;
    hw_smart_card_device[instance_id].level     = PROTO_SECURITY_NORMAL;
}

static HW_SMART_CARD_status_t _start_session(HW_SMART_CARD_device_t *device_p)
{
    uint32_t                host_div;
    CARNAC_sk_id_t          sk_id;
    CARNAC_mk_id_t          mk_id;
    HW_SMART_CARD_status_t  status = HW_SMART_CARD_success_e;
    CARNAC_sk_t             sk;

    device_p->sk_valid = FALSE;

    // if host key list not built, do that now
    if (host_mkdb_count < 1)
    {
        hw_smart_card_multikey_init();
    }
    if (host_mkdb_count < 1)
    {
        return(HW_SMART_CARD_aux_failure_e);
    }

    if (HP_HOST_SMART_CARD_gen_rand(&host_div) != HP_HOST_SMART_CARD_ok_e)
    {
        return(HW_SMART_CARD_aux_failure_e);
    }

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_AUTH_INFO, 3,
                    "_start_session: Host diversifier generated = 0x%x\n", host_div);

    if (HW_SMART_CARD_interrogate_acumen(device_p) != HW_SMART_CARD_success_e)
    {
        return(HW_SMART_CARD_aux_failure_e);
    }

    mk_id = HW_SMART_CARD_find_host_device_mkid_match(host_mkdb,
                                                      host_mkdb_count,
                                                      device_p->bkdb,
                                                      HW_SMART_CARD_NUM_BKIDS);

    // not a valid key
    HP_ASSERT(mk_id.rk_id[0] != 0 || mk_id.rk_id[1] != 0);

    // log key used for session
    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_AUTH_INFO, 3,
                    "_start_session: using key 0x%02x%02x\n", mk_id.rk_id[0], mk_id.rk_id[1]);

    status = hw_smart_card_proto_start_session_with_mkid(device_p,
                                                         host_div,
                                                         &mk_id,
                                                         &sk_id,
                                                         PROTO_SECURITY_NORMAL);

    /* check that we don't get failures that we are NOT expecting
     * in general */
    HP_ASSERT(status != HW_SMART_CARD_failed_e);
    HP_ASSERT(status != HW_SMART_CARD_proto_failure_e);
    HP_ASSERT(status != HW_SMART_CARD_not_auth_e);

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_AUTH_INFO, 4,
                    "_start_session: sk_id generated. rk_id = 0x%02x%02x\n mk_depth = 0x%02x\n base_div = 0x%02x%02x%02x%02x%02x\n Host div = 0x%02x%02x%02x%02x\n Session div = 0x%02x%02x%02x\n",
                    sk_id.bk_id.mk_id.rk_id[1], sk_id.bk_id.mk_id.rk_id[0],
                    sk_id.bk_id.mk_id.mk_depth[0],
                    sk_id.bk_id.base_div[4], sk_id.bk_id.base_div[3], sk_id.bk_id.base_div[2], sk_id.bk_id.base_div[1], sk_id.bk_id.base_div[0],
                    sk_id.host_div[3], sk_id.host_div[2], sk_id.host_div[1], sk_id.host_div[0],
                    sk_id.session_div[2], sk_id.session_div[1], sk_id.session_div[0]);

    if (status != HW_SMART_CARD_success_e)
    {
        HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 2,
                        "_start_session: Failed to get sk_id from smart card\n");

        HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 2,
                        "_start_session: sk_id generated. rk_id = 0x%02x%02x\n mk_depth = 0x%02x\n base_div = 0x%02x%02x%02x%02x%02x\n Host div = 0x%02x%02x%02x%02x\n Session div = 0x%02x%02x%02x\n",
                        sk_id.bk_id.mk_id.rk_id[1], sk_id.bk_id.mk_id.rk_id[0],
                        sk_id.bk_id.mk_id.mk_depth[0],
                        sk_id.bk_id.base_div[4], sk_id.bk_id.base_div[3], sk_id.bk_id.base_div[2], sk_id.bk_id.base_div[1], sk_id.bk_id.base_div[0],
                        sk_id.host_div[3], sk_id.host_div[2], sk_id.host_div[1], sk_id.host_div[0],
                        sk_id.session_div[2], sk_id.session_div[1], sk_id.session_div[0]);

        return(status);
    }


    // Save smart card ID portion of the sk_id
    memcpy(&device_p->sk_id, &sk_id, sizeof(CARNAC_bk_id_t));

    if (HP_HOST_SMART_CARD_gen_sk(&sk_id, &sk) != HP_HOST_SMART_CARD_ok_e)
    {
        HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 2,
                        "_start_session: Failed to generate Session Key\n");

        HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                        HP_DBG_LEVEL_ERROR, 2,
                        "_start_session: sk_id generated. rk_id = 0x%02x%02x\n mk_depth = 0x%02x\n base_div = 0x%02x%02x%02x%02x%02x\n Host div = 0x%02x%02x%02x%02x\n Session div = 0x%02x%02x%02x\n",
                        sk_id.bk_id.mk_id.rk_id[1], sk_id.bk_id.mk_id.rk_id[0],
                        sk_id.bk_id.mk_id.mk_depth[0],
                        sk_id.bk_id.base_div[4], sk_id.bk_id.base_div[3], sk_id.bk_id.base_div[2], sk_id.bk_id.base_div[1], sk_id.bk_id.base_div[0],
                        sk_id.host_div[3], sk_id.host_div[2], sk_id.host_div[1], sk_id.host_div[0],
                        sk_id.session_div[2], sk_id.session_div[1], sk_id.session_div[0]);


        return(HW_SMART_CARD_aux_failure_e);
    }

    if (AES_CALL(set_key)(AES, (uint8_t *) &sk, sizeof(sk)) != CRYPTO_status_OK)
    {
        return(HW_SMART_CARD_aux_failure_e);
    }

    device_p->sk_valid  = TRUE;
    device_p->mac_div   = 0;

    return(HW_SMART_CARD_success_e);
}

static HW_SMART_CARD_status_t _start_session_if_needed(HW_SMART_CARD_device_t *device_p)
{
    if (device_p->sk_valid)
    {
        return HW_SMART_CARD_success_e;
    }
    return _start_session(device_p);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_raw_read(HW_SMART_CARD_device_t *device_p, uint32_t addr,
                       int len, uint8_t *buf_p, int *bytes_read)
{
    int                     retry       = 0;
    int                     failures    = 0;
    HW_SMART_CARD_status_t  status      = HW_SMART_CARD_failed_e;

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Entering HW_SMART_CARD_raw_read()\n");

    (void) failures;

    *bytes_read = 0;
    while (len > 0 && retry < HW_SMART_CARD_MAX_RETRIES)
    {
        status = _start_session_if_needed(device_p);
        if (status != HW_SMART_CARD_success_e)
        {
            failures++;
            retry++;
        }
        else
        {
            int rlen = MIN(PROTO_MAX_LEN_BODY, len);

            status = hw_smart_card_proto_read(device_p, addr, rlen, buf_p);

            if (status != HW_SMART_CARD_success_e)
            {
                retry++;
                failures++;
                device_p->sk_valid = FALSE;
                if (_stats_func_cb_p != NULL)
                {
                    // log the failure and the fact that we retried
                    (*_stats_func_cb_p)(device_p->idx, HW_SMART_CARD_retry_e);
                    (*_stats_func_cb_p)(device_p->idx, status);
                }
            }
            else
            {
                retry       = 0;
                addr        += rlen;
                buf_p       += rlen;
                len         -= rlen;
                *bytes_read += rlen;
            }
        }
    }

    if (_stats_func_cb_p != NULL)
    {
        (*_stats_func_cb_p)(device_p->idx, status);
    }


    // Invalidate the session key if the read attempt was unsuccessful
    if (status != HW_SMART_CARD_success_e)
    {
        device_p->sk_valid = FALSE;
    }

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Exiting HW_SMART_CARD_raw_read(). Status = %d\n", status);

    return(status);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_raw_write(HW_SMART_CARD_device_t *device_p, uint32_t addr,
                        int len, uint8_t *buf_p, int *bytes_written)
{
    int                     retry       = 0;
    int                     failures    = 0;
    HW_SMART_CARD_status_t  status      = HW_SMART_CARD_failed_e;

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Entering HW_SMART_CARD_raw_write()\n");

    (void) failures;

    *bytes_written = 0;
    while (len > 0 && retry < HW_SMART_CARD_MAX_RETRIES)
    {
        status = _start_session_if_needed(device_p);
        if (status != HW_SMART_CARD_success_e)
        {
            retry++;
            failures++;
        }
        else
        {
            int rlen = MIN(PROTO_MAX_LEN_BODY, len);

            status = hw_smart_card_proto_write(device_p, addr, rlen, buf_p);

            if (status != HW_SMART_CARD_success_e)
            {
                retry++;
                failures++;
                device_p->sk_valid = FALSE;
                if (_stats_func_cb_p != NULL)
                {
                    // log the failure and the fact that we retried
                    (*_stats_func_cb_p)(device_p->idx, HW_SMART_CARD_retry_e);
                    (*_stats_func_cb_p)(device_p->idx, status);
                }
            }
            else
            {
                retry           = 0;
                addr            += rlen;
                buf_p           += rlen;
                len             -= rlen;
                *bytes_written  += rlen;
            }
        }
    }
    if (_stats_func_cb_p != NULL)
    {
        (*_stats_func_cb_p)(device_p->idx, status);
    }

    // Invalidate the session key if the write attempt was unsuccessful
    if (status != HW_SMART_CARD_success_e)
    {
        device_p->sk_valid = FALSE;
    }

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Exiting HW_SMART_CARD_raw_write(). Status = %d\n", status);

    return(status);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_change_partition(HW_SMART_CARD_device_t *device_p, int id, int access_mode)
{
    HW_SMART_CARD_status_t status = HW_SMART_CARD_failed_e;

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Entering HW_SMART_CARD_change_partition()\n");

    // Start a session if needed, then issue the change_partition command
    status = _start_session_if_needed(device_p);
    if (status == HW_SMART_CARD_success_e)
    {
        status = hw_smart_card_proto_change_part(device_p, (uint8_t) id, (uint8_t) access_mode);
    }

    // Invalidate the session key if the command was not successful
    if (status != HW_SMART_CARD_success_e)
    {
        device_p->sk_valid = FALSE;
    }

    HP_DEBUG_printf(DBG_HW_SMART_CARD_ID,
                    HP_DBG_LEVEL_SC_TRACE, 2,
                    "Exiting HW_SMART_CARD_change_partition(). Status = %d\n", status);

    return(status);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_echo(HW_SMART_CARD_device_t *device_p, uint8_t *data_p, int data_len)
{
    return hw_smart_card_proto_echo(device_p, data_p, data_len);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_list_bkdb(HW_SMART_CARD_device_t *device_p)
{
    return hw_smart_card_proto_list_bkdb(device_p);
}

HW_SMART_CARD_status_t
HW_SMART_CARD_interrogate_acumen(HW_SMART_CARD_device_t *device_p)
{
    /* Null out the device's bkdb array... */
    CARNAC_bk_id_t  null_bkid = { { { 0, 0 }, { 0 } }, { 0, 0, 0, 0, 0 } };
    int             idx;
    for (idx = 0; idx < HW_SMART_CARD_NUM_BKIDS; idx++)
    {
        device_p->bkdb[idx].bk_id = null_bkid;
    }

    /* 3.x device is assumed */
    return HW_SMART_CARD_list_bkdb(device_p);
}
