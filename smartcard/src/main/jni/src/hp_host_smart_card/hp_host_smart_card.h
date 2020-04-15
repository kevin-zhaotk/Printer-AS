/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_HOST_SMART_CARD_H_
#define _HP_HOST_SMART_CARD_H_

#include "hp_host_smart_card_xpt.h"
#include "hp_host_smart_card_chip.h"

extern hp_host_smart_card_xpt_t *hp_host_smart_card_xpt;

void hp_host_smart_card_init(void);
void hp_host_smart_card_shutdown(void);
void hp_host_smart_card_set_xpt(hp_host_smart_card_xpt_t *xpt_p);

#define RETRY_COUNT                     0x03
#define DEFAULT_DELAY                   0x0040

#define MAX_DATA_LENGTH                 0x200
#define MAX_BUFFER_LENGTH               (MAX_DATA_LENGTH + 2)
/* Command not used. H.M.Wang 2020-4-12
#define ADDR_CMD                        0x00
#define ADDR_RSP_HDR                    0x80
#define ADDR_RSP_BODY                   0x82
*/

//COMMAND CODES for Host Smart Card
#define CMD_ECHO                        0x80
#define CMD_GEN_SK                      0x01
#define CMD_GEN_RAND                    0x02
#define CMD_LIST_MKDB                   0x03
#define CMD_GET_ID                      0x04
#define CMD_GET_TRACEABILITY_DATA       0x06
#define CMD_MANAGE_MKS                  0x07
#define CMD_MAC_SETUP                   0x08
#define CMD_MAC_DATA                    0x09
#define CMD_MAC_FINAL                   0x0A

#define CMD_SETUP_SESSION               0x8C
#define CMD_END_SESSION                 0x8D

//FIELD LENGTHS
//session key
#define SESSION_ID_LEN          0x0F
#define SESSION_KEY_LEN         0x10

//root key
#define ROOT_KEY_ID_LEN         0x02

//master key
#define MASTER_ID_LEN           0x03
#define MASTER_KEY_DEPTH_LEN    0x01

//base key
#define BASE_ID_LEN             0x08
#define BASE_DIV_LEN            0x05

//diversifiers
#define BASE_DIV_LEN            0x05
#define SESSION_DIV_LEN         0x03
#define HOST_DIV_LEN            0x04

//RETURN LENGTHS

//mac_check
#define MAC_FINAL_DATA_LEN      0x10
#define MAC_LEN                 0x10
#define MAC_DATA_BLOCK_SIZE     0x10
#define MAC_DATA_MAX_BLOCK      0x04
#define MAC_DATA_MAX_SIZE       0x40

//list mkdb
#define LIST_MKDB_ENTRY_LEN     0x04
#define LIST_MKDB_LEN           0x10

//get_info
#define GET_INFO_LEN            0x0D
#define DEVICE_ID_LEN           0x08
#define KEY_CNT_MIN_LEN         0x03
#define KEY_CNT_WINDOW_LEN      0x01
#define LSTATE_LEN              0x01

//gen_rand
#define RANDOM_LEN              0x04

#define CMAC_LEN                0x08
#endif /* _HP_HOST_SMART_CARD_H_ */
