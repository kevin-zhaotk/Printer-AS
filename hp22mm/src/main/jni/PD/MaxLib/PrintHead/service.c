/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <string.h>
#include "ids_service.h"
#include "max_common_types.h"
#include "debug_log.h"
#include "service.h"
#include "frame.h"
#include "uart_ifc.h"

//static const char *SRVC_MODULE_NAME = "SRVC";
//#define SRVC_DEBUG_LOG(level, ...) DEBUG_LOG(SRVC_MODULE_NAME, level, __VA_ARGS__)

#if (BUILD == DEBUG_BUILD)
    /* If it is a debug build allow longer time out. Should it be "-1" ?? */
    #define DEFAULT_RESPONSE_TIMEOUT    1000
    #define POWER_ON_RESPONSE_TIMEOUT   10000
    #define POWER_OFF_RESPONSE_TIMEOUT   3000
    #define CALIB_PULSE_WIDTH_RESPONSE_TIMEOUT   10000     
    #define PURGING_RESPONSE_TIMEOUT    90000   /* 90 sec timeout on debug build */
    #define PAIRING_RESPONSE_TIMEOUT    5000
#else
    #define DEFAULT_RESPONSE_TIMEOUT    1000    /* 1 sec time out on Release build */
    #define POWER_ON_RESPONSE_TIMEOUT   10000   /* 10 sec time out on Release build */
    #define POWER_OFF_RESPONSE_TIMEOUT   3000   /* 3 sec time out on Release build */
    #define CALIB_PULSE_WIDTH_RESPONSE_TIMEOUT   10000   /* 10 sec time out on Release build */
    #define PURGING_RESPONSE_TIMEOUT    90000   /* 90 sec timeout on release build */
    #define PAIRING_RESPONSE_TIMEOUT    5000
#endif

static int _response_timeout = DEFAULT_RESPONSE_TIMEOUT;
static int _get_response_timeout() {
    return _response_timeout;
}

void _set_pairing_response_timeout() {
   _response_timeout = PAIRING_RESPONSE_TIMEOUT;
}

void _set_default_response_timeout() {
    _response_timeout = DEFAULT_RESPONSE_TIMEOUT;
}

static void  _set_response_timeout(int t) {
    _response_timeout = t;
}

/* Forward declaration */
ServiceResult_t service_execute(Frame_t             *frame, 
                                int32_t             instance,
                                uint8_t             *rsp_buf,
                                uint32_t            rsp_buf_size,
                                uint32_t            *rsp_size
                                );

ServiceResult_t pd_service_set_secure_overrides(int32_t instance, uint8_t ph_id, uint8_t slot_id, uint8_t *in, uint16_t in_len, int32_t *status) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == in) return SERVICE_ERROR;
    if(NULL == status) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame,BLUR_SERVICE_SET_SECURE_OVERRIDES);
    if(fr != FRAME_OK) {
        LOGE("pd_service_set_secure_overrides(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    

    frame_encode8(&frame, ph_id);
    frame_encode8(&frame, slot_id);
    for (int i=0; i < in_len; i++)
        fr = frame_encode8(&frame, in[i]);

    ServiceResult_t sr;
    Response_t response;
    sr = service_execute( &frame, instance,
                          &response.data[0], sizeof(response.data), &response.res_size);
    if(sr != SERVICE_OK) {
        LOGE("pd_service_set_secure_overrides(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    }

    *status = response.data[0];

    return SERVICE_OK;
}

ServiceResult_t service_get_used_ink_weight(int32_t instance, uint8_t ph_id, uint8_t slot_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    /* Create the device identification request frame */
    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, BLUR_SERVICE_HANDSHAKE_REQ);
    if(fr != FRAME_OK) {
        LOGE("service_device_identification(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    fr = frame_encode8(&frame, ph_id);
    fr = frame_encode8(&frame, slot_id);

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_device_identification(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_device_identification(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t pd_service_supply_status(int32_t instance, uint8_t ph_id, uint8_t slot_id, uint8_t *in, uint16_t in_len, int32_t *status) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == in) return SERVICE_ERROR;
    if(NULL == status) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame,BLUR_SERVICE_IDS_STATUS_UPDATE);
    if(fr != FRAME_OK) {
        LOGE("pd_service_supply_status(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    frame_encode8(&frame, ph_id);
    frame_encode8(&frame, slot_id);
    for (int i=0; i < in_len; i++)
        fr = frame_encode8(&frame, in[i]);

    ServiceResult_t sr;
    Response_t response;
    sr = service_execute( &frame, instance,
                          &response.data[0], sizeof(response.data), &response.res_size);
    if(sr != SERVICE_OK) {
        LOGE("pd_service_supply_status(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    }

    *status = response.data[0];

    return SERVICE_OK;
}

ServiceResult_t pd_service_terminate_session(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

	// currently IDS and Blur share the same value for IDS_TERMINATE_SESSION
	return _simple_command(instance, -1, BLUR_TERMINATE_SESSION, "sevice_terminate_session");
}

ServiceResult_t service_device_identification(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    /* Create the device identification request frame */
    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_DEVICE_IDENTIFICATION);
    if(fr != FRAME_OK) {
        LOGE("service_device_identification(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_device_identification(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_device_identification(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_device_configuration(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_fw_upgrade(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    /* Create the FW upgrade request frame */
    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_FW_UPGRADE);
    if(fr != FRAME_OK) {
        LOGE("service_fw_upgrade(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_fw_upgrade(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_fw_upgrade(): successfully executed. \n");
    }

    return SERVICE_OK;
}

ServiceResult_t service_check_device(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}


ServiceResult_t service_host_warning(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    
    /* Create the host warning - shutdown request frame */
    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_HOST_WARNING);
    if(fr != FRAME_OK) {
        LOGE("service_host_warning(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* send shutdown command */
    fr = frame_encode16(&frame, 1);
    if(fr != FRAME_OK) {
        LOGE("service_host_warning(): ERROR: Encoding frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_host_warning(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_host_warning(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_enable(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_disable(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_byte(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_byte(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_short(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_short(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_integer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_integer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}


ServiceResult_t service_set_pointer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_data(int32_t instance, uint32_t address, size_t data_size, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(data_size <= 0 || data_size > PROTOCOL_DATA_SIZE) return SERVICE_ERROR;
    if(response == NULL) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_READ_DATA);
    if(fr != FRAME_OK) {
        LOGE("service_read_data(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the address */
    fr = frame_encode32(&frame, address);
    if(fr != FRAME_OK) {
        LOGE("service_read_data(): ERROR: Encoding address. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the size */
    fr = frame_encode16(&frame, data_size);
    if(fr != FRAME_OK) {
        LOGE("service_read_data(): ERROR: Encoding size. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_read_data(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_read_data(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_write_data(int32_t instance, uint32_t address, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(data_size <= 0 || data_size > PROTOCOL_DATA_SIZE) return SERVICE_ERROR;
    if(data == NULL) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_WRITE_DATA);
    if(fr != FRAME_OK) {
        LOGE("service_write_data(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the address */
    fr = frame_encode32(&frame, address);
    if(fr != FRAME_OK) {
        LOGE("service_write_data(): ERROR: Encoding address. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the data */
    fr = frame_encode_bytes(&frame, data, data_size);
    if(fr != FRAME_OK) {
        LOGE("service_write_data(): ERROR: Encoding data. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_write_data(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_write_data(): successfully executed \n");
    }
 
    return SERVICE_OK;
}

ServiceResult_t service_clear_data(int32_t instance, uint32_t first_address, size_t total_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(total_size <= 0 || total_size > PROTOCOL_DATA_SIZE) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_CLEAR_DATA);
    if(fr != FRAME_OK) {
        LOGE("service_clear_data(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* encode starting addess and size in bytes */
    fr = frame_encode32(&frame, first_address);
    if(fr != FRAME_OK) {
        LOGE("service_clear_data(): ERROR: Encoding address. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode32(&frame, total_size);
    if(fr != FRAME_OK) {
        LOGE("service_clear_data(): ERROR: Encoding size. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
        
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_clear_data(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    uint32_t timeout = 0;
    fr = frame_decode32(&frame, &timeout);
    if(fr != FRAME_OK)  {
        LOGE("service_clear_data(): ERROR: code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_clear_data(): successfully executed \n");
    }

    return SERVICE_OK;
}

ServiceResult_t service_object_handling(int32_t instance, uint8_t req_id, uint16_t obj_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
//    MAX_ASSERT(req_id >= SUBSERVICE_OBJECT_LOCK);       /* currently supports only obj lock and obj unlock */
    if(req_id < SUBSERVICE_OBJECT_LOCK) return SERVICE_ERROR;
//    MAX_ASSERT(req_id <= SUBSERVICE_OBJECT_UNLOCK);     /* currently supports only obj lock and obj unlock */
    if(req_id > SUBSERVICE_OBJECT_UNLOCK) return SERVICE_ERROR;
    if(response == NULL) return SERVICE_ERROR;
    
    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_OBJECT_HANDLING);
    if(fr != FRAME_OK) {
        LOGE("service_obj_handling(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }   
    
    fr = frame_encode8(&frame, req_id);
    if(fr != FRAME_OK) {
        LOGE("service_obj_handling(): ERROR: Encoding req_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, obj_id);
    if(fr != FRAME_OK) {
        LOGE("service_obj_handling(): ERROR: Encoding obj_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_obj_handling(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_obj_handling(): successfully executed \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_rpc_message(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_io_port(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_parameter(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_frame_conf_status(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_trace_conf_status(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_vendor_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_bootloader_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_debug(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_extra_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_power_on(int32_t instance, Headinfo_t *info, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;

    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_POWON_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_power_on(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_power_on(): ERROR: Encodinging ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    
    /* Power ON takes longer for the firmware to execute, 
     * hence increase the response.
     */
    int old_timeout = _get_response_timeout(); /* Save the current time out */
    _set_response_timeout(POWER_ON_RESPONSE_TIMEOUT);
    sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        _set_response_timeout(old_timeout); /* Restore the time out */
        LOGE("service_power_on(): ERROR: Power ON failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_power_on(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_power_on(): successfully executed \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_power_off(int32_t instance, Headinfo_t *info, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    
    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_POWOFF_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_power_off(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_power_off(): ERROR: Encoding frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    /* Worst case timeout for Power off is approx around 2sec . Hence, we set ~3sec timeout */
    int old_timeout = _get_response_timeout(); /* Save the current time out */
    _set_response_timeout(POWER_OFF_RESPONSE_TIMEOUT);    
    sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        _set_response_timeout(old_timeout); /* Restore the time out */
        LOGE("service_power_off(): ERROR: Power Off failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_power_off(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_power_off(): successfully executed \n");
    }
    
    return SERVICE_OK;
}

/* 
 * Excute the requested service and return the return 
 * the result synchronously. This is a blocking call. internally
 * this function will send the service request to remote microcontroller,
 * wait for the response, validate the response.
 * 
 * service - Service number
 * rsp_buf - Buffer allocated by the caller where the response data will be filled.
 * rsp_buf_size - Size of the response buffer
 * rsp_size - Number of bytes of response data filled.
 * 
 * Returns - Result code
 */
ServiceResult_t service_execute(Frame_t             *frame, 
                                int32_t             instance,
                                uint8_t             *rsp_buf,
                                uint32_t            rsp_buf_size,
                                uint32_t            *rsp_size
                                ) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == frame) return SERVICE_ERROR;
    if(NULL == rsp_buf) return SERVICE_ERROR;
    if(NULL == rsp_size) return SERVICE_ERROR;

    uint8_t buf[MAX_FRAME_SIZE];
    uint16_t data_size = 0;
    FrameResult_t fr = frame_serialize(frame, buf, MAX_FRAME_SIZE, &data_size);
    if(fr != FRAME_OK) {
        LOGE("service_execute(): ERROR: serializing frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
	
	uart_lock();	// @@@ LOCK UART @@@
    
    LOGD("Service Request : %d\n", frame->service);
    char log_buf[1024] = {0x00};
    memset(log_buf, 0x00, 1024);
    toHexString(buf, log_buf, (data_size > 1024 ? 1024 : data_size), ',');
    LOGD("UART_Send_buffer: [%s](%d)", log_buf, data_size);
                            
    /* Send the command to uart */
    UartResult_t ur = UART_OK;
    ur = uart_send(instance, buf, data_size);
    if(ur != UART_OK) {
        LOGE("service_execute() : ERROR sending command to UART. Error code = %d\n", ur);
		uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }
    
    LOGD("About to read the response header\n");
    
    /* Read the response header */
    uint8_t resp_buf[MAX_FRAME_SIZE];
    size_t  recvd_size;
    ur = uart_recv(instance, resp_buf, RESP_HEADER_SIZE, &recvd_size, _get_response_timeout());
    if(ur != UART_OK) {
        LOGE("service_execute() : ERROR receiving response from UART. Error code = %d\n", ur);
		uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }
    
    LOGD("service_execute(): Response : No. Bytes recvd = %d\n", recvd_size);
    memset(log_buf, 0x00, 1024);
    toHexString(resp_buf, log_buf, (recvd_size > 1024 ? 1024 : recvd_size), ',');
    LOGD("UART_Resp_buffer: [%s](%d)", log_buf, recvd_size);

    fr = frame_response_init(frame, resp_buf, recvd_size);
    if(fr != FRAME_OK) {
        LOGE("service_execute(): ERROR: initializing response frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint32_t rsp_data_size = 0;
    fr = frame_response_get_data_size(frame, &rsp_data_size);
    if(fr != FRAME_OK) {
        LOGE("service_execute(): ERROR: get response data size. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    LOGE("service_execute(): Number of response data to read = %d\n", rsp_data_size);
    
    /* Now read the variable data */
    size_t n=0;
    ur = uart_recv(instance, resp_buf+recvd_size, rsp_data_size+1, /* +1 for crc,(FW is not sending Line break now : +1 for line break)  */
                        &n, _get_response_timeout());
                        
    LOGD("service_execute(): Response : No. Bytes recvd = %d\n", n);
    memset(log_buf, 0x00, 1024);
    toHexString(resp_buf, log_buf, (n > 1024 ? 1024 : n), ',');
    LOGD("UART_Resp_buffer: [%s](%d)", log_buf, n);

    /* Deserialize the frame to structure */
    fr = frame_response_init_data(frame, resp_buf+recvd_size, n); 
    if(fr != FRAME_OK) {
        LOGE("service_execute(): ERROR: deserializing frame. Error code = %d\n", fr);
		uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }

	// if response buffer is provided, copy to buffer
	if(rsp_buf != NULL && rsp_buf_size > 0)	{
		if(rsp_buf_size < frame->data_size) {
            uart_unlock();	// @@@ UN-LOCK UART @@@
            return SERVICE_ERROR;
        };
		memcpy(rsp_buf, frame->data, frame->data_size);
	}
    *rsp_size = frame->data_size;
    
	uart_unlock();	// @@@ UN-LOCK UART @@@
    return SERVICE_OK;
}

/*
 *  Checks particular extension id is supported by the device.
 *  If the extension is supported, it returns 
 * 		- first service no
 * 		- revision no 
 * 		- total no of services
 * 		- total no of events
 * 		- first event no
 */	
 
ServiceResult_t service_check_extension(int32_t instance, uint16_t ext_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ext_id <= 0) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_CHECK_EXTENSION);
    if(fr != FRAME_OK) {
        LOGE("service_check_extension(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* encode the extension id in the first two bytes */
    fr = frame_encode16(&frame, ext_id);
    if(fr != FRAME_OK) {
        LOGE("service_check_extension(): ERROR: Encoding ext_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_check_extension(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_check_extension(): successfully executed.\n");
    }
    
    return SERVICE_OK;
}

/*
 *  Get the uCtlr's device id of the fpga flash.
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */	
ServiceResult_t service_flash_devid(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+GET_DEV_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_devid(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* encode 1 in the first byte */
    fr = frame_encode8(&frame, 0x1);
    if(fr != FRAME_OK) {
        LOGE("service_flash_devid(): ERROR: Encoding frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* calculate the length of the string */
    uint8_t len;
    for(len=0;  (info->dev_name[len]!='\0') &&  (len < MAX_NAME_LENGTH ) ;  len++);
    if(len >= MAX_NAME_LENGTH) {
        LOGE("service_flash_devid(): ERROR: Length Over. len = %d\n", len);
        return SERVICE_ERROR;
    }

    /* encode dev name in the next bytes */
    fr = frame_encode_bytes(&frame, info->dev_name, len);
    if(fr != FRAME_OK) {
        LOGE("service_flash_devid(): ERROR: Encoding info->dev_name. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_devid(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_devid(): successfully executed.\n");
    }
    
    return SERVICE_OK;
}

/*
 *  Send erase cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */	

ServiceResult_t service_flash_erase(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);   /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 1);             /* writesize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_ERASE_ID);/* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_RDBUF_ID);/*rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_erase(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_erase(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_erase(): successfully executed.\n");
    }

    return SERVICE_OK;
}

/*
 *  Send page program cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_pgm(int32_t instance, Flashinfo_t * info, Response_t * response, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
//    MAX_ASSERT(data_size > 3); /* other than 3 address bytes there should be at least one byte of data */
    if(data_size <= 3) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);   /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, data_size+1);   /* writesize +1 is for cmd */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding data_size. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_PGPGM_ID);/* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_RDBUF_ID);/* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_pgm(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_pgm(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_pgm(): successfully executed.\n");
    }

    return SERVICE_OK;
}

/*
 *  Send write enable cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_wrenbl(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, info->dev_id);   /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 1);             /* writesize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_WREN_ID); /* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_RDBUF_ID);/* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_wrenbl(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_wrenbl(): successfully executed.\n");
    }
    
    return SERVICE_OK;	
}

/*
 *  Send read status cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_rdstatus(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);   /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 2);             /* writesize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 1);             /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 1);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_GETSTS_ID); /* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, FLASH_RDBUF_ID);  /* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_rdstatus(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_rdstatus(): successfully executed.\n");
    }

    return SERVICE_OK;
}

/*
 *  Send write status cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_wrstatus(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);       /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 2);                 /* writesize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);                 /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 0);                 /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_WRSTS_ID);    /* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_RDBUF_ID);    /* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_wrstatus(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_wrstatus(): successfully executed.\n");
    }

    return SERVICE_OK;
}

/*
 *  Send read id cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_rdid(int32_t instance, Flashinfo_t * info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);   /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 21);            /* writesize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, 20);            /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, 1);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_RDID_ID); /* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_RDBUF_ID);/* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_rdid(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_rdid(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_rdid(): successfully executed.\n");
    }
    
    return SERVICE_OK;
}

/*
 *  Send page read cmd to flash
 * 
 * 
 *  This needs to be called with all the flashinfo filled.
 */
ServiceResult_t service_flash_read(int32_t instance, Flashinfo_t * info, Response_t * response, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;
//    MAX_ASSERT(data_size > 3); /* other than 3 address bytes there should be atleast one byte of data */
    if(data_size <= 3) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, info->first_srvc_id+WRRD_INDEX);
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode8(&frame, info->dev_id);  /* device id */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding info->dev_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    fr = frame_encode16(&frame, data_size+1);   /* writesize +1 is for cmd ..data to be written into spi for reading */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding writesize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, data_size-3);   /* readsize */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding readsize. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, 4);             /* readoffset */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding readoffset. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_RDPG_ID); /* writobjid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding writobjid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode16(&frame, FLASH_RDBUF_ID);/* rdobjiid */
    if(fr != FRAME_OK) {
        LOGE("service_flash_read(): ERROR: Encoding rdobjiid. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_flash_read(): ERROR: Service execution failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_flash_read(): successfully executed.\n");
    }
    
    return SERVICE_OK;
}

/*
 *  Set or clear the fpga chip reset 
 * 
 * 
 *  
 */
ServiceResult_t service_fpga_set_chipreset(int32_t instance, bool highlow) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_SETORCLEAR_FPGA_RESET );
    if(fr != FRAME_OK) {
        LOGE("service_fpga_set_chipreset(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode value */
    fr = frame_encode8(&frame, highlow);
    if(fr != FRAME_OK) {
        LOGE("service_fpga_set_chipreset(): ERROR: Encoding highlow. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
        
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_fpga_set_chipreset(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_fpga_set_chipreset(): successfully executed.\n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_head_count(int32_t instance, Headinfo_t *info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_HDCNT_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_head_count(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_head_count(): ERROR: Print head count failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_head_count(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_head_count(): successfully executed. Headcounts: %d \n",response->data[0]);
    }

    return SERVICE_OK;
}

/*
 *   
 * 
 * 
 *  
 */
ServiceResult_t service_head_status(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_STATUS_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_head_status(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_head_status(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_head_status(): ERROR: Print head status failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_head_status(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_head_status(): successfully executed. STATUS: %d \n",response->data[0]);
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_system_info(int32_t instance, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_SYSTEM_INFO);
    if(fr != FRAME_OK) {
        LOGE("service_get_system_info(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_system_info(): ERROR: Get system info failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_system_info(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_system_info(): successfully executed. \n");
    }
    
    return SERVICE_OK;	
}

ServiceResult_t service_set_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint32_t level) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_SET_FP_ENERGYLEVEL_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_set_fpenergylevel(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_set_fpenergylevel(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the level */
    fr = frame_encode32(&frame, level);
    if(fr != FRAME_OK) {
        LOGE("service_set_fpenergylevel(): ERROR: Encoding level. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_set_fpenergylevel(): ERROR: Failed to set firing pulse energy level. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_fpenergylevel(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_fpenergylevel(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_FP_ENERGYLEVEL_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_fpenergylevel(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_fpenergylevel(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_fpenergylevel(): ERROR: Get firing pulse energy level failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_fpenergylevel(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_fpenergylevel(): successfully executed. \n");
    }
    
    return SERVICE_OK;	            
}

ServiceResult_t service_get_fpenergylevels(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;
    
    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_FP_ENERGYLEVELS_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_fpenergylevels(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_fpenergylevels(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_fpenergylevels(): ERROR: Get firing pulse energy levels failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_fpenergylevels(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_fpenergylevels(): successfully executed. \n");
    }
    
    return SERVICE_OK;	            
}

ServiceResult_t service_read_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;

    if(info->initialized != true) return SERVICE_ERROR;

    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OEM_FIELD_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_read_oem_field(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_read_oem_field(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode read_write */
    uint8_t read_write = MULTIHEAD_OEMDATA_READ;
    fr = frame_encode8(&frame, read_write);
    if(fr != FRAME_OK) {
        LOGE("service_read_oem_field(): ERROR: Encoding read_write. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode Field ID */
    fr = frame_encode8(&frame, field_id);
    if(fr != FRAME_OK) {
        LOGE("service_read_oem_field(): ERROR: Encoding field_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_read_oem_field(): ERROR: Reading of OEM field failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_read_oem_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_read_oem_field(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_write_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, uint8_t data) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;

    if(info->initialized != true) return SERVICE_ERROR;

    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OEM_FIELD_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_write_oem_field(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_write_oem_field(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode read_write */
    uint8_t read_write = MULTIHEAD_OEMDATA_WRITE;
    fr = frame_encode8(&frame, read_write);
    if(fr != FRAME_OK) {
        LOGE("service_write_oem_field(): ERROR: Encoding read_write. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode Field ID */
    fr = frame_encode8(&frame, field_id);
    if(fr != FRAME_OK) {
        LOGE("service_write_oem_field(): ERROR: Encoding field_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode data */
    fr = frame_encode8(&frame, data);
    if(fr != FRAME_OK) {
        LOGE("service_write_oem_field(): ERROR: Encoding data. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_write_oem_field(): ERROR: Writing of OEM field failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_write_oem_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_write_oem_field(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_calibrate_pulsewidth(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
//    MAX_ASSERT(slot < NUM_TRENCH_PER_PH );
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_FP_WIDTH_CALIB_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_calibrate_pulsewidth(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_calibrate_pulsewidth(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode Slot ID */
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_calibrate_pulsewidth(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Calibrate Pulse Width takes longer for the firmware to execute, 
     * hence increase the response.
     */
    int old_timeout = _get_response_timeout(); /* Save the current time out */
    _set_response_timeout(CALIB_PULSE_WIDTH_RESPONSE_TIMEOUT);

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        _set_response_timeout(old_timeout); /* Restore the time out */
        LOGE("service_calibrate_pulsewidth(): ERROR: Failed to execute temp calibration. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_calibrate_pulsewidth(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_calibrate_pulsewidth(): successfully executed. \n");
    }

    return SERVICE_OK;
}

ServiceResult_t service_get_used_ink_weight_unsecured(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
//    MAX_ASSERT(slot < NUM_TRENCH_PER_PH );
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_DRPVLM_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_used_ink_weight_unsecured(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_used_ink_weight_unsecured(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode Slot ID */
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_get_used_ink_weight_unsecured(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_used_ink_weight_unsecured(): ERROR: Get drop volume failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_used_ink_weight_unsecured(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_used_ink_weight_unsecured(): successfully executed. \n");
    }

    return SERVICE_OK;  
}

ServiceResult_t service_get_actual_temp(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_TEMP_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_actual_temp(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_actual_temp(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_TEMP_GET );
    if(fr != FRAME_OK) {
        LOGE("service_get_actual_temp(): ERROR: Encoding MULTIHEAD_TEMP_GET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode temp  */
    fr = frame_encode8(&frame,MULTIHEAD_ACTUALTEMP);
    if(fr != FRAME_OK) {
        LOGE("service_get_actual_temp(): ERROR: Encoding MULTIHEAD_ACTUALTEMP. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_actual_temp(): ERROR: Print head get actual temp failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_actual_temp(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_actual_temp(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_set_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t temp) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_TEMP_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_set_temperature_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_set_temperature_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_TEMP_SET );
    if(fr != FRAME_OK) {
        LOGE("service_set_temperature_override(): ERROR: Encoding MULTIHEAD_TEMP_SET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the temperature */
    fr = frame_encode32(&frame, temp);
    if(fr != FRAME_OK) {
        LOGE("service_set_temperature_override(): ERROR: Encoding temperature. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_set_temperature_override(): ERROR: Failed to set Override temperature. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_temperature_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_temperature_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;                   
}

ServiceResult_t service_get_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_TEMP_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_temperature_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_temperature_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_TEMP_GET );
    if(fr != FRAME_OK) {
        LOGE("service_get_temperature_override(): ERROR: Encoding MULTIHEAD_TEMP_GET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_temperature_override(): ERROR: Print head get temperature override failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_temperature_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_temperature_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_set_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t voltage) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_VOLTAGE_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_set_voltage_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_set_voltage_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_VOLT_SET );
    if(fr != FRAME_OK) {
        LOGE("service_set_voltage_override(): ERROR: Encoding MULTIHEAD_VOLT_SET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the voltage */
    fr = frame_encode32(&frame, voltage);
    if(fr != FRAME_OK) {
        LOGE("service_set_voltage_override(): ERROR: Encoding voltage. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_set_voltage_override(): ERROR: Failed to set ovverride voltage. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_voltage_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_voltage_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;                   
}

ServiceResult_t service_get_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_VOLTAGE_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_voltage_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_voltage_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_VOLT_GET );
    if(fr != FRAME_OK) {
        LOGE("service_get_voltage_override(): ERROR: Encoding MULTIHEAD_VOLT_GET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_voltage_override(): ERROR: Print head get voltage override failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_voltage_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_voltage_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_set_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, uint8_t energy) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OVERENERGY_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_OVERENERGY_SET );
    if(fr != FRAME_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Encoding MULTIHEAD_OVERENERGY_SET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode the slot */
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode the energy */
    fr = frame_encode8(&frame, energy);
    if(fr != FRAME_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Encoding energy. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_set_over_energy_override(): ERROR: Failed to set ovverride energy. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_over_energy_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_over_energy_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OVERENERGY_OVERRIDE_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_over_energy_override(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_over_energy_override(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode get/set */
    fr = frame_encode8(&frame,MULTIHEAD_OVERENERGY_GET );
    if(fr != FRAME_OK) {
        LOGE("service_get_over_energy_override(): ERROR: Encoding MULTIHEAD_OVERENERGY_GET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode the slot */
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_get_over_energy_override(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_over_energy_override(): ERROR: Print head get over energy override failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_over_energy_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_over_energy_override(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_ph_configuration(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_PH_CONFIGURATION_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_ph_configuration(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_ph_configuration(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /*Encode slot*/
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_get_ph_configuration(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_ph_configuration(): ERROR: Print head get configuration failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_ph_configuration(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_ph_configuration(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_column_spacing(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_COLUMN_SPACING_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_column_spacing(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_column_spacing(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /*Encode slot*/
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_get_column_spacing(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_column_spacing(): ERROR: Get Column Spacing failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_column_spacing(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_column_spacing(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OEM_FIELD_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_get_field_data(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_field_data(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode ID */
    fr = frame_encode8(&frame,field_idx);
    if(fr != FRAME_OK) {
        LOGE("service_get_field_data(): ERROR: Encoding field_idx. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode Get/Set */
    fr = frame_encode8(&frame,MULTIHEAD_OEMDATA_READ);
    if(fr != FRAME_OK) {
        LOGE("service_get_field_data(): ERROR: Encoding MULTIHEAD_OEMDATA_READ. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_field_data(): ERROR: Get field data failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_field_data(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_field_data(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_set_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == data) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OEM_FIELD_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_set_field(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_set_field(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode ID */
    fr = frame_encode8(&frame,field_idx);
    if(fr != FRAME_OK) {
        LOGE("service_set_field(): ERROR: Encoding field_idx. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode Get/Set */
    fr = frame_encode8(&frame,MULTIHEAD_OEMDATA_WRITE);
    if(fr != FRAME_OK) {
        LOGE("service_set_field(): ERROR: Encoding MULTIHEAD_OEMDATA_WRITE. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /*Encode data*/
    fr = frame_encode_bytes(&frame,data, data_size);
    if(fr != FRAME_OK) {
        LOGE("service_set_field(): ERROR: Encoding data. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf , sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_set_field(): ERROR: Set field data failed. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_field(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_get_fpga_log(int32_t instance, FpgaRecord_t *records, size_t records_size, size_t *records_returned) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == records) return SERVICE_ERROR;
    if(NULL == records_returned) return SERVICE_ERROR;
    if(records_size <= 0) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    fr = frame_init(&frame, SERVICE_GET_FPGA_LOG);
    if(fr != FRAME_OK) {
        LOGE("SERVICE_GET_FPGA_LOG(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    uint8_t  rsp_buf[256];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("SERVICE_GET_FPGA_LOG(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* Decode the response bytes */
    *records_returned = 0; /* Initialize the records size to zero */
    int remaining = frame.data_size;
    const int SIZE_OF_RECORD = 9;
    for (int i = 0; (remaining >= SIZE_OF_RECORD) ; i++, remaining -= SIZE_OF_RECORD) {
        fr = frame_decode32(&frame, &records[i].timeStamp);
        if(fr != FRAME_OK) return SERVICE_ERROR;
        
        fr = frame_decode8(&frame, &records[i].b1);
        if(fr != FRAME_OK) return SERVICE_ERROR;
        
        fr = frame_decode8(&frame, &records[i].b2);
        if(fr != FRAME_OK) return SERVICE_ERROR;
        
        fr = frame_decode8(&frame, &records[i].b3);
        if(fr != FRAME_OK) return SERVICE_ERROR;

        fr = frame_decode8(&frame, &records[i].reply);
        if(fr != FRAME_OK) return SERVICE_ERROR;
        
        fr = frame_decode8(&frame, &records[i].result);
        if(fr != FRAME_OK) return SERVICE_ERROR;
        
        (*records_returned)++; /* We added a record, so increment number of returned records */
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_lock_oem_partition(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t partition_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id + MULTIHEAD_LOCK_OEM_PARTITION_IDX );
    if(fr != FRAME_OK) {
        LOGE("service_lock_oem_partition(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_lock_oem_partition(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode the partition ID */
    fr = frame_encode8(&frame, partition_id);
    if(fr != FRAME_OK) {
        LOGE("service_lock_oem_partition(): ERROR: Encoding partition_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);                          
    if(sr != SERVICE_OK) {
        LOGE("service_lock_oem_partition(): ERROR: Failed to lock OEM partition. Error code = %d\n", sr);
        return SERVICE_ERROR;
    }
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_lock_oem_partition(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_lock_oem_partition(): successfully executed. \n");
    }
    
    return SERVICE_OK;
}

ServiceResult_t service_start_purging(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_PURGE_START_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_start_purging(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_start_purging(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode slot */
    fr = frame_encode8(&frame, slot);
    if(fr != FRAME_OK) {
        LOGE("service_start_purging(): ERROR: Encoding slot. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
                          
    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    /* Purging takes longer for the firmware to execute, 
     * hence increase the response.
     */
    int old_timeout = _get_response_timeout(); /* Save the current time out */
    _set_response_timeout(PURGING_RESPONSE_TIMEOUT);
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) {
        LOGE("service_start_purging(): ERROR: Print head purging failed. Error code = %d\n", sr);
        _set_response_timeout(old_timeout);
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout);
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_start_purging(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_start_purging(): successfully executed. \n");
    }

    return SERVICE_OK;
}

ServiceResult_t service_get_state_info(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == response) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_STATE_INFO_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_get_state_info(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    /* Encode PH ID */
    fr = frame_encode8(&frame, ph_id);
    if(fr != FRAME_OK) {
        LOGE("service_get_state_info(): ERROR: Encoding ph_id. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
    
    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data, sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) {
        LOGE("service_get_state_info(): ERROR: Get state info failed. Error code = %d\n", sr);
        
        return SERVICE_ERROR;
    }
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_state_info(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_state_info(): successfully executed. \n");
    }

    return SERVICE_OK;
}

ServiceResult_t service_set_date(int32_t instance, Headinfo_t *info, int32_t year, int32_t month, int32_t day) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_DATE_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_set_date(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode Get/Set */
    fr = frame_encode8(&frame,MULTIHEAD_DATE_SET);
    if(fr != FRAME_OK) {
        LOGE("service_set_date(): ERROR: Encoding MULTIHEAD_DATE_SET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* encode values */
    fr = frame_encode16(&frame, year);
    if(fr != FRAME_OK) {
        LOGE("service_set_date(): ERROR: Encoding year. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, month);
    if(fr != FRAME_OK) {
        LOGE("service_set_date(): ERROR: Encoding month. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, day);
    if(fr != FRAME_OK) {
        LOGE("service_set_date(): ERROR: Encoding day. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }
       
    
    uint8_t  rsp_buf[4];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);

    if(sr != SERVICE_OK) {
        LOGE("service_set_date(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_date(): successfully executed. \n");
    }

    return SERVICE_OK;    
}

ServiceResult_t service_set_platform_info(int32_t instance, Headinfo_t *info, PlatformInfo_t *platform_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) return SERVICE_ERROR;
    if(NULL == platform_info) return SERVICE_ERROR;

    Frame_t         frame;
    FrameResult_t   fr;
    uint8_t idx;
    
    if(info->initialized != true) return SERVICE_ERROR;
    
    fr = frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_PLATFORMINFO_IDX);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Creating frame. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    /* Encode Get/Set */
    fr = frame_encode8(&frame,MULTIHEAD_PLATFORMINFO_SET);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding MULTIHEAD_PLATFORMINFO_SET. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    for(idx = 0; idx < 12; idx++) {
        fr = frame_encode8(&frame,platform_info->model[idx]);
        if(fr != FRAME_OK) {
            LOGE("service_set_platform_info(): ERROR: Encoding platform_info->model[%d]. Error code = %d\n", fr, idx);
            return SERVICE_ERROR;
        }
    }

    fr = frame_encode16(&frame, platform_info->mfg_year);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->mfg_year. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, platform_info->mfg_woy);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->mfg_woy. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, platform_info->mfg_country);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->mfg_country. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, platform_info->mfg_rev_major);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->mfg_rev_major. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, platform_info->mfg_rev_minor);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->mfg_rev_minor. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    fr = frame_encode8(&frame, platform_info->orientation);
    if(fr != FRAME_OK) {
        LOGE("service_set_platform_info(): ERROR: Encoding platform_info->orientation. Error code = %d\n", fr);
        return SERVICE_ERROR;
    }

    uint8_t  rsp_buf[4];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);

    if(sr != SERVICE_OK) {
        LOGE("service_set_platform_info(): Error code = %d\n", sr);
        return SERVICE_ERROR;
    } else {
        LOGI("service_set_platform_info(): successfully executed. \n");
    }

    return SERVICE_OK;         
}
