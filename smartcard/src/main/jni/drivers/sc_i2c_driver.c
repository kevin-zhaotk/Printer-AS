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
    @Reference
        https://www.kernel.org/doc/Documentation/i2c/dev-interface
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <hp_debug_log_internal.h>
#include "drivers/i2c_tools/smbus.h"
#include "drivers/i2c_tools/smfuncs.h"

#include "internal_ifc/sc_i2c_driver.h"
#include "common_log.h"

#define I2C_DEVICE          "/dev/i2c-%d"                      // I2C设备入口

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
        >0: 文件描述符
        -1：失败
**********************************************************************************/

int SC_I2C_DRIVER_open(int group_id, int address) {
    LOGI(">>> SC_I2C_DRIVER_open: Group_id: %d, Address: 0x%02X", group_id, address);

    // 检查参数
    if(group_id < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Invalid group id [%d]", group_id);
        return -1;
    }

    if(address < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Invalid address [0x%02X]", address);
        return -1;
    }

    char device[256];

    // 构建打开设备命令
    sprintf(device, I2C_DEVICE, group_id);

    // 打开I2C设备管理文件
    int fd = open(device, O_RDWR);

    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Open [%s] failed. %s", device, strerror(errno));
        return -1;
    } else {
//        LOGD(">>> SC_I2C_DRIVER_open: [%s] opened as [%d]", device, fd);
    }

    // 传送打开寄存器地址
    if (ioctl(fd, I2C_SLAVE_FORCE, address) < 0) {
        LOGE(">>> SC_I2C_DRIVER_open: Could not set address to [0x%02X]. %s", address, strerror(errno));
        return -1;
    } else {
//        LOGD(">>> SC_I2C_DRIVER_open: Set address to [0x%02X].", address);
    }

    return fd;
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
        -1：失败
**********************************************************************************/

int SC_I2C_DRIVER_write(int fd, uint8_t reg, uint8_t *data, int length) {
    char buf[1024] = {0x00};
//    memset(buf, 0x00, 1024);
    toHexString(data, buf, length, ',');
    LOGI(">>> SC_I2C_DRIVER_write: Write [%s](%d bytes) to 0x%02X", buf, length, reg);

    // 检查参数
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Invalid file id [%d]", fd);
        return -1;
    }

    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Invalid address [0x%02X]", reg);
        return -1;
    }

    if(NULL == data || sizeof(data) <= 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Null or empty data");
        return -1;
    }

    if(check_write_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: I2C_FUNC_SMBUS_WRITE_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    int write_length = 0;
/*
    int try_times = 0;

    while(write_length < length) {
        int _length = (length - write_length) > I2C_SMBUS_BLOCK_MAX ? I2C_SMBUS_BLOCK_MAX : length - write_length;
        if(i2c_smbus_write_i2c_block_data(fd, reg + write_length, _length, data + write_length) < 0) {
            try_times++;
            if(try_times >= 100) {
                LOGE(">>> SC_I2C_DRIVER_write: Write i2c_block_data failed. %s", strerror(errno));
                return -1;
            }
            usleep(10);
            continue;
        }
        usleep(10);
        write_length += _length;
    }

    return 0;
*/

    // Set the register address at position 0 followd by data to be written
    char send_buf[length + 1];
    send_buf[0] = reg;
    memcpy(send_buf + 1, data, length);
    write_length = write(fd, send_buf, length + 1);

    if(write_length < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Write data failed. %s", strerror(errno));
        return -1;
    }

    return write_length;
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
        >=0: 成功
        -1：失败
**********************************************************************************/

int SC_I2C_DRIVER_read(int fd, uint8_t reg, uint8_t *result, int length) {
    LOGI(">>> SC_I2C_DRIVER_read: Read %d bytes from 0x%02X to buffer [0x%08X]", length, reg, (unsigned int )result);

    // 检查参数
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Invalid file id [%d]", fd);
        return -1;
    }

    if(length <= 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Invalid number [%d]", length);
        return -1;
    }

    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Invalid address [0x%02X]", reg);
        return -1;
    }

    if(result == NULL) {
        LOGE(">>> SC_I2C_DRIVER_read: Null buffer");
        return -1;
    }

    if(check_read_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: I2C_FUNC_SMBUS_READ_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    int read_length = 0;

/*    int try_times = 0;

    while(read_length < length) {
        int _length = i2c_smbus_read_i2c_block_data(fd, reg + read_length, length - read_length, result + read_length);
        if(_length < 0) {
            try_times++;
            if(try_times >= 100) {
                LOGE(">>> SC_I2C_DRIVER_read: Read i2c_block_data failed. %s", strerror(errno));
                return -1;
            }
            usleep(10);
            continue;
        }
        read_length += _length;
        usleep(1000);
    }

    char buf[1024];
    memset(buf, 0x00, 1024);
    toHexString(result, buf, length, ',');
    LOGD(">>> SC_I2C_DRIVER_read: [%s](%d bytes) read.", buf, read_length);

    return read_length;
*/

    // Write register address to I2C device
    char reg_buf[1];
    reg_buf[0] = reg;
    ssize_t ret = write(fd, reg_buf, 1);
    if(ret < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Write register address failed. %s", strerror(errno));
        return -1;
    }

    // Read data from indicated register of the device
    read_length = read(fd, result, length);
    if(read_length < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Read data failed. %s", strerror(errno));
        return -1;
    }

    // Print received data
    char buf[1024] = {0x00};
//    memset(buf, 0x00, 1024);
    toHexString(result, buf, length, ',');
    LOGD(">>> SC_I2C_DRIVER_read: [%s](%d bytes) read.", buf, read_length);

    return read_length;
}

int SC_I2C_DRIVER_close(int fd) {
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_close: Invalid file id [%d]", fd);
    }

    return close(fd);
}
