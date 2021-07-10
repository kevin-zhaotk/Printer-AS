/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "max_common_types.h"
#include "common_service.h"
#include "debug_log.h"
#include "uart_ifc.h"
#include "frame.h"
#include "extension.h"
#include "print_head_driver_ifc.h"

/*
 * List of supported remotes services.
 */
typedef enum
{
    SERVICE_DEVICE_IDENTIFICATION  =   0,
    SERVICE_DEVICE_CONFIGURATION   =   1,
    SERVICE_FW_UPGRADE             =   2,
    SERVICE_CHECK_EXTENSION        =   3,
    SERVICE_CHECK_DEVICE           =   4,
    SERVICE_HOST_WARNING           =   5,
    SERVICE_ENABLE                 =   6,
    SERVICE_DISABLE                =   7,
    SERVICE_READ_BYTE              =   8,
    SERVICE_WRITE_BYTE             =   9,
    SERVICE_READ_SHORT             =  10,
    SERVICE_WRITE_SHORT            =  11,
    SERVICE_READ_INTEGER           =  12,
    SERVICE_WRITE_INTEGER          =  13,
    SERVICE_SET_POINTER            =  14,
    SERVICE_READ_DATA              =  15,
    SERVICE_WRITE_DATA             =  16,
    SERVICE_CLEAR_DATA             =  17,
    SERVICE_OBJECT_HANDLING        =  18,
    SERVICE_RPC_MESSAGES           =  19,
    SERVICE_IO_PORT                =  20,
    SERVICE_PARAMETER              =  21,
    SERVICE_FRAME_CONF_STATUS      =  22,
    SERVICE_TRACE_CONF_STATUS      =  23,
    SERVICE_VENDOR_INFO            =  24,
    SERVICE_BOOTLOADER_INFO        =  25,
    SERVICE_DEBUG                  =  26,
    SERVICE_EXTRA_INFO             =  27,

    /* Blur specific commands */

    SERVICE_SYSTEM_INFO		       =  137,
    SERVICE_SETORCLEAR_FPGA_RESET  =  138,
    
    SERVICE_GET_FPGA_LOG            = 136,
    
    BLUR_SERVICE_IDS_STATUS_UPDATE  = 139,
    BLUR_SERVICE_HANDSHAKE_REQ      = 140,
    BLUR_SERVICE_SET_SECURE_OVERRIDES=141,
    BLUR_TERMINATE_SESSION           = 155,    
    /* Leave this as the last value */
    SERVICE_MAX                    = 255 
} Service_t;

#define SUBSERVICE_OBJECT_CREATE	 	1
#define SUBSERVICE_OBJECT_DELETE		2
#define SUBSERVICE_OBJECT_LOCK 			3
#define SUBSERVICE_OBJECT_UNLOCK		4
typedef struct
{
    uint8_t     data[MAX_FRAME_DATA_SIZE];
    uint32_t    buf_size;
    uint32_t    res_size;
} Response_t;
/* Services shared by IDS and PD uCs */
void _set_pairing_response_timeout();
void _set_default_response_timeout();

ServiceResult_t service_pairing_delete(int32_t instance);
ServiceResult_t service_pairing(int32_t instance, int32_t step, int32_t param1, int32_t param2, uint8_t *in, int32_t in_size, int32_t *status, uint8_t *out, int32_t *out_size, int32_t out_buffer_size);

ServiceResult_t pd_service_supply_status(int32_t instance, uint8_t ph_id, uint8_t slot_id,uint8_t * in, uint16_t in_len, int32_t *status);

/* PD specific services */
ServiceResult_t pd_service_terminate_session(int32_t instance);
ServiceResult_t pd_service_set_secure_overrides(int32_t instance, uint8_t ph_id, uint8_t slot_id,uint8_t * in, uint16_t in_len, int32_t *status);
ServiceResult_t service_get_used_ink_weight(int32_t instance, uint8_t ph_id, uint8_t sot_id, Response_t *response);
ServiceResult_t service_device_identification(int32_t instance);
ServiceResult_t service_device_configuration(int32_t instance);
ServiceResult_t service_fw_upgrade(int32_t instance);
ServiceResult_t service_check_device(int32_t instance);
ServiceResult_t service_host_warning(int32_t instance);
ServiceResult_t service_enable(int32_t instance);
ServiceResult_t service_disable(int32_t instance);
ServiceResult_t service_read_byte(int32_t instance);
ServiceResult_t service_write_byte(int32_t instance);
ServiceResult_t service_read_short(int32_t instance);
ServiceResult_t service_write_short(int32_t instance);
ServiceResult_t service_read_integer(int32_t instance);
ServiceResult_t service_write_integer(int32_t instance);
ServiceResult_t service_set_pointer(int32_t instance);
ServiceResult_t service_read_data(int32_t instance, uint32_t address, size_t data_size, Response_t *response);
ServiceResult_t service_write_data(int32_t instance, uint32_t address, uint8_t *data, size_t data_size);
ServiceResult_t service_clear_data(int32_t instance, uint32_t first_address, size_t total_size);
ServiceResult_t service_object_handling(int32_t instance, uint8_t req_id, uint16_t obj_id, Response_t *response);
ServiceResult_t service_rpc_message(int32_t instance);
ServiceResult_t service_io_port(int32_t instance);
ServiceResult_t service_parameter(int32_t instance);
ServiceResult_t service_frame_conf_status(int32_t instance);
ServiceResult_t service_trace_conf_status(int32_t instance);
ServiceResult_t service_vendor_info(int32_t instance);
ServiceResult_t service_bootloader_info(int32_t instance);
ServiceResult_t service_debug(int32_t instance);
ServiceResult_t service_extra_info(int32_t instance);

ServiceResult_t service_check_extension(int32_t instance, uint16_t ext_id, Response_t * response);
ServiceResult_t service_flash_devid(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_erase(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_pgm(int32_t instance, Flashinfo_t * info, Response_t * response, size_t data_size);
ServiceResult_t service_flash_wrenbl(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_rdstatus(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_wrstatus(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_rdid(int32_t instance, Flashinfo_t * info, Response_t * response);
ServiceResult_t service_flash_read(int32_t instance, Flashinfo_t * info, Response_t * response, size_t data_size);
ServiceResult_t service_fpga_set_chipreset(int32_t instance, bool highlow);

ServiceResult_t service_power_on(int32_t instance, Headinfo_t *info, uint8_t ph_id);
ServiceResult_t service_power_off(int32_t instance, Headinfo_t *info, uint8_t ph_id);
ServiceResult_t service_head_count(int32_t instance, Headinfo_t *info,Response_t * response);
ServiceResult_t service_head_status(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_get_system_info(int32_t instance, Response_t * response);
ServiceResult_t service_set_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint32_t level);
ServiceResult_t service_get_fpenergylevel(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_get_fpenergylevels(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_read_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, Response_t * response);
ServiceResult_t service_write_oem_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_id, uint8_t data);
ServiceResult_t service_calibrate_pulsewidth(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response);
ServiceResult_t service_get_used_ink_weight_unsecured(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response);
ServiceResult_t service_get_actual_temp(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_set_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t temp);
ServiceResult_t service_get_temperature_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_get_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_set_voltage_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t voltage);
ServiceResult_t service_get_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response);
ServiceResult_t service_set_over_energy_override(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, uint8_t energy);

ServiceResult_t service_get_ph_configuration(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response);
ServiceResult_t service_get_column_spacing(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot, Response_t * response);
ServiceResult_t service_get_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, Response_t * response);
ServiceResult_t service_get_fpga_log(int32_t instance, FpgaRecord_t *records, size_t records_size, size_t *records_returned);
ServiceResult_t service_lock_oem_partition(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t partition_id, Response_t * response);
ServiceResult_t service_set_field(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t field_idx, uint8_t *data, size_t data_size);
ServiceResult_t service_start_purging(int32_t instance, Headinfo_t *info, uint8_t ph_id, uint8_t slot);
ServiceResult_t service_get_state_info(int32_t instance, Headinfo_t *info, uint8_t ph_id, Response_t * response);
ServiceResult_t service_set_date(int32_t instance, Headinfo_t *info, int32_t year, int32_t month, int32_t day);
ServiceResult_t service_set_platform_info(int32_t instance, Headinfo_t *info, PlatformInfo_t *platform_info);


#endif /* __SERVICE_H__ */
