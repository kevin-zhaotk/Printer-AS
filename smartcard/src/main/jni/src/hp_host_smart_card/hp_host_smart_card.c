/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include <string.h>

#include "hp_host_smart_card.h"
#include "hp_host_smart_card_xpt.h"
#include "hp_host_smart_card_ifc.h"
#include "hp_host_smart_card_private.h"

#include "hp_os_service.h"
#include "hp_assert.h"
#include "hp_generic_macros.h"
#include "hp_debug_log_internal.h"

#define HOST_SMART_CARD_DBG_ID    "HSC"   /* Host smart card library. Abbreviated to make the log cleaner */

hp_host_smart_card_xpt_t    *hp_host_smart_card_xpt;
static HP_MUTEX_ID          mutex;

static smart_dev_t          _default_smart_dev;

void hp_host_smart_card_set_xpt(hp_host_smart_card_xpt_t *xpt_p)
{
    hp_host_smart_card_xpt = xpt_p;
}

void _check_override(smart_dev_t * smart_dev)
{
    // If the caller wants the current default, then give him the current default device
    if (*smart_dev == smart_dev_default_e)
        *smart_dev = _default_smart_dev;
}

static int _cmd_rsp(smart_dev_t smart_dev,
                    uint8_t cmd,
                    const uint8_t *cmd_body,
                    uint32_t cmd_body_len,
                    uint32_t cmd_processing_time_usec,
                    HP_HOST_SMART_CARD_rsp_t *rsp,
                    uint8_t      *rsp_body,
                    int max_rsp_len)
{
    uint8_t                     cmd_pkt[MAX_BUFFER_LENGTH];
    uint8_t                     ersp_params[MAX_DATA_LENGTH];

    uint8_t                     *p;
    int                         sz_ersp_params;

    host_smart_card_cmd_rsp_t   pkt;
    HP_SMART_CARD_i2c_result_t  result;
    int                         body_len = 0;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Entering _cmd_rsp()\n");

    HP_ASSERT(cmd_body_len < MAX_BUFFER_LENGTH);

    p       = cmd_pkt;
    *p++    = cmd;
    *p++    = (uint8_t) cmd_body_len;

    if (cmd_body_len)
    {
        memcpy(p, cmd_body, cmd_body_len);
        p += cmd_body_len;
    }

    pkt.cmd_p               = cmd_pkt;
    pkt.cmd_len             = p - cmd_pkt;
    pkt.delay_till_rsp_usec = cmd_processing_time_usec;
    pkt.rsp_p               = rsp;
    pkt.body_p              = ersp_params;
    pkt.max_body_len        = MIN(max_rsp_len + CMAC_LEN, MAX_DATA_LENGTH);
    pkt.body_len            = &sz_ersp_params;
    pkt.smart_dev           = smart_dev;

    result = IFC_CALL(hp_host_smart_card_xpt, cmd_rsp)(hp_host_smart_card_xpt, &pkt);
    if (result != HP_SMART_CARD_I2C_SUCCESS)
    {
        HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                        HP_DBG_LEVEL_HSC_TRACE, 3,
                        "Exiting _cmd_rsp(): ERROR result = %d\n", result);
        return(-1);
    }

    body_len = sz_ersp_params;
    memcpy(rsp_body, ersp_params, body_len);

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Exiting _cmd_rsp(): Body_len = %d\n", body_len);

    return(body_len);
}

static uint32_t _get_u32(uint8_t *buf_p)
{
    return(((uint32_t) buf_p[0]) << 24 |
           ((uint32_t) buf_p[1]) << 16 |
           ((uint32_t) buf_p[2]) << 8 |
           ((uint32_t) buf_p[3]) << 0);
}

// CMD_ECHO
HP_HOST_SMART_CARD_return_t hw_smart_dev_echo(uint8_t       *input,
                                              int input_length,
                                              smart_dev_t smart_dev)
{
    HP_HOST_SMART_CARD_rsp_t    rsp;
    uint8_t                     reply[MAX_DATA_LENGTH];
    int                         reply_len;

    HP_HOST_SMART_CARD_return_t result = HP_HOST_SMART_CARD_failed_e;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Entering hw_smart_dev_echo()\n");

    // If an override was defined, use it
    _check_override(&smart_dev);

    reply_len = _cmd_rsp(smart_dev,
                         CMD_ECHO,
                         input,
                         input_length,
                         HOST_SMART_CARD_SHORT_TIME_USEC,
                         &rsp,
                         reply,
                         sizeof(reply));

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Return value from _cmd_rsp = %d\n", reply_len);

    if ((reply_len == input_length) &&
        (rsp == RSP_SUCCESS) &&
        (memcmp(reply, input, input_length) == 0))
    {
        result = HP_HOST_SMART_CARD_ok_e;
    }

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Exiting hw_smart_dev_echo(). result = %d\n", result);

    return(result);
}

// CMD_GEN_SK
HP_HOST_SMART_CARD_return_t hw_smart_dev_gen_sk(CARNAC_sk_id_t* sk_id,
                                                CARNAC_sk_t* sk,
                                                smart_dev_t smart_dev)
{
    HP_HOST_SMART_CARD_rsp_t    rsp;
    int                         reply_len;
    HP_HOST_SMART_CARD_return_t result = HP_HOST_SMART_CARD_failed_e;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Entering hw_smart_dev_gen_sk()\n");

    /* make sure sk_id & sk is packed */
    HP_ASSERT(sizeof(*sk_id) == SESSION_ID_LEN);
    HP_ASSERT(sizeof(*sk) == SESSION_KEY_LEN);

    // If an override was defined, use it
    _check_override(&smart_dev);

    reply_len = _cmd_rsp(smart_dev,
                         CMD_GEN_SK,
                         (uint8_t *) sk_id,
                         sizeof(*sk_id),
                         HOST_SMART_CARD_MEDIUM_DELAY_USEC,
                         &rsp,
                         (uint8_t *) sk,
                         sizeof(*sk));

    if (reply_len == sizeof(*sk))
    {
        if (rsp == RSP_SUCCESS)
        {
            result = HP_HOST_SMART_CARD_ok_e;
        }
    }
    else if (rsp == RSP_INVALID_RK_ID)
    {
        result = HP_HOST_SMART_CARD_invalid_rk_id_e;
    }
    else if (rsp == RSP_INVALID_KEY_TYPE)
    {
        result = HP_HOST_SMART_CARD_invalid_key_type_e;
    }

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Exiting hw_smart_dev_gen_sk(). result = %d\n", result);

    return(result);
}

// CMD_GEN_RAND
HP_HOST_SMART_CARD_return_t hw_smart_dev_gen_rand(uint32_t* rand_num,
                                                  smart_dev_t smart_dev)
{
    HP_HOST_SMART_CARD_rsp_t    rsp;
    uint8_t                     rand[RANDOM_LEN];
    int                         reply_len;

    HP_HOST_SMART_CARD_return_t result = HP_HOST_SMART_CARD_failed_e;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Entering hw_smart_dev_gen_rand()\n");

    HP_ASSERT(sizeof(*rand_num) == RANDOM_LEN);

    // If an override was defined, use it
    _check_override(&smart_dev);

    reply_len = _cmd_rsp(smart_dev,
                         CMD_GEN_RAND,
                         (uint8_t *) NULL,
                         0,
                         HOST_SMART_CARD_MEDIUM_DELAY_USEC,
                         &rsp,
                         (uint8_t *) rand,
                         sizeof(rand));

    if (reply_len == sizeof(rand) && (rsp == RSP_SUCCESS))
    {
        *rand_num   = _get_u32(rand);
        result      = HP_HOST_SMART_CARD_ok_e;
    }

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Exiting hw_smart_dev_gen_rand(). result = %d\n", result);

    return(result);
}

// CMD_LIST_MKDB
HP_HOST_SMART_CARD_return_t hw_smart_dev_list_mkdb(CARNAC_mk_entry_t  *db_entries,
                                                   int                *entry_count,
                                                   smart_dev_t smart_dev)
{
    HP_HOST_SMART_CARD_rsp_t    rsp;
    int                         max_entries;
    int                         current_offset = 0;
    int                         return_length;
    HP_HOST_SMART_CARD_return_t result = HP_HOST_SMART_CARD_ok_e;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Entering hw_smart_dev_list_mkdb()\n");

    // If an override was defined, use it
    _check_override(&smart_dev);

    do
    {
        max_entries     = MIN(*entry_count, LIST_MKDB_LEN);
        return_length   = _cmd_rsp(smart_dev,
                                   CMD_LIST_MKDB,
                                   ((uint8_t *) &current_offset) + 2,
                                   2,
                                   HOST_SMART_CARD_LONG_DELAY_USEC,
                                   &rsp,
                                   (uint8_t *) (db_entries += current_offset),
                                   max_entries * LIST_MKDB_ENTRY_LEN) / LIST_MKDB_ENTRY_LEN;
        if ((return_length <= 0) || rsp != RSP_SUCCESS)
        {
            result = HP_HOST_SMART_CARD_failed_e;
            break;
        }
        current_offset  += return_length;
        *entry_count    -= return_length;
    } while ((return_length == LIST_MKDB_LEN) && (max_entries > 0));

    *entry_count = current_offset;

    HP_DEBUG_printf(HOST_SMART_CARD_DBG_ID,
                    HP_DBG_LEVEL_HSC_TRACE, 3,
                    "Exiting hw_smart_dev_list_mkdb(). result = %d\n", result);

    return(result);
}

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_echo(uint8_t* input,
                                                    int input_length)
{
    return hw_smart_dev_echo(input, input_length, smart_dev_default_e);
}

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_gen_sk(CARNAC_sk_id_t* sk_id,
                                                      CARNAC_sk_t* sk)
{
    return hw_smart_dev_gen_sk(sk_id, sk, smart_dev_default_e);
}

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_gen_rand(uint32_t* rand_num)
{
    return hw_smart_dev_gen_rand(rand_num, smart_dev_default_e);
}

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_list_mkdb(CARNAC_mk_entry_t* db_entries,
                                                         int* entry_count)
{
    return hw_smart_dev_list_mkdb(db_entries, entry_count, smart_dev_default_e);
}

// ********************************************************
// host smart card component startup and shutdown functions
// ********************************************************
void hp_host_smart_card_init(void)
{
    hp_host_smart_card_xpt_i2c_init();
    HP_HOST_SMART_CARD_chip_init();

    HP_mutex_init(&mutex);

    /* Set the deafult crypto device as smart_dev_host_smart_card_v2_e */
    _default_smart_dev = smart_dev_host_smart_card_v2_e;
}

void hp_host_smart_card_shutdown(void)
{
}
