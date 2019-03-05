/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef __INC__lib_crypto_cmac_h_
#define __INC__lib_crypto_cmac_h_

#include "crypto_types.h"

typedef struct CRYPTO_cmac_st
{
    CRYPTO_digest_t     digest;

    /* block cipher used */
    CRYPTO_blkcipher_t  *blkcipher_p;

    /* can handle up to 256-bit block ciphers */
    uint32_t            mac[32 / 4];
    uint32_t            id[32 / 4];
    int                 id_len;
} CRYPTO_cmac_t;

extern CRYPTO_digest_t *CRYPTO_cmac_init(CRYPTO_cmac_t *cmac_p,
                                         CRYPTO_blkcipher_t *blkcipher_p);
#endif /* __INC__lib_crypto_cmac_h_ */
