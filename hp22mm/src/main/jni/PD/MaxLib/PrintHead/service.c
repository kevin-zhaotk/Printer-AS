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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot_id);
        return SERVICE_ERROR;
    }
    if(NULL == in) {
        LOGE("in NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == status) {
        LOGE("status NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame,BLUR_SERVICE_SET_SECURE_OVERRIDES) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, slot_id) != FRAME_OK) return SERVICE_ERROR;
    for (int i=0; i < in_len; i++) {
        if(frame_encode8(&frame, in[i]) != FRAME_OK) return SERVICE_ERROR;
    }

    ServiceResult_t sr;
    Response_t response;
    sr = service_execute( &frame, instance,
                          &response.data[0], sizeof(response.data), &response.res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    *status = response.data[0];

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_used_ink_weight(int32_t instance, uint8_t ph_id, uint8_t slot_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot_id);
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    /* Create the device identification request frame */
    Frame_t         frame;
    if(frame_init(&frame, BLUR_SERVICE_HANDSHAKE_REQ) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, slot_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t pd_service_supply_status(int32_t instance, uint8_t ph_id, uint8_t slot_id, uint8_t *in, uint16_t in_len, int32_t *status) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot_id < 0 || slot_id >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot_id);
        return SERVICE_ERROR;
    }
    if(NULL == in) {
        LOGE("in NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == status) {
        LOGE("status NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame,BLUR_SERVICE_IDS_STATUS_UPDATE) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, slot_id) != FRAME_OK) return SERVICE_ERROR;
    for (int i=0; i < in_len; i++)
        if(frame_encode8(&frame, in[i]) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    Response_t response;
    sr = service_execute( &frame, instance,
                          &response.data[0], sizeof(response.data), &response.res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    *status = response.data[0];

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t pd_service_terminate_session(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

	// currently IDS and Blur share the same value for IDS_TERMINATE_SESSION
	return _simple_command(instance, -1, BLUR_TERMINATE_SESSION, "sevice_terminate_session");
}

ServiceResult_t service_device_identification(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    /* Create the device identification request frame */
    Frame_t         frame;
    if(frame_init(&frame, SERVICE_DEVICE_IDENTIFICATION) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_device_configuration(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_fw_upgrade(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    /* Create the FW upgrade request frame */
    Frame_t         frame;
    if(frame_init(&frame, SERVICE_FW_UPGRADE) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_check_device(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}


ServiceResult_t service_host_warning(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    
    /* Create the host warning - shutdown request frame */
    Frame_t         frame;
    if(frame_init(&frame, SERVICE_HOST_WARNING) != FRAME_OK) return SERVICE_ERROR;
    /* send shutdown command */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_enable(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_disable(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_byte(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_byte(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_short(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_short(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_integer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_write_integer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}


ServiceResult_t service_set_pointer(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_read_data(int32_t instance, uint32_t address, size_t data_size, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(data_size <= 0 || data_size > PROTOCOL_DATA_SIZE) {
        LOGE("Invalid data size [%d]!", data_size);
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_READ_DATA) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the address */
    if(frame_encode32(&frame, address) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the size */
    if(frame_encode16(&frame, data_size) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_write_data(int32_t instance, uint32_t address, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(data_size <= 0 || data_size > PROTOCOL_DATA_SIZE) {
        LOGE("Invalid data size [%d]!", data_size);
        return SERVICE_ERROR;
    }
    if(NULL == data) {
        LOGE("data NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_WRITE_DATA) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the address */
    if(frame_encode32(&frame, address) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the data */
    if(frame_encode_bytes(&frame, data, data_size) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_clear_data(int32_t instance, uint32_t first_address, size_t total_size) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(total_size <= 0 || total_size > PROTOCOL_DATA_SIZE) {
        LOGE("Invalid data size [%d]!", total_size);
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_CLEAR_DATA) != FRAME_OK) return SERVICE_ERROR;
    /* encode starting addess and size in bytes */
    if(frame_encode32(&frame, first_address) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode32(&frame, total_size) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    uint32_t timeout = 0;
    if(frame_decode32(&frame, &timeout) != FRAME_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_object_handling(int32_t instance, uint8_t req_id, uint16_t obj_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
//    MAX_ASSERT(req_id >= SUBSERVICE_OBJECT_LOCK);       /* currently supports only obj lock and obj unlock */
    if(req_id < SUBSERVICE_OBJECT_LOCK) {
        LOGE("Invalid req_id [%d] (<SUBSERVICE_OBJECT_LOCK)!", req_id);
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(req_id <= SUBSERVICE_OBJECT_UNLOCK);     /* currently supports only obj lock and obj unlock */
    if(req_id > SUBSERVICE_OBJECT_UNLOCK) {
        LOGE("Invalid req_id [%d] (>SUBSERVICE_OBJECT_UNLOCK)!", req_id);
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_OBJECT_HANDLING) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, req_id) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode16(&frame, obj_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_rpc_message(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_io_port(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_parameter(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_frame_conf_status(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_trace_conf_status(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_vendor_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_bootloader_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_debug(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_extra_info(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    return SERVICE_ERROR;
}

ServiceResult_t service_power_on(int32_t instance, Headinfo_t *info, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_POWON_IDX) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

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
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_power_on(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_power_off(int32_t instance, Headinfo_t *info, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_POWOFF_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

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
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_power_off(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

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

    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
		LOGE("Invalid Instance!");
		return SERVICE_ERROR;
	}
    if(NULL == frame) {
        LOGE("frame NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == rsp_buf) {
        LOGE("rsp_buf NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == rsp_size) {
        LOGE("rsp_size NULL!");
        return SERVICE_ERROR;
    }

    uint8_t buf[MAX_FRAME_SIZE];
    uint16_t data_size = 0;
    FrameResult_t fr = frame_serialize(frame, buf, MAX_FRAME_SIZE, &data_size);
    if(fr != FRAME_OK) return SERVICE_ERROR;

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
		uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }
    
    LOGD("About to read the response header\n");
    
    /* Read the response header */
    uint8_t resp_buf[MAX_FRAME_SIZE];
    size_t  recvd_size;
    ur = uart_recv(instance, resp_buf, RESP_HEADER_SIZE, &recvd_size, _get_response_timeout());
    if(ur != UART_OK) {
		uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }
    
    LOGD("service_execute(): Response : No. Bytes recvd = %d\n", recvd_size);
    memset(log_buf, 0x00, 1024);
    toHexString(resp_buf, log_buf, (recvd_size > 1024 ? 1024 : recvd_size), ',');
    LOGD("UART_Resp_buffer: [%s](%d)", log_buf, recvd_size);

    fr = frame_response_init(frame, resp_buf, recvd_size);
    if(fr != FRAME_OK) {
        uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }

    uint32_t rsp_data_size = 0;
    fr = frame_response_get_data_size(frame, &rsp_data_size);
    if(fr != FRAME_OK) {
        uart_unlock();	// @@@ UN-LOCK UART @@@
        return SERVICE_ERROR;
    }

    LOGD("service_execute(): Number of response data to read = %d\n", rsp_data_size);
    
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

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ext_id <= 0) {
        LOGE("Invalid ext_id!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_CHECK_EXTENSION) != FRAME_OK) return SERVICE_ERROR;
    /* encode the extension id in the first two bytes */
    if(frame_encode16(&frame, ext_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+GET_DEV_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* encode 1 in the first byte */
    if(frame_encode8(&frame, 0x1) != FRAME_OK) return SERVICE_ERROR;

    /* calculate the length of the string */
    uint8_t len;
    for(len=0;  (info->dev_name[len]!='\0') &&  (len < MAX_NAME_LENGTH ) ;  len++);
    if(len >= MAX_NAME_LENGTH) {
        LOGE("service_flash_devid(): ERROR: Length Over. len = %d\n", len);
        return SERVICE_ERROR;
    }

    /* encode dev name in the next bytes */
    if(frame_encode_bytes(&frame, info->dev_name, len) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_ERASE_ID) != FRAME_OK) return SERVICE_ERROR;
    /*rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
//    MAX_ASSERT(data_size > 3); /* other than 3 address bytes there should be at least one byte of data */
    if(data_size <= 3) {
        LOGE("Invalid data size [%d]!", data_size);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize +1 is for cmd */
    if(frame_encode16(&frame, data_size+1) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_PGPGM_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_WREN_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize */
    if(frame_encode16(&frame, 2) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_GETSTS_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize */
    if(frame_encode16(&frame, 2) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 0) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_WRSTS_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize */
    if(frame_encode16(&frame, 21) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, 20) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 1) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_RDID_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
//    MAX_ASSERT(data_size > 3); /* other than 3 address bytes there should be atleast one byte of data */
    if(data_size <= 3) {
        LOGE("Invalid data size [%d]!", data_size);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+WRRD_INDEX) != FRAME_OK) return SERVICE_ERROR;
    /* device id */
    if(frame_encode8(&frame, info->dev_id) != FRAME_OK) return SERVICE_ERROR;
    /* writesize +1 is for cmd ..data to be written into spi for reading */
    if(frame_encode16(&frame, data_size+1) != FRAME_OK) return SERVICE_ERROR;
    /* readsize */
    if(frame_encode16(&frame, data_size-3) != FRAME_OK) return SERVICE_ERROR;
    /* readoffset */
    if(frame_encode16(&frame, 4) != FRAME_OK) return SERVICE_ERROR;
    /* writobjid */
    if(frame_encode16(&frame, FLASH_RDPG_ID) != FRAME_OK) return SERVICE_ERROR;
    /* rdobjiid */
    if(frame_encode16(&frame, FLASH_RDBUF_ID) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr = service_execute( &frame, instance,
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_SETORCLEAR_FPGA_RESET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode value */
    if(frame_encode8(&frame, highlow) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_head_count(int32_t instance, Headinfo_t *info, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_HDCNT_IDX) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_head_count(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

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

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_STATUS_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_head_status(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_system_info(int32_t instance, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_SYSTEM_INFO) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_system_info(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint32_t level) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_SET_FP_ENERGYLEVEL_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the level */
    if(frame_encode32(&frame, level) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_fpenergylevel(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_FP_ENERGYLEVEL_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_fpenergylevel(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_fpenergylevels(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }
    
    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_FP_ENERGYLEVELS_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_fpenergylevels(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_read_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OEM_FIELD_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode read_write */
    uint8_t read_write = MULTIHEAD_OEMDATA_READ;
    if(frame_encode8(&frame, read_write) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Field ID */
    if(frame_encode8(&frame, field_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_read_oem_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_write_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, uint8_t data) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OEM_FIELD_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode read_write */
    uint8_t read_write = MULTIHEAD_OEMDATA_WRITE;
    if(frame_encode8(&frame, read_write) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Field ID */
    if(frame_encode8(&frame, field_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode data */
    if(frame_encode8(&frame, data) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_write_oem_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_calibrate_pulsewidth(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_FP_WIDTH_CALIB_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Slot ID */
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

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
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout); /* Restore the time out */
    
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_calibrate_pulsewidth(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_used_ink_weight_unsecured(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_DRPVLM_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Slot ID */
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_used_ink_weight_unsecured(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_actual_temp(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_TEMP_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_TEMP_GET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode temp  */
    if(frame_encode8(&frame,MULTIHEAD_ACTUALTEMP) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_actual_temp(): Cannot be executed \n");
        return SERVICE_ERROR;
    } else {
        LOGI("service_get_actual_temp(): successfully executed. \n");
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t temp) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_TEMP_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_TEMP_SET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the temperature */
    if(frame_encode32(&frame, temp) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_temperature_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_TEMP_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_TEMP_GET) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_temperature_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t voltage) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_VOLTAGE_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_VOLT_SET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the voltage */
    if(frame_encode32(&frame, voltage) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_voltage_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_VOLTAGE_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_VOLT_GET) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_voltage_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, uint8_t energy) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_GET_SET_OVERENERGY_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_OVERENERGY_SET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the slot */
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the energy */
    if(frame_encode8(&frame, energy) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_over_energy_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OVERENERGY_OVERRIDE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode get/set */
    if(frame_encode8(&frame,MULTIHEAD_OVERENERGY_GET) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the slot */
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_over_energy_override(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_ph_configuration(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_PH_CONFIGURATION_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /*Encode slot*/
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_ph_configuration(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_column_spacing(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_COLUMN_SPACING_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /*Encode slot*/
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_column_spacing(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, Response_t *response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OEM_FIELD_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode ID */
    if(frame_encode8(&frame,field_idx) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Get/Set */
    if(frame_encode8(&frame,MULTIHEAD_OEMDATA_READ) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_field_data(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == data) {
        LOGE("data NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_OEM_FIELD_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode ID */
    if(frame_encode8(&frame,field_idx) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Get/Set */
    if(frame_encode8(&frame,MULTIHEAD_OEMDATA_WRITE) != FRAME_OK) return SERVICE_ERROR;
    /*Encode data*/
    if(frame_encode_bytes(&frame,data, data_size) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    uint8_t  rsp_buf[32];
    uint32_t rsp_size;
    sr = service_execute( &frame, instance,                          
                          rsp_buf , sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_set_field(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_fpga_log(int32_t instance, FpgaRecord_t *records, size_t records_size, size_t *records_returned) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == records) {
        LOGE("records NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == records_returned) {
        LOGE("records_returned NULL!");
        return SERVICE_ERROR;
    }
    if(records_size <= 0) {
        LOGE("Invalid records_size [%d]!", records_size);
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, SERVICE_GET_FPGA_LOG) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[256];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* Decode the response bytes */
    *records_returned = 0; /* Initialize the records size to zero */
    int remaining = frame.data_size;
    const int SIZE_OF_RECORD = 9;
    for (int i = 0; (remaining >= SIZE_OF_RECORD) ; i++, remaining -= SIZE_OF_RECORD) {
        if(frame_decode32(&frame, &records[i].timeStamp) != FRAME_OK) return SERVICE_ERROR;
        if(frame_decode8(&frame, &records[i].b1) != FRAME_OK) return SERVICE_ERROR;
        if(frame_decode8(&frame, &records[i].b2) != FRAME_OK) return SERVICE_ERROR;
        if(frame_decode8(&frame, &records[i].b3) != FRAME_OK) return SERVICE_ERROR;
        if(frame_decode8(&frame, &records[i].reply) != FRAME_OK) return SERVICE_ERROR;
        if(frame_decode8(&frame, &records[i].result) != FRAME_OK) return SERVICE_ERROR;
        (*records_returned)++; /* We added a record, so increment number of returned records */
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_lock_oem_partition(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t partition_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id + MULTIHEAD_LOCK_OEM_PARTITION_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode the partition ID */
    if(frame_encode8(&frame, partition_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr; 
    sr = service_execute( &frame, instance,                          
                          response->data , sizeof(response->data), &response->res_size);                          
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_lock_oem_partition(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_start_purging(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(slot < 0 || slot >= NUM_TRENCH_PER_PH) {
        LOGE("Invalid Slot ID [%d]!", slot);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_PURGE_START_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;
    /* Encode slot */
    if(frame_encode8(&frame, slot) != FRAME_OK) return SERVICE_ERROR;

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
        _set_response_timeout(old_timeout);
        return SERVICE_ERROR;
    }
    _set_response_timeout(old_timeout);
    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_start_purging(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_get_state_info(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(ph_id < 0 || ph_id >= NUM_SUPPORTED_PH) {
        LOGE("Invalid Pen ID [%d]!", ph_id);
        return SERVICE_ERROR;
    }
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == response) {
        LOGE("response NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_STATE_INFO_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode PH ID */
    if(frame_encode8(&frame, ph_id) != FRAME_OK) return SERVICE_ERROR;

    ServiceResult_t sr;
    sr = service_execute( &frame, instance,                          
                          response->data, sizeof(response->data), &response->res_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* check if error bit is set in the response header */
    if(frame.ctrl & RESP_HEADER_ERROR_BITMASK) {
        LOGE("service_get_state_info(): Cannot be executed \n");
        return SERVICE_ERROR;
    }

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_date(int32_t instance, Headinfo_t *info, int32_t year, int32_t month, int32_t day) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_DATE_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Get/Set */
    if(frame_encode8(&frame,MULTIHEAD_DATE_SET) != FRAME_OK) return SERVICE_ERROR;
    /* encode values */
    if(frame_encode16(&frame, year) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, month) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, day) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[4];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t service_set_platform_info(int32_t instance, Headinfo_t *info, PlatformInfo_t *platform_info) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == info) {
        LOGE("info NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == platform_info) {
        LOGE("platform_info NULL!");
        return SERVICE_ERROR;
    }
    if(info->initialized != true) {
        LOGE("info not initialized!");
        return SERVICE_ERROR;
    }

    Frame_t         frame;
    uint8_t idx;
    if(frame_init(&frame, info->first_srvc_id+MULTIHEAD_GET_SET_PLATFORMINFO_IDX) != FRAME_OK) return SERVICE_ERROR;
    /* Encode Get/Set */
    if(frame_encode8(&frame,MULTIHEAD_PLATFORMINFO_SET) != FRAME_OK) return SERVICE_ERROR;
    for(idx = 0; idx < 12; idx++) {
        if(frame_encode8(&frame,platform_info->model[idx]) != FRAME_OK) return SERVICE_ERROR;
    }
    if(frame_encode16(&frame, platform_info->mfg_year) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, platform_info->mfg_woy) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, platform_info->mfg_country) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, platform_info->mfg_rev_major) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, platform_info->mfg_rev_minor) != FRAME_OK) return SERVICE_ERROR;
    if(frame_encode8(&frame, platform_info->orientation) != FRAME_OK) return SERVICE_ERROR;

    uint8_t  rsp_buf[4];
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          rsp_buf, sizeof(rsp_buf), &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}
