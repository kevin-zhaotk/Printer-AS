/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include <stdio.h>

#include "hw_smart_card.h"
#include "hp_os_service.h"
#include "hw_smart_card_proto.h"
#include "hp_assert.h"
#include "hp_generic_macros.h"
#include "hp_debug_log_internal.h"
#include "hp_host_smart_card_ifc.h"
#include "hp_host_smart_card_ifc.h"
#include "lib_crc.h"


#define SMART_CARD_PROTO_DBG_ID     "SCP"
#define CMD_RECV_RETRY_INTERVAL     (1000)
#define MAX_CMD_RESP_TIME           (50000)

static HP_MUTEX_ID  mutex;
static uint8_t      last_bad_pkt[PROTO_MAX_LEN_PKT];

static void _save_pkt(uint8_t * pkt, int body_len)
{
    memset(last_bad_pkt, 0, PROTO_MAX_LEN_PKT);
    memcpy(last_bad_pkt, pkt, body_len + 2);                                                                             // need to account for header bytes (2) in pkt
}

static HW_SMART_CARD_status_t _map2status(uint8_t rsp)
{
    return (HW_SMART_CARD_status_t) rsp;
}

static void _acquire()
{
    HP_mutex_lock(&mutex);
}

static HW_SMART_CARD_status_t _release(HW_SMART_CARD_status_t return_val)
{
    HP_mutex_unlock(&mutex);
    return(return_val);
}

void hw_smart_card_proto_init(void)
{
    HP_mutex_init(&mutex);
}


static uint8_t _get_u8(uint8_t *buf_p)
{
    return buf_p[0];
}

static uint16_t _get_u16(uint8_t *buf_p)
{
    return(((uint32_t) buf_p[0]) << 8 |
           ((uint32_t) buf_p[1]) << 0);
}

static uint32_t _get_u32(uint8_t *buf_p)
{
    return(((uint32_t) buf_p[0]) << 24 |
           ((uint32_t) buf_p[1]) << 16 |
           ((uint32_t) buf_p[2]) << 8 |
           ((uint32_t) buf_p[3]) << 0);
}


static void _put_u8(uint8_t *buf_p, uint8_t data)
{
    buf_p[0] = data;
}

static void _put_u16(uint8_t *buf_p, uint16_t data)
{
    buf_p[0]    = (uint8_t) (data >> 8);
    buf_p[1]    = (uint8_t) (data >> 0);
}

static void _put_u24(uint8_t *buf_p, uint32_t data)
{
    buf_p[0]    = (uint8_t) (data >> 16);
    buf_p[1]    = (uint8_t) (data >> 8);
    buf_p[2]    = (uint8_t) (data >> 0);
}

static void _put_u32(uint8_t *buf_p, uint32_t data)
{
    buf_p[0]    = (uint8_t) (data >> 24);
    buf_p[1]    = (uint8_t) (data >> 16);
    buf_p[2]    = (uint8_t) (data >> 8);
    buf_p[3]    = (uint8_t) (data >> 0);
}

static void _put(uint8_t *dst_p, const uint8_t *src_p, int size)
{
    memcpy(dst_p, src_p, size);
}


/*
 * ********************
 * Helper functions
 * ********************
 */
static int _send_cmd(hw_smart_card_xpt_t *xpt_p,
                     uint8_t cmd,
                     uint8_t *body,
                     int body_len)
{
    HP_SMART_CARD_i2c_result_t  result;
    uint8_t                     pkt[PROTO_MAX_LEN_PKT];
    int32_t                     i;

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering _send_cmd()\n");

    HP_ASSERT(body_len >= 0 && body_len <= (PROTO_MAX_LEN_PKT - PROTO_OFFS_BODY));
    pkt[0]  = cmd;
    pkt[1]  = (uint8_t) body_len;
    if (body_len > 0)
    {
        memcpy(pkt + PROTO_OFFS_BODY, body, body_len);
    }
/*
    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 4,
                    "Data in _send_cmd():");

    for (i = 0; i < body_len + 2; i++)
    {
        HP_DEBUG_printf(NULL,
                        HP_DBG_LEVEL_SCP_TRACE, 4,
                        " 0x%02x", pkt[i]);
    }
    HP_DEBUG_printf(NULL,
                    HP_DBG_LEVEL_SCP_TRACE, 4,
                    "\n");
*/
    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 4,
                    "Data in _send_cmd():", toHexString(pkt, body_len));

    result = IFC_CALL(xpt_p, write)(xpt_p, PROTO_CMD_ADDR, pkt, PROTO_LEN_HDR + body_len);

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Exiting _send_cmd(): Result = %d\n", result);

    return (result == HP_SMART_CARD_I2C_SUCCESS) ? body_len : -1;
}

/*
 * NOTE: For the _recv_rsp and recv_rsp_with_a2d functions, the body_len returned by the
 *       smart_card part does not reflect the header portion of the response (2 bytes).
 *       However, the client allocates a buffer (rcv_buf) large enough to contain both
 *       the header length and the body length and _recv_rsp* functions depend on that.
 */

static HW_SMART_CARD_status_t _recv_rsp(hw_smart_card_xpt_t *xpt_p,
                                        uint8_t *rcv_buf,
                                        int expected_rsp_len,
                                        int * returned_rsp_len)
{
    HP_SMART_CARD_i2c_result_t  result;
    int                         rsp_len;
    int32_t                     i;
    int32_t                     time_elapsed    = 0;
    int                         sleep_count     = 0;
    char                        sleep_count_msg[30];

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering _recv_rsp()\n");

    HP_ASSERT(expected_rsp_len >= 0 && expected_rsp_len <= (PROTO_MAX_LEN_PKT));

    while (1)
    {
        if (time_elapsed > MAX_CMD_RESP_TIME)
        {
            HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                            HP_DBG_LEVEL_SCP_TRACE, 2,
                            "Exiting _recv_rsp(): HW_SMART_CARD_xpt_no_device_e\n");
            return HW_SMART_CARD_xpt_no_device_e;
        }

        result = IFC_CALL(xpt_p, read)(xpt_p, PROTO_RSP_HDR_ADDR, rcv_buf, PROTO_LEN_HDR);
        if (result != HP_SMART_CARD_I2C_SUCCESS)
        {
            /*
             * This means that smart card is not ready to send the response yet.
             * Wait for a while and try to get the response.
             */
            sleep_count++;
            HP_USLEEP(CMD_RECV_RETRY_INTERVAL);
            time_elapsed += CMD_RECV_RETRY_INTERVAL;
            continue;
        }
        else
        {
            break;
        }
    }                                                                             /* End of while loop */

    rsp_len = rcv_buf[1];

    // create sleep message (DO NOT EXCEED BUF SIZE)
    snprintf(sleep_count_msg, 29, ", Retry=%d", sleep_count);

    if (rcv_buf[0] == HW_SMART_CARD_success_e)
    {
        if (returned_rsp_len != NULL)
        {
            // then the client expects the rsp_len to be returned to it
            // expected_rsp_len "becomes" the maximum rsp_len allowed by the caller
            *returned_rsp_len = rsp_len;
            if (rsp_len > expected_rsp_len)
            {
                HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                                HP_DBG_LEVEL_SCP_TRACE, 2,
                                "Exiting _recv_rsp()%s: HW_SMART_CARD_proto_failure_e. Expected length<=%d Received length=%d\n",
                                sleep_count_msg, expected_rsp_len, rsp_len);

                return HW_SMART_CARD_rsp_len_too_long_e;
            }
        }
        else
        {
            // then the client knows what the response length should be
            if (rsp_len != expected_rsp_len)
            {
                HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                                HP_DBG_LEVEL_SCP_TRACE, 2,
                                "Exiting _recv_rsp()%s: HW_SMART_CARD_proto_failure_e.  Expected length=%d Received length=%d\n",
                                sleep_count_msg, expected_rsp_len, rsp_len);
                return HW_SMART_CARD_rsp_len_mismatch_e;
            }
        }

        if (rsp_len > 0)
        {
            result = IFC_CALL(xpt_p, read)(xpt_p, PROTO_RSP_BODY_ADDR, rcv_buf + PROTO_OFFS_BODY, rsp_len);
            if (result != HP_SMART_CARD_I2C_SUCCESS)
            {
                HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                                HP_DBG_LEVEL_SCP_TRACE, 2,
                                "Exiting _recv_rsp()%s: HW_SMART_CARD_xpt_rsp_failed_e.  Received length=%d\n",
                                sleep_count_msg, rsp_len);

                return HW_SMART_CARD_xpt_rsp_failed_e;
            }
        }
    }
/*
    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 4,
                    "Data in rcv_buf (%d):", rsp_len);
    for (i = 0; i < rsp_len + 2; i++)
    {
        HP_DEBUG_printf(NULL,
                        HP_DBG_LEVEL_SCP_TRACE, 4,
                        " 0x%02x", rcv_buf[i]);
    }
    HP_DEBUG_printf(NULL,
                    HP_DBG_LEVEL_SCP_TRACE, 4,
                    "\n");
*/
    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 4,
                    "Data in rcv_buf (%d): [%s]", rsp_len, toHexString(rcv_buf, rsp_len));

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Exiting _recv_rsp()%s\n",
                    sleep_count_msg);

    return _map2status(_get_u8(rcv_buf + PROTO_OFFS_RSP));
}

static void _compute_mac(HW_SMART_CARD_device_t *device_p,
                         uint8_t *pkt, int offs_mac)
{
    if (device_p->sk_valid)
    {
        memset(pkt + offs_mac, 0, PROTO_LEN_MAC);
    }
    else
    {
        return;
    }

    /* clear the crypto engine, put mac_div in pkt, then compute MAC based on pkt + mac_div */
    CMAC_CALL(init)(CMAC);
    _put_u16(pkt + offs_mac, device_p->mac_div);
    CMAC_CALL(update)(CMAC, pkt, offs_mac + PROTO_LEN_MACDIV);
    CMAC_CALL(final)(CMAC, pkt + offs_mac, PROTO_LEN_MAC);
}

static HW_SMART_CARD_status_t _verify_mac(HW_SMART_CARD_device_t *device_p,
                                          uint8_t *pkt_p, int offs_mac,
                                          uint8_t *cmd_mac)
{
    uint8_t rsp;
    if ((rsp = _get_u8(pkt_p + PROTO_OFFS_RSP)) == PROTO_RSP_SUCCESS)
    {
        uint8_t mac[PROTO_LEN_MAC];

        /* clear the crypto engine, put mac_div in pkt, then compute MAC based on pkt + mac_div */
        CMAC_CALL(init)(CMAC);
        CMAC_CALL(update)(CMAC, pkt_p, offs_mac);
        CMAC_CALL(update)(CMAC, cmd_mac, PROTO_LEN_MAC);
        CMAC_CALL(final)(CMAC, mac, sizeof(mac));

        /* if mac is different then invalidate session */
        if (memcmp(pkt_p + offs_mac, mac, sizeof(mac)))
        {
            device_p->sk_valid = FALSE;
            /* LIB_UDW_printf(LIB_UDW_DEFAULT_HANDLE, "mac error: expected: %s, received: %s\n",
               mac, pkt_p + offs_mac); */
            return HW_SMART_CARD_mac_verification_failure_e;
        }
        /* if mac matched then we can update mac_div */
        else
        {
            HP_ASSERT(device_p->level == PROTO_SECURITY_NORMAL);

            device_p->mac_div++;
        }
    }
    /* there was a failure; that automatically invalidates the session */
    else
    {
        device_p->sk_valid = FALSE;
    }

    return _map2status(rsp);
}

/*
 * ********************
 * Protocol Layer
 * ********************
 */

/*
 * START_SESSION CMD
 */
HW_SMART_CARD_status_t
hw_smart_card_proto_start_session_with_mkid(HW_SMART_CARD_device_t *device_p,
                                            uint32_t host_div,
                                            CARNAC_mk_id_t *mk_id_p,
                                            CARNAC_sk_id_t *sk_id_p,
                                            int action)
{
    HW_SMART_CARD_status_t  status;
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;
    uint8_t                 cmd_start_session_body[PROTO_LEN_HOSTDIV + \
                                                   PROTO_LEN_LEVEL +   \
                                                   PROTO_LEN_MKID +    \
                                                   PROTO_LEN_CRC];
    uint8_t     pkt[PROTO_MAX_LEN_PKT];
    int         body_len, expected_rsp_len, rsp_len;
    uint16_t    crc, crcv;

    HP_ASSERT(action == PROTO_SECURITY_NORMAL);

    _put_u32(cmd_start_session_body, host_div);
    body_len                            = PROTO_LEN_HOSTDIV;
    cmd_start_session_body[body_len]    = (uint8_t) action;
    body_len++;

    /* Since we're starting this session with an MKID, we need to add it to the
       packet. */
    HP_ASSERT(mk_id_p != NULL);
    cmd_start_session_body[body_len++]  = mk_id_p->rk_id[0];
    cmd_start_session_body[body_len++]  = mk_id_p->rk_id[1];
    cmd_start_session_body[body_len++]  = mk_id_p->mk_depth[0];

    pkt[0]  = PROTO_CMD_START_SESSION;
    pkt[1]  = 10;
    memcpy(pkt + PROTO_OFFS_BODY, cmd_start_session_body, body_len);
    crc = lib_crc_ccitt_calc(pkt, pkt + PROTO_OFFS_BODY + body_len - 1);
    _put_u16(cmd_start_session_body + body_len, crc);
    body_len            += PROTO_LEN_CRC;
    expected_rsp_len    = PROTO_LEN_SKID + PROTO_LEN_CRC;

    _acquire();
    /* check for no transport */
    if (!xpt_p)
    {
        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_START_SESSION, cmd_start_session_body, body_len) < 0)
    {
        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, expected_rsp_len, &rsp_len);

    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, expected_rsp_len);
        return _release(status);
    }

    memcpy(sk_id_p, pkt + PROTO_OFFS_BODY, PROTO_LEN_SKID);

    /* make sure the host_div that we received is the same as was sent */
    if (_get_u8(pkt + PROTO_OFFS_RSP) == PROTO_RSP_SUCCESS &&
        _get_u32(sk_id_p->host_div) != host_div)
    {
        return _release(HW_SMART_CARD_proto_failure_e);
    }

    HP_ASSERT(rsp_len == PROTO_LEN_SKID + PROTO_LEN_CRC);
    crc     = lib_crc_ccitt_calc(pkt, pkt + PROTO_OFFS_BODY + PROTO_LEN_SKID - 1);
    crcv    = _get_u16(pkt + PROTO_OFFS_BODY + PROTO_LEN_SKID);
    if (crcv != crc)
    {
        _release(HW_SMART_CARD_proto_failure_e);
    }

    /* update the security level for this device */
    device_p->level = (int8_t) action;

    return _release(status);
}

/*
 * READ CMD
 */
HW_SMART_CARD_status_t
hw_smart_card_proto_read(HW_SMART_CARD_device_t *device_p, uint32_t addr, int size,
                         uint8_t *result_p)
{
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;
    uint8_t                 pkt[PROTO_MAX_LEN_PKT], cmd_mac[PROTO_LEN_MAC];
    HW_SMART_CARD_status_t  status;

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering hw_smart_card_proto_read()\n");

    _acquire();

    /* check for no transport */
    if (!xpt_p)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_read(). Status = %d\n", HW_SMART_CARD_xpt_not_set_failure_e);

        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    /* put together cmd_mac_data  & compute cmd_mac & save it */
    _put_u8(pkt + PROTO_OFFS_CMD, PROTO_CMD_READ);
    _put_u8(pkt + PROTO_OFFS_LEN, PROTO_LEN_READ_CMD_BODY);
    _put_u24(pkt + PROTO_OFFS_BODY + PROTO_OFFS_READADDR, (uint32_t) addr);
    _put_u8(pkt + PROTO_OFFS_BODY + PROTO_OFFS_READLEN, (uint8_t) size);
    _compute_mac(device_p, pkt, PROTO_OFFS_BODY + PROTO_OFFS_READ_MACDIV);
    _put(cmd_mac, pkt + PROTO_OFFS_BODY + PROTO_OFFS_READ_MAC, PROTO_LEN_MAC);

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_READ, pkt + PROTO_OFFS_BODY, PROTO_LEN_READ_CMD_BODY) < 0)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_read(). Status = %d\n", HW_SMART_CARD_xpt_cmd_failed_e);

        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, size + PROTO_LEN_MAC, NULL);

    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, size + PROTO_LEN_MAC);

        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_read(). Status = %d\n", status);

        return _release(status);
    }

    /* check mac */
    status = _verify_mac(device_p, pkt, PROTO_LEN_HDR + size, cmd_mac);

    /* if successful copy data */
    if (status == HW_SMART_CARD_success_e)
    {
        memcpy(result_p, pkt + PROTO_OFFS_BODY, size);
    }

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Exiting hw_smart_card_proto_read(). Status = %d\n", status);

    return _release(status);
}

/*
 * WRITE CMD
 */
HW_SMART_CARD_status_t
hw_smart_card_proto_write(HW_SMART_CARD_device_t *device_p, uint32_t addr, int size,
                          uint8_t *buf_p)
{
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;
    uint8_t                 pkt[PROTO_MAX_LEN_PKT], cmd_mac[PROTO_LEN_MAC];
    HW_SMART_CARD_status_t  status;
    int                     offset_mac = 0;

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering hw_smart_card_proto_write()\n");

    HP_ASSERT(device_p->level == PROTO_SECURITY_NORMAL);

    _acquire();

    /* check for no transport */
    if (!xpt_p)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_write(). Status = %d\n", HW_SMART_CARD_xpt_not_set_failure_e);

        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    /* put together cmd_mac_data  & compute cmd_mac & save it */
    _put_u8(pkt + PROTO_OFFS_CMD, PROTO_CMD_WRITE);
    _put_u8(pkt + PROTO_OFFS_LEN, PROTO_LEN_WRITEADDR + PROTO_LEN_WRITELEN + (uint8_t) size + PROTO_LEN_MAC);
    _put_u24(pkt + PROTO_OFFS_BODY + PROTO_OFFS_WRITEADDR, (uint32_t) addr);
    _put_u8(pkt + PROTO_OFFS_BODY + PROTO_OFFS_WRITELEN, (uint8_t) size);
    _put(pkt + PROTO_OFFS_BODY + PROTO_OFFS_WRITEDATA, buf_p, size);
    offset_mac = PROTO_LEN_HDR + PROTO_LEN_WRITEADDR + PROTO_LEN_WRITELEN + size;
    _compute_mac(device_p, pkt, offset_mac);
    _put(cmd_mac, pkt + offset_mac, PROTO_LEN_MAC);

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_WRITE, pkt + PROTO_OFFS_BODY,
                  PROTO_LEN_WRITEADDR + PROTO_LEN_WRITELEN + size + PROTO_LEN_MAC) < 0)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_write(). Status = %d\n", HW_SMART_CARD_xpt_cmd_failed_e);

        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, PROTO_LEN_MAC, NULL);

    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, PROTO_LEN_MAC);

        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_write(). Status = %d\n", status);

        return _release(status);
    }

    /* check mac */
    status = _verify_mac(device_p, pkt, PROTO_LEN_HDR, cmd_mac);

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Exiting hw_smart_card_proto_write(). Status = %d\n", status);

    return _release(status);
}


/*
 * ECHO CMD
 */
HW_SMART_CARD_status_t
hw_smart_card_proto_echo(HW_SMART_CARD_device_t *device_p, uint8_t *data_p, int data_len)
{
    uint8_t                 pkt[PROTO_MAX_LEN_PKT]; int rlen;
    HW_SMART_CARD_status_t  status;
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering hw_smart_card_proto_echo()\n");

    _acquire();

    /* check for no transport */
    if (!xpt_p)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_echo(): Status = %d\n", HW_SMART_CARD_xpt_not_set_failure_e);

        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_ECHO, data_p, data_len) < 0)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_echo(): Status = %d\n", HW_SMART_CARD_xpt_cmd_failed_e);

        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, PROTO_MAX_LEN_BODY, &rlen);
    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, PROTO_MAX_LEN_BODY);

        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_echo(): Status = %d\n", status);

        return _release(status);
    }

    /* check for identical echo */
    if ((rlen == data_len) && !memcmp(data_p, pkt + PROTO_OFFS_BODY, data_len))
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 3,
                        "Exiting hw_smart_card_proto_echo(): Status = %d\n", HW_SMART_CARD_success_e);

        return _release(HW_SMART_CARD_success_e);
    }
    else
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_echo(): Status = %d\n", HW_SMART_CARD_failed_e);

        return _release(HW_SMART_CARD_failed_e);
    }
}

/*
 * CHANGE PARTITION CMD
 */
HW_SMART_CARD_status_t
hw_smart_card_proto_change_part(HW_SMART_CARD_device_t *device_p,
                                uint8_t id, uint8_t access_mode)
{
    uint8_t                 pkt[PROTO_MAX_LEN_PKT], cmd_mac[PROTO_LEN_MAC];
    HW_SMART_CARD_status_t  status;
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Entering hw_smart_card_proto_change_part()\n");

    HP_ASSERT(device_p->level == PROTO_SECURITY_NORMAL);

    _acquire();

    /* check for no transport */
    if (!xpt_p)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_change_part(). Status = %d\n", HW_SMART_CARD_xpt_not_set_failure_e);

        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    /* put together command because it needs to be MAC'ed */
    _put_u8(pkt + PROTO_OFFS_CMD, PROTO_CMD_CHANGE_PART);
    _put_u8(pkt + PROTO_OFFS_LEN, PROTO_LEN_CHANGE_PART_CMD_BODY);
    _put_u8(pkt + PROTO_OFFS_BODY + PROTO_OFFS_ID, id);
    _put_u8(pkt + PROTO_OFFS_BODY + PROTO_OFFS_MODE, access_mode);
    _compute_mac(device_p, pkt, PROTO_OFFS_BODY + PROTO_OFFS_CHANGE_PART_MAC);
    _put(cmd_mac, pkt + PROTO_OFFS_BODY + PROTO_OFFS_CHANGE_PART_MAC, PROTO_LEN_MAC);

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_CHANGE_PART, pkt + PROTO_OFFS_BODY, PROTO_LEN_CHANGE_PART_CMD_BODY) < 0)
    {
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_change_part(). Status = %d\n", HW_SMART_CARD_xpt_cmd_failed_e);

        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, PROTO_LEN_MAC, NULL);
    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, PROTO_LEN_MAC);

        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_SCP_TRACE, 2,
                        "Exiting hw_smart_card_proto_change_part(). Status = %d\n", status);

        return _release(status);
    }

    /* check rsp_mac */
    status = _verify_mac(device_p, pkt, PROTO_LEN_HDR, cmd_mac);

    HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                    HP_DBG_LEVEL_SCP_TRACE, 3,
                    "Exiting hw_smart_card_proto_change_part(). Status = %d\n", status);

    return _release(status);
}


HW_SMART_CARD_status_t
hw_smart_card_proto_list_bkdb(HW_SMART_CARD_device_t *device_p)
{
    uint8_t                 pkt[PROTO_MAX_LEN_PKT];
    HW_SMART_CARD_status_t  status;
    hw_smart_card_xpt_t     *xpt_p = device_p->xpt_p;
    int                     rsp_len, actual_rsp_len, body_len;
    uint8_t                 bkdb_pkt[PROTO_MAX_LEN_BODY];
    uint16_t                crc, crcv;
    int                     idx;
    int                     data_offset, num_keys;

    HP_ASSERT(device_p->level == PROTO_SECURITY_NORMAL);

    _acquire();

    /* check for no transport */
    if (!xpt_p)
    {
        return _release(HW_SMART_CARD_xpt_not_set_failure_e);
    }

    _put_u8(pkt + PROTO_OFFS_CMD, PROTO_CMD_LIST_BKDB);
    _put_u8(pkt + PROTO_OFFS_LEN, PROTO_LEN_LIST_BKDB_CMD_BODY);

    /* Compute the CRC and append it to the end */
    body_len    = 0;
    body_len    += PROTO_LEN_CRC;
    crc         = lib_crc_ccitt_calc(pkt, pkt + body_len - 1);
    _put_u16(pkt + body_len, crc);

    /* write command */
    if (_send_cmd(xpt_p, PROTO_CMD_LIST_BKDB, pkt + PROTO_OFFS_BODY, PROTO_LEN_LIST_BKDB_CMD_BODY) < 0)
    {
        return _release(HW_SMART_CARD_xpt_cmd_failed_e);
    }

    /* read response */
    status = _recv_rsp(xpt_p, pkt, PROTO_MAX_LEN_BODY, &rsp_len);
    if (status != HW_SMART_CARD_success_e)
    {
        _save_pkt(pkt, PROTO_LEN_MAC);
        return _release(status);
    }

    actual_rsp_len = rsp_len;

    /* check the crc */
    crc     = lib_crc_ccitt_calc(pkt, pkt + actual_rsp_len - 1);
    crcv    = _get_u16(pkt + actual_rsp_len);
    if (crc != crcv)
    {
        _release(HW_SMART_CARD_proto_failure_e);
    }

    /* if successful copy data */
    if ((status == HW_SMART_CARD_success_e) && rsp_len)
    {
        /* Pull out the bkdb information. */
        memcpy(bkdb_pkt, pkt + PROTO_OFFS_BODY, actual_rsp_len - 2);

        /* Verify data is of correct length */
        if ((actual_rsp_len - PROTO_OFFS_BODY) % sizeof(HW_SMART_CARD_bkdb_entry_t))
        {
            _release(HW_SMART_CARD_proto_failure_e);
        }

        /* If we get here then the data is valid */
        /* Parse the returned data into the bkdb */
        data_offset = 0;
        num_keys    = (actual_rsp_len - PROTO_OFFS_BODY) / sizeof(HW_SMART_CARD_bkdb_entry_t);
        HP_DEBUG_printf(SMART_CARD_PROTO_DBG_ID,
                        HP_DBG_LEVEL_AUTH_INFO, 3,
                        "Component:");
        for (idx = 0; idx < num_keys; idx++)
        {
            data_offset = idx * sizeof(HW_SMART_CARD_bkdb_entry_t);
            /* Copy the bk_id */
            memcpy(&(device_p->bkdb[idx].bk_id.mk_id.rk_id[0]), &bkdb_pkt[data_offset], 2);
            memcpy(&(device_p->bkdb[idx].bk_id.mk_id.mk_depth[0]), &bkdb_pkt[data_offset + 2], 1);
            memcpy(&(device_p->bkdb[idx].bk_id.base_div[0]), &bkdb_pkt[data_offset + 3], 5);
            /* Copy the sk_deriv */
            memcpy(&(device_p->bkdb[idx].sk_deriv), &bkdb_pkt[data_offset + 8], 1);
            HP_DEBUG_printf(NULL,
                            HP_DBG_LEVEL_AUTH_INFO, 3,
                            " 0x%02x%02x", device_p->bkdb[idx].bk_id.mk_id.rk_id[0], device_p->bkdb[idx].bk_id.mk_id.rk_id[1]);
        }
        HP_DEBUG_printf(NULL,
                        HP_DBG_LEVEL_AUTH_INFO, 3,
                        "\n");
    }
    else
    {
        status = HW_SMART_CARD_failed_e;
    }
    return _release(status);
}

