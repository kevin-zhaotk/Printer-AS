/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _HW_SMART_CARD_PRIVATE_H_
#define _HW_SMART_CARD_PRIVATE_H_


// NOTE: StatusToString array must match this enum.
typedef enum
{
    /* protocol responses */
    /* this first group must match PROTO_RSP_* response codes */
    HW_SMART_CARD_success_e         = 0x00,
    HW_SMART_CARD_not_auth_e        = 0x01,
    HW_SMART_CARD_failed_e          = 0x02,
    HW_SMART_CARD_illegal_access_e  = 0x03,
    HW_SMART_CARD_memory_error_e    = 0x04,
    HW_SMART_CARD_invalid_param_e   = 0x05,
    HW_SMART_CARD_invalid_change_e  = 0x06,
    HW_SMART_CARD_invalid_crc_e     = 0x07,
    /* protocol failure */
    HW_SMART_CARD_proto_failure_e,
    HW_SMART_CARD_rsp_len_too_long_e,
    HW_SMART_CARD_rsp_len_mismatch_e,
    /* transport failed */
    HW_SMART_CARD_xpt_not_set_failure_e,
    HW_SMART_CARD_xpt_cmd_failed_e,
    HW_SMART_CARD_xpt_rsp_failed_e,
    HW_SMART_CARD_xpt_no_device_e,
    /* some auxilary library failed */
    HW_SMART_CARD_aux_failure_e,
    /* mac could not be verified */
    HW_SMART_CARD_mac_verification_failure_e,
    HW_SMART_CARD_high_security_mac_rejected_e,
    /* device id revoked */
    HW_SMART_CARD_device_revoked_id_e,
    /* device id changed unexpectedly */
    HW_SMART_CARD_unexpected_device_change_e,
    /* unknown failure */
    HW_SMART_CARD_unknown_failure_e,

    /* retry - for stats purposes */
    HW_SMART_CARD_retry_e,

    /* null pointer error (to help caller) */
    HW_SMART_CARD_null_pointer_e,
    /* get acumen type returns unknow device type*/
    HW_SMART_CARD_invalid_device_type_e,

    /* not forward compatible */
    HW_SMART_CARD_nfc_key_e,
    /* not compatible */
    HW_SMART_CARD_incompatible_key_e,
} HW_SMART_CARD_status_t;
#endif /* _HW_SMART_CARD_PRIVATE_H_ */
