/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "max_common_types.h"
#include "debug_log.h"
#include "uart_ifc.h"
#include "frame.h"

#define SPIFLASHNAME	"FpgaFlash"
#define EXTENSION_SPIMASTER_ID	2
#define MAX_SPI_SERVICES		3
#define GET_DEV_INDEX			0
#define GET_OBJ_INDEX			1
#define WRRD_INDEX				2

#define	MIN_SPI_REV				1

#define MAX_NAME_LENGTH			64
#define MAX_FLASH_OBJECTS		8
#define FLASH_ERASE				0
#define FLASH_WREN				FLASH_ERASE+1
#define FLASH_GETSTS			FLASH_WREN+1
#define FLASH_PGPGM				FLASH_GETSTS+1
#define FLASH_RDBUF				FLASH_PGPGM+1
#define FLASH_RDID				FLASH_RDBUF+1
#define FLASH_WRSTS				FLASH_RDID+1
#define FLASH_RDPG				FLASH_WRSTS+1		
#define FLASH_ERASE_ID			16
#define FLASH_WREN_ID			FLASH_ERASE_ID+1
#define FLASH_GETSTS_ID			FLASH_WREN_ID+1
#define FLASH_PGPGM_ID			FLASH_GETSTS_ID+1
#define FLASH_RDBUF_ID			FLASH_PGPGM_ID+1
#define FLASH_RDID_ID			FLASH_RDBUF_ID+1
#define FLASH_WRSTS_ID			FLASH_RDID_ID+1
#define FLASH_RDPG_ID			FLASH_WRSTS_ID+1

#define FLASH_WIP_BIT			1
#define FLASH_BP0_BIT			1<<2
#define FLASH_BP1_BIT			1<<3
#define FLASH_BP2_BIT			1<<4
#define FLASH_WP_MASK			(FLASH_BP0_BIT | FLASH_BP1_BIT | FLASH_BP2_BIT)
#define FLASH_PAGE_SIZE			256

#define EXTENSION_I2CMASTER_ID	3
#define I2C1					0
#define I2C2					1
#define MAX_I2C_BUSES			2
#define MAX_I2C_OBJPERBUS		1
#define MAX_I2C_OBJECTS			MAX_I2C_BUSES*MAX_I2C_OBJPERBUS
#define I2C_7BIT_MODE			0
#define I2C_8BIT_MODE			1
#define I2C_16BIT_MODE			2
#define I2C_11BIT_MODE			3
#define I2C_19BIT_MODE			4
#define I2C_SLVADDR_BITMASK		0x7
#define I2C_RESTART_MODE		8
#define I2C_REGSTARTSTOP_MODE	0

#define I2C_WROBJ_INDEX			0
#define I2C_RDOBJ_INDEX			0

#define I2C_READ_INDEX			0
#define I2C_WRITE_INDEX			1	
#define I2C_GETOBJINFO_INDEX	2
#define I2C_GETSTATS_INDEX		3
#define I2C_RESETSTATS_INDEX	4

#define EXTENSION_MULTIHEAD_ID              18
#define MULTIHEAD_POWON_IDX                         0
#define MULTIHEAD_POWOFF_IDX                        1
#define MULTIHEAD_HDCNT_IDX                         2
#define MULTIHEAD_STATUS_IDX                        3
#define MULTIHEAD_GET_SET_FP_WIDTHS_IDX             4
#define MULTIHEAD_GET_SET_FP_VOLT_IDX               5
#define MULTIHEAD_GET_SET_TEMP_IDX                  6
#define MULTIHEAD_CONTROL_HEATING_IDX               7
#define MULTIHEAD_PURGE_CONTROL_IDX                 8  /* Not Used */
#define MULTIHEAD_THERMAL_CALIB_IDX                 9
#define MULTIHEAD_SET_FP_ENERGYLEVEL_IDX            10
#define MULTIHEAD_GET_FP_ENERGYLEVELS_IDX           11
#define MULTIHEAD_GET_DRPCNT_IDX                    12
#define MULTIHEAD_GET_SET_OEM_FIELD__IDX            13 /* Not used. should be removed*/
#define MULTIHEAD_GET_FP_ENERGYLEVEL_IDX            14
#define MULTIHEAD_FP_WIDTH_CALIB_IDX                15
#define	MULTIHEAD_CONTROL_DPWA_IDX                  16
#define	MULTIHEAD_STATUS_DPWA_IDX                   17
#define MULTIHEAD_GET_SET_PARAMETERS_DPWA_IDX       18
#define MULTIHEAD_GET_SET_SCARD_FIELD_IDX           19
#define MULTIHEAD_CHNGE_PARTITION_SCARD_IDX         20
#define	MULTIHEAD_CONTROL_PH_VALIDATION_IDX         21
#define MULTIHEAD_GET_SET_FAULT_TEMP_IDX            22
#define MULTIHEAD_GET_SET_PARAMETERS_DTS_IDX        23
#define MULTIHEAD_GET_DRPVLM_IDX                    24
#define MULTIHEAD_GET_SET_TEMP_OVERRIDE_IDX         25
#define MULTIHEAD_GET_SET_VOLTAGE_OVERRIDE_IDX      26
#define MULTIHEAD_GET_SET_OVERENERGY_OVERRIDE_IDX   27
#define MULTIHEAD_GET_SET_SLOT_FP_WIDTHS_IDX        28
#define MULTIHEAD_GET_PH_CONFIGURATION_IDX          29
#define MULTIHEAD_GET_COLUMN_SPACING_IDX            30
#define MULTIHEAD_GET_SET_OEM_FIELD_IDX             31
#define MULTIHEAD_GET_SLOT_TTOE_DATA_TEMP           32
#define MULTIHEAD_GET_SLOT_TTOE_DATA_FPWS           33
#define MULTIHEAD_SET_TTOE_PARAMS                   34
#define MULTIHEAD_LOCK_OEM_PARTITION_IDX            35
#define MULTIHEAD_STATE_INFO_IDX                    36
#define MULTIHEAD_PURGE_START_IDX                   37
#define MULTIHEAD_GET_SLOT_PURGE_DATA_TEMP_IDX      38
#define MULTIHEAD_GET_SET_PREENERGY_OVERRIDE_IDX 	39
#define MULTIHEAD_GET_SET_DATE_IDX 					40
#define MULTIHEAD_GET_SET_PLATFORMINFO_IDX			41
#define MULTIHEAD_GET_POWERONLOG_IDX				42



#define MULTIHEAD_FP_WIDTH_GET		1
#define MULTIHEAD_FP_WIDTH_SET		2

#define MULTIHEAD_FP_PCP_WIDTH		1
#define MULTIHEAD_FP_PCP_DTIME_WIDTH	2
#define MULTIHEAD_FP_WIDTH		3

#define MULTIHEAD_VPPVOLT_GET       1
#define MULTIHEAD_VPPVOLT_SET       2

#define MULTIHEAD_TEMP_GET    	    1
#define MULTIHEAD_TEMP_SET    	    2

#define MULTIHEAD_OEMDATA_READ       1
#define MULTIHEAD_OEMDATA_WRITE       2

#define MULTIHEAD_DPWA_PARAMETERS_GET	1
#define MULTIHEAD_DPWA_PARAMETERS_SET	2

#define MULTIHEAD_SAMRTCARD_READ	1
#define MULTIHEAD_SAMRTCARD_WRITE	2

#define MULTIHEAD_TARGETTEMP	    1
#define MULTIHEAD_ACTUALTEMP	    2
#define MULTIHEAD_DTS_OFFSET_TEMP   3

#define MULTIHEAD_FAULT_TEMP_GET	1
#define MULTIHEAD_FAULT_TEMP_SET	2

#define MULTIHEAD_DTS_PARAMETERS_GET	1
#define MULTIHEAD_DTS_PARAMETERS_SET	2

#define MULTIHEAD_VOLT_GET    	    1
#define MULTIHEAD_VOLT_SET    	    2

#define MULTIHEAD_OVERENERGY_GET    	    1
#define MULTIHEAD_OVERENERGY_SET    	    2

#define MULTIHEAD_SLOT_FP_WIDTH_GET		1
#define MULTIHEAD_SLOT_FP_WIDTH_SET		2

#define MULTIHEAD_SLOT_FP_PCP_WIDTH		1
#define MULTIHEAD_SLOT_FP_PCP_DTIME_WIDTH	2
#define MULTIHEAD_SLOT_FP_WIDTH		3

#define MULTIHEAD_OVRPREENERGY_GET 		1
#define MULTIHEAD_OVRPREENERGY_SET 		2

#define MULTIHEAD_DATE_GET 		1
#define MULTIHEAD_DATE_SET 		2

#define MULTIHEAD_PLATFORMINFO_GET	1
#define MULTIHEAD_PLATFORMINFO_SET	2



typedef struct{
	
	uint16_t id;
	uint32_t size;
	uint32_t addr;
}Objinfo_t;

typedef struct{
	uint8_t rev;
	uint8_t total_srvcs;
	uint8_t total_evnts;
	uint8_t first_srvc_id;
	uint32_t first_evnt_id;
	uint8_t total_dvcs;
	uint8_t dev_name[MAX_NAME_LENGTH];
	uint8_t dev_id;
	Objinfo_t objects[MAX_FLASH_OBJECTS];
}Flashinfo_t;


typedef struct{
	uint8_t rev;
	uint8_t total_srvcs;
	uint8_t total_evnts;
	uint8_t first_srvc_id;
	uint32_t first_evnt_id;
	uint8_t busid;
	uint8_t initialized;	
	Objinfo_t objects[MAX_I2C_OBJPERBUS];
}I2cinfo_t;

typedef struct{
	uint8_t rev;
	uint8_t total_srvcs;
	uint8_t total_evnts;
	uint8_t first_srvc_id;
	uint32_t first_evnt_id;
	uint8_t initialized;	
}Headinfo_t;
#endif /*  __EXTENSION_H__ */
