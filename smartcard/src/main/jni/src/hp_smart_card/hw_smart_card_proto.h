/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HW_SMART_CARD_PROTO_H_
#define _HW_SMART_CARD_PROTO_H_

#include "carnac_types.h"
#include "hw_smart_card_private.h"

/* cmd/rsp addresses */
#define PROTO_CMD_ADDR              0x00
#define PROTO_RSP_HDR_ADDR          0x80
#define PROTO_RSP_BODY_ADDR         0x82

/* packet length & structure definition */
#define PROTO_BUFFER_LEN            254
#define PROTO_LEN_HDR               2
#define PROTO_LEN_MAC               8
#define PROTO_LEN_MAC_SECURE        8
#define PROTO_LEN_MACDIV            2
#define PROTO_MAX_LEN_BODY          150
#define PROTO_MAX_LEN_PKT           4 + PROTO_MAX_LEN_BODY + PROTO_LEN_MAC + PROTO_LEN_MAC_SECURE

#define PROTO_OFFS_CMD              0
#define PROTO_OFFS_RSP              0
#define PROTO_OFFS_LEN              1
#define PROTO_OFFS_HDR              0
#define PROTO_OFFS_BODY             2

/* command codes */
#define PROTO_CMD_ECHO              0x20
#define PROTO_CMD_START_SESSION     0x00
#define PROTO_CMD_LIST_BKDB         0x44
#define PROTO_CMD_READ              0xC1
#define PROTO_CMD_WRITE             0xC2
#define PROTO_CMD_SEND_ZEROS        0x03
#define PROTO_CMD_GET_TRACE_DATA    0x05
#define PROTO_CMD_CHANGE_PART       0xC7

/* response codes */
#define PROTO_RSP_SUCCESS           0x00
#define PROTO_RSP_NOT_AUTH          0x01
#define PROTO_RSP_FAILED            0x02
#define PROTO_RSP_ILLEGAL_ACCESS    0x03
#define PROTO_RSP_MEMORY_ERROR      0x04
#define PROTO_RSP_INVALID_PARAM     0x05
#define PROTO_RSP_INVALID_CHANGE    0x06
#define PROTO_RSP_INVALID_CRC       0x07

/* miscellaneous constants */
// C_SESSION_NORMAL
#define PROTO_SECURITY_NORMAL               0x00
// C_SESSION_HIGH
#define PROTO_SECURITY_HIGH                 0x01
#define PROTO_C_SESSION_QUERY               0x0f
#define PROTO_SESSION_SECURITY_UNKNOWN      (-1)

/* start_session */
#define PROTO_OFFS_BKID                     0
#define PROTO_LEN_BKID                      8
#define PROTO_OFFS_HOSTDIV                  (PROTO_OFFS_BKID + PROTO_LEN_BKID)
#define PROTO_LEN_HOSTDIV                   4
#define PROTO_OFFS_SESSIONDIV               (PROTO_OFFS_HOSTDIV + PROTO_LEN_HOSTDIV)
#define PROTO_LEN_SESSIONDIV                3
#define PROTO_LEN_SKID                      (PROTO_LEN_BKID + PROTO_LEN_HOSTDIV + PROTO_LEN_SESSIONDIV)
#define PROTO_LEN_RKID                      2
#define PROTO_LEN_MK_DEPTH                  1
#define PROTO_LEN_MKID                      (PROTO_LEN_RKID + PROTO_LEN_MK_DEPTH)
#define PROTO_LEN_LEVEL                     1
#define PROTO_LEN_CRC                       2

/* read */
#define PROTO_OFFS_READADDR                 0
#define PROTO_LEN_READADDR                  3
#define PROTO_OFFS_READLEN                  (PROTO_OFFS_READADDR + PROTO_LEN_READADDR)
#define PROTO_LEN_READLEN                   1
#define PROTO_OFFS_READ_MACDIV              (PROTO_OFFS_READLEN + PROTO_LEN_READLEN)
#define PROTO_OFFS_READ_MAC                 PROTO_OFFS_READ_MACDIV
#define PROTO_LEN_READ_CMD_BODY             (PROTO_LEN_READADDR + PROTO_LEN_READLEN + PROTO_LEN_MAC)

/* write */
#define PROTO_OFFS_WRITEADDR                0
#define PROTO_LEN_WRITEADDR                 3
#define PROTO_OFFS_WRITELEN                 (PROTO_OFFS_WRITEADDR + PROTO_LEN_WRITEADDR)
#define PROTO_LEN_WRITELEN                  1
#define PROTO_OFFS_WRITEDATA                (PROTO_OFFS_WRITELEN + PROTO_LEN_WRITELEN)

/* change part */
#define PROTO_OFFS_ID                       0
#define PROTO_LEN_ID                        1
#define PROTO_OFFS_MODE                     (PROTO_OFFS_ID + PROTO_LEN_ID)
#define PROTO_LEN_MODE                      1
#define PROTO_OFFS_CHANGE_PART_MAC          (PROTO_OFFS_MODE + PROTO_LEN_MODE)
#define PROTO_LEN_CHANGE_PART_CMD_BODY      (PROTO_LEN_ID + PROTO_LEN_MODE + PROTO_LEN_MAC)

/* list_bkdb */
#define PROTO_OFFS_LIST_BKDB_MAC            0
#define PROTO_LEN_LIST_BKDB_CMD_BODY        2

HW_SMART_CARD_status_t
hw_smart_card_proto_start_session_with_mkid(HW_SMART_CARD_device_t *device_p,
                                            uint32_t host_div,
                                            CARNAC_mk_id_t *mk_id_p,
                                            CARNAC_sk_id_t *sk_id_p,
                                            int action);

HW_SMART_CARD_status_t
hw_smart_card_proto_read(HW_SMART_CARD_device_t *device_p, uint32_t addr,
                         int size, uint8_t *result_p);

HW_SMART_CARD_status_t
hw_smart_card_proto_write(HW_SMART_CARD_device_t *device_p, uint32_t addr,
                          int size, uint8_t *result_p);

HW_SMART_CARD_status_t
hw_smart_card_proto_echo(HW_SMART_CARD_device_t *device_p,
                         uint8_t *data_p, int data_len);

HW_SMART_CARD_status_t
hw_smart_card_proto_change_part(HW_SMART_CARD_device_t *device_p,
                                uint8_t id, uint8_t access_mode);

HW_SMART_CARD_status_t
hw_smart_card_proto_list_bkdb(HW_SMART_CARD_device_t *device_p);

void hw_smart_card_proto_init(void);
#endif /* _HW_SMART_CARD_PROTO_H_ */
