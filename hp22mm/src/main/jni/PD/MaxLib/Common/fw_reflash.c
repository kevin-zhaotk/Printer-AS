/*
Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

HP Inc. Confidential (C) Copyright 2017 HP DEVELOPMENT COMPANY, L.P.

Made in U.S.A.
*/

#include <unistd.h>

#include "max_common_types.h"
#include "file.h"
#include "srec.h"
#include "common_service.h"
#include "fw_reflash.h"
#include "debug_log.h"
#include "service.h" /* To be removed */

#define FLASH_ERASE_DELAY   10 /* 10 seconds */

//static const char *REFLASH_MODULE_NAME = "RFLS";
//#define REFLASH_DEBUG_LOG(level, ...) DEBUG_LOG(REFLASH_MODULE_NAME, level, __VA_ARGS__)

/* Forward declarations */
static ReflashResult_t _set_micro_bootloader_mode(int32_t instance);
static ReflashResult_t _reboot_mirco(int32_t instance);
static ReflashResult_t _erase_flash(int32_t instance, uint32_t first_address, size_t total_size);
static ReflashResult_t _write_fw(int32_t instance, const char *fw_file_name, bool verify);


ReflashResult_t validate_srec(const char *fw_file_name, uint32_t *first_address, size_t *total_size)
{
    uint32_t last_address = 0;
    *first_address = 0xffffffff;
    *total_size = 0;
    
    /* Open the file */
    void *file_handle = file_open(fw_file_name);
    if(file_handle == NULL) 
    {
        LOGE("validate_srec() : ERROR : Failed to open the file : %s\n", fw_file_name);
        return REFLASH_ERROR;
    }  
    
    /* Iterate through the records and program the micro controller */
    SrecResult_t sr;
    while(1)
    {
        int len = 0; 
        char buf[MAX_SREC_RECORD_SIZE]; /* TODO: Change to const */

        len = file_read_line(file_handle, buf, sizeof(buf));
        if(len <= 0) break; /* End of file */
        
        /* parse the records */
        uint8_t srec_data[MAX_SREC_DATA_SIZE];
        size_t srec_data_len = 0;
        uint32_t address;
        sr = srec_parse_line(buf, &address, srec_data, sizeof(srec_data), &srec_data_len);
        
        /* If this was a file header skip it */
        if(sr == SREC_OK_RECORD_HEADER) continue;
        if(sr < 0) 
        {
            LOGE("validate_srec(): ERROR : SREC parsing failure : %d\n", sr);
            file_close(file_handle); /* Close the file */
            return REFLASH_INVALID_FILE;
        }
        
        if(srec_data_len == 0) 
            break;
            
        if(address < *first_address) 
            *first_address = address;
        
        if((address + srec_data_len) > last_address) 
            last_address = address + srec_data_len;
    }
    
    *total_size = last_address - *first_address;
    
    LOGD("validate_srec(): first address = 0x%x, Last Address = 0x%x,Size = %d\n",
                                        *first_address, last_address, *total_size);
    
    file_close(file_handle); /* Close the file */
    
    if(*first_address == 0xffffffff || *total_size == 0)
    {
        LOGE("validate_srec(): ERROR : Invalid file ? No valid record found\n");
        return REFLASH_INVALID_FILE;
    }
    
    return REFLASH_OK;
}

/* Put the micro controller in bootloader mode */
static ReflashResult_t _set_micro_bootloader_mode(int32_t instance)
{
    ServiceResult_t sr = service_fw_upgrade(instance);
    if(sr != SERVICE_OK) return REFLASH_ERROR;
    
    sleep(2); /* TODO: Allow some time for firmware to reset and jump to bootloader */
    
    /* Microcontroller would not respond to commands until we send 
     * a device identification command after a reboot. 
     * We can fix the bootloader to remove this restriction. 
     * At that time, we won't need to  do this additional 
     * service_device_identification().
     */
    sr = service_device_identification(instance);
    if(sr != SERVICE_OK) return REFLASH_ERROR;
    
    return REFLASH_OK;
}

/* Reboot micro controller in to main program mode */
static ReflashResult_t _reboot_mirco(int32_t instance)
{
    ServiceResult_t sr = service_host_warning(instance);
    if(sr != SERVICE_OK) return REFLASH_ERROR;
    
    sleep(10); /* TODO: Allow some time for firmware to reset and jump to bootloader */
    
    /* Microcontroller would not respond to commands until we send 
     * a device identification command after a reboot. 
     * We can fix the bootloader to remove this restriction. 
     * At that time, we won't need to  do this additional 
     * service_device_identification().
     */
    sr = service_device_identification(instance);
    if(sr != SERVICE_OK) return REFLASH_ERROR;
    
    return REFLASH_OK;
}

static ReflashResult_t _erase_flash(int32_t instance, uint32_t first_address, size_t total_size)
{
    ServiceResult_t sr = service_clear_data(instance, first_address, total_size);
    if(sr != SERVICE_OK) return REFLASH_ERROR;
    
    sleep(FLASH_ERASE_DELAY); /* TODO: wait for flash to be erased */
    
    return REFLASH_OK;
}

static ReflashResult_t _write_fw(int32_t instance, const char *fw_file_name, bool verify)
{
    /* Open the file */
    void *file_handle = file_open(fw_file_name);
    if(file_handle == NULL) 
    {
        LOGE("_write_fw() : ERROR : Failed to open the file : %s\n", fw_file_name);
        return REFLASH_ERROR;
    }  
    
    /* Iterate through the records and program the micro controller */
    SrecResult_t sr;
    while(1)
    {
        int len = 0; 
        char buf[MAX_SREC_RECORD_SIZE]; /* TODO: Change to const */

        len = file_read_line(file_handle, buf, sizeof(buf));
        if(len <= 0) break; /* End of file */
        
        /* parse the records */
        uint8_t srec_data[MAX_SREC_DATA_SIZE];
        size_t srec_data_len = 0;
        uint32_t address;
        sr = srec_parse_line(buf, &address, srec_data, sizeof(srec_data), &srec_data_len);
        
        /* If this was a file header skip it */
        if(sr == SREC_OK_RECORD_HEADER) continue;
        if(sr < 0) 
        {
            LOGE("_write_fw(): ERROR : SREC parsing failure : %d\n", sr);
            return REFLASH_INVALID_FILE;
        }
        
        /* Write the record to flash */
        ServiceResult_t srvr = service_write_data(instance, address, srec_data, srec_data_len);
        if(SERVICE_OK != srvr) 
        {
            LOGE("ERROR : _write_fw() : Flash write error\n");
            return REFLASH_ERROR;
        }
            
    }
    
    /* Close the file */
    file_close(file_handle);
    
    return REFLASH_OK;
}

#define MICRO_START_ADDR (0x0800FC00)
#define MICRO_FLASH_SIZE ((256 - 64) * 1024) /* 256KB flash, first 64K is used for booltoader */

ReflashResult_t micro_fw_reflash(int32_t instance, const char *fw_file_name, bool verify, bool reset)
{
    LOGI("micro_fw_reflash(): instance = %d, file = %s\n", instance, fw_file_name);

    uint32_t first_address = 0;
    uint32_t total_size = 0;
    
    /* Validate the firmware file and extract the starting address and size */
    ReflashResult_t pr = validate_srec(fw_file_name, &first_address, &total_size);
    if(REFLASH_OK != pr) 
    { 
        return pr; 
    }
    
    /* Verify that this is a valid flash address and flash file size
     * is less than the microcontroller flash size.
     */
    if(((first_address & 0x0FFFFFFF) < MICRO_START_ADDR)    ||
       !(first_address & 0x80000000)                        ||
        (total_size > MICRO_FLASH_SIZE))
    {
        LOGE("ERROR : micro_fw_reflash() : invalid file. First Address = 0x%x, Size = %d\n", first_address, total_size);
                    
        return REFLASH_INVALID_FILE;
    }
    
    if(reset)
    {
        /* Put microcontroller in Bootloader mode */
        pr = _set_micro_bootloader_mode(instance);
        if(REFLASH_OK != pr) 
        {
            LOGE("ERROR : micro_fw_reflash() : Failed to jump to bootloader mode\n");
            return pr;
        }
    }
    
    /* Erase flash sectors */
    pr = _erase_flash(instance, first_address, total_size);
    if(REFLASH_OK != pr) 
    {
        LOGE("ERROR : micro_fw_reflash() : Failed to erase flash\n");
        return pr;
    }
    
    /* Write the firmware binary to flash */
    pr = _write_fw(instance, fw_file_name, verify);
    if(REFLASH_OK != pr) 
    {
        LOGE("ERROR : micro_fw_reflash() : Failed to write to flash\n");
        return pr;
    }
        
    /* Reboot into main program mode */
    pr = _reboot_mirco(instance);
    if(REFLASH_OK != pr) 
    {
        LOGE("ERROR : micro_fw_reflash() : Failed to reboot microcontroller\n");
        return pr;
    }

    return REFLASH_OK;
}

