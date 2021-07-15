/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include "srec.h"
#include "max_common_types.h"
#include "debug_log.h"

typedef enum
{
    SREC_FILE_NOT_INITALIZED,
    SREC_FILE_INITIALIZED,
    SREC_FILE_ERROR,
}SREC_FILE_STATE_t;

typedef struct
{
    void                *file_handle;
    uint32_t            start_address;
    uint32_t            end_address;
    size_t              total_size;
    SREC_FILE_STATE_t   file_state;
} SREC_FILE_t;

/* Helper fucntion to convert the ASCII character code to integer */
static uint8_t getDigit(char a)
{
    uint8_t value = 0;
    if((a >= '0') && (a <= '9'))
            value = a - '0';
    if((a >= 'A') && (a <= 'F'))
            value = a - 'A' + 10;
    if((a >= 'a') && (a <= 'f'))
            value = a - 'a' + 10;

    return value;
}

/* Helper function to extract two ASCII characters from the given pointer and
 * convert to interger type.
 */
static uint8_t getValue(char *a)
{
    uint8_t value;

    value = getDigit(a[0]) * 16 + getDigit(a[1]);

    return value;
}


SrecResult_t srec_parse_line(   char        *record, 
                                uint32_t    *address, 
                                uint8_t     *buf, 
                                size_t      buf_size, 
                                size_t      *data_size 
                            )
{
    if(NULL == record) {
        LOGE("record NULL");
        return SREC_INVALID_RECORD;
    }
    if(NULL == address) {
        LOGE("address NULL");
        return SREC_INVALID_RECORD;
    }
    if(NULL == buf) {
        LOGE("buf NULL");
        return SREC_INVALID_RECORD;
    }
    if(NULL == data_size) {
        LOGE("data_size NULL");
        return SREC_INVALID_RECORD;
    }
    if(buf_size <= 0) {
        LOGE("buf_size 0");
        return SREC_INVALID_RECORD;
    }

    /* If the first character is anything other than "S", 
     * it is considered invalid.
     */
    if(record[0] != 'S') return SREC_INVALID_RECORD;
    if(record[1] == '0') return SREC_OK_RECORD_HEADER;
    if(record[1] == '7') return SREC_OK_RECORD_HEADER;
    if(record[1] == '5') return SREC_OK_RECORD_HEADER;
    
    /* We only support S3 records now */
    if(record[1] != '3') {
        return SREC_INVALID_RECORD;
    }
    
    
    uint8_t count   = getValue(&record[2]); /* Get the total size of the record */
    uint8_t chksum  = count;                
    char *q         = &record[4];           /* Starting address of record data */
    
    /* Extract the address */
    *address = getValue(q+6) +getValue(q+4) * 256 +  getValue(q+2) * 256 * 256 + getValue(q) * 256 * 256 * 256;
    chksum += getValue(q);
    chksum += getValue(q+2);
    chksum += getValue(q+4);
    chksum += getValue(q+6);
    q += 8;
    
    /* Subtract 4 address and 1 checksum from the data count */
    count -= 5;
    
    /* Make sure input buffer is large enough to hold data */
    if(buf_size < count) {
        LOGE("buf_size < count");
        return SREC_INVALID_CRC;
    }
    
    /* Copy the data to caller's buffer */
    uint32_t i = 0;
    for(i = 0; i < count; i++)
    {
        buf[i] = getValue(q);
        chksum += getValue(q);
        q += 2;
    }
    
    /* Validate checksum */
    if((chksum + getValue(q)) != 0xFF) return SREC_INVALID_CRC;
    
    *data_size = count;
    
    return SREC_OK;
}
