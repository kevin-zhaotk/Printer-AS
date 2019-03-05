/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef _CARNAC_TYPES_H_
#define _CARNAC_TYPES_H_

/* IMPORTANT Note: We want to pack all the structures defined in this file. */
#ifdef _MSC_VER
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #pragma pack(push)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #pragma pack(1)

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #define PACKED
#else

#ifdef __GNUC__
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #define PACKED    __attribute__((__packed__))
#else
                                                                                                                                                                                                                                                                #error "All the structures in this file need to be packed"
#endif
#endif /* _MSC_VER */


#define CARNAC_RK_ID_LEN        2
#define CARNAC_MK_DEPTH_LEN     1
#define CARNAC_BASE_DIV_LEN     5

typedef struct CARNAC_mk_id_st
{
    uint8_t rk_id[2];
    uint8_t mk_depth[1];
} PACKED CARNAC_mk_id_t;

typedef struct CARNAC_mk_entry_st
{
    CARNAC_mk_id_t  mk_id;
    uint8_t         max_depth;
} PACKED CARNAC_mk_entry_t;

typedef struct CARNAC_bk_id_st
{
    CARNAC_mk_id_t  mk_id;
    uint8_t         base_div[5];
} PACKED CARNAC_bk_id_t;

typedef struct CARNAC_sk_id_st
{
    CARNAC_bk_id_t  bk_id;
    uint8_t         host_div[4];
    uint8_t         session_div[3];
} PACKED CARNAC_sk_id_t;

typedef struct CARNAC_sk_st
{
    uint8_t key[16];
} PACKED CARNAC_sk_t;

typedef struct CARNAC_host_smart_card_info_st
{
    uint8_t device_id[8];
    uint8_t admin_key_cnt_min[3];
    uint8_t admin_key_cnt_window[1];
    uint8_t lstate[1];
} PACKED CARNAC_host_smart_card_info_t;

#ifdef _MSC_VER
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                #pragma pack(pop)
#endif
#endif /* _CARNAC_TYPES_H_ */
