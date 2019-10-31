/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

/***********************************************************
 * hp_host_smart_card_chip.c
 ***********************************************************
 *
 * Implementation of host smart card command-response protocol.
 *
 */

#include <stdio.h>

#include "hp_types.h"
#include "hp_host_smart_card_chip.h"
#include "../../drivers/internal_ifc/hp_smart_card_gpio_ifc.h"
#include "hp_debug_log_internal.h"
#include "hp_smart_card.h"
#include "hp_host_smart_card_private.h"

#include "hp_assert.h"
#include "hp_generic_macros.h"
#include "hp_os_service.h"

#define _RESPONSE       0
#define _BODY_LEN_LO    1
#define _BODY_LEN_HI    2

/*********************************
* Local typedef
*********************************/
typedef enum
{
    HOST_SMART_CARD_READY,
    HOST_SMART_CARD_NO_RESPONSE,
} host_smart_card_ready_t;

static HP_MUTEX_ID  chip_mutex;

static void         (*_cb_fn_p)(void) = NULL;

// Arbitrary number of retries
#define _NUM_RETRIES                    4
#define HOST_SMART_CARD_CHIP_DBG_ID     "HSCC"      /* Host smart card chip. Abbreviated to make the log cleaner */

/**************************
* Local functions
**************************/
static host_smart_card_ready_t _wait_till_ready(uint32_t wait_usec);

/*********************************
 * local implementation functions
 *********************************
 */
static host_smart_card_ready_t  _wait_till_ready(uint32_t wait_usec)
{
    HP_SMART_CARD_bool_t    gpio_val;
    uint32_t                iterations  = 0;
    host_smart_card_ready_t response    = HOST_SMART_CARD_NO_RESPONSE;
    char                    iterate_count_msg[30];

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Entering _wait_till_ready()\n");

    do
    {
        /* Read HP_SMART_CARD_GPIO_HOST_READY GPIO
           When the host smart card becomes ready, it
           drives this GPIO signal low.*/
        HP_USLEEP(wait_usec);

        gpio_val = HP_SMART_CARD_gpio_get_value(HP_SMART_CARD_GPIO_HOST_READY);
        iterations++;
    } while ((gpio_val == TRUE) && (iterations < 100));                         /* Arbitrary value of 100 - Need to fine tune */

    // create iterate message (DO NOT EXCEED BUF SIZE)
    snprintf(iterate_count_msg, 29, ", Retry=%d", (iterations - 1));

    if (gpio_val == FALSE)
    {
        response = HOST_SMART_CARD_READY;
    }

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Exiting _wait_till_ready()%s: Response = %d\n", iterate_count_msg, response);

    return response;
}

static HP_SMART_CARD_i2c_result_t _cmd_rsp(host_smart_card_cmd_rsp_t *cmd_rsp_p,
                                           int tries)
{
    uint8_t                     rsp_header[HOST_SMART_CARD_RSP_HDR_SZ];
    HP_SMART_CARD_i2c_result_t  result  = HP_SMART_CARD_I2C_FAILED;
    int                         retries = 0;
    uint32_t                    body_len;
    int32_t                     i;

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Entering _cmd_rsp()\n");

    /* Assume no body until successfully read response. */
    *cmd_rsp_p->body_len = 0;

    // for this implementation, there are no retries
    HP_ASSERT(tries == 1);

    do
    {
        if (retries)
        {
            HP_HOST_SMART_CARD_chip_reset();
        }
        retries++;

        result = HP_SMART_CARD_I2C_FAILED;
        if (cmd_rsp_p->cmd_len)
        {
            HP_ASSERT(cmd_rsp_p->cmd_len <= HOST_SMART_CARD_MAX_CMD_LEN);

            HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                            HP_DBG_LEVEL_HSCC_TRACE, 4,
                            "Data in _send_cmd():");

            for (i = 0; i < (int) cmd_rsp_p->cmd_len; i++)
            {
                HP_DEBUG_printf(NULL,
                                HP_DBG_LEVEL_HSCC_TRACE, 4,
                                " 0x%02x", cmd_rsp_p->cmd_p[i]);
            }
            HP_DEBUG_printf(NULL,
                            HP_DBG_LEVEL_HSCC_TRACE, 4,
                            "\n");
            result = HP_SMART_CARD_i2c_write(HP_SMART_CARD_DEVICE_HOST,
                                             HOST_SMART_CARD_ADDR_CMD,
                                             cmd_rsp_p->cmd_p,
                                             cmd_rsp_p->cmd_len
                                             );
            if (result != HP_SMART_CARD_I2C_SUCCESS)
            {
                continue;                                                                                                                                                                                                                                                                                                                 // Must resend the command
            }

            // Increase wait time if failed at first.
            if (HOST_SMART_CARD_NO_RESPONSE ==
                _wait_till_ready(retries * retries * (cmd_rsp_p->delay_till_rsp_usec)))
            {
                HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                                HP_DBG_LEVEL_ERROR, 0,
                                "ERROR - HOST_SMART_CARD_NO_RESPONSE\n");
//                HP_ASSERT(0);
//                continue;                                                                                                                                                                                                                                                                                                                 // Must reset and resend the command
            }
        }
        if (cmd_rsp_p->rsp_p)
        {
            rsp_header[_BODY_LEN_HI]    = 0;
            result                      = HP_SMART_CARD_i2c_read(HP_SMART_CARD_DEVICE_HOST,
                                                                 HOST_SMART_CARD_ADDR_RSP_HDR,
                                                                 rsp_header,
                                                                 HOST_SMART_CARD_RSP_HDR_SZ);

            if (result != HP_SMART_CARD_I2C_SUCCESS)
            {
                HP_ASSERT(0);                                                                                                                                                                                                                                                                                                                   // What caused the lack of response?
                continue;                                                                                                                                                                                                                                                                                                                       // Must resend the command
            }

            HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                            HP_DBG_LEVEL_HSCC_TRACE, 4,
                            "Data in rsp_header(): 0x%02x 0x%02x 0x%02x\n",
                            rsp_header[0], rsp_header[1], rsp_header[2]);
            if ((HP_HOST_SMART_CARD_rsp_t) rsp_header[_RESPONSE] == RSP_SUCCESS)
            {
                body_len    = rsp_header[_BODY_LEN_LO];
                body_len    |= rsp_header[_BODY_LEN_HI] << 8;
            }
            else
            {
                HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                                HP_DBG_LEVEL_HSCC_TRACE, 2,
                                "Response from _cmd_rsp() was %d (Setting body_len=0).\n",
                                rsp_header[_RESPONSE]);
                body_len = 0;
            }
            *(cmd_rsp_p->rsp_p) = (HP_HOST_SMART_CARD_rsp_t) rsp_header[_RESPONSE];

            // Most commands expect an exact length, does anyone check?
            // Limit to the client's expected length
            body_len = MIN(body_len, cmd_rsp_p->max_body_len);

            if (body_len)
            {
                result = HP_SMART_CARD_i2c_read(HP_SMART_CARD_DEVICE_HOST,
                                                HOST_SMART_CARD_ADDR_RSP_BODY,
                                                cmd_rsp_p->body_p,
                                                body_len);
                if (result == HP_SMART_CARD_I2C_FAILED)
                {
                    HP_ASSERT(0);                                                                                                                                                                                                                                                                                                                                                                                               // What caused the lack of response?
                    continue;                                                                                                                                                                                                                                                                                                                                                                                                   // Must resend the command
                }
                HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                                HP_DBG_LEVEL_HSCC_TRACE, 4,
                                "Data in response body_p():");

                for (i = 0; i < (int) body_len; i++)
                {
                    HP_DEBUG_printf(NULL,
                                    HP_DBG_LEVEL_HSCC_TRACE, 4,
                                    " 0x%02x", cmd_rsp_p->body_p[i]);
                }
                HP_DEBUG_printf(NULL,
                                HP_DBG_LEVEL_HSCC_TRACE, 4,
                                "\n");
            }
            *cmd_rsp_p->body_len = body_len;
        }

        // If we made it here, we can break out
        break;
    } while (retries < tries);

    if (result != HP_SMART_CARD_I2C_SUCCESS)
    {
        // Reset if failed
        HP_HOST_SMART_CARD_chip_reset();
    }

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Exiting _cmd_rsp(): Result = %d\n", result);

    return(result);
}

/************************************
* public entry points
************************************/
HP_SMART_CARD_i2c_result_t HP_HOST_SMART_CARD_chip__cmd_rsp(host_smart_card_cmd_rsp_t *cmd_rsp_p,
                                                            int tries)
{
    HP_SMART_CARD_i2c_result_t result;

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Entering HP_HOST_SMART_CARD_chip__cmd_rsp()\n");

    HP_mutex_lock(&chip_mutex);

    result = _cmd_rsp(cmd_rsp_p, tries);

    HP_mutex_unlock(&chip_mutex);

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Exiting HP_HOST_SMART_CARD_chip__cmd_rsp()\n");

    return(result);
}


void HP_HOST_SMART_CARD_chip_reset(void)
{
    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Entering HP_HOST_SMART_CARD_chip_reset()\n");

    HP_mutex_lock(&chip_mutex);

    /* assert RESET signal */
    HP_SMART_CARD_gpio_set_value(HP_SMART_CARD_GPIO_HOST_RESET,
                                 HP_SMART_CARD_FALSE);

    /* Wait for a while */
    HP_USLEEP(_HOST_SMART_CARD_RESET_PULSE_USEC);

    /* deassert RESET signal */
    HP_SMART_CARD_gpio_set_value(HP_SMART_CARD_GPIO_HOST_RESET,
                                 HP_SMART_CARD_TRUE);

    /* REVISIT: Thu Nov 19 11:00:38 2009
       Should we wait till HOST SMART CARD is ready here ??
     */

    /* Added 2015-07: On fast PC, found I needed about 10ms delay here, then saw comment above */
    _wait_till_ready(HOST_SMART_CARD_SHORT_TIME_USEC);

    /* notify interested client that host smart card reset */
    if (_cb_fn_p)
    {
        (*_cb_fn_p)();
    }

    HP_mutex_unlock(&chip_mutex);

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Exiting HP_HOST_SMART_CARD_chip_reset()\n");
}


void HP_HOST_SMART_CARD_chip_init(void)
{
    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Entering HP_HOST_SMART_CARD_chip_init()\n");

    /* Create the semaphores */
    HP_mutex_init(&chip_mutex);

    /* Reset host smart card */
    HP_HOST_SMART_CARD_chip_reset();

    /* REVISIT : Echo command to host smart card to verfiy it is present */

    HP_DEBUG_printf(HOST_SMART_CARD_CHIP_DBG_ID,
                    HP_DBG_LEVEL_HSCC_TRACE, 3,
                    "Exiting HP_HOST_SMART_CARD_chip_init()\n");
}
