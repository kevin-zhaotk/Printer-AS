/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __FRAME_H__
#define __FRAME_H__


#include "max_common_types.h"

/* Max Frame data size */
//#define PROTOCOL_DATA_SIZE		64
//#define MAX_FRAME_DATA_SIZE     64
#define PROTOCOL_DATA_SIZE	507//	268
#define MAX_FRAME_DATA_SIZE     507//268
#define MAX_FRAME_HEADER_SIZE   4
#define MAX_FRAME_SIZE          (MAX_FRAME_DATA_SIZE + MAX_FRAME_HEADER_SIZE)

#define RESP_HEADER_SIZE        8
#define RESP_HEADER_ERROR_BITMASK	0x20	

typedef struct
{
    uint8_t     ctrl;
    uint8_t     size_low;
    uint8_t     id;
    uint8_t     service;
    uint8_t     data[MAX_FRAME_DATA_SIZE];
    uint16_t    data_size;
    uint16_t    decode_ptr;
    uint8_t     crc;
} Frame_t;

/* Frame error codes */
typedef enum
{
    FRAME_OK                    =  0,
    FRAME_ERROR                 = -1,
    FRAME_ERROR_CRC             = -2,
    FRMAE_ERROR_DATA_TOO_BIG    = -3,
    FRAME_ERROR_INVALID_FRAME   = -4,
} FrameResult_t;

/* 
 * Initialize the frame structure.
 */
FrameResult_t frame_init(Frame_t *frame, uint8_t service);

/*
 * Serialize the frame to bytes required to send over the bus.
 * 
 */
FrameResult_t frame_serialize(Frame_t *frame, uint8_t *data, 
                                        uint16_t buffer_size,
                                        uint16_t *data_size);
                                        
/*
 * Load the response frame with the header
 */                                        
FrameResult_t frame_response_init(Frame_t *frame, uint8_t *buf, uint32_t buf_size);

/*
 * Returns the number of bytes of variable data payload
 */
FrameResult_t frame_response_get_data_size(Frame_t *frame, uint32_t *data_size);
                                        
/*
 * Deserialize the recieved bytes to Frame structure.
 */                                        
FrameResult_t frame_response_init_data(Frame_t *frame, uint8_t *data, 
                                        uint16_t data_size);

/* 
 * Helper functions to encode the service specific fields to command frame
 */
FrameResult_t frame_encode8(Frame_t *frame, uint8_t data);
FrameResult_t frame_encode16(Frame_t *frame, uint16_t data);
FrameResult_t frame_encode32(Frame_t *frame, uint32_t data);
FrameResult_t frame_encode_bytes(Frame_t *frame, uint8_t * data, uint16_t data_size);

/*
 * Helper functions to decode field from response frame
 */
FrameResult_t frame_decode8(Frame_t *frame, uint8_t *data);
FrameResult_t frame_decode16(Frame_t *frame, uint16_t *data);
FrameResult_t frame_decode32(Frame_t *frame, uint32_t *data);
FrameResult_t frame_decode_bytes(Frame_t *frame, uint8_t *data, uint16_t data_size);

#endif /* __FRAME_H__ */



