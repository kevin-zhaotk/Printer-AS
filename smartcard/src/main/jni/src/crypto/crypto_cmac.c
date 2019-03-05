/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2009-2015 HP Development Company, L.P.
   Made in U.S.A.
 */

#include <string.h>

#include "hp_generic_macros.h"
#include "hp_assert.h"
#include "hp_debug_log_internal.h"

#include "lib_crypto_cmac.h"

#define BLKSIZE     32

#define THIS        CRYPTO_digest_t * digest_p
static CRYPTO_status_t _init(THIS)
{
    CRYPTO_cmac_t *this_p = IMPL(CRYPTO_cmac_t, digest, digest_p);
    this_p->id_len = 0;
    memset(this_p->mac, 0, sizeof(this_p->mac));
    return CRYPTO_status_OK;
}

static int _blk_len(CRYPTO_cmac_t *this_p)
{
    return IFC_CALL(this_p->blkcipher_p, get_blk_len)(this_p->blkcipher_p);
}
static void _process_blk(CRYPTO_cmac_t *this_p,
                         const uint8_t *blk_p)
{
    CRYPTO_status_t status;
    int             i;
    int             blk_len = _blk_len(this_p);

    uint32_t        blk[BLKSIZE / 4];
    memcpy(blk, blk_p, blk_len);
    for (i = 0; i < blk_len / 4; i++)
    {
        blk[i] ^= this_p->mac[i];
    }

    status = IFC_CALL(this_p->blkcipher_p, encrypt)(this_p->blkcipher_p,
                                                    (uint8_t *) blk, blk_len,
                                                    (uint8_t *) this_p->mac,
                                                    blk_len);

    HP_ASSERT(status == CRYPTO_status_OK);
}

CRYPTO_status_t _update(THIS,
                        const uint8_t *in_p,
                        int in_len)
{
    CRYPTO_cmac_t   *this_p = IMPL(CRYPTO_cmac_t, digest, digest_p);
    int             blk_len = _blk_len(this_p);

    if (this_p->id_len < 0)
        return CRYPTO_status_ERR_STATE;

    if (this_p->id_len > 0)
    {
        int len = MIN(blk_len - this_p->id_len, in_len);
        if (len > 0)
        {
            memcpy(((uint8_t *) this_p->id) + this_p->id_len, in_p, len);
            this_p->id_len  += len;
            in_len          -= len;
            in_p            += len;
        }

        if (this_p->id_len == blk_len && in_len > 0)
        {
            _process_blk(this_p, (uint8_t *) this_p->id);
            this_p->id_len = 0;
        }
    }

    while (in_len > blk_len)
    {
        _process_blk(this_p, in_p);
        in_p    += blk_len;
        in_len  -= blk_len;
    }

    if (in_len > 0)
    {
        HP_ASSERT(this_p->id_len == 0);
        memcpy(this_p->id, in_p, in_len);
        this_p->id_len = in_len;
    }

    return(CRYPTO_status_OK);
}

static void leftShift(unsigned char *dataStream, int length)
{
    int             i;
    unsigned char   temp;

    if (length < 16)
        return;

    dataStream[0] <<= 1;                                                                                    //Left Shift MSB by 1
    for (i = 1; i < (length); i++)
    {
        temp                = dataStream[i];
        dataStream[i]       <<= 1;
        temp                >>= 7;
        dataStream[i - 1]   |= temp;
    }
}

static void _shift(uint32_t *data_p, int data_len)
{
    if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
    {
        leftShift((unsigned char *) data_p, (data_len * 4));
    }
    else
    {
        int         i;
        uint32_t    carry = 0;
        for (i = data_len - 1; i >= 0; i--)
        {
            uint32_t d = data_p[i];
            data_p[i]   = d << 1 | carry;
            carry       = d >> 31;
        }
    }
}

CRYPTO_status_t _final(THIS,
                       uint8_t *out_p,
                       int out_len)
{
    CRYPTO_cmac_t   *this_p = IMPL(CRYPTO_cmac_t, digest, digest_p);
    int             blk_len = _blk_len(this_p);

    int             pad = FALSE, i;
    uint32_t        msb;
    uint32_t        zero[BLKSIZE / 4];
    uint32_t        k[BLKSIZE / 4];

    CRYPTO_status_t status;

    if (this_p->id_len < 0)
        return CRYPTO_status_ERR_STATE;
    if (!out_p || out_len < 0 || out_len > blk_len)
        return CRYPTO_status_ERR_OUTLEN;

    /* pad if needed */
    if (this_p->id_len < blk_len)
    {
        int i   = this_p->id_len;
        int len = blk_len - (i + 1);
        ((uint8_t *) this_p->id)[i] = 0x80;
        if (len > 0)
            memset(((uint8_t *) this_p->id) + i + 1, 0, len);
        pad = TRUE;
    }

    this_p->id_len = -1;

    memset(zero, 0, blk_len);
    status = IFC_CALL(this_p->blkcipher_p, encrypt)(this_p->blkcipher_p,
                                                    (uint8_t *) zero, blk_len,
                                                    (uint8_t *) k, blk_len);
    if (status != CRYPTO_status_OK)
        return status;

    if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
    {
        msb = k[0] & 0x00000080;
    }
    else
    {
        msb = k[0] & 0x80000000;
    }

    _shift(k, blk_len / 4);

    if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
    {
        if (msb)
            k[blk_len / 4 - 1] ^= 0x87000000;
    }
    else
    {
        if (msb)
            k[blk_len / 4 - 1] ^= 0x87;
    }

    if (pad)
    {
        if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
        {
            msb = k[0] & 0x00000080;
        }
        else
        {
            msb = k[0] & 0x80000000;
        }

        _shift(k, blk_len / 4);

        if (HP_SMART_CARD_TRUE == IS_LITTLE_ENDIAN())
        {
            if (msb)
                k[blk_len / 4 - 1] ^= 0x87000000;
        }
        else
        {
            if (msb)
                k[blk_len / 4 - 1] ^= 0x87;
        }
    }

    for (i = 0; i < blk_len / 4; i++)
        this_p->id[i] ^= k[i];

    _process_blk(this_p, (uint8_t *) this_p->id);

    memcpy(out_p, this_p->mac, out_len);
    return CRYPTO_status_OK;
}

static int _get_digest_len(THIS)
{
    CRYPTO_cmac_t *this_p = IMPL(CRYPTO_cmac_t, digest, digest_p);
    return _blk_len(this_p);
}

static const CRYPTO_digest_vtbl_t _cmac_digest_vtbl = {
    _get_digest_len,
    _init,
    _update,
    _final
};

CRYPTO_digest_t *CRYPTO_cmac_init(CRYPTO_cmac_t *this_p,
                                  CRYPTO_blkcipher_t *blkcipher_p)
{
    this_p->digest.vtbl_p   = &_cmac_digest_vtbl;
    this_p->blkcipher_p     = blkcipher_p;
    this_p->id_len          = -1;
    return &this_p->digest;
}
