/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <string.h>

#include "frame.h"
#include "service.h"
#include "max_common_types.h"
#include "crc.h"
#include "debug_log.h"

/* TODO: Avoid using this static variable and instead 
 * move this to some handler structure ?
 */
static uint8_t _frame_counter = 1;

//static const char *FRAME_MODULE_NAME = " FRM"; /* Leading space to make module name 4 characters */
//#define FRAME_DEBUG_LOG(level, ...) DEBUG_LOG(FRAME_MODULE_NAME, level, __VA_ARGS__)

static const uint32_t   CRC_SIZE        = 1;
static const uint8_t    SERVICE_CNTRL   = 0x8C;

static const uint32_t   FRAME_CNTRL_INDEX     = 0;
static const uint32_t   FRAME_SIZE_LOW_INDEX  = 1;
static const uint32_t   FRAME_ID_INDEX        = 2;
static const uint32_t   FRAME_SERVICE_INDEX   = 3;
static const uint32_t   FRAME_DATA_INDEX      = 4;

static const uint32_t   RESP_HEADER_START_INDEX = 4;

/* 
 * Initialize the frame structure.
 */
FrameResult_t frame_init(Frame_t *frame, uint8_t service) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(service >= SERVICE_MAX) {
        LOGE("Invalied service!");
        return FRAME_ERROR;
    }
	
    frame->ctrl         = SERVICE_CNTRL | (_frame_counter % 2) << 4; _frame_counter ++;
    frame->id           = 1; 
    frame->service      = service;
    frame->data_size    = 0;
    frame->decode_ptr   = 0;
    frame->crc          = 0;
    
    /* Initialize the data to zero */
    memset(frame->data, 0, MAX_FRAME_DATA_SIZE);

    LOGI("%s success!", __FUNCTION__);

    return FRAME_OK;
}

/*
 * This function serialize the frame to bytes required to send over the bus.
 * It also computes and appends the CRC.
 */
FrameResult_t frame_serialize(Frame_t *frame, uint8_t *buf, 
                                        uint16_t buf_size,
                                        uint16_t *data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(buf == NULL) {
        LOGE("Buffer NULL");
        return FRAME_ERROR;
    }
    if(buf_size <= (MAX_FRAME_HEADER_SIZE + frame->data_size + CRC_SIZE)) {
        LOGE("Not enough buffer size");
        return FRAME_ERROR;
    }
    if(frame->data_size > MAX_FRAME_DATA_SIZE) {
        LOGE("Frame data overflow");
        return FRAME_ERROR;
    }

    buf[FRAME_CNTRL_INDEX]      = frame->ctrl | ((frame->data_size & 0x300) >> 8);
    buf[FRAME_SIZE_LOW_INDEX]   = frame->data_size & 0xFF;
    buf[FRAME_ID_INDEX]         = frame->id;
    buf[FRAME_SERVICE_INDEX]    = frame->service;
    int i;
    for (i = 0; i < frame->data_size; ++i) {
        buf[FRAME_DATA_INDEX + i] = frame->data[i];
    }
    buf[FRAME_DATA_INDEX + frame->data_size] = crc_compute(buf, MAX_FRAME_HEADER_SIZE + frame->data_size);
    
    *data_size = MAX_FRAME_HEADER_SIZE + CRC_SIZE + frame->data_size;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;    
}                                        
                                        
/*
 * Load the response frame with the header
 */                                        
FrameResult_t frame_response_init(Frame_t *frame, uint8_t *buf, uint32_t buf_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(buf == NULL) {
        LOGE("Buffer NULL");
        return FRAME_ERROR;
    }
    if(buf_size < RESP_HEADER_SIZE) {
        LOGE("Buffer size overflow");
        return FRAME_ERROR;
    }

    /* Ignore the RESP_HEADER_START_INDEX number of initlial bytes */
    frame->ctrl         = buf[RESP_HEADER_START_INDEX + FRAME_CNTRL_INDEX];
    frame->size_low     = buf[RESP_HEADER_START_INDEX + FRAME_SIZE_LOW_INDEX];
    frame->id           = buf[RESP_HEADER_START_INDEX + FRAME_ID_INDEX];
    frame->service      = buf[RESP_HEADER_START_INDEX + FRAME_SERVICE_INDEX];
    frame->data_size    = (frame->ctrl & 0x3) << 8 | frame->size_low;
    frame->decode_ptr   = 0;
    frame->crc          = 0;
    
    /* Initialize the data to zero */
    memset(frame->data, 0, MAX_FRAME_DATA_SIZE);

    LOGI("%s success!", __FUNCTION__);

    return FRAME_OK;
}

/*
 * Returns the number of bytes of variable data payload
 */
FrameResult_t frame_response_get_data_size(Frame_t *frame, uint32_t *data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data_size == NULL) {
        LOGE("Data size area null!");
        return FRAME_ERROR;
    }
    if(frame->ctrl == 0) {
        LOGE("Frame not initialized!");
        return FRAME_ERROR;
    }

    *data_size = frame->data_size;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}


/*
 * Deserialize the recieved bytes to Frame structure.
 */                                        
FrameResult_t frame_response_init_data(Frame_t *frame, uint8_t *buf, uint16_t buf_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(buf == NULL) {
        LOGE("Buffer NULL");
        return FRAME_ERROR;
    }
    if(buf_size < frame->data_size) {
        LOGE("Sanity check failed!");
        return FRAME_ERROR;
    }

    int i;
    for(i = 0; i < frame->data_size; ++i) {
        frame->data[i] = buf[i];
    }
    
    frame->crc = buf[frame->data_size];
    
#if (FRAME_CRC_CHECKING == FEATURE_ON)   
    uint8_t crc = 0;
    crc = crc_add(crc, frame->ctrl);
    crc = crc_add(crc, frame->size_low);
    crc = crc_add(crc, frame->id);
    crc = crc_add(crc, frame->service);
    crc = crc_add_buf(crc, buf, frame->data_size);
    
    if(crc != frame->crc) {
        LOGE("frame_desrialize(): CRC error. computed = %d, recieved = %d\n", crc, frame->crc);
        return FRAME_ERROR_CRC;
    }
#endif

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

/* 
 * Helper functions to encode the serivce specific fields to command frame
 */
FrameResult_t frame_encode8(Frame_t *frame, uint8_t data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    
    frame->data[frame->data_size++] = data;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_encode16(Frame_t *frame, uint16_t data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    
    frame->data[frame->data_size++] = (data & 0xFF);
    frame->data[frame->data_size++] = (data >>   8) & 0xFF;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_encode32(Frame_t *frame, uint32_t data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    
    frame->data[frame->data_size++] = (data & 0xFF);
    frame->data[frame->data_size++] = (data >>   8) & 0xFF;
    frame->data[frame->data_size++] = (data >>  16) & 0xFF;
    frame->data[frame->data_size++] = (data >>  24) & 0xFF;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_encode_bytes(Frame_t *frame, uint8_t * data, uint16_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data == NULL) {
        LOGE("Data pointer null!");
        return FRAME_ERROR;
    }

    memcpy((void *)&frame->data[frame->data_size], data, data_size);
    frame->data_size += data_size;

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

/*
 * Helper functions to decode field from response frame
 */
FrameResult_t frame_decode8(Frame_t *frame, uint8_t *data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data == NULL) {
        LOGE("Data pointer null!");
        return FRAME_ERROR;
    }
    
    if(frame->decode_ptr < frame->data_size) {
        *data = frame->data[frame->decode_ptr++];
    } else {
        LOGE("Data overflow!");
        return FRAME_ERROR;
    }

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_decode16(Frame_t *frame, uint16_t *data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data == NULL) {
        LOGE("Data pointer null!");
        return FRAME_ERROR;
    }
    
    if(frame->decode_ptr + 1 < frame->data_size) {
        *data  = frame->data[frame->decode_ptr++];
        *data |= frame->data[frame->decode_ptr++] << 8; 	
    } else {
        LOGE("Data overflow!");
        return FRAME_ERROR;
    }

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_decode32(Frame_t *frame, uint32_t *data) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data == NULL) {
        LOGE("Data pointer null!");
        return FRAME_ERROR;
    }
    
    if((frame->decode_ptr + 3) < frame->data_size) {
        *data  = frame->data[frame->decode_ptr++];
        *data |= frame->data[frame->decode_ptr++] <<  8; 
        *data |= frame->data[frame->decode_ptr++] << 16; 
        *data |= frame->data[frame->decode_ptr++] << 24; 
    } else {
        LOGE("Data overflow!");
        return FRAME_ERROR;
    }

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

FrameResult_t frame_decode_bytes(Frame_t *frame, uint8_t *data, uint16_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(frame == NULL) {
        LOGE("Frame pointer null!");
        return FRAME_ERROR;
    }
    if(data == NULL) {
        LOGE("Data pointer null!");
        return FRAME_ERROR;
    }
    
    if((frame->decode_ptr + data_size -1) < frame->data_size) {
        memcpy(data, &frame->data[frame->decode_ptr], data_size);
        frame->decode_ptr += data_size;
    } else {
        LOGE("Data overflow!");
        return FRAME_ERROR;
    }

    LOGI("%s done!", __FUNCTION__);

    return FRAME_OK;
}

