/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <unistd.h>
#include <string.h>
#include <time.h>

#include "max_common_types.h"
#include "uart_ifc.h"
#include "service.h"
#include "debug_log.h"
#include "file.h"
#include "srec.h"
#include "lock.h"
#include "ids_ifc.h"
#include "ids.h"
#include "ids_service.h"
#include "fw_reflash.h"
#include "oem_lib.h"

//static const char *IDS_MODULE_NAME = " IDS";
//#define IDS_DEBUG_LOG(level, ...) DEBUG_LOG(IDS_MODULE_NAME, level, __VA_ARGS__)

static IDSHandle_t _ids_handle[NUM_BLUR_INSTANCES];      /* Array of IDS handle structure */
static bool _is_lib_initialized = false;


/*
 * This function should be called to initialize the IDS
 * library before any other functions can be invoked.
 */
IDSResult_t ids_lib_init() {
    LOGI("Enter %s", __FUNCTION__);
    
    /* Initialize the handle structure */
    int i;
    for(i = 0; i < NUM_BLUR_INSTANCES; ++i)
    {
        _ids_handle[i].is_initialized = false;
    }
    
    /* Initialize oem lib */
    oem_lib_init();
    
    _is_lib_initialized = true;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

/* 
 * This function will cleanup the data structures. This is a place holder
 * for now.
 */
IDSResult_t ids_lib_shutdown() {
    LOGI("Enter %s", __FUNCTION__);
    
    _is_lib_initialized = false;
    oem_lib_shutdown();

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

/* 
 * Initializes the specified IDS instance.
 */
IDSResult_t ids_init(int32_t instance) {
    LOGI("Enter %s(instance=%d)", __FUNCTION__, instance);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    /* Initialize oem lib instance */
    oem_init(instance);

    /* TODO: Verify the response received is for the same request we have sent */
    /* TODO: Check if we are talking to IDS bulli device */
    ServiceResult_t sr = service_device_identification(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;
    
    _ids_handle[instance-1].is_initialized = true;

    LOGI("%s done", __FUNCTION__);
    
    return IDS_OK;
}

/* 
 * Shutdown specified Print Head Controller instance.
 */
IDSResult_t ids_shutdown(int32_t instance) {
    LOGI("Enter %s(instance=%d)", __FUNCTION__, instance);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
    
    oem_shutdown(instance);
    _ids_handle[instance-1].is_initialized = false;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

/*
 * Returns a string that describes the given error code.
 */
const char *ids_get_error_description(IDSResult_t r) {
    switch(r) {
        case IDS_OK:            return "Success";
        case IDS_ERROR:         return "IDS error";
        case IDS_IO_ERROR:      return "IO error";
        default:                return "Unknown error";
    }
    
    return "Unknown error";
}


static IDSResult_t _reflash_result_to_ids_result(ReflashResult_t r) {
    if (r == REFLASH_OK) return IDS_OK;
    else                 return IDS_ERROR;
}

IDSResult_t ids_micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//    MAX_ASSERT(NULL != fw_file_name);
    
    oem_lock(instance);
    ReflashResult_t r = micro_fw_reflash(instance, fw_file_name, verify, true);
    oem_unlock(instance);

    LOGI("%s done", __FUNCTION__);

    return _reflash_result_to_ids_result(r);
}

IDSResult_t ids_terminate_session(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_terminate_session(instance);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_pairing_delete(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = service_pairing_delete(instance);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

// (param1=pd_id, param2=ids_idx for IDS uC)
IDSResult_t ids_pairing(int32_t instance, int32_t step, int32_t param1, int32_t param2, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(step < 1 || step > 3) {
        LOGE("Invalid step [%d]!", step);
        return IDS_ERROR;
    }
    if(NULL == out) {
        LOGE("out NULL!");
        return IDS_ERROR;
    }
    if(NULL == out_size) {
        LOGE("out_size NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(step >= 1 && step <= 3);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != out);
//	MAX_ASSERT(NULL != out_size);

    oem_lock(instance);
    ServiceResult_t sr = service_pairing(instance, step, param1, param2, in, in_size, status, out, out_size, out_buffer_size);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_overrides(int32_t instance, int32_t blur_id, int32_t supply_idx, int32_t *status, uint8_t *overrides, int32_t *out_size, int32_t out_buffer_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == out_size) {
        LOGE("out_size NULL!");
        return IDS_ERROR;
    }
    if(NULL == overrides) {
        LOGE("overrides NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != overrides);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_overrides(instance, blur_id, supply_idx, status, overrides, out_size, out_buffer_size);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_ink_use(int32_t instance, int32_t pd_id, int32_t supply_idx, uint8_t *in, int32_t in_size, int32_t *status, int32_t *supply_status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == in) {
        LOGE("in NULL!");
        return IDS_ERROR;
    }
    if(NULL == status) {
        LOGE("status NULL!");
        return IDS_ERROR;
    }
    if(NULL == supply_status) {
        LOGE("supply_status NULL!");
        return IDS_ERROR;
    }
    if(NULL == out) {
        LOGE("out NULL!");
        return IDS_ERROR;
    }
    if(NULL == out_size) {
        LOGE("out_size NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//    MAX_ASSERT(NULL != out);
//    MAX_ASSERT(NULL != out_size);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_ink_use(instance, pd_id, supply_idx, in, in_size, status, supply_status, out, out_size, out_buffer_size);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_read_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == value) {
        LOGE("value NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != value);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_read_field(instance, supply_idx, field_id, value);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_read_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == value) {
        LOGE("value NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != value);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_read_oem_field(instance, supply_idx, field_id, value);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_read_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == str) {
        LOGE("str NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != str);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_read_string(instance, supply_idx, field_id, str_len, str);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_read_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t buffer_size, uint8_t *str) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == str) {
        LOGE("str NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != str);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_read_oem_string(instance, supply_idx, field_id, buffer_size, str);
    oem_unlock(instance);    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_write_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_write_field(instance, supply_idx, field_id, value);
    oem_unlock(instance);    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_write_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_write_oem_field(instance, supply_idx, field_id, value);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_write_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == str) {
        LOGE("str NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != str);
    
    oem_lock(instance);
    ServiceResult_t sr = ids_service_write_string(instance, supply_idx, field_id, str_len, str);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_write_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == str) {
        LOGE("str NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != str);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_write_oem_string(instance, supply_idx, field_id, str_len, str);
    oem_unlock(instance);
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_lock_partition(int32_t instance, int32_t supply_idx, int32_t partition_id) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_lock_partition(instance, supply_idx, partition_id);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_set_platform_info(int32_t instance, PlatformInfo_t *platform_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_set_platform_info(instance, platform_info);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_set_date(int32_t instance, int32_t year, int32_t month, int32_t day) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_set_date(instance, year, month, day);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_set_stall_insert_count(int32_t instance, int32_t supply_idx, int32_t count) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_set_stall_insert_count(instance, supply_idx, count);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_set_out_of_ink(int32_t instance, int32_t supply_idx) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_set_out_of_ink(instance, supply_idx);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_set_faulty(int32_t instance, int32_t supply_idx) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_set_faulty(instance, supply_idx);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_flush_smart_card(int32_t instance, int32_t supply_idx) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_flush_smart_card(instance, supply_idx);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_supply_status(int32_t instance, int32_t supply_idx, SupplyStatus_t *supply_status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == supply_status) {
        LOGE("supply_status NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != supply_status);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_supply_status(instance, supply_idx, supply_status);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_supply_status_detail(int32_t instance, int32_t supply_idx, SupplyStatusDetail_t *supply_status_detail) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == supply_status_detail) {
        LOGE("supply_status_detail NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != supply_status_detail);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_supply_status_detail(instance, supply_idx, supply_status_detail);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_supply_id(int32_t instance, int32_t supply_idx, SupplyID_t *supply_id) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == supply_id) {
        LOGE("supply_id NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != supply_id);
    
    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_supply_id(instance, supply_idx, supply_id);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_supply_info(int32_t instance, int32_t supply_idx, SupplyInfo_t *supply_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == supply_info) {
        LOGE("supply_info NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != supply_info);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_supply_info(instance, supply_idx, supply_info);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_mru_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *mru_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == mru_info) {
        LOGE("mru_info NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != mru_info);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_mru_info(instance, supply_idx, mru_info);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_get_first_install_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *first_install_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
    if(NULL == first_install_info) {
        LOGE("first_install_info NULL!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0 || instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);
//	MAX_ASSERT(NULL != first_install_info);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_first_install_info(instance, supply_idx, first_install_info);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

IDSResult_t ids_info(int32_t instance, IdsSysInfo_t *sys_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_get_sys_info(instance, sys_info);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

/* @@@ Test functions @@@ */
IDSResult_t ids_increment(int32_t instance, uint8_t in, uint8_t *out) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
        LOGE("Not initialized!");
        return IDS_ERROR;
    }
//    MAX_ASSERT(instance > 0);
//    MAX_ASSERT(instance <= NUM_BLUR_INSTANCES);
//    MAX_ASSERT(_is_lib_initialized == true);

    oem_lock(instance);
    ServiceResult_t sr = ids_service_increment(instance, in, out);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) return IDS_ERROR;

    LOGI("%s done", __FUNCTION__);

    return IDS_OK;
}

