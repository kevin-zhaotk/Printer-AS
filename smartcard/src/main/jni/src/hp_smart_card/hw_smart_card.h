/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HW_SMART_CARD_H_
#define _HW_SMART_CARD_H_

#include "hw_smart_card_xpt.h"
#include "carnac_types.h"
#include "lib_crypto_aes.h"
#include "lib_crypto_cmac.h"
#include "hw_smart_card_private.h"
#include "hp_smart_card_config.h"

#define HW_SMART_CARD_NUM_DEVICES       HP_SMART_CARD_DEVICE_NUM_DEVICES
#define HW_SMART_CARD_MAX_RETRIES       3

#define HW_SMART_CARD_MAX_MKDB_LEN      0x10

#define HW_SMART_CARD_NUM_BKIDS         6

/* the following function is provided for simulation; it allows
 * another module to provide the transport layer for a smart card
 * instance */
void hw_smart_card_set_xpt(int instance_id, hw_smart_card_xpt_t *xpt_p);

void hw_smart_card_init(void);
void hw_smart_card_multikey_init(void);
void hw_smart_card_xpt_i2c_init(void);

typedef struct hw_device_bkdb_entry_st
{
    CARNAC_bk_id_t  bk_id;
    uint8_t         sk_deriv;
} HW_SMART_CARD_bkdb_entry_t;

typedef struct hw_smart_card_device_st
{
    hw_smart_card_xpt_t *xpt_p;
    struct
    {
        CRYPTO_aes_t    aes;
        CRYPTO_cmac_t   cmac;
    }                           sk;
    HP_SMART_CARD_bool_t        sk_valid;
    int8_t                      level;
    uint8_t                     mac_countdown;
    uint16_t                    mac_div;
    CARNAC_sk_id_t              sk_id;
    HP_SMART_CARD_bool_t        compare_ids;
    int                         idx;        // just for hw_smart_card_stats
    HW_SMART_CARD_bkdb_entry_t  bkdb[HW_SMART_CARD_NUM_BKIDS];
} HW_SMART_CARD_device_t;

extern HW_SMART_CARD_device_t hw_smart_card_device[HW_SMART_CARD_NUM_DEVICES];

CARNAC_mk_id_t
HW_SMART_CARD_find_host_device_mkid_match(CARNAC_mk_entry_t *host_mkids,
                                          int host_keyID_ct,
                                          HW_SMART_CARD_bkdb_entry_t *device_bkids,
                                          int device_keyID_ct);

HW_SMART_CARD_status_t
HW_SMART_CARD_interrogate_acumen(HW_SMART_CARD_device_t *device_p);

HW_SMART_CARD_status_t
HW_SMART_CARD_raw_read(HW_SMART_CARD_device_t *device_p,
                       uint32_t addr, int len, uint8_t *buf_p, int *bytes_read);

HW_SMART_CARD_status_t
HW_SMART_CARD_raw_write(HW_SMART_CARD_device_t *device_p,
                        uint32_t addr, int len, uint8_t *buf_p, int *bytes_written);

HW_SMART_CARD_status_t
HW_SMART_CARD_echo(HW_SMART_CARD_device_t *device_p, uint8_t *data_p, int data_len);

HW_SMART_CARD_status_t
HW_SMART_CARD_change_partition(HW_SMART_CARD_device_t *device_p, int id, int access_mode);

HW_SMART_CARD_status_t
HW_SMART_CARD_list_bkdb(HW_SMART_CARD_device_t *device_p);

#define CMAC    &device_p->sk.cmac.digest
#define AES     &device_p->sk.aes.blkcipher

#define CMAC_CALL(m)    IFC_CALL(CMAC, m)
#define AES_CALL(m)     IFC_CALL(AES, m)

// Attribute Sizes
#define _COUNTDOWN_MASK    0xf
#endif /* _HW_SMART_CARD_H_ */
