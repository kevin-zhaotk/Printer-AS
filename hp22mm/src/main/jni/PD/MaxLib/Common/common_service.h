/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __COMMON_SERVICE_H__
#define __COMMON_SERVICE_H__

#define PAIR_CMD_CODE                     0xBC

#define IDS_PAIR_CMD_CODE_STEP1           151
#define IDS_PAIR_CMD_CODE_STEP2           152
#define IDS_PAIR_CMD_CODE_STEP3           153


#define BLUR_PAIR_CMD_CODE_STEP1          151
#define BLUR_PAIR_CMD_CODE_STEP2          152
#define BLUR_PAIR_CMD_CODE_STEP3          153


typedef enum
{
    SERVICE_OK = 0,
    SERVICE_ERROR = -1,
} ServiceResult_t;

typedef enum
{
    SUCCESS_e                    = 0,
    INVALID_SLOT_ID_e            = 1,
    INVALID_PH_ID_e              = 2,
    INVALID_IDS_STATUS_MSG_e     = 3,
    INVALID_BLUR_HANDSHAKE_MSG_e = 4,
    SYSTEM_UNPAIRED_e            = 5,
    HANDSHAKE_REQ_ERROR_e        = 6, 
    USED_INK_UPDATE_FAILED_e     = 7,
    INCOMPLETE_MSG_LENGTH_e      = 8,
    ERROR_READING_DATA_e         = 9,
    SYSTEM_INVALID_STATE_e       = 10,
    ZERO_USAGE_PACKET_e          = 11,
    NEED_HANDSHAKE_MSG_e         = 12,
    SET_OVERRIDE_ERROR_e         = 13,
    IDS_STATUS_REQ_ERROR_e       = 14,
    IDS_OVERRIDE_READ_ERROR_e    = 15,
    IDS_READ_FIELD_ERROR_e       = 16,

}Ink_Use_Status_t;


typedef enum
{
    KEY_NEGOTIATION_SESSION_OK          =0,
    KEYS_CLEARED_OK                     =1, 
    KEY_NEGOTIATION_SESSION_COMPLETE    =2,

    
    
    ERROR_INVALID_PAIRING_CARD_1        =3,
    ERROR_INVALID_PAIRING_CARD_2        =4,
    ERROR_INVALID_PAIRING_CARD_3        =5,
    ERROR_INVALID_PAIRING_CARD_4        =6,
    ERROR_INVALID_PAIRING_CARD_5        =7,
    ERROR_CRITICAL_ERROR_MAX            =8,
    
    ERROR_INVALID_CARD_1                =9,
    ERROR_INVALID_CARD_2                =10,
    ERROR_INVALID_CARD_3                =11,

    ERROR_DEVICE_FAILURE                =12,
    ERROR_MESSAGE_INCOMPLETE            =13,
    ERROR_INCORRECT_MESSAGE             =14,

    ERROR_INCOMPATIBLE_DGTRCARD         =15,

    ERROR_DEVICE_SEEN_BEFORE            =16,
    ERROR_SESSION_TERMINATED            =17,
    ERROR_PACKET_DATA_ERROR             =18,
    ERROR_COMMON_STATUS_MAX             =19,

  /* Firing Sys only errors*/
    ERROR_INVALID_SLOT_ID               =20,
    ERROR_INVALID_PEN_ID                =21,
    ERROR_SLOTS_PAIRED_BEFORE           =22,
    ERROR_SLOT_PAIRING_LIMIT            =23,
    ERROR_PAIRING_SC_MISMATCH           =24,
    ERROR_PEN_SLOTS_MISMATCH            =25,
    ERROR_INK_TREE_MISMATCH             =26,
    ERROR_FIRING_SYSTEM_MAX             =27,

    /*Supply system only errors*/
    SUPPLY_CARD_OK                      =28,
    ERROR_FS_PAIRED_LIMIT_REACHED       =29,
    ERROR_INVALID_FS_ACCESSED           =30,
    ERROR_INVALID_SUPPLY_ACCESSED       =31,
    ERROR_INVALID_PRIMARY_SUPPLY_IDX    =32,
    ERROR_PRIMARY_SUPPLY_IDX_MISMATCH   =33,
    ERROR_SUPPLY_SYSTEM_MAX             =34,

}Key_Pairing_Status_t;


#endif