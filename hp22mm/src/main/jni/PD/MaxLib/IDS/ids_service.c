/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

/* Implementation of IDS specific Bulli services */

#include <string.h>

#include "max_common_types.h"
#include "debug_log.h"
#include "service.h"
#include "frame.h"
#include "uart_ifc.h"
#include "ids_service.h"

//static const char *SRVC_MODULE_NAME = "SRVC";
//#define SRVC_DEBUG_LOG(level, ...) DEBUG_LOG(SRVC_MODULE_NAME, level, __VA_ARGS__)

/* Forward declaration */
ServiceResult_t service_execute(Frame_t             *frame,
                                 int32_t             instance,
                                 uint8_t             *rsp_buf,
                                 uint32_t            rsp_buf_size,
                                 uint32_t            *rsp_size
                                 );

ServiceResult_t _simple_command(int32_t instance, int32_t param, IdsService_t service, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);

//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;
    /* encode param */
	if(param >= 0) {
		if(frame_encode8(&frame, param) != FRAME_OK) return SERVICE_ERROR;
	}
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t _payload_command(int32_t instance, int32_t service, int32_t param1, int32_t param2, uint8_t *in, int32_t in_size, int32_t *status, int32_t *status2, uint8_t *out, int32_t *out_size, int32_t out_buffer_size, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);

//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) return SERVICE_ERROR;
    if(NULL == in) {
        LOGE("in NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == status) {
        LOGE("status NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == status2) {
        LOGE("status2 NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == out) {
        LOGE("out NULL!");
        return SERVICE_ERROR;
    }
    if(NULL == out_size) {
        LOGE("out_size NULL!");
        return SERVICE_ERROR;
    }

//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    uint8_t ui8;
	
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;
    /* encode parameters */
    if (param1 >= 0) {
		if(frame_encode8(&frame, param1) != FRAME_OK) return SERVICE_ERROR;
	}
    if (param2 >= 0) {
		if(frame_encode8(&frame, param2) != FRAME_OK) return SERVICE_ERROR;
	}
	/* if input payload exists (must be at least 2 bytes), encode that also */
	if (in_size >= 2) {
		if (frame_encode_bytes(&frame, in, in_size) != FRAME_OK) return SERVICE_ERROR;
	}
    _set_pairing_response_timeout();

    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    _set_default_response_timeout();

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(out_buffer_size < rsp_size) {
        LOGE("%d - %s(): Response size returned/maximum = %d/%d\n", instance, service_name, rsp_size, out_buffer_size);
        return SERVICE_ERROR;		
	}

    /* Decode status */
    if(rsp_size < 1) {
        LOGE("%d - %s(): No response status returned\n", instance, service_name);
        return SERVICE_ERROR;		
	}

    if(frame_decode8(&frame, &ui8) != FRAME_OK) return SERVICE_ERROR;
	*status = ui8;
	rsp_size--;
	
	/* If needed, decode status2 */
	if (status2 != NULL) {
		if(rsp_size < 1) {	// (NOTE: was decremented above)
			LOGE("%d - %s(): No response status2 returned\n", instance, service_name);
			return SERVICE_ERROR;		
		}
		if(frame_decode8(&frame, &ui8) != FRAME_OK) return SERVICE_ERROR;
		*status2 = ui8;
		rsp_size--;
	}
	
    /* Decode payload */ 
    if(rsp_size > 0) {
		if (frame_decode_bytes(&frame, out, rsp_size) != FRAME_OK) return SERVICE_ERROR;
	}
	*out_size = rsp_size;

    LOGI("%s done", __FUNCTION__);

	return SERVICE_OK;
}

ServiceResult_t ids_service_terminate_session(int32_t instance) {
	// currently IDS and Blur share the same value for IDS_TERMINATE_SESSION
	return _simple_command(instance, -1, IDS_TERMINATE_SESSION, "sevice_terminate_session");
}

ServiceResult_t service_pairing_delete(int32_t instance) {
	return _simple_command(instance, -1, SERVICE_PAIRING_DELETE, "service_pairing_delete");
}

ServiceResult_t service_pairing(int32_t instance, int32_t step, int32_t param1, int32_t param2, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
	char service_name[50];
	snprintf(service_name, 50, "ids_service_pairing step %d", step);
	return _payload_command(instance, (SERVICE_PAIRING_STEP_1 + step - 1), param1, param2, in, in_size, status, NULL, out, out_size, out_buffer_size, service_name);
}

ServiceResult_t ids_service_ink_use(int32_t instance, int32_t pd_id, int32_t supply_idx, uint8_t *in, int32_t in_size, int32_t *status, int32_t *supply_status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
	return _payload_command(instance, IDS_SERVICE_INK_USED, pd_id, supply_idx, in, in_size, status, supply_status, out, out_size, out_buffer_size, "ids_service_ink_use");
}

ServiceResult_t ids_service_get_overrides(int32_t instance, int32_t blur_id, int32_t supply_idx, int32_t *status, uint8_t *overrides, int32_t *out_size, int32_t out_buffer_size) {
  	return _payload_command(instance, IDS_SERVICE_GET_PHA_OVERRIDES, blur_id, supply_idx, NULL, 0, status, NULL, overrides, out_size, out_buffer_size, "ids_service_overrides");
}

ServiceResult_t _ids_service_read_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value, IdsService_t service, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == value) {
        LOGE("value NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;

    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode8(&frame, field_id) != FRAME_OK)
		return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* decode value */
    if (frame_decode32(&frame, value) != FRAME_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_read_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value) {
	return _ids_service_read_field(instance, supply_idx, field_id, value, IDS_SERVICE_READ_SC_FIELD, "ids_service_read_field");
}

ServiceResult_t ids_service_read_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value) {
	return _ids_service_read_field(instance, supply_idx, field_id, value, IDS_SERVICE_READ_OEM_FIELD, "ids_service_read_oem_field");
}

ServiceResult_t _ids_service_write_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value, IdsService_t service, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);
//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode8(&frame, field_id) != FRAME_OK ||
		frame_encode32(&frame, value) != FRAME_OK)
		return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_write_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value) {
	return _ids_service_write_field(instance, supply_idx, field_id, value, IDS_SERVICE_WRITE_SC_FIELD, "ids_service_write_field");
}

ServiceResult_t ids_service_write_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value) {
	return _ids_service_write_field(instance, supply_idx, field_id, value, IDS_SERVICE_WRITE_OEM_FIELD, "ids_service_write_eom_field");
}

ServiceResult_t _ids_service_read_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str, IdsService_t service, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == str) {
        LOGE("str NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != str);

    Frame_t         frame;
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode8(&frame, field_id) != FRAME_OK ||
		frame_encode8(&frame, str_len) != FRAME_OK)
		return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);
    if(sr != SERVICE_OK)return SERVICE_ERROR;

    /* Decode string */
	if (rsp_size > str_len) {
        LOGE("rsp_size > str_len!");
        return SERVICE_ERROR;
    }
    if (frame_decode_bytes(&frame, str, rsp_size) != FRAME_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

	return SERVICE_OK;
}

ServiceResult_t ids_service_read_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
	return _ids_service_read_string(instance, supply_idx, field_id, str_len, str, IDS_SERVICE_READ_SC_STRING, "ids_service_read_string");
}

ServiceResult_t ids_service_read_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t buffer_size, uint8_t *str) {
	return _ids_service_read_string(instance, supply_idx, field_id, buffer_size, str, IDS_SERVICE_READ_OEM_STRING, "ids_service_read_oem_string");
}

ServiceResult_t _ids_service_write_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str, IdsService_t service, char *service_name) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == str) {
        LOGE("str NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != str);

    Frame_t         frame;
    if(frame_init(&frame, service) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode8(&frame, field_id) != FRAME_OK ||
		frame_encode8(&frame, str_len) != FRAME_OK ||
		frame_encode_bytes(&frame, str, (uint16_t)str_len) != FRAME_OK)		// string
		return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_write_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
	return _ids_service_write_string(instance, supply_idx, field_id, str_len, str, IDS_SERVICE_WRITE_SC_STRING, "ids_service_write_string");
}

ServiceResult_t ids_service_write_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
	return _ids_service_write_string(instance, supply_idx, field_id, str_len, str, IDS_SERVICE_WRITE_OEM_STRING, "ids_service_write_oem_string");
}

ServiceResult_t ids_service_lock_partition(int32_t instance, int32_t supply_idx, int32_t partition_id) {
    LOGI("Enter %s", __FUNCTION__);

//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_LOCK_PARTITION) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode8(&frame, partition_id) != FRAME_OK) 
		return SERVICE_ERROR;
      
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_set_platform_info(int32_t instance, PlatformInfo_t *platform_info) {
    LOGI("Enter %s", __FUNCTION__);
//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_SET_PLATFORM_INFO) != FRAME_OK) return SERVICE_ERROR;
	/* encode params */
	if (frame_encode_bytes(&frame, (uint8_t*)platform_info->model, 12) != FRAME_OK ||
		frame_encode16(&frame, platform_info->mfg_year) != FRAME_OK ||
		frame_encode8(&frame, platform_info->mfg_woy) != FRAME_OK ||
		frame_encode8(&frame, platform_info->mfg_country) != FRAME_OK ||
		frame_encode8(&frame, platform_info->mfg_rev_major) != FRAME_OK ||
		frame_encode8(&frame, platform_info->mfg_rev_minor) != FRAME_OK)
		/* NOTE: platform_info->orientation is not used at this time */
		return SERVICE_ERROR;
	
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_set_date(int32_t instance, int32_t year, int32_t month, int32_t day) {
    LOGI("Enter %s", __FUNCTION__);
//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_SET_DATE) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode16(&frame, year) != FRAME_OK ||
		frame_encode8(&frame, month) != FRAME_OK ||
		frame_encode8(&frame, day) != FRAME_OK) 
		return SERVICE_ERROR;

    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_set_stall_insert_count(int32_t instance, int32_t supply_idx, int32_t count) {
    LOGI("Enter %s", __FUNCTION__);
//    MAX_ASSERT(instance > 0);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_SET_STALL_INSERT_COUNT) != FRAME_OK) return SERVICE_ERROR;
    /* encode params */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK ||
		frame_encode16(&frame, count) != FRAME_OK) 
		return SERVICE_ERROR;
	
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_set_out_of_ink(int32_t instance, int32_t supply_idx) {
	return _simple_command(instance, supply_idx, IDS_SERVICE_SET_OUT_OF_INK, "ids_service_set_out_of_ink");
}

ServiceResult_t ids_service_set_faulty(int32_t instance, int32_t supply_idx) {
	return _simple_command(instance, supply_idx, IDS_SERVICE_SET_FAULTY, "ids_service_set_faulty");
}

ServiceResult_t ids_service_flush_smart_card(int32_t instance, int32_t supply_idx) {
	return _simple_command(instance, supply_idx, IDS_SERVICE_FLUSH_SMART_CARD, "ids_service_flush_smart_card");
}

ServiceResult_t ids_service_get_supply_status(int32_t instance, int32_t supply_idx, SupplyStatus_t *supply_status) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == supply_status) {
        LOGE("supply_status NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != supply_status);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_SUPPLY_STATUS) != FRAME_OK) return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyStatus_t) != rsp_size) {
        LOGE("ids_service_get_supply_status(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyStatus_t));
        return SERVICE_ERROR;		
	}

	/* decode values */
	if (frame_decode8(&frame, &supply_status->state) != FRAME_OK ||
		frame_decode8(&frame, &supply_status->status_bits) != FRAME_OK ||
		frame_decode16(&frame, &supply_status->consumed_volume) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_get_supply_status_detail(int32_t instance, int32_t supply_idx, SupplyStatusDetail_t *supply_status_detail) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == supply_status_detail) {
        LOGE("supply_status_detail NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != supply_status_detail);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_SUPPLY_DETAIL) != FRAME_OK) return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyStatusDetail_t) != rsp_size) {
        LOGE("ids_service_get_supply_status_detail(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyStatusDetail_t));
        return SERVICE_ERROR;		
	}
  
	/* decode values */
	if (frame_decode16(&frame, &supply_status_detail->current_shelf_days) != FRAME_OK ||
		frame_decode16(&frame, &supply_status_detail->shelf_life_days) != FRAME_OK ||
		frame_decode16(&frame, &supply_status_detail->current_install_days) != FRAME_OK ||
		frame_decode16(&frame, &supply_status_detail->install_life_days) != FRAME_OK ||
		frame_decode8(&frame, &supply_status_detail->first_failure_code) != FRAME_OK ||
		frame_decode8(&frame, &supply_status_detail->last_failure_code) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_get_supply_id(int32_t instance, int32_t supply_idx, SupplyID_t *supply_id) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == supply_id) {
        LOGE("supply_id NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != supply_id);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_SUPPLY_ID) != FRAME_OK)  return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyID_t) != rsp_size) {
        LOGE("ids_service_get_supply_id(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyID_t));
        return SERVICE_ERROR;		
	}
 
	/* decode values */
	if (frame_decode8(&frame, &supply_id->mfg_site) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_line) != FRAME_OK ||
		frame_decode16(&frame, &supply_id->mfg_year) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_woy) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_dow) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_hour) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_min) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_sec) != FRAME_OK ||
		frame_decode8(&frame, &supply_id->mfg_pos) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

	return SERVICE_OK;
}

ServiceResult_t ids_service_get_supply_info(int32_t instance, int32_t supply_idx, SupplyInfo_t *supply_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == supply_info) {
        LOGE("supply_info NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != supply_info);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_SUPPLY_INFO) != FRAME_OK) return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyInfo_t) != rsp_size) {
        LOGE("ids_service_get_supply_info(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyInfo_t));
        return SERVICE_ERROR;		
	}

	/* decode values */
	if (frame_decode8(&frame, &supply_info->mfg_site) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_line) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->mfg_year) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_woy) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_dow) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_hour) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_min) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_sec) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->mfg_pos) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->sensor_gain) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->ink_density) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->usable_vol) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->insert_count) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->ext_oem_id) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->hp_oem_designate) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->formulator_id) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->ink_vehicle) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->ink_family) != FRAME_OK ||
		frame_decode16(&frame, &supply_info->ink_member) != FRAME_OK ||
		frame_decode8(&frame, &supply_info->ink_revision) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

	return SERVICE_OK;
}

ServiceResult_t ids_service_get_mru_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *mru_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == mru_info) {
        LOGE("mru_info NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != mru_info);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_MRU_INFO) != FRAME_OK) return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyUseInfo_t) != rsp_size) {
        LOGE("ids_service_get_mru_info(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyUseInfo_t));
        return SERVICE_ERROR;		
	}
 
	/* decode values */
	if (frame_decode16(&frame, &mru_info->year) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->woy) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->dow) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->HP_rev_major) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->HP_rev_minor) != FRAME_OK ||
		frame_decode16(&frame, &mru_info->stall_count) != FRAME_OK ||
		frame_decode_bytes(&frame, (uint8_t*)mru_info->platform_model, 12) != FRAME_OK ||
		frame_decode16(&frame, &mru_info->platform_mfg_year) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->platform_mfg_woy) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->platform_mfg_country) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->platform_mfg_rev_major) != FRAME_OK ||
		frame_decode8(&frame, &mru_info->platform_mfg_rev_minor) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_get_first_install_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *first_install_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == first_install_info) {
        LOGE("first_install_info NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != first_install_info);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_FIRST_INSTALL_INFO) != FRAME_OK) return SERVICE_ERROR;
    /* encode supply_idx */
    if (frame_encode8(&frame, supply_idx) != FRAME_OK) return SERVICE_ERROR;
        
    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	if(sizeof(SupplyUseInfo_t) != rsp_size)	{
        LOGE("ids_service_get_first_install_info(): Response size returned/expected = %d/%d\n", rsp_size, sizeof(SupplyUseInfo_t));
        return SERVICE_ERROR;		
	}
 
	/* decode values */
	if (frame_decode16(&frame, &first_install_info->year) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->woy) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->dow) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->HP_rev_major) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->HP_rev_minor) != FRAME_OK ||
		frame_decode16(&frame, &first_install_info->stall_count) != FRAME_OK ||
		frame_decode_bytes(&frame, (uint8_t*)first_install_info->platform_model, 12) != FRAME_OK ||
		frame_decode16(&frame, &first_install_info->platform_mfg_year) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->platform_mfg_woy) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->platform_mfg_country) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->platform_mfg_rev_major) != FRAME_OK ||
		frame_decode8(&frame, &first_install_info->platform_mfg_rev_minor) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

ServiceResult_t ids_service_get_sys_info(int32_t instance, IdsSysInfo_t *sys_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == sys_info) {
        LOGE("sys_info NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(sys_info != NULL);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_GET_SYS_INFO) != FRAME_OK) return SERVICE_ERROR;

    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);

    if(sr != SERVICE_OK) return SERVICE_ERROR;

	/* decode values */
	if (frame_decode8(&frame, &sys_info->fw_minor_rev) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->fw_major_rev) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->fpga_minor_rev) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->fpga_major_rev) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->board_rev_bd1) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->board_rev_bd0) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->board_rev_bd) != FRAME_OK ||
		frame_decode32(&frame, &sys_info->status) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->bootload_minor) != FRAME_OK ||
		frame_decode8(&frame, &sys_info->bootload_major) != FRAME_OK ||
		frame_decode16(&frame, &sys_info->board_id) != FRAME_OK)
		return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}

/* @@@ Test functions @@@ */

ServiceResult_t ids_service_increment(int32_t instance, uint8_t in, uint8_t *out) {
    LOGI("Enter %s", __FUNCTION__);

    if(NULL == out) {
        LOGE("out NULL!");
        return SERVICE_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(NULL != out);

    Frame_t         frame;
    if(frame_init(&frame, IDS_SERVICE_INCREMENT) != FRAME_OK)  return SERVICE_ERROR;
    /* encode value */
    if (frame_encode8(&frame, in) != FRAME_OK) return SERVICE_ERROR;

    uint32_t rsp_size;
    ServiceResult_t sr = service_execute( &frame, instance,
                          NULL, 0, &rsp_size);
    if(sr != SERVICE_OK) return SERVICE_ERROR;

    /* Decode the Value */
    if(frame_decode_bytes(&frame, out, rsp_size) != FRAME_OK) return SERVICE_ERROR;

    LOGI("%s done", __FUNCTION__);

    return SERVICE_OK;
}
   
