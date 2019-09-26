/*
    sc_i2c_adapter.c

    @Function
        Implementation of basic operations on i2c device

    @Create
        2019.9.20

    @Auther
        H.M. Wang

    @Copyrights
        Copyrights 2019 - 2029, Allrights reserved.

    @Loacation
        Beijing
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_i2c_adapter.h"
#include "common_log.h"

#define I2C_DEVICE  "/sys/class/device_of_i2c/device"
#define I2C_READ    "/sys/class/device_of_i2c/read"
#define I2C_WRITE   "/sys/class/device_of_i2c/write"

/*********************************************************************************
    i2c_adapter_open_device

    @Description
        Open a i2c device for further reading and/or writing.

    @Params
        group_id: i2c device group id
        device_address: i2c device address

    @Return
        0: Success
        1：Failed
**********************************************************************************/

SC_I2C_ADAPTER_RESULT i2c_adapter_open_device(int group_id, int device_address) {

    // 检查参数
    if(group_id < 0) {
        LOGE("Error: i2c_adapter_open_device - Invalid group id");
        return RESULT_FAILED_INVALID_GROUPID;
    }

    if(device_address < 0) {
        LOGE("Error: i2c_adapter_open_device - Invalid address");
        return RESULT_FAILED_INVALID_ADDRESS;
    }

    char cmd[256];

    // 构建打开设备命令
    sprintf(cmd, "%d,0x%x", group_id, device_address);

    LOGD("Open command: %s", cmd);

    FILE *fp;

    // 打开I2C设备管理文件
    if((fp = fopen(I2C_DEVICE, "w")) == NULL) {
        LOGE("Error: i2c_adapter_open_device - File open error");
        return RESULT_FAILED_CANNOT_OPEN_FILE;
    } else {
        LOGD("Device management file opened: %s", I2C_DEVICE);
    }

    // 传送打开设备命令
    if(fprintf(fp, "%s", cmd) > 0) {
        LOGD("Open command sent: %s", cmd);
    } else {
        LOGE("Error: i2c_adapter_open_device - Write file error");
        return RESULT_FAILED_WRITE_FILE_FAILED;
    }

    fclose(fp);

    return RESULT_OK;
}

/*********************************************************************************
    i2c_adapter_write

    @Description
        Write data to a i2c device.

    @Params
        reg: address of the register on the opened i2c device
        data: data to be written

    @Return
        0: Success
        1：Failed
**********************************************************************************/

SC_I2C_ADAPTER_RESULT i2c_adapter_write(int reg, int* data) {

    // 检查参数
    if(reg < 0) {
        LOGE("Error: i2c_adapter_write - Invalid address");
        return RESULT_FAILED_INVALID_ADDRESS;
    }

    if(NULL == data || sizeof(data) < 0) {
        LOGE("Error: i2c_adapter_write - Empty data");
        return RESULT_FAILED_DATA_EMPTY;
    }

    char cmd[256];

    int size = sizeof(data)/sizeof(int);

    // 构建写入数据命令
    sprintf(cmd, "0x%x", reg);

    for(int i=0; i<size; i++) {
        char s[256];
        sprintf(s, ",0x%x", data[i]);
        strcat(cmd, s);
    }

    LOGD("Write Command: %s", cmd);

    FILE *fp;

    // 打开对应设备写入文件
    if((fp = fopen(I2C_WRITE, "w")) == NULL) {
        LOGE("Error: i2c_adapter_write - File open error");
        return RESULT_FAILED_CANNOT_OPEN_FILE;
    } else {
        LOGD("Writing file opened: %s", I2C_WRITE);
    }

    // 传送写入数据命令
    if(fprintf(fp, "%s", cmd) > 0) {
        LOGD("Write command sent: %s", cmd);
    } else {
        LOGE("Error: i2c_adapter_write - Write file error");
        return RESULT_FAILED_WRITE_FILE_FAILED;
    }

    fclose(fp);

    return RESULT_OK;
}



