/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef __INC__lib_crypto_aes_h_
#define __INC__lib_crypto_aes_h_

#include "crypto_types.h"

typedef enum
{
    CRYPTO_aes_not_initialized_e,
    CRYPTO_aes_encrypt_e,
    CRYPTO_aes_decrypt_e,
} CRYPTO_aes_mode_t;

typedef struct CRYPTO_aes_st
{
    CRYPTO_blkcipher_t  blkcipher;

    /* mode */
    CRYPTO_aes_mode_t   mode;
    /* key data and length (in bytes) */
    uint8_t             key[32];
    int                 key_len;
    /* round keys (cached) - optional; if used must be big enough
     * to hold the (Nr+1)*4 where Nr is 10,12 and 14 for
     * 128, 192 and 256 bit keys */
    uint32_t    *rk_p;
    int         rk_max;
} CRYPTO_aes_t;

extern CRYPTO_blkcipher_t *CRYPTO_aes_init(CRYPTO_aes_t *aes_p,
                                           uint32_t *rk_p, int rk_max);
#endif /* __INC__lib_crypto_aes_h_ */
