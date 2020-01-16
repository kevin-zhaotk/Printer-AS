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
#include <fcntl.h>
#include <unistd.h>

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
    LOGI(">>> SC_I2C_DRIVER_open: %d, 0x%02X", group_id, device_address);

    // 检查参数
    if(group_id < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Error[Invalid group id]");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_GROUPID;
    }

    if(device_address < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Error[Invalid address]");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    char cmd[256];
    memset (cmd, 0x00, 256);

    // 构建打开设备命令
    sprintf(cmd, "%d,0x%02X", group_id, device_address);

//    LOGD("Open command: %s", cmd);

    // 打开I2C设备管理文件
    int fd = open(I2C_DEVICE, O_RDWR);

    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Open [%s] failed. %s", I2C_DEVICE, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD(">>> SC_I2C_DRIVER_open: [%s] opened", I2C_DEVICE);
    }

    // 传送打开设备命令
    if(write(fd, cmd, strlen(cmd)) > 0) {
        LOGD(">>> SC_I2C_DRIVER_open: Command sent [%s]", cmd);
    } else {
        LOGE(">>> SC_I2C_DRIVER_open: Write command to [%s] failed. %s", I2C_DEVICE, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_DEVICE;
    }

    close(fd);

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
        reg: I2C设备内需要写入数据的起始寄存器号
        data: 写入的数据，格式必须为没有空格的有逗号分开的16进制数据
    @返回值
        0: 成功
        其它值：失败
**********************************************************************************/

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_write(int reg, char* data) {
    LOGI(">>> SC_I2C_DRIVER_write: 0x%02X,%s", reg, data);

    // 检查参数
    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Error[Invalid address]");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    if(NULL == data || sizeof(data) <= 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Error[Empty data]");
        return SC_I2C_DRIVER_RESULT_FAILED_DATA_EMPTY;
    }

    char cmd[256];

    // 构建写入数据命令
    sprintf(cmd, "0x%02X,%s", reg, data);

//    LOGD("Write Command: %s", cmd);

    int fd = open(I2C_WRITE, O_RDWR);

    // 打开对应设备写入文件
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Open [%s] failed. %s", I2C_WRITE, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD(">>> SC_I2C_DRIVER_write: [%s] opened", I2C_WRITE);
    }

    // 传送写入数据命令
    if(write(fd, cmd, strlen(cmd)) > 0) {
        LOGD(">>> SC_I2C_DRIVER_write: Command sent [%s]", cmd);
    } else {
        LOGE(">>> SC_I2C_DRIVER_write: Write command to [%s] failed. %s", I2C_WRITE, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_SEND_WRITE_COMMAND;
    }

    close(fd);

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
    LOGI(">>> SC_I2C_DRIVER_read: %d, 0x%02X, 0x%08X", num, reg, (unsigned int )result);

    // 检查参数
    if(num <= 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Error[Invalid number]");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_NUMBER;
    }

    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Error[Invalid address]");
        return SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS;
    }

    if(result == NULL) {
        LOGE(">>> SC_I2C_DRIVER_read: Error[Null buffer]");
        return SC_I2C_DRIVER_RESULT_FAILED_NULL_BUFFER;
    }

    char cmd[256];

    // 构建写入数据命令
    sprintf(cmd, "%d,0x%02X", num, reg);

//    LOGD("Read Command: %s", cmd);

    int fd = open(I2C_READ, O_RDWR);

    // 打开对应设备写入文件
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Open [%s] failed. %s", I2C_READ, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD(">>> SC_I2C_DRIVER_read: [%s] opened", I2C_READ);
    }

    // 传送读取数据命令
    if(write(fd, cmd, strlen(cmd)) > 0) {
        LOGD(">>> SC_I2C_DRIVER_read: Command sent [%s]", cmd);
    } else {
        LOGE(">>> SC_I2C_DRIVER_read: Write command to [%s] failed. %s", I2C_READ, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_SEND_READ_COMMAND;
    }

    close(fd);

    fd = open(I2C_READ, O_RDWR);

    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Open [%s] failed. %s", I2C_READ, strerror(errno));
        return SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE;
    } else {
        LOGD(">>> SC_I2C_DRIVER_read: [%s] opened", I2C_READ);
    }

    int rnum = 0;

    char buffer[1024];
    memset(buffer, 0x00, 1024);

    while(1) {
        int r = read(fd, &buffer[rnum], 1024);
        if(r == 0 || r == -1) break;

        rnum += r;
        if(rnum >= 1024) {
            break;
        }
    }

    close(fd);

    LOGD(">>> SC_I2C_DRIVER_read: Data read \n[%s](%d bytes)", buffer, strlen(buffer));

    char *pe = buffer;
    int bSize = strlen(buffer);
    char *ps = NULL;
    int cnt = 0;
//    int lines = 0;

    while(pe < buffer + bSize) {
        ps = pe;
        pe = strstr(ps, "\n");

        if(NULL != pe) {
            *pe = 0x00;
            pe++;
        }

//        LOGD(">>> line = [%s]", ps);
        if(strncmp(ps, "0x", 2) == 0) {
            ps += 2;
            ps = strstr(ps, "0x");
            if(ps != NULL) {
                ps += 2;
                int aaa = 0;
                while(*ps != 0x00) {
                    char c = *ps++;
                    aaa *= 16;
                    if(c >= '0' && c <= '9') {
                        aaa += (c - '0');
                    } else if(c >= 'A' && c <= 'Z') {
                        aaa += (c - 'A' + 10);
                    } else if(c >= 'a' && c <= 'z') {
                        aaa += (c - 'a' + 10);
                    }
                }
                LOGD(">>> result[%d] = [%04X]", cnt, aaa);
                result[cnt++] = aaa;
            }
        }
    }
/*
    memset(buffer, 0x00, 1024);
    char s[16];
    for(int i=0; i<cnt; i++) {
        memset(s, 0x00, 16);
        if(i == 0) {
            sprintf(s, "%02X", result[i]);
        } else {
            sprintf(s, ", %02X", result[i]);
        }
        strcat(buffer, s);
    }

    LOGD(">>> SC_I2C_DRIVER_read: Data Read[%s](%d bytes)", buffer, cnt);
*/
    return SC_I2C_DRIVER_RESULT_OK;
}
