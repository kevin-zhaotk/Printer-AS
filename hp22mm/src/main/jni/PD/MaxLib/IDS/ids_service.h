/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __IDS_SERVICE_H__
#define __IDS_SERVICE_H__

#include "max_common_types.h"
#include "common_service.h"
#include "debug_log.h"
#include "uart_ifc.h"
#include "frame.h"
#include "service.h"
#include "ids_ifc.h"


typedef enum
{
	/* NOTE: SERVICE_PAIRING commands are the same for IDS and PD uCs */
    SERVICE_PAIRING_DELETE             = 150,
	/* NOTE: SERVICE_PAIRING steps (1-3) must be consecutive numbers */
	SERVICE_PAIRING_STEP_1             = 151,
	SERVICE_PAIRING_STEP_2             = 152,
	SERVICE_PAIRING_STEP_3             = 153,
	IDS_SERVICE_GET_PHA_OVERRIDES      = 154,
    IDS_TERMINATE_SESSION              = 155,
    
    IDS_SERVICE_INK_USED               = 55,
    IDS_SERVICE_READ_SC_FIELD          = 56,
    IDS_SERVICE_WRITE_SC_FIELD         = 57,
    IDS_SERVICE_LOCK_PARTITION         = 58,
    //IDS_SERVICE_TO_MONTH_AND_DAY       = 59,
    IDS_SERVICE_SET_PLATFORM_INFO      = 60,
    IDS_SERVICE_SET_DATE               = 61,
    IDS_SERVICE_SET_STALL_INSERT_COUNT = 62,
    IDS_SERVICE_SET_OUT_OF_INK         = 63,
    IDS_SERVICE_SET_FAULTY             = 64,
    IDS_SERVICE_FLUSH_SMART_CARD       = 65,
    IDS_SERVICE_GET_SUPPLY_STATUS      = 66,
    IDS_SERVICE_GET_SUPPLY_DETAIL      = 67,
    IDS_SERVICE_GET_SUPPLY_ID          = 68,
    IDS_SERVICE_GET_SUPPLY_INFO        = 69,
    IDS_SERVICE_GET_MRU_INFO           = 70,
    IDS_SERVICE_GET_FIRST_INSTALL_INFO = 71,
	
    IDS_SERVICE_INCREMENT              = 72,
    IDS_SERVICE_GET_SYS_INFO           = 73,
    
    IDS_SERVICE_GET_MFG_INFO           = 74,
    IDS_SERVICE_GET_INK_FILL           = 75,
    IDS_SERVICE_GET_INK_TREE           = 76,
    IDS_SERVICE_GET_PRINTER_INFO       = 77,
	
    IDS_SERVICE_GET_ILG_BITS           = 79,
    
    IDS_SERVICE_GET_EXT_OEM_ID         = 80,
    IDS_SERVICE_GET_REORDER_PN         = 81,
    //IDS_SERVICE_GET_ACUMEN_DROP_COUNT  = 82,
	
    IDS_SERVICE_READ_SC_STRING         = 83,
    IDS_SERVICE_WRITE_SC_STRING        = 84,

    IDS_SERVICE_READ_OEM_FIELD         = 85,	
    IDS_SERVICE_WRITE_OEM_FIELD        = 86,	
    IDS_SERVICE_READ_OEM_STRING        = 87,	
    IDS_SERVICE_WRITE_OEM_STRING       = 88,	
    
    /* Max supported service code is 255 as we only have 1 byte in the protocol */
} IdsService_t;

typedef struct
{
  uint8_t  rsp_data[MAX_FRAME_DATA_SIZE];
  uint32_t bug_size;
  uint32_t res_size;
} IdsResponse_t;


/* Max size of pairing packets */
#define MAX_PAIRING_PACKET_SIZE 1000

ServiceResult_t _simple_command(int32_t instance, int32_t param, IdsService_t service, char *service_name);
/* IDS uC services */
ServiceResult_t ids_service_terminate_session(int32_t instance);
ServiceResult_t ids_service_get_overrides(int32_t instance, int32_t blur_id, int32_t supply_idx, int32_t *status, uint8_t *overrides, int32_t *out_size, int32_t out_buffer_size);
ServiceResult_t ids_service_ink_use(int32_t instance, int32_t pd_id, int32_t supply_idx, uint8_t *in, int32_t in_size, int32_t *status, int32_t *supply_status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);
ServiceResult_t ids_service_read_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value);
ServiceResult_t ids_service_write_oem_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value);
ServiceResult_t ids_service_read_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t buffer_size, uint8_t *str);
ServiceResult_t ids_service_write_oem_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);
ServiceResult_t ids_service_lock_partition(int32_t instance, int32_t supply_idx, int32_t partition_id);
//ServiceResult_t ids_service_to_month_and_day(int32_t instance, int32_t year, int32_t woy, int32_t dow, int32_t *month, int32_t *day);
ServiceResult_t ids_service_set_platform_info(int32_t instance, PlatformInfo_t *platform_info);
ServiceResult_t ids_service_set_date(int32_t instance, int32_t year, int32_t month, int32_t day);
ServiceResult_t ids_service_set_stall_insert_count(int32_t instance, int32_t supply_idx, int32_t count);
ServiceResult_t ids_service_set_out_of_ink(int32_t instance, int32_t supply_idx);
ServiceResult_t ids_service_set_faulty(int32_t instance, int32_t supply_idx);
ServiceResult_t ids_service_flush_smart_card(int32_t instance, int32_t supply_idx);
ServiceResult_t ids_service_get_supply_status(int32_t instance, int32_t supply_idx, SupplyStatus_t *supply_status);
ServiceResult_t ids_service_get_supply_status_detail(int32_t instance, int32_t supply_idx, SupplyStatusDetail_t *supply_status_detail);
ServiceResult_t ids_service_get_supply_id(int32_t instance, int32_t supply_idx, SupplyID_t *supply_id);
ServiceResult_t ids_service_get_supply_info(int32_t instance, int32_t supply_idx, SupplyInfo_t *supply_info);
ServiceResult_t ids_service_get_mru_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *mru_info);
ServiceResult_t ids_service_get_first_install_info(int32_t instance, int32_t supply_idx, SupplyUseInfo_t *first_install_info);
ServiceResult_t ids_service_get_sys_info(int32_t instance, IdsSysInfo_t *sys_info);

/* direct SC read/write services are only valid for debug builds */
ServiceResult_t ids_service_read_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t *value);
ServiceResult_t ids_service_write_field(int32_t instance, int32_t supply_idx, int32_t field_id, uint32_t value);
ServiceResult_t ids_service_read_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);
ServiceResult_t ids_service_write_string(int32_t instance, int32_t supply_idx, int32_t field_id, int32_t str_len, uint8_t *str);

/* test functions */
ServiceResult_t ids_service_increment(int32_t instance, uint8_t in, uint8_t *out);

#endif /* __IDS_SERVICE_H__ */
