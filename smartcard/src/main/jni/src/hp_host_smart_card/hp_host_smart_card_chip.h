/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

/****************************************************************
 * hp_host_smart_card_bus.h
 ****************************************************************
 *
 * Description:
 *
 * Hardware driver to implement behavior of overall host smart card chip.
 * Implements the command-response protocol.
 *
 */

#ifndef _HP_HOST_SMART_CARD_BUS_H_
#define _HP_HOST_SMART_CARD_BUS_H_

#include "hp_types.h"
#include "../drivers/internal_ifc/hp_smart_card_i2c_ifc.h"

#define HOST_SMART_CARD_MAX_BUSY_MSEC           (100)

#define HOST_SMART_CARD_ADDR_CMD                0x00
#define HOST_SMART_CARD_ADDR_RSP_HDR            0x80
#define HOST_SMART_CARD_ADDR_RSP_BODY           0x82
//#define HOST_SMART_CARD_RSP_HDR_SZ    2
#define HOST_SMART_CARD_RSP_HDR_SZ              3
#define HOST_SMART_CARD_CRC_SZ                  2

// Guesses on sizes
#define HOST_SMART_CARD_MAX_PAYLOAD_LEN         0x200
#define HOST_SMART_CARD_MAX_CMD_LEN             (HOST_SMART_CARD_MAX_PAYLOAD_LEN + 2)

// Spec is 8mS till ready, 1 uS reset pulse width. Added some margin.
#define _HOST_SMART_CARD_RESET_TIME_USEC        (9000)
#define _HOST_SMART_CARD_RESET_PULSE_USEC       (10)

typedef enum
{
    RSP_SUCCESS,
    RSP_FAILED,
    RSP_NOT_AUTH,
    RSP_INVALID_PARAM,
    RSP_INVALID_LSTATE,
    RSP_INVALID_RK_ID,
    RSP_INVALID_KEY_TYPE,
    RSP_INVALID_MK_DEPTH,
    RSP_INVALID_ADMIN_KEY_CNT,
    RSP_SESSION_NOT_AUTH,
    RSP_INVALID_PAD,
    RSP_INTERNAL_ERROR,
    RSP_NO_BK,
    RSP_INSUFFICIENT_NVM,
    RSP_INVALID_CRYPTO_STATE,
    RSP_INVALID_DIGEST,
    RSP_INVALID_KEY_ACCESS,
    RSP_INVALID_CRC,
    RSP_CMD_NOT_ENABLED,
    RSP_INVALID_DEVICE_ID,
    RSP_INVALID_SIGNATURE,

    RSP_PROTOCOL_ERROR      = 0x81,
    RSP_INVALID_SEQUENCE    = 0x82,
} HP_HOST_SMART_CARD_rsp_t;

/* REVISIT: Ram - Thu Nov 19 08:24:27 2009
   Do we need this enum definition ?? */
typedef enum
{
    smart_dev_host_smart_card_v2_e,
    smart_dev_host_smart_card_v1_e,
    smart_dev_default_e,
} smart_dev_t;

typedef struct
{
    uint8_t                     *cmd_p;                                                                 // Pointer to command packet to send
    uint32_t                    cmd_len;                                                                // Length of command packet to send
    uint32_t                    delay_till_rsp_usec;                                                    // Time to delay between command and
    // expected response.
    HP_HOST_SMART_CARD_rsp_t    *rsp_p;                                                                 // Pointer to response value
    // If NULL, the response header will be
    // stored in body_p, and the max body
    // is reduced by the header length.
    uint8_t     *body_p;                                // Pointer to body of the response
    uint32_t    max_body_len;                           // Length of the body packet
    int         *body_len;                              // Actual length of the packet body
    smart_dev_t smart_dev;                              // Which smart device we are using (smart_dev_host_smart_card_v2_e vs. smart_dev_host_smart_card_v1_e)
} host_smart_card_cmd_rsp_t;


#define HP_HOST_SMART_CARD_chip_cmd_rsp(p)    HP_HOST_SMART_CARD_chip__cmd_rsp(p, 1)

HP_SMART_CARD_i2c_result_t HP_HOST_SMART_CARD_chip__cmd_rsp(host_smart_card_cmd_rsp_t *cmd_rsp_p,
                                                            int tries);
void HP_HOST_SMART_CARD_chip_reset(void);

void HP_HOST_SMART_CARD_chip_init(void);
#endif /* _HP_HOST_SMART_CARD_BUS_H_ */
