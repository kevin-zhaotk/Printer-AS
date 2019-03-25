/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HP_HOST_SMART_CARD_IFC_H_
#define _HP_HOST_SMART_CARD_IFC_H_

#include "carnac_types.h"
#include "hp_types.h"

typedef enum
{
    HP_HOST_SMART_CARD_success_e,
    HP_HOST_SMART_CARD_failed_e,
    HP_HOST_SMART_CARD_not_auth_e,
    HP_HOST_SMART_CARD_invalid_param_e,
    HP_HOST_SMART_CARD_invalid_lstate,
    HP_HOST_SMART_CARD_invalid_rk_id_e,
    HP_HOST_SMART_CARD_invalid_key_type_e,
    HP_HOST_SMART_CARD_invalid_mk_depth_e,
    HP_HOST_SMART_CARD_invalid_admin_key_cnt_e,
    HP_HOST_SMART_CARD_session_not_auth_e,
    HP_HOST_SMART_CARD_invalid_pad_e,
    HP_HOST_SMART_CARD_internal_error_e,
    HP_HOST_SMART_CARD_no_bk_e,
    HP_HOST_SMART_CARD_insufficient_nvm_e,
    HP_HOST_SMART_CARD_invalid_crypto_state_e,
    HP_HOST_SMART_CARD_invalid_digest_e,
    HP_HOST_SMART_CARD_invalid_key_access_e,
    HP_HOST_SMART_CARD_invalid_crc_e,
    HP_HOST_SMART_CARD_cmd_not_enabled_e,
    HP_HOST_SMART_CARD_invalid_device_id_e,
    HP_HOST_SMART_CARD_invalid_signature_e,
    HP_HOST_SMART_CARD_xpt_error_e,
    HP_HOST_SMART_CARD_crypto_error_e,
    HP_HOST_SMART_CARD_ok_e = HP_HOST_SMART_CARD_success_e,
} HP_HOST_SMART_CARD_return_t;


/* Smart interface functions */

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_echo(uint8_t* input_data,
                                                    int input_length);

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_gen_sk(CARNAC_sk_id_t* sk_id,
                                                      CARNAC_sk_t* sk);

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_gen_rand(uint32_t* rand_num);

HP_HOST_SMART_CARD_return_t HP_HOST_SMART_CARD_list_mkdb(CARNAC_mk_entry_t* db_entries,
                                                         int* entry_count);
#endif /* _HP_HOST_SMART_CARD_IFC_H_ */
