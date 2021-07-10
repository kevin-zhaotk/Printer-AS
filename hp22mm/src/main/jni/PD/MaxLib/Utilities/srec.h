/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __SREC_H__
#define __SREC_H__

#include "max_common_types.h"

/* 
 * Library to parse the srec file 
 */
typedef enum
{
    SREC_OK_RECORD_HEADER   =  1,
    SREC_OK                 =  0,
    SREC_INVALID_RECORD     = -1,
    SREC_INVALID_CRC        = -2,
} SrecResult_t;


#define MAX_SREC_RECORD_SIZE    128 /* Mac record charater size in bytes */
#define MAX_SREC_DATA_SIZE      64  /* Max srec data size in bytes. */
/*
 * Parse the given record, validate the checksum, extract starting address and data.
 * IMPORTANT NOTE: We are currently supporting only S3 records.
 * 
 * record : NULL terminated input buffer containing one srecord.
 * address : returns the starting address
 * buf : This function will fill this buffer with the data
 * buf_size : Size of the input buffer
 * data_size : Actual size of the data filled in buf.
 */
SrecResult_t srec_parse_line(char *record, uint32_t *address, uint8_t *buf, size_t buf_size, size_t *data_size);

#endif /* __SREC_H__ */

