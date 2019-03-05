/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#ifndef __INC__crypto_types_h_
#define __INC__crypto_types_h_

#include "hp_types.h"

typedef enum
{
    CRYPTO_status_OK,
    CRYPTO_status_ERR,
    CRYPTO_status_ERR_KEYLEN,
    CRYPTO_status_ERR_BLKLEN,
    CRYPTO_status_ERR_OUTLEN,
    CRYPTO_status_ERR_STATE,
    CRYPTO_status_ERR_PADDING,
} CRYPTO_status_t;

/* block cipher - capable of encrypting/decrypting a single block
 * at a time */
typedef struct CRYPTO_blkcipher_st
{
    const struct CRYPTO_blkcipher_vtbl_st *vtbl_p;
} CRYPTO_blkcipher_t;

// for Windows, THIS would cause a warning
#define CRYPTO_THIS    CRYPTO_blkcipher_t * this_p
typedef struct CRYPTO_blkcipher_vtbl_st
{
    CRYPTO_status_t (*set_key)(CRYPTO_THIS, uint8_t *key_p, int key_len);
    int (*get_blk_len)(CRYPTO_THIS);
    CRYPTO_status_t (*encrypt)(CRYPTO_THIS,
                               const uint8_t *in_p, int in_len,
                               uint8_t *out_p, int out_len);
    CRYPTO_status_t (*decrypt)(CRYPTO_THIS,
                               const uint8_t *in_p, int in_len,
                               uint8_t *out_p, int out_len);
} CRYPTO_blkcipher_vtbl_t;
#undef CRYPTO_THIS

/* digest - capable of generating a (cryptographic) digest */
typedef struct CRYPTO_digest_st
{
    const struct CRYPTO_digest_vtbl_st *vtbl_p;
} CRYPTO_digest_t;

// for Windows, THIS would cause a warning
#define CRYPTO_THIS    CRYPTO_digest_t * this_p
typedef struct CRYPTO_digest_vtbl_st
{
    int (*get_digest_len)(CRYPTO_THIS);
    CRYPTO_status_t (*init)(CRYPTO_THIS);
    CRYPTO_status_t (*update)(CRYPTO_THIS, const uint8_t *in_p, int in_len);
    CRYPTO_status_t(*final)(CRYPTO_THIS, uint8_t * out_p, int out_len);
} CRYPTO_digest_vtbl_t;
#undef CRYPTO_THIS
#endif /* __INC__crypto_types_h_ */
