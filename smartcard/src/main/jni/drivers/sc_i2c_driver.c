/*********************************************************************************
    sc_i2c_driver.c

    @功能
        通过I2C总线向特定设备的特定寄存器写数据或者读数据
    @创建
        2019.9.20
    @作者
        H.M. Wang
    @版权
        Copyrights 2019 - 2029, Allrights reserved.
    @地点
        北京
    @变更履历
        2019.9.20 创建 Version 1.0
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal_ifc/sc_i2c_driver.h"
#include "common_log.h"

#define I2C_DEVICE          "/sys/class/device_of_i2c/device"           // I2C设备入口
#define I2C_READ            "/sys/class/device_of_i2c/read"             // I2C读取数据设备
#define I2C_WRITE           "/sys/class/device_of_i2c/write"             // I2C写入数据设备

/*********************************************************************************
    SC_I2C_DRIVER_open
----------------------------------------------------------------------------------
    @描述
        打开一个I2C设备。
        命令格式为："<组别号>,<设备地址>", 如"1,0x2B"，1为组别号，0x2B为设备在该组别的地址
    @参数
        group_id: I2C设备的组别号
        device_address: I2C设备的地址
    @返回值
        0: 成功
        其它值：失败
**********************************************************************************/

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_open(int group_id, int device_address) {
    LOGI(">>> SC_I2C_DRIVER_open: %d, %d", group_id, device_address);

    // 检查参数
    if(group_id < 0) {
        LOGE("Error: SC_I2C_DRIVER_open - Invalid group id");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_GROUPID;
    }

    if(device_address < 0) {
        LOGE("Error: SC_I2C_DRIVER_open - Invalid address");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    char cmd[256];

    // 构建打开设备命令
    sprintf(cmd, "%d,0x%02X", group_id, device_address);

//    LOGD("Open command: %s", cmd);

    FILE *fp;

    // 打开I2C设备管理文件
    if((fp = fopen(I2C_DEVICE, "w")) == NULL) {
        LOGE("Error: SC_I2C_DRIVER_open - File open error");
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD("Device management file opened: %s", I2C_DEVICE);
    }

    // 传送打开设备命令
    if(fprintf(fp, "%s", cmd) > 0) {
        LOGD("Open command sent: %s", cmd);
    } else {
        LOGE("Error: SC_I2C_DRIVER_open - Open command error");
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_DEVICE;
    }

    fclose(fp);

    return SC_I2C_DRIVER_RESULT_OK;
}

/*********************************************************************************
    SC_I2C_DRIVER_write
----------------------------------------------------------------------------------
    @描述
        向刚刚打开的I2C设备写入数据。
        命令格式为："<起始寄存器号>,<数据1>,<数据2>,...,<数据n>"
        如："0xf80,0x00,0x01,0x02"，极为向寄存器0xf80开始的连续三个寄存器分别写入0x00,0x01,0x02
    @参数
        reg: I2C设备内需要写入数据的其实寄存器号
        data: 写入的数据，格式必须为没有空格的有逗号分开的16进制数据
    @返回值
        0: 成功
        其它值：失败
**********************************************************************************/

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_write(int reg, char* data) {
    LOGI(">>> SC_I2C_DRIVER_write: %d, %s", reg, data);

    // 检查参数
    if(reg < 0) {
        LOGE("Error: SC_I2C_DRIVER_write - Invalid address");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    if(NULL == data || sizeof(data) <= 0) {
        LOGE("Error: SC_I2C_DRIVER_write - Empty data");
        return SC_I2C_DRIVER_RESULT_FAILED_DATA_EMPTY;
    }

    char cmd[256];

    // 构建写入数据命令
    sprintf(cmd, "0x%04X,%s", reg, data);

//    LOGD("Write Command: %s", cmd);

    FILE *fp;

    // 打开对应设备写入文件
    if((fp = fopen(I2C_WRITE, "w+")) == NULL) {
        LOGE("Error: SC_I2C_DRIVER_write - File open error");
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD("Writing file opened: %s", I2C_WRITE);
    }

    // 传送写入数据命令
    if(fprintf(fp, "%s", cmd) > 0) {
        LOGD("Write command sent: %s", cmd);
    } else {
        LOGE("Error: SC_I2C_DRIVER_write - Write data error");
        return SC_I2C_DRIVER_RESULT_FAILED_SEND_WRITE_COMMAND;
    }

    fclose(fp);

    return SC_I2C_DRIVER_RESULT_OK;
}

/*********************************************************************************
    SC_I2C_DRIVER_read
----------------------------------------------------------------------------------
    @描述
        从刚刚打开的I2C设备读取数据。
        命令格式为："<数据数量>,<起始寄存器地址>"
        如："3,0xf80"，极为从寄存器0xf80开始的连续读取三个寄存器的数据
    @参数
        reg: I2C设备内需要写入数据的其实寄存器号
        data: 写入的数据，格式必须为没有空格的有逗号分开的16进制数据
    @返回值
        0: 成功
        其它值：失败
**********************************************************************************/

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_read(int num, int reg, uint8_t *result) {
    LOGI(">>> SC_I2C_DRIVER_read: %d, 0x%04X, 0x%08X", num, reg, (unsigned int)result);

    // 检查参数
    if(num <= 0) {
        LOGE("Error: SC_I2C_DRIVER_read - Invalid number");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_NUMBER;
    }

    if(reg < 0) {
        LOGE("Error: SC_I2C_DRIVER_read - Invalid address");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    if(result == NULL) {
        LOGE("Error: SC_I2C_DRIVER_read - null buffer");
        return SC_I2C_DRIVER_RESULT_FAILED_NULL_BUFFER;
    }

    char cmd[256];

    // 构建写入数据命令
    sprintf(cmd, "0x%04X,0x%04X", num, reg);

//    LOGD("Read Command: %s", cmd);

    FILE *fp;

    // 打开对应设备写入文件
    if((fp = fopen(I2C_READ, "w+")) == NULL) {
        LOGE("Error: SC_I2C_DRIVER_read - File open error");
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD("Reading file opened: %s", I2C_READ);
    }

    // 传送读取数据命令
    if(fprintf(fp, "%s", cmd) > 0) {
        LOGD("Read command sent: %s", cmd);
    } else {
        LOGE("Error: SC_I2C_DRIVER_read - Read data error");
        return SC_I2C_DRIVER_RESULT_FAILED_SEND_READ_COMMAND;
    }

    int rnum = 0;

    char buffer[1024];
    memset(buffer, 0x00, 1024);

    while(1) {
        int r = fread(&buffer[rnum], num, 1, fp);
        if(r == 0) break;

        rnum += r;
        if(rnum > num) {
            rnum = num;
            break;
        }
    }

    memcpy(result, buffer, rnum);

    memset(buffer, 0x00, 1024);
    char s[16];
    for(int i=0; i<rnum; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "%02X", result[i]);
        } else {
            sprintf(s, " %02X", result[i]);
        }
        strcat(buffer, s);
    }

    LOGD("Data Read: [%s] - %d bytes read", buffer, rnum);

    fclose(fp);

    return SC_I2C_DRIVER_RESULT_OK;
}
