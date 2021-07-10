/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <unistd.h>
#include <string.h>

#include "max_common_types.h"
#include "print_head_driver_ifc.h"
#include "uart_ifc.h"
#include "service.h"
#include "debug_log.h"
#include "file.h"
#include "srec.h"
#include "extension.h"
#include "lock.h"
#include "print_head_driver.h"
#include "fw_reflash.h"
#include "oem_lib.h"

//static const char *PD_MODULE_NAME = "  PD";
//#define PD_DEBUG_LOG(level, ...) DEBUG_LOG(PD_MODULE_NAME, level, __VA_ARGS__)

/*Field indexes defined for OEM field access*/
#define MULTIHEAD_SERIAL_NUMBER_PART1_IDX   (TOTAL_NUMBER_OF_ID + 0)
#define MULTIHEAD_SERIAL_NUMBER_PART2_IDX   (TOTAL_NUMBER_OF_ID + 1)
#define SC_GAS_GAUGE_IDX                    (TOTAL_NUMBER_OF_ID + 2)
#define SC_DIE_USAGE_IDX                    (TOTAL_NUMBER_OF_ID + 3)
#define SC_STATUS_IDX                       (TOTAL_NUMBER_OF_ID + 4)
#define SC_INFO_IDX                         (TOTAL_NUMBER_OF_ID + 5)

/* Array of print head control handle structure */
PDHandle_t _pd_handle[NUM_BLUR_INSTANCES];
bool _is_lib_initialized = false;
Headinfo_t headcontrolinfo = {0,0,0,0,0,false};

/*
 * This function should be called to initialize the Print head control 
 * library before any other functions can be invoked.
 */
PDResult_t pd_lib_init() {
    LOGI("Enter %s", __FUNCTION__);
    
    /* Initialize the handle structure */
    int i;
    for(i = 0; i < NUM_BLUR_INSTANCES; ++i) {
        _pd_handle[i].is_initialized = false;
    }

    /* Initialize oem lib */
    oem_lib_init();
    _is_lib_initialized = true;

    LOGI("Exit %s", __FUNCTION__);

    return PD_OK;
}

/* 
 * This function will cleanup the data structures. This is a place holder
 * for now.
 */
PDResult_t pd_lib_shutdown() {
    LOGI("Enter %s", __FUNCTION__);
    
    _is_lib_initialized = false;
    oem_lib_shutdown();

    LOGI("Exit %s", __FUNCTION__);

    return PD_OK;
}

PDResult_t pd_supply_status(int32_t instance, uint8_t ph_id, uint8_t slot_id, uint8_t *in, uint16_t in_len, int32_t *status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }

    //Response_t response;
    oem_lock(instance);
    ServiceResult_t sr = pd_service_supply_status(instance, ph_id, slot_id, in, in_len, status);
    oem_unlock(instance);

    if(sr != SERVICE_OK) { return PD_ERROR; }

    LOGI("Exit %s", __FUNCTION__);

    return PD_OK;
}

// (param1=slot_bits, param2=ids_id for PD uC)
PDResult_t pd_pairing(int32_t instance, int32_t step, int32_t param1, int32_t param2, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(step < 1 || step > 3) return PD_ERROR;
//	if(NULL == out) return PD_ERROR;
//	if(NULL == out_size) return PD_ERROR;

    oem_lock(instance);
    ServiceResult_t sr = service_pairing(instance, step, param1, param2, in, in_size, status, out, out_size, out_buffer_size);
    oem_unlock(instance);

    if(sr != SERVICE_OK) { return PD_ERROR; }

    return PD_OK;  
}

PDResult_t pd_pairing_delete(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }

    oem_lock(instance);
    ServiceResult_t sr = service_pairing_delete(instance);
    oem_unlock(instance);

    if(sr != SERVICE_OK) { return PD_ERROR; }
    
    return PD_OK;
}

PDResult_t pd_terminate_session(int32_t instance) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }

    oem_lock(instance);
    ServiceResult_t sr = pd_service_terminate_session(instance);
    oem_unlock(instance);

    if(sr != SERVICE_OK) { return PD_ERROR; }
    
    return PD_OK;
}

PDResult_t pd_set_secure_overrides(int32_t instance, int32_t ph_id, int32_t slot_idx, uint8_t *in, int32_t in_size, int32_t *status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    oem_lock(instance);
    ServiceResult_t sr = pd_service_set_secure_overrides(instance, ph_id, slot_idx, in, in_size, status);
    oem_unlock(instance);
    
    if(sr != SERVICE_OK) { return PD_ERROR; }

    return PD_OK;  
}

PDResult_t pd_get_ph_status(int32_t instance, int32_t *status0, int32_t *status1) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
    if(NULL == status0) return PD_ERROR;
    if(NULL == status1) return PD_ERROR;

    /* No locking required in this function since pd_get_print_head_status()
     * already has locking in it.
     */
	// Get status of each printhead, returning boolean if valid-present or not.
	PrintHeadStatus phs;
	if(pd_get_print_head_status(instance, 0, &phs) != PD_OK) {
        return PD_ERROR;
    }
	*status0 = (phs.print_head_state != PH_STATE_NOT_PRESENT && phs.print_head_state != PH_STATE_INVALID);
    
	if(pd_get_print_head_status(instance, 1, &phs) != PD_OK) {
        return PD_ERROR;
    }
	*status1 = (phs.print_head_state != PH_STATE_NOT_PRESENT && phs.print_head_state != PH_STATE_INVALID);
    
    return PD_OK;  
}

/*
 *  Initializes the printhead control related info, that will be used for doing print head related services.
 *  @params : instance, info - pointer to the printhead info struct.
 *  @returns : 
 */
static PDResult_t _pd_init_head_info(int32_t instance,  Headinfo_t * info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
	if(info == NULL) return PD_ERROR;
	
	info->initialized = false;	
    
	Response_t res;
	ServiceResult_t sr =  service_check_extension( instance, EXTENSION_MULTIHEAD_ID, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
	/* if there is response to be processed..8 bytes of response is expected */
	if(res.res_size != 0 && res.res_size == 8) {
			/* decode the response data */
			info->first_srvc_id = res.data[0] ;
			info->rev = res.data[1];
			info->total_srvcs = res.data[2];
			info->total_evnts = res.data[3];
			/* decode event id */
			info->first_evnt_id = res.data[4];
			info->first_evnt_id |= res.data[5] << 8;
			info->first_evnt_id |= res.data[6] << 16;
			info->first_evnt_id |= res.data[7] << 24;
	} else {
		return PD_ERROR;
    }

    LOGD("MultiHeadControl info:  first srvc id = %d, rev = %d, tot srvcs = %d, tot evns = %d, first evnt id = %d\n",
                                        info->first_srvc_id, info->rev, info->total_srvcs, info->total_evnts, info->first_evnt_id);		

	info->initialized = true;	

	return PD_OK;
}

/* 
 * Initializes the specified Print Head Controller instance.
 */
PDResult_t pd_init(int32_t instance) {
    LOGI("Enter %s(instance=%d)", __FUNCTION__, instance);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }

    /* Lock starts here */
    oem_lock(instance);
    
    /* Initialize oem lib instance */
    oem_init(instance);

    /* TODO: Verify the response received is for the same request we have sent */
    ServiceResult_t sr = service_device_identification(instance);
    if(sr != SERVICE_OK) {
        /* Lock ends here. Unlock mutex is done here */
        oem_unlock(instance);     
        return PD_ERROR;
    }
    
    /* Init head control info struct */
    PDResult_t pr;
    pr = _pd_init_head_info( instance, &headcontrolinfo);
	if(PD_OK != pr) {
        /* do nothing */
    }
    
    _pd_handle[instance-1].is_initialized = true;

    /* Lock ends here. Unlock mutex is done here */
    oem_unlock(instance);
    
    return PD_OK;
}

/* 
 * Shutdown specified Print Head Controller instance.
 */
PDResult_t pd_shutdown(int32_t instance) {
    LOGI("Enter %s(instance=%d)", __FUNCTION__, instance);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }

    _is_lib_initialized = false;
    oem_shutdown(instance);    
    
    return PD_OK;
}

/*
 * Returns a string that describes the given error code.
 */
const char *pd_get_error_description(PDResult_t r) {
    switch(r) {
        case PD_OK:            return "Success";
        case PD_ERROR:         return "Print head error";
        case PD_ERROR_IO:      return "IO error";
        case PD_INVALID_FILE:  return "Invalid file";
        default:               return "Unknown error";
    }
    return "Unknown error";
}


uint8_t _pd_getResponseData8(Response_t *res, uint16_t offset) {
    return res->data[offset]; 
}
 
uint16_t _pd_getResponseData16(Response_t *res, uint16_t offset) {
    return (_pd_getResponseData8(res,offset) |  ((uint16_t) _pd_getResponseData8(res, (offset+1)) << 8)) ;
}
 
uint32_t _pd_getResponseData32(Response_t *res, uint16_t offset) {
    return (_pd_getResponseData16(res,offset) |  ((uint32_t) _pd_getResponseData16(res, (offset+2)) << 16)) ;
}

static PDResult_t _reflash_result_to_pd_result(ReflashResult_t r) {
    switch(r) {
        case REFLASH_OK:            return PD_OK;
        case REFLASH_ERROR:         return PD_ERROR;
        case REFLASH_INVALID_FILE:  return PD_INVALID_FILE;
        default: return PD_ERROR;        
    }
}

PDResult_t pd_micro_fw_reflash_no_reset(int32_t instance, const char *fw_file_name, bool verify) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(NULL == fw_file_name) return PD_ERROR;

    oem_lock(instance);
    ReflashResult_t r = micro_fw_reflash(instance, fw_file_name, verify, false);
    oem_unlock(instance);

    return _reflash_result_to_pd_result(r);
}

PDResult_t pd_micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(NULL == fw_file_name) return PD_ERROR;

    oem_lock(instance);
    ReflashResult_t r = micro_fw_reflash(instance, fw_file_name, verify, true);
    oem_unlock(instance);

    return _reflash_result_to_pd_result(r);
}

/*
 * Power on the print head
 */
PDResult_t pd_power_on(int32_t instance, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);
    LOGD("instance = %d, PH ID = %d\n", instance, ph_id);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_power_on(instance, &headcontrolinfo, ph_id);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        return PD_ERROR;
    } else {
        LOGI(" Power on success \n");
    }

    return PD_OK;
}

/* 
 * Power Off the Print Head
 */
PDResult_t pd_power_off(int32_t instance, uint8_t ph_id) {
    LOGI("Enter %s", __FUNCTION__);
    LOGD("instance = %d, PH ID = %d\n", instance, ph_id);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    ServiceResult_t sr;

    /* Lock starts here. Locking mutex is done here */
    oem_lock(instance);
    sr = service_power_off(instance, &headcontrolinfo, ph_id);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        return PD_ERROR;
    } else {
        LOGI(" Power off success \n");
    }

    return PD_OK;
}

PDResult_t pd_get_system_status(int32_t instance, PDSystemStatus *pd_system_status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
    if(NULL == pd_system_status) return PD_ERROR;

    ServiceResult_t sr;
    
    Response_t res;
    uint16_t fw_rev         = 0;
    uint16_t fpga_rev       = 0;
    uint16_t bootloader_rev = 0;
    uint32_t board_rev      = 0;
    uint16_t board_id       = 0;
    uint32_t sys_status;

    oem_lock(instance);
    sr = service_get_system_info(instance, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_system_status(): instance = %d\n", instance);
        return PD_ERROR;
    }
    
    if(res.res_size != 0 && res.res_size >= 12) {
		fw_rev 		= _pd_getResponseData16(&res,0);
		fpga_rev 	= _pd_getResponseData16(&res,2);
		board_rev 	= _pd_getResponseData32(&res,4);
		sys_status	= _pd_getResponseData32(&res,8);
        
        if(res.res_size > 12) {
            bootloader_rev = _pd_getResponseData16(&res,12);
        }
        
        if(res.res_size > 14) {
            board_id = _pd_getResponseData16(&res,14);
        }
    } else {
        return PD_ERROR;
    }
    
    pd_system_status->fw_rev_major 	        = (fw_rev >> 8) & 0xFF;
    pd_system_status->fw_rev_minor 	        = fw_rev & 0xFF;
    pd_system_status->boot_rev_major        = (bootloader_rev >> 8) & 0xFF;
    pd_system_status->boot_rev_minor        = bootloader_rev & 0xFF;
    pd_system_status->fpga_rev_major 	    = (fpga_rev >> 8) & 0xFF;
    pd_system_status->fpga_rev_minor 	    = fpga_rev & 0xFF;
    pd_system_status->blur_board_rev 	    = (board_rev & 0xFF000000)>>24;
    pd_system_status->driver_board0_rev     = (board_rev & 0x00FF0000)>>16;
    pd_system_status->driver_board1_rev     = (board_rev & 0x0000FF00)>>8;
    pd_system_status->pd_status             = sys_status;
    pd_system_status->board_id              = board_id;

    LOGD(" pd_get_system_status success .  FW-rev-major:%d FW-rev-minor:%d FPGA-rev-major:%d FPGA-rev-minor:%d\n",
											pd_system_status->fw_rev_major, pd_system_status->fw_rev_minor, pd_system_status->fpga_rev_major,pd_system_status->fpga_rev_minor);
    LOGD(" pd_get_system_status success .  BLR-bd-rev:%d DRV_bd0-rev:%d DRV_bd1_rev:%d System-status:%d\n",
											pd_system_status->blur_board_rev, pd_system_status->driver_board0_rev, pd_system_status->driver_board1_rev,pd_system_status->pd_status); 

    return PD_OK;
}

PDResult_t pd_get_print_head_status(int32_t instance, uint8_t ph_id , PrintHeadStatus *print_head_status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == print_head_status) return PD_ERROR;

    ServiceResult_t sr;
    
    Response_t res;
    uint32_t phstatus, pherrorstate;

    oem_lock(instance);
    sr = service_head_status(instance, &headcontrolinfo, ph_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_print_head_status(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    if(res.res_size != 0 && res.res_size == 8) {
        phstatus = _pd_getResponseData32(&res,0);
        pherrorstate = _pd_getResponseData32(&res,4);
    } else {
        LOGE("ERROR: pd_get_print_head_status(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    print_head_status->print_head_state = (phstatus & 0xFF);
    print_head_status->print_head_error = pherrorstate;
    print_head_status->energy_calibrated = ((phstatus >> 8) & 0x01) ? 1 : 0;
    print_head_status->temp_calibrated  = ((phstatus >> 8) & 0x02) ? 1 : 0;
    print_head_status->slot_a_purge_completed  = ((phstatus >> 8) & 0x04) ? 1 : 0;
    print_head_status->slot_b_purge_completed  = ((phstatus >> 8) & 0x08) ? 1 : 0;
    print_head_status->overdrive_warning        = ((phstatus >> 16) & 0x01) ? 1 : 0;
    print_head_status->overtemp_warning         = ((phstatus >> 16) & 0x02) ? 1 : 0;
    print_head_status->supplyexpired_warning    = ((phstatus >> 16) & 0x04) ? 1 : 0;

    LOGD("pd_get_print_head_status() : PH STATE: %d, PH ERROR: %d. E cal = %d\n", phstatus, pherrorstate, print_head_status->energy_calibrated);

    return PD_OK;
}
 
/*
 * 
 * 
 */
static PDResult_t _pd_init_fpgaflash_info(int32_t instance,Flashinfo_t * info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
	if(info == NULL) return PD_ERROR;
	
	Response_t res;
	ServiceResult_t sr = service_check_extension( instance, EXTENSION_SPIMASTER_ID, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
	/* if there is response to be processed..8 bytes of response is expected */
	if(res.res_size != 0 && res.res_size == 8) {
			/* decode the response data */
			info->first_srvc_id = res.data[0] ;
			info->rev = res.data[1];
			info->total_srvcs = res.data[2];
			info->total_evnts = res.data[3];
			/* decode event id */
			info->first_evnt_id = res.data[4];
			info->first_evnt_id |= res.data[5] << 8;
			info->first_evnt_id |= res.data[6] << 16;
			info->first_evnt_id |= res.data[7] << 24;
			
	} else {
        return PD_ERROR;
    }

    LOGD("  extspi first srvc id = %d, extspi rev = %d, extspi tot srvcs = %d, extspi tot evns = %d, extspi first evnt id = %d\n",
                                        info->first_srvc_id, info->rev, info->total_srvcs, info->total_evnts, info->first_evnt_id);		
	
	strcpy((char *)info->dev_name,SPIFLASHNAME);
	
	sr =  service_flash_devid(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;	
	
	/* if there is response to be processed...1 byte of response is expected */
	if(res.res_size != 0 && res.res_size == 1){
		/* decode the response data */
		info->dev_id = res.data[0];
    } else {
        return PD_ERROR;
    }

    LOGD("   fpgaflash devid = %d\n", info->dev_id);

	/* init all the object ids */
	info->objects[FLASH_ERASE].id	= (uint16_t)FLASH_ERASE_ID;
	info->objects[FLASH_WREN].id	= (uint16_t)FLASH_WREN_ID;
	info->objects[FLASH_GETSTS].id	= (uint16_t)FLASH_GETSTS_ID;
	info->objects[FLASH_PGPGM].id	= (uint16_t)FLASH_PGPGM_ID;
	info->objects[FLASH_RDBUF].id	= (uint16_t)FLASH_RDBUF_ID;
	info->objects[FLASH_RDID].id	= (uint16_t)FLASH_RDID_ID;
	info->objects[FLASH_WRSTS].id	= (uint16_t)FLASH_WRSTS_ID;
	info->objects[FLASH_RDPG].id	= (uint16_t)FLASH_RDPG_ID;
	
	/* get all the objects info */
	uint8_t objs;
	for(objs = 0; objs < MAX_FLASH_OBJECTS; objs++){
		sr = service_object_handling( instance, SUBSERVICE_OBJECT_LOCK, info->objects[objs].id, &res);
		if(sr != SERVICE_OK) return PD_ERROR;
		
		/* if there is response to be processed...8 bytes of response is expected */
		if(res.res_size != 0 && res.res_size == 8) {
			/* decode the response data */
			info->objects[objs].addr  = res.data[0];
			info->objects[objs].addr |= res.data[1] << 8;
			info->objects[objs].addr |= res.data[2] << 16;
			info->objects[objs].addr |= res.data[3] << 24;
			
			info->objects[objs].size = res.data[4];
			info->objects[objs].size |= res.data[5] << 8;
			info->objects[objs].size |= res.data[6] << 16;
			info->objects[objs].size |= res.data[7] << 24;
        } else {
            return PD_ERROR;
        }

        LOGD("  obj[%d], id=%d, addr=0x%x, size=%d\n",
                                        objs, info->objects[objs].id, info->objects[objs].addr, info->objects[objs].size);				
			
		sr = service_object_handling( instance, SUBSERVICE_OBJECT_UNLOCK, info->objects[objs].id, &res);
		if(sr != SERVICE_OK) return PD_ERROR;			
	}
		
    return PD_OK;
}

/*
 * 
 * 
 */
static PDResult_t _pd_fpgaflash_getstatus(int32_t instance,Flashinfo_t *info, uint8_t *status) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//        LOGE("Invalid Instance!");
//        return PD_ERROR;
//    }
	if(info == NULL) return PD_ERROR;
	if(status == NULL) return PD_ERROR;
	
	Response_t res;
	ServiceResult_t sr =  service_flash_rdstatus(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;	
	
	sr = service_read_data(instance, info->objects[FLASH_RDBUF].addr,1, &res);
    if(sr != SERVICE_OK) return PD_ERROR;	
	
	/* if there is response to be processed...1 byte of response is expected */
	if(res.res_size != 0 && res.res_size == 1){
		/* decode the response data */
		*status = res.data[0];
	} else {
        return PD_ERROR;
    }

    return PD_OK;	 
}
 
 /*
 * 
 * 
 */
static PDResult_t _pd_fpgaflash_erase(int32_t instance,Flashinfo_t * info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//     if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//         LOGE("Invalid Instance!");
//         return PD_ERROR;
//     }
//	MAX_ASSERT(info != NULL);

	Response_t res;
	ServiceResult_t sr =  service_flash_wrenbl(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;

	sr =  service_flash_erase(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
    return PD_OK;	 	
}
  /*
 * 
 * 
 */
static PDResult_t _pd_fpgaflash_writeprotect(int32_t instance,Flashinfo_t * info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
	if(info == NULL) return PD_ERROR;

	Response_t res;
	ServiceResult_t sr =  service_flash_wrenbl(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
	uint8_t statusdata = FLASH_WP_MASK; /* 0x1C */
	sr =  service_write_data(instance, info->objects[FLASH_WRSTS].addr+1, &statusdata, sizeof(statusdata));
    if(sr != SERVICE_OK) return PD_ERROR;
	
	sr =  service_flash_wrstatus( instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
    return PD_OK;
}
  /*
 * 
 * 
 */
static PDResult_t _pd_fpgaflash_remove_wp(int32_t instance,Flashinfo_t * info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
    if(info == NULL) return PD_ERROR;

	Response_t res;
	ServiceResult_t sr =  service_flash_wrenbl(instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
	uint8_t statusdata = 0x0; 
	sr =  service_write_data(instance, info->objects[FLASH_WRSTS].addr+1, &statusdata, sizeof(statusdata));
    if(sr != SERVICE_OK) return PD_ERROR;
	
	sr =  service_flash_wrstatus( instance, info, &res);
    if(sr != SERVICE_OK) return PD_ERROR;
	
    return PD_OK;
}
/*
 * 
 *  Data input to this should include 3 bytes of address at the starting.
 *  Data size to this should include 3 bytes of address at the starting. 
 */
static PDResult_t _pd_fpgaflash_program(int32_t instance,Flashinfo_t * info, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
	if(info == NULL) return PD_ERROR;
//	MAX_ASSERT(data != NULL);
//	MAX_ASSERT(data_size > 0);
	
	Response_t res;
	ServiceResult_t sr =  service_flash_wrenbl(instance, info, &res);
    if(sr != SERVICE_OK) {
        LOGE(" service_flash_wrenbl ERROR  \n");
         return PD_ERROR;
    }
	 
	sr =  service_write_data(instance, info->objects[FLASH_PGPGM].addr+1, data, data_size);
    if(sr != SERVICE_OK) {
        LOGE(" service_write_data ERROR  \n");
         return PD_ERROR;
    }
	
	sr =  service_flash_pgm(instance, info, &res, data_size);
    if(sr != SERVICE_OK) {
        LOGE(" service_flash_pgm ERROR  \n");
         return PD_ERROR;	
    }

    return PD_OK;
}
 
 /*
 *  Data input to this should include 3 bytes of address at the starting.
 *  Data size to this should include 3 bytes of address at the starting. 
 */
static PDResult_t _pd_fpgaflash_read(int32_t instance,Flashinfo_t * info, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
	if(info == NULL) return PD_ERROR;
//	MAX_ASSERT(data != NULL);
//	MAX_ASSERT(data_size > 3);
	
	Response_t res;
				
	/* write read address into read page object - 3bytes */
	ServiceResult_t sr =  service_write_data(instance, info->objects[FLASH_RDPG].addr+1, data, 3);
    if(sr != SERVICE_OK) return PD_ERROR;	
	
	/* initiate read */
	sr =  service_flash_read(instance,info, &res, data_size);
    if(sr != SERVICE_OK) {
        LOGE(" SERVICE FLASH READ ERROR  \n");
        return PD_ERROR;
     }
    
    /* sr = service_read_data(instance, info->objects[FLASH_RDBUF].addr+1, data_size-3,&res); */
    sr = service_read_data(instance, info->objects[FLASH_RDBUF].addr, data_size-3,&res);
    if(sr != SERVICE_OK) return PD_ERROR;
    
	/* if there is response to be processed...1 byte of response is expected */
	if(res.res_size != 0 && res.res_size == data_size-3){
        LOGD("Successfully READ - %d bytes \n",res.res_size);
		/* decode the response data..overwrite the first 3 address bytes with data */
        size_t no;
		for(no=0; no < (data_size-3); no++){
			*(data+no) = res.data[no];
		}
	} else {
        return PD_ERROR;
    }

    return PD_OK;
}
 
/*
 * 
 *  Data input to this should include 3 bytes of address at the starting.
 *  Data size to this should include 3 bytes of address at the starting. 
 */
static PDResult_t _pd_fpgaflash_program_verify(int32_t instance,Flashinfo_t * info, uint8_t *data, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
	if(info == NULL) return PD_ERROR;
	if(data == NULL) return PD_ERROR;
	if(data_size <= 0) return PD_ERROR;
	
	PDResult_t pr = _pd_fpgaflash_program(instance, info, data, data_size);
    if(PD_OK != pr) return pr;
    
	/* check fpgaflash status */
	while(1) {
		/* check fpgaflash status */
		uint8_t status;
		pr = _pd_fpgaflash_getstatus( instance, info, &status);
		if(PD_OK != pr) return pr;
		if( !(status & FLASH_WIP_BIT) ) break;
	}
	
	uint8_t rddata[data_size];
	/* put 3 bytes of address from which data needs to be read */
	rddata[0] = *(data+0);
	rddata[1] = *(data+1);
	rddata[2] = *(data+2);
	/* read back */
	pr = _pd_fpgaflash_read( instance,info, rddata, data_size);
	if(PD_OK != pr) return pr;

	/* verify */
    size_t no;
	for(no=0;no < (data_size-3);no++) {
        if(*(data+no+3) != rddata[no]) {
            LOGE("pd_fpgafw_reflash(): Verify failure @ dataoffset: %d\n", no);
            return PD_ERROR;
        }
	}

    return PD_OK;
}
 /*
 * 
 *  
 */
static PDResult_t _pd_fpgaflash_validate_addr(int32_t instance, uint32_t addr, size_t data_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
	if(data_size <= 0) return PD_ERROR;
	
	uint8_t addr_distance = 0;
	if( (addr & (FLASH_PAGE_SIZE-1) ) != 0X0 ) {
		addr_distance = (addr % FLASH_PAGE_SIZE);
		if(  ((uint32_t) data_size + addr_distance) <= FLASH_PAGE_SIZE ) return PD_OK;
	} else if(data_size <= FLASH_PAGE_SIZE) {
        return PD_OK;
    }

    return PD_FLASH_ADR_RO;
}
 
 /*
 * 
 * 
 */
static PDResult_t _pd_write_fpgafw(int32_t instance, Flashinfo_t *info, const char *fw_file_name, bool verify) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}

    uint32_t pages = 0;
    uint32_t lines = 0;
     
    /* Open the file */
    void *file_handle = file_open(fw_file_name);
    if(file_handle == NULL) {
        LOGE("Failed to open the file : %s\n", fw_file_name);
        return PD_ERROR;
    }  
    
    /* Iterate through the records and program the fpga flash */

    SrecResult_t sr;
    
    int len = 0; 
    char buf[MAX_SREC_RECORD_SIZE]; /* TODO: Change to const */
        
    uint8_t srec_data[MAX_SREC_DATA_SIZE+3]; /* 3 extra is to add 3 bytes of address before sending it for flashing */
    size_t srec_data_len = 0;
    uint32_t address;
        
    uint32_t srec_lines_per_page = 8;
    uint8_t page_data[256+3];
    uint32_t page_offset = 0;
    uint32_t page_data_count = 0;
    uint32_t page_address = 0;
    uint32_t srec_lines;
    uint32_t srec_data_offset;
    PDResult_t pr;
    uint8_t last_page = 0;
        
    while(1) {
        page_address = 0;
        page_offset = 3;
        page_data_count = 0;
        // process 8 srec lines to get a page full of data
        for(srec_lines = 1; srec_lines <= srec_lines_per_page ;  ) {
            len = file_read_line(file_handle, buf, sizeof(buf));
            if(len <= 0) {
                last_page = 1;
                break; /* End of file */
            }
        
            /* parse the records */
            sr = srec_parse_line(buf, &address, &srec_data[0], sizeof(srec_data), &srec_data_len);
            
            /* If this was a file header skip it */
            if(sr == SREC_OK_RECORD_HEADER) {
                srec_lines = 1;
                continue;
            }
            if(sr < 0) {
                LOGE("SREC parsing failure : %d\n", sr);
                return PD_INVALID_FILE;
            }

            /* validate address and data */
            pr = _pd_fpgaflash_validate_addr( instance, address, srec_data_len);
            if(PD_OK != pr) return pr;

            if(srec_lines == 1 ) {
                 page_address = address;
            }

            /* valid srec line. So increment lines */
            lines++;            
            
            /* copy data into page buffer */
            for( srec_data_offset  = 0; (srec_data_offset < srec_data_len)&& (page_offset < (256+3)); srec_data_offset++ ) {
                page_data[page_offset]=srec_data[srec_data_offset];
                page_offset++;
                page_data_count++;
            }

            srec_lines++;
        }

        /* if there is page data */   
        if( page_offset != 3 ) {
            /* get the 3 bytes of address and put it in the first 3 bytes of buffer to be sent */
            page_data[0] = (page_address & 0x00FF0000) >> 16;
            page_data[1] = (page_address & 0x0000FF00) >> 8;
            page_data[2] = (page_address & 0x000000FF) ;

            while(1) {
                /* check fpgaflash status */
                uint8_t status;
                pr = _pd_fpgaflash_getstatus( instance, info, &status);
                if(PD_OK != pr) return pr;
                if( !(status & FLASH_WIP_BIT) ) break;
            }

            LOGD("PROGRAMMING addr - 0x%x , PROGRAMMING len - %d over\n",page_address , page_data_count);
        
            if(!verify) { /* Program flash */
                pr = _pd_fpgaflash_program( instance, info, &page_data[0], (page_data_count+ 3) );
                if(PD_OK != pr) return pr;
            } else {
                /* program flash and verify */
                pr = _pd_fpgaflash_program_verify(instance, info, &page_data[0], (page_data_count+ 3));
                if(PD_OK != pr) return pr;
            }            
            
            pages++;
            if(last_page) break;
        }
    }
    
    /* Close the file */
    file_close(file_handle);
    
    return PD_OK;    
}
 
 
/*
 *  set or clear the fpga chip reset
 * 
 */
static PDResult_t _pd_fpga_setreset(int32_t instance, bool highorlow ) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}

    ServiceResult_t sr = service_fpga_set_chipreset(instance, highorlow);
    if(sr != SERVICE_OK) return PD_ERROR; 
    
    return PD_OK;
}

#define FPGA_FLASH_SIZE (2 * 1024 *1024)
/*
 *  Reprogram the fpga flash with the given firmware file
 * 
 */
PDResult_t pd_fpga_fw_reflash(int32_t instance, const char *fw_file_name, bool verify) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
    MAX_ASSERT(NULL != fw_file_name);

    LOGD("instance = %d, file name = %s\n", instance, fw_file_name);

    uint32_t first_address = 0;
    uint32_t total_size = 0;

    /* Validate the firmware file and extract the starting address and size */
    oem_lock(instance);
    ReflashResult_t r = validate_srec(fw_file_name, &first_address, &total_size);

    if(REFLASH_OK != r) {
        oem_unlock(instance);
        return _reflash_result_to_pd_result(r);
    }
	
    /* Verify start address and size */
    if((first_address != 0) || (total_size > FPGA_FLASH_SIZE)) {
        oem_unlock(instance);
        LOGE("ERROR: Invalid file. First Address = 0x%x, size = %d\n",
                        first_address, total_size);
        return PD_INVALID_FILE;
    }
    
	/* initialize fpgaflash info */
	Flashinfo_t fpgaflashinfo;
    PDResult_t pr = _pd_init_fpgaflash_info( instance, &fpgaflashinfo);
    if(PD_OK != pr) {
        oem_unlock(instance);
        return pr;
    }

    /* Put FPGA in reset mode before starting reflash. */
    pr = _pd_fpga_setreset( instance, 0);
    if(PD_OK != pr) {
        oem_unlock(instance);
        return pr;
    }

    LOGI("  Checking for write protect..\n");
		
	/* check for write protect. */
    uint32_t  try = 0;
	uint8_t status = 0;
	pr = _pd_fpgaflash_getstatus( instance, &fpgaflashinfo, &status);
	if(PD_OK != pr) {
    	oem_unlock(instance);
    	return pr;
    }
	if(status & FLASH_WP_MASK) {
        LOGI("FPGA flash is write protected. Trying to write enable..\n");
						
		/* fpga flash is write protected...try removing it */
        while(1) {
            pr = _pd_fpgaflash_remove_wp( instance, &fpgaflashinfo);
            if(PD_OK != pr) {
                oem_unlock(instance);
                return pr;
            }
            try++;
            
            /* ensure  write protect removal */
            status = 0xff;
            pr = _pd_fpgaflash_getstatus( instance, &fpgaflashinfo, &status);
            if(PD_OK != pr) {
                oem_unlock(instance);
                return pr;
            }
            if(status & FLASH_WP_MASK) {
                if( try >= 3) {
                    LOGE("FPGA flash is write protected. Cannot flash FW.\n");
                    /* might be h/w write protected. Can't program flash */
                    oem_unlock(instance);
                    return PD_FLASH_WP;
                }
            } else {
                LOGI("  FPGA flash is write enabled.\n");
                break;
            }
        }
	} else {
        LOGI("  FPGA flash is not write protected..\n");
    }
	
    /* Erase fpga flash */
    pr = _pd_fpgaflash_erase( instance, &fpgaflashinfo);
	if(PD_OK != pr) {
    	oem_unlock(instance);
    	return pr;
	}
	/* get the status to check whether erase is over */
	while(1) {
        sleep(1);  /* in secs */
        pr = _pd_fpgaflash_getstatus( instance, &fpgaflashinfo, &status);
        if(PD_OK != pr) {
            oem_unlock(instance);
            return pr;
        }
        LOGD(" FPGA flash: Status - 0x%x\n",status);
        if( !(status & FLASH_WIP_BIT) ) break;
	}
   
    /* Write the firmware binary to flash */
	pr =_pd_write_fpgafw( instance, &fpgaflashinfo, fw_file_name, verify);
    if(PD_OK != pr) {
        oem_unlock(instance);
        return pr;
    }

    LOGI(" Pgmng over.Write protecting fpga flash..\n");
    
	/* write protect the fpga flash */
	pr = _pd_fpgaflash_writeprotect( instance,&fpgaflashinfo);
    if(PD_OK != pr) {
        oem_unlock(instance);
        return pr;
    }
    
    /* Enable FPGA */
    pr = _pd_fpga_setreset( instance, 1);
    if(PD_OK != pr) {
        oem_unlock(instance);
        return pr;
    }

    LOGI("Release FPGA reset..\n");

    oem_unlock(instance);
    
    return PD_OK;
}

PDResult_t pd_calibrate_pulsewidth(int32_t instance, uint8_t ph_id, uint8_t slot, uint32_t *calibrated_fpwidth) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
//    MAX_ASSERT(slot < NUM_TRENCH_PER_PH );
	if(NULL == calibrated_fpwidth) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_calibrate_pulsewidth(instance, &headcontrolinfo, ph_id, slot, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_calibrate_pulsewidth(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
     
    
    /* if there is response to be processed..4 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 4) {
        *calibrated_fpwidth = _pd_getResponseData32(&res,0);
    } else {
        LOGE("ERROR: pd_calibrate_pulsewidth(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_calibrate_pulsewidth(): instance = %d, Pen ID = %d, FP Calibrated Firing Pulse Width = %d\n", instance, ph_id, *calibrated_fpwidth);

    return PD_OK;
}


PDResult_t pd_get_dropsvolume(int32_t instance, uint8_t ph_id, uint8_t slot, float *dropsvolume) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
//    MAX_ASSERT(slot < NUM_TRENCH_PER_PH );
	if(NULL  == dropsvolume) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_used_ink_weight_unsecured(instance, &headcontrolinfo, ph_id, slot, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_dropsvolume(): instance = %d\n", instance);
        return PD_ERROR;
    } else {
        LOGI(" pd_get_dropsvolume() success \n");
    }

    /* if there is response to be processed..4 byte of response is expected */
    if(res.res_size != 0 && res.res_size == 4) {
        // Reponse is drop volume multiplied by 1000.
        *dropsvolume = _pd_getResponseData32(&res,0)/1000.0;
    } else {
        LOGE("ERROR: pd_get_dropsvolume(): instance = %d\n", instance);
        return PD_ERROR;
    }

    LOGD("pd_get_dropsvolume(): instance = %d, PH Count = %f\n", instance, *dropsvolume);

    return PD_OK;
}

PDResult_t pd_get_used_ink_weight(int32_t instance, uint8_t ph_id, uint8_t slot, float *ink_weight) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
//    MAX_ASSERT(slot < NUM_TRENCH_PER_PH );
	if(NULL == ink_weight) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_used_ink_weight_unsecured(instance, &headcontrolinfo, ph_id, slot, &res);
    oem_unlock(instance);
    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_dropsvolume(): instance = %d\n", instance);
        return PD_ERROR;
    } else {
        LOGI(" pd_get_dropsvolume() success \n");
    }
       
    /* if there is response to be processed..4 byte of response is expected */
    if(res.res_size != 0 && res.res_size == 4) {
        // Reponse is drop volume multiplied by 1000.
        *ink_weight = _pd_getResponseData32(&res,0)/1000.0;
    } else {
        LOGE("ERROR: pd_get_dropsvolume(): instance = %d\n", instance);
        return PD_ERROR;
    }

    LOGD("pd_get_dropsvolume(): instance = %d, PH Count = %f\n", instance, *ink_weight);

    return PD_OK;
}

PDResult_t pd_ink_use(int32_t instance, uint8_t ph_id, uint8_t slot_id, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
    if(NULL == out) return PD_ERROR;
    if(NULL == out_size) return PD_ERROR;

    Response_t response;

    /* Lock starts here. Locking mutex is done here */
    oem_lock(instance);
    ServiceResult_t sr = service_get_used_ink_weight(instance, ph_id, slot_id, &response);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR pd_ink_use(inst=%d pen=%d slot=%d) returned sr=%d\n", instance, (int)ph_id, (int)slot_id, (int)sr);
		return PD_ERROR; 
	}
   
    *status = response.data[0];
    memcpy(out, &response.data[1], (response.res_size-1));
    *out_size = response.res_size - 1;
	
    return PD_OK;
}

PDResult_t pd_get_temperature(int32_t instance, uint8_t ph_id, uint32_t *degree_c) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == degree_c) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_actual_temp(instance, &headcontrolinfo, ph_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_temperature(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
       
    /* if there is response to be processed..4 byte of response is expected */
    if(res.res_size != 0 && res.res_size == 4) {
        *degree_c= _pd_getResponseData32(&res,0);
    } else {
        LOGE("ERROR: pd_get_temperature(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_get_temperature(): instance = %d, Pen ID = %d, ACTUAL_TEMP = %d\n", instance, ph_id, *degree_c);

    return PD_OK;
}

PDResult_t pd_set_temperature_override(int32_t instance, uint8_t ph_id, uint8_t t) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_temperature_override(instance, &headcontrolinfo, ph_id, t);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_set_temperature_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    return PD_OK;
}

PDResult_t pd_get_temperature_override(int32_t instance, uint8_t ph_id, uint8_t *temp) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//	if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == temp) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_temperature_override(instance, &headcontrolinfo, ph_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_temperature_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    /* if there is response to be processed..1 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 1) {
        *temp = _pd_getResponseData8(&res,0);
    } else {
        LOGE("ERROR: pd_get_temperature_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_get_temperature_override(): instance = %d, Pen ID = %d, Oveeride temperature = %d\n", instance, ph_id, *temp);

    return PD_OK;   
}

PDResult_t pd_set_voltage_override(int32_t instance, uint8_t ph_id, uint8_t voltage) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_voltage_override(instance, &headcontrolinfo, ph_id, voltage);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_set_voltage_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    return PD_OK;
}

PDResult_t pd_get_voltage_override(int32_t instance, uint8_t ph_id, uint8_t *voltage) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//	if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == voltage) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_voltage_override(instance, &headcontrolinfo, ph_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_voltage_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    /* if there is response to be processed..1 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 1) {
        *voltage = _pd_getResponseData8(&res,0);
    } else {
        LOGE("ERROR: pd_get_voltage_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_get_voltage_override(): instance = %d, Pen ID = %d, Override voltage = %d\n", instance, ph_id, *voltage);

    return PD_OK;   
}

PDResult_t pd_set_over_energy_override(int32_t instance, uint8_t ph_id, uint8_t slot, uint8_t energy) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
//	MAX_ASSERT(slot < NUM_TRENCH_PER_PH );

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_over_energy_override(instance, &headcontrolinfo, ph_id, slot, energy);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_set_over_energy_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    return PD_OK;
}

PDResult_t pd_get_over_energy_override(int32_t instance, uint8_t ph_id,  uint8_t slot, uint8_t *energy) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
    MAX_ASSERT(NULL != energy);
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
//	MAX_ASSERT(slot < NUM_TRENCH_PER_PH );

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_over_energy_override(instance, &headcontrolinfo, ph_id, slot, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_over_energy_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    /* if there is response to be processed..1 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 1) {
        *energy = _pd_getResponseData8(&res,0);
    } else {
        LOGE("ERROR: pd_get_over_energy_override(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_get_over_energy_override(): instance = %d, Pen ID = %d, slot = %d, override over energy = %d\n", instance, ph_id, slot, *energy);

    return PD_OK;
}

PDResult_t pd_get_pen_serial_number(int32_t instance, uint8_t ph_id, char *serial_number) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == serial_number) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, MULTIHEAD_SERIAL_NUMBER_PART1_IDX, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_pen_serial_number(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    uint32_t serial1;
    uint32_t serial2;
    char serial_num[20] = "Serial Number";
    /* if there is response to be processed..4 bytes of response is expected */
    if(res.res_size != 0 && res.res_size >= 4) {
        serial1 = _pd_getResponseData32(&res,0);
        sprintf(serial_num,"%u",serial1);
        strcpy(serial_number,serial_num);
    } else {
        LOGE("ERROR: pd_get_pen_serial_number(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, MULTIHEAD_SERIAL_NUMBER_PART2_IDX, &res);
    oem_unlock(instance);
    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_pen_serial_number(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..4 bytes of response is expected */
    if(res.res_size != 0 && res.res_size >= 4) {
        serial2 = _pd_getResponseData32(&res,0);
        sprintf(serial_num,"%u",serial2);
        strcat(serial_number,serial_num);
    } else {
        LOGE("ERROR: pd_get_pen_serial_number(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_get_pen_serial_number(): instance = %d, Pen ID = %d, serial1 = %d, serial2 = %d\n", instance, ph_id, serial1, serial2);
    
    return PD_OK;
}

PDResult_t pd_get_fpga_log(int32_t instance, FpgaRecord_t *records, size_t records_size, size_t *records_returned) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
    if(records == NULL) return PD_ERROR;
    if(records_returned == NULL) return PD_ERROR;

    oem_lock(instance);
    ServiceResult_t sr = service_get_fpga_log(instance, records, records_size, records_returned);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_fpga_log(): instance = %d\n", instance);
        return PD_ERROR;
    }

    return PD_OK;
}

PDResult_t pd_sc_read_oem_field(int32_t instance, uint8_t ph_id, PDSmartCardOemFieldId_t field_id, uint32_t *data, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == data) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id,(uint8_t) field_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_read_oem_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..5 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 5) {
        *data = _pd_getResponseData32(&res,0);
        *sc_result = _pd_getResponseData8(&res,4);
    } else {
        LOGE("ERROR: pd_sc_read_oem_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_read_oem_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
    
    return PD_OK;
}

PDResult_t pd_sc_get_gas_gauge(int32_t instance, uint8_t ph_id, uint32_t *gas_gauge, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == gas_gauge) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, SC_GAS_GAUGE_IDX, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_get_gas_gauge(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..5 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 5) {
        *gas_gauge = _pd_getResponseData32(&res,0);
        *sc_result = _pd_getResponseData8(&res,4);
    } else {
        LOGE("ERROR: pd_sc_get_gas_gauge(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_get_gas_gauge(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;
}

PDResult_t pd_sc_get_die_usage(int32_t instance, uint8_t ph_id, PDSmartCardDieUsage *die_usage, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == die_usage) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, SC_DIE_USAGE_IDX, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_get_die_usage(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..9 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 9) {
        die_usage->slot_a_high = _pd_getResponseData16(&res,0);
        die_usage->slot_a_low = _pd_getResponseData16(&res,2);
        die_usage->slot_b_high = _pd_getResponseData16(&res,4);
        die_usage->slot_b_low = _pd_getResponseData16(&res,6);
        *sc_result = _pd_getResponseData8(&res,8);
    } else {
        LOGE("ERROR: pd_sc_get_die_usage(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_get_die_usage(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;
}

PDResult_t pd_sc_lock_oem_partition(int32_t instance, uint8_t ph_id, uint8_t partition_id, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_lock_oem_partition(instance, &headcontrolinfo, ph_id, partition_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_lock_oem_partition(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..1 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 1) {
        *sc_result = _pd_getResponseData8(&res,0);
    } else {
        LOGE("ERROR: pd_sc_lock_oem_partition(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_lock_oem_partition(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;
}

PDResult_t pd_sc_write_oem_field(int32_t instance, uint8_t ph_id, PDSmartCardOemFieldId_t field_id, uint32_t data, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_field(instance, &headcontrolinfo, ph_id, field_id, (uint8_t *)&data, sizeof(data));
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_write_oem_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_write_oem_field(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;
}

PDResult_t pd_sc_get_status(int32_t instance, uint8_t ph_id, PDSmartCardStatus *sc_status, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;
    if(NULL == sc_status) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, SC_STATUS_IDX, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_get_status(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..15 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 15) {
        sc_status->out_of_ink                       = _pd_getResponseData8(&res,0);
        sc_status->purge_complete_slot_a            = _pd_getResponseData8(&res,1);
        sc_status->purge_complete_slot_b            = _pd_getResponseData8(&res,2);
        sc_status->altered_ph_detected_slot_a       = _pd_getResponseData8(&res,3);
        sc_status->altered_ph_detected_slot_b       = _pd_getResponseData8(&res,4);
        sc_status->altered_supply_detected_slot_a   = _pd_getResponseData8(&res,5);
        sc_status->altered_supply_detected_slot_b   = _pd_getResponseData8(&res,6);
        sc_status->faulty_replace_immediately       = _pd_getResponseData8(&res,7);
        sc_status->pen_short_detected_slot_a        = _pd_getResponseData8(&res,8);
        sc_status->pen_short_detected_slot_b        = _pd_getResponseData8(&res,9);
        sc_status->expired_supply_slot_a            = _pd_getResponseData8(&res,10);
        sc_status->expired_supply_slot_b            = _pd_getResponseData8(&res,11);
        sc_status->crtdg_insertion_count            = _pd_getResponseData8(&res,12);
        sc_status->last_failure_code                = _pd_getResponseData8(&res,13);
        
        *sc_result = _pd_getResponseData8(&res,14);
    } else {
        LOGE("ERROR: pd_sc_get_status(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_get_status(): instance = %d, Pen ID = %d\n", instance, ph_id);
    
    return PD_OK;
}

PDResult_t pd_sc_get_info(int32_t instance, uint8_t ph_id, PDSmartCardInfo_t *sc_info, uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;
    if(NULL == sc_info) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_field(instance, &headcontrolinfo, ph_id, SC_INFO_IDX, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_get_info(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    /* if there is response to be processed..111 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 115+1) { //+1 for scresult
        sc_info->max_usable_slot_volume         = (uint16_t)_pd_getResponseData16(&res,0) * 0.1;
        sc_info->column_spacing_slota_ldw       = _pd_getResponseData8(&res,2);
        sc_info->column_spacing_slota_hdw       = _pd_getResponseData8(&res,3);
        sc_info->column_spacing_slotb_ldw       = _pd_getResponseData8(&res,4);
        sc_info->column_spacing_slotb_hdw       = _pd_getResponseData8(&res,5);
        sc_info->pen_serial_num1                = _pd_getResponseData32(&res,6);
        sc_info->pen_serial_num2                = _pd_getResponseData32(&res,10);
        
        sc_info->ctrdg_fill_site_id             = _pd_getResponseData8(&res,14);
        sc_info->ctrdg_fill_line                = _pd_getResponseData8(&res,15);
        sc_info->ctrdg_fill_year                = _pd_getResponseData8(&res,16) + 2010;
        sc_info->ctrdg_fill_woy                 = _pd_getResponseData8(&res,17);
        sc_info->ctrdg_fill_dow                 = _pd_getResponseData8(&res,18);
        sc_info->ctrdg_fill_hour                = _pd_getResponseData8(&res,19);
        sc_info->ctrdg_fill_min                 = _pd_getResponseData8(&res,20);
        sc_info->ctrdg_fill_sec                 = _pd_getResponseData8(&res,21);
        sc_info->ctrdg_fill_procpos             = _pd_getResponseData8(&res,22);
        
        sc_info->ink_formulator_id_slota        = _pd_getResponseData8(&res,23);
        sc_info->ink_vehicle_slota              = _pd_getResponseData8(&res,24);
        sc_info->ink_family_slota               = _pd_getResponseData16(&res,25);
        sc_info->ink_family_member_slota        = _pd_getResponseData16(&res,27);
        sc_info->ink_revision_slota             = _pd_getResponseData8(&res,29);
        sc_info->ink_formulator_id_slotb        = _pd_getResponseData8(&res,30);
        sc_info->ink_vechicle_slotb             = _pd_getResponseData8(&res,31);
        sc_info->ink_family_slotb               = _pd_getResponseData16(&res,32);
        sc_info->ink_family_member_slotb        = _pd_getResponseData16(&res,34);
        sc_info->ink_revision_slotb             = _pd_getResponseData8(&res,36);
        
        sc_info->ink_drop_wt_slota_hi           = _pd_getResponseData8(&res,37);
        sc_info->ink_drop_wt_slota_lo           = _pd_getResponseData8(&res,38);
        sc_info->ink_drop_wt_slotb_hi           = _pd_getResponseData8(&res,39);
        sc_info->ink_drop_wt_slotb_lo           = _pd_getResponseData8(&res,40);
        sc_info->ink_density_slota              = _pd_getResponseData16(&res,41) * 0.001 + 0.7; 
        sc_info->ink_density_slotb              = _pd_getResponseData16(&res,43) * 0.001 + 0.7;
        sc_info->shelf_life_weeks               = _pd_getResponseData8(&res,45);
        sc_info->shelf_life_days                = _pd_getResponseData8(&res,46);
        sc_info->installed_life_weeks           = _pd_getResponseData8(&res,47);
        sc_info->installed_life_days            = _pd_getResponseData8(&res,48);
        sc_info->vert_print_usable_ink_wt_slota = _pd_getResponseData8(&res,49) * 0.5;
        sc_info->horz_print_usable_ink_wt_slota = _pd_getResponseData8(&res,50) * 0.5;
        sc_info->vert_print_usable_ink_wt_slotb = _pd_getResponseData8(&res,51) * 0.5;
        sc_info->horz_print_usable_ink_wt_slotb = _pd_getResponseData8(&res,52) * 0.5;
        
        sc_info->operating_temp                 = _pd_getResponseData8(&res,53);
        sc_info->high_temp_warning              = _pd_getResponseData8(&res,54) * 5 + 35;
        sc_info->max_firing_freq                = (uint8_t)_pd_getResponseData8(&res,55) * 0.5;
        sc_info->oe_override_percent_slota      = _pd_getResponseData8(&res,56);
        sc_info->oe_override_percent_slotb      = _pd_getResponseData8(&res,57);
        sc_info->volt_offset_override_percent   = _pd_getResponseData8(&res,58);
        sc_info->operating_temp_ovrride         = _pd_getResponseData8(&res,59);
        sc_info->shf_present_slota              = _pd_getResponseData8(&res,60);
        sc_info->shf_present_slotb              = _pd_getResponseData8(&res,61);
        
        sc_info->first_platform_mfg_year        = _pd_getResponseData8(&res,62) + 2010;
        sc_info->first_platform_mfg_woy         = _pd_getResponseData8(&res,63);
        sc_info->first_platform_mfg_country     = _pd_getResponseData8(&res,64);
        sc_info->first_platform_fw_rev_major    = _pd_getResponseData8(&res,65);
        sc_info->first_platform_fw_rev_minor    = _pd_getResponseData8(&res,66);
        sc_info->first_install_ctrdg_count      = _pd_getResponseData8(&res,67);
        sc_info->ctrdg_first_install_year       = _pd_getResponseData8(&res,68) + 2010;
        sc_info->ctrdg_first_install_woy        = _pd_getResponseData8(&res,69);
        sc_info->ctrdg_first_install_dow        = _pd_getResponseData8(&res,70);
        sc_info->ilg_resolution                 = _pd_getResponseData8(&res,71);
        sc_info->ph_orientation                 = _pd_getResponseData8(&res,72);
        sc_info->post_purged_ink_family_member_slota = _pd_getResponseData16(&res,73);
        sc_info->post_purged_ink_family_member_slotb = _pd_getResponseData16(&res,75);
        sc_info->ext_oem_id                     = _pd_getResponseData8(&res,77);
        sc_info->single_use                     = _pd_getResponseData8(&res,78);
        sc_info->hp_or_oem_ink_designator_slota = _pd_getResponseData8(&res,79);
        sc_info->hp_or_oem_ink_designator_slotb = _pd_getResponseData8(&res,80);
        sc_info->regionalization_id             = _pd_getResponseData8(&res,81);
		sc_info->drop_weight_cfg_slota			= _pd_getResponseData8(&res,82);
		sc_info->nozzle_density_cfg_slota		= _pd_getResponseData8(&res,83);
		sc_info->drop_weight_cfg_slotb			= _pd_getResponseData8(&res,84);
		sc_info->nozzle_density_cfg_slotb		= _pd_getResponseData8(&res,85);
		
        for(int loop = 0; loop < 12; loop++)       
            sc_info->platform_id[loop] = _pd_getResponseData8(&res,86+loop);

        for(int loop = 0; loop < 5; loop++)       
            sc_info->trademark_string[loop] = _pd_getResponseData8(&res,98+loop);

        for(int loop = 0; loop < 12; loop++)       
            sc_info->ctrdg_reorder_part_num[loop] = _pd_getResponseData8(&res,103+loop);        


        *sc_result = _pd_getResponseData8(&res,115);
    } else {
        LOGE("ERROR: pd_sc_get_info(): instance = %d, Pen ID = %d\n", instance, ph_id);
        LOGE("Please make sure PD FW version is 3.9 or above\n");
        return PD_ERROR;
    }

    return PD_OK;

}

PDResult_t pd_sc_set_trademark_string(int32_t instance, uint8_t ph_id, char trademark_string[], uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_field(instance, &headcontrolinfo, ph_id, 100, (uint8_t *)trademark_string, 5);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_set_trademark_string(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_set_trademark_string(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;    
}

PDResult_t pd_sc_set_reorder_part_num_string(int32_t instance, uint8_t ph_id, char reorder_part_num_string[], uint8_t *sc_result) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == sc_result) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_set_field(instance, &headcontrolinfo, ph_id, 101, (uint8_t *)reorder_part_num_string, 12);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_set_trademark_string(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_sc_set_trademark_string(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;      
}

PDResult_t pd_sc_read_oem_string_field(int32_t instance, uint8_t ph_id, PDScOemStrFieldId_t str_field_id, char *str, uint8_t buf_size) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    if(str_field_id > 1) {
        LOGE("ERROR: pd_sc_read_oem_string_field(): Invalid str_field_id = %d\n", str_field_id);
        return PD_ERROR;
    }
    
    oem_lock(instance);  /* Lock */
    sr = service_get_field(instance, &headcontrolinfo, ph_id, (103+str_field_id), &res);
    oem_unlock(instance); /* Unlock */
    
    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_read_oem_string_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    /* Process the received data */
    if(res.res_size != 0 && (res.res_size - 1) < buf_size) { //-1 for scresult
        int i = 0;
        for (i = 0; i < (res.res_size - 1); ++i) {
            str[i] = _pd_getResponseData8(&res,i);
        }            
        str[i] = '\0';
    } else {
        LOGE("ERROR: pd_sc_read_oem_string_field(): Failed. instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    return PD_OK;
}

PDResult_t pd_sc_write_oem_string_field(int32_t instance, uint8_t ph_id, PDScOemStrFieldId_t str_field_id, char *str) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;

    LOGI("pd_sc_write_oem_string_field(): instance = %d, Pen ID = %d, \n", instance, ph_id);
    
    ServiceResult_t sr;

    oem_lock(instance); /* Lock */
    sr = service_set_field(instance, &headcontrolinfo, ph_id, (103+str_field_id), (uint8_t *)str, strlen(str));    
    oem_unlock(instance); /* Unlock */
    
    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_sc_write_oem_string_field(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
        
    return PD_OK;      
}

PDResult_t pd_start_purging(int32_t instance, uint8_t ph_id, uint8_t slot) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(slot >= (NUM_TRENCH_PER_PH + 1)) return PD_ERROR;

    ServiceResult_t sr;

    oem_lock(instance);
    sr = service_start_purging(instance, &headcontrolinfo, ph_id, slot);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_start_purging(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }

    LOGD("pd_start_purging(): instance = %d, Pen ID = %d\n", instance, ph_id);

    return PD_OK;
}

PDResult_t pd_get_state_info(int32_t instance, uint8_t ph_id, uint8_t *data) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}
//    if(ph_id >= NUM_SUPPORTED_PH) return PD_ERROR;
    if(NULL == data) return PD_ERROR;

    ServiceResult_t sr;
    Response_t res;

    oem_lock(instance);
    sr = service_get_state_info(instance, &headcontrolinfo, ph_id, &res);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_get_state_info(): instance = %d, Pen ID = %d\n", instance, ph_id);
        return PD_ERROR;
    }
    
    /* if there is response to be processed..20 bytes of response is expected */
    if(res.res_size != 0 && res.res_size == 20) {
        for(int i=0;i<20;i++) {
            data[i] = _pd_getResponseData8(&res,i);
        }
    }

    LOGD("pd_get_state_info(): instance = %d, Pen ID = %d\n", instance, ph_id);
     
    return PD_OK;
}

PDResult_t pd_set_date(int32_t instance, uint16_t year, uint8_t month, uint8_t day) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}

    oem_lock(instance);
    ServiceResult_t sr = service_set_date(instance, &headcontrolinfo, year, month, day);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_set_date(): instance = %d\n", instance);
        return PD_ERROR;
    }

    LOGD("pd_set_date(): instance = %d \n", instance);
    
    return PD_OK;
}

PDResult_t pd_set_platform_info(int32_t instance, PlatformInfo_t *platform_info) {
    LOGI("Enter %s", __FUNCTION__);

    if(_is_lib_initialized == false) {
		LOGE("Not initialized!");
		return PD_ERROR;
	}
//    if(instance <= 0 || instance > NUM_BLUR_INSTANCES) {
//		LOGE("Invalid Instance!");
//		return PD_ERROR;
//	}

    oem_lock(instance);
    ServiceResult_t sr = service_set_platform_info(instance, &headcontrolinfo, platform_info);
    oem_unlock(instance);

    if(sr != SERVICE_OK) {
        LOGE("ERROR: pd_set_platform_info(): instance = %d\n", instance);
        return PD_ERROR;
    }

    LOGD("pd_set_platform_info(): instance = %d \n", instance);

    return PD_OK;
}
