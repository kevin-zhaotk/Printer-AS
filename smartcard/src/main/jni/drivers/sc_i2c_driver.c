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
#include <time.h>
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
//    LOGI(">>> SC_I2C_DRIVER_open: Group_id: %d, Address: 0x%02X", group_id, address);

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

    LOGD(">>> SC_I2C_DRIVER_open: Group: %s, Address: 0x%02X opened.", device, address);

    return fd;
}

int SC_I2C_DRIVER_close(int fd) {
    if(fd < 0) {
        LOGE(">>> SC_I2C_DRIVER_close: Invalid file id [%d]", fd);
    }

    return close(fd);
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
/*
int SC_I2C_DRIVER_write(int group_id, int device_address, uint8_t reg, uint8_t *data, int length) {
//    char buf[1024] = {0x00};
////    memset(buf, 0x00, 1024);
//    toHexString(data, buf, length, ',');
//    LOGI(">>> SC_I2C_DRIVER_write: Write [%s](%d bytes) to 0x%02X", buf, length, reg);

    // 检查参数
    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Invalid address [0x%02X]", reg);
        return -1;
    }

    if(NULL == data || sizeof(data) <= 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Null or empty data");
        return -1;
    }

    int fd = SC_I2C_DRIVER_open(group_id, device_address);

    if(fd < 0) {
        return -1;
    }

    if(check_write_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: I2C_FUNC_SMBUS_WRITE_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    struct i2c_rdwr_ioctl_data i2c_data;
    i2c_data.msgs = (struct i2c_msg *)malloc(sizeof(struct i2c_msg));

    char send_buf[length+1];

    i2c_data.nmsgs = 1;
    i2c_data.msgs[0].len = length+1;                       //目标reg的地址的长度
    i2c_data.msgs[0].addr = device_address;         //i2c设备地址
    i2c_data.msgs[0].flags = 0;                     //write flag
    send_buf[0] = reg;
    memcpy(&send_buf[1], data, length);
    i2c_data.msgs[0].buf = send_buf;                //目标reg地址

    int try = 0;
    time_t s = time(NULL);
    while(1) {
        if(ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data) < 0) {
            usleep(100);
            try++;
            LOGI(">>> SC_I2C_DRIVER_write: retry %d", try);
        } else {
            LOGI(">>> SC_I2C_DRIVER_write: done.");
            break;
        }
        if(time(NULL)-s > 3) {
            LOGE(">>> SC_I2C_DRIVER_write: Write data failed. %s", strerror(errno));
            return -1;
        }
    }

    char buf[1024] = {0x00};
    toHexString(data, buf, length, ',');
    LOGD(">>> SC_I2C_DRIVER_write: %d bytes written to 0x%02X.\n[%s]", length, reg, buf);

    free(i2c_data.msgs);
    SC_I2C_DRIVER_close(fd);

    return length;
}
*/
// 第二种实现方法，注意，这个方法还是需要实现打开文件
int SC_I2C_DRIVER_write(int group_id, int device_address, uint8_t reg, uint8_t *data, int length) {
//int SC_I2C_DRIVER_write(int fd, uint8_t reg, uint8_t *data, int length) {
//    char buf[1024] = {0x00};
////    memset(buf, 0x00, 1024);
//    toHexString(data, buf, length, ',');
//    LOGI(">>> SC_I2C_DRIVER_write: Write [%s](%d bytes) to 0x%02X", buf, length, reg);

    // 检查参数
    if(reg < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Invalid address [0x%02X]", reg);
        return -1;
    }

    if(NULL == data || sizeof(data) <= 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Null or empty data");
        return -1;
    }

    int fd = SC_I2C_DRIVER_open(group_id, device_address);

    if(fd < 0) {
        return -1;
    }

    if(check_write_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: I2C_FUNC_SMBUS_WRITE_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    int write_length = 0;

    // Set the register address at position 0 followd by data to be written
    char send_buf[length + 1];
    send_buf[0] = reg;
    memcpy(send_buf + 1, data, length);

    int try = 0;
    while (try < 100) {
        write_length = write(fd, send_buf, length + 1);

        if(write_length >= 0) {
            break;
        } else {
//            LOGE(">>> SC_I2C_DRIVER_write: Write data failed. %s", strerror(errno));
        }
        try++;
        usleep(10);
    }

    if(write_length < 0) {
        LOGE(">>> SC_I2C_DRIVER_write: Write data failed. %s", strerror(errno));
        return -1;
    }

    char buf[1024] = {0x00};
    toHexString(data, buf, write_length-1, ',');
    LOGD(">>> SC_I2C_DRIVER_write: %d bytes written to 0x%02X. Retry = %d\n[%s]", write_length-1, reg, try, buf);
//    LOGD(">>> SC_I2C_DRIVER_write: [%s](%d bytes) written to 0x%02X", buf, write_length-1, reg);

    SC_I2C_DRIVER_close(fd);

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
/*
int SC_I2C_DRIVER_read(int group_id, int device_address, uint8_t reg, uint8_t *result, int length) {
//    LOGI(">>> SC_I2C_DRIVER_read: Read %d bytes from 0x%02X to buffer [0x%08X]", length, reg, (unsigned int )result);

    // 检查参数
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

    int fd;

    fd = SC_I2C_DRIVER_open(group_id, device_address);

    if(fd < 0) {
        return -1;
    }

    if(check_read_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: I2C_FUNC_SMBUS_READ_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 500);                      //设置超时
    ioctl(fd, I2C_RETRIES, 100);                      //设置重试次数

//    uint8_t buf1[length] = {0x00};
//    uint8_t buf2[length] = {0x00};

    struct i2c_rdwr_ioctl_data i2c_data;
    i2c_data.msgs = (struct i2c_msg *)malloc(2 * sizeof(struct i2c_msg));

    i2c_data.nmsgs = 2;
    i2c_data.msgs[0].len = 1;                       //目标reg的地址的长度
    i2c_data.msgs[0].addr = device_address;         //i2c设备地址
    i2c_data.msgs[0].flags = 0;                     //write flag
    i2c_data.msgs[0].buf = &reg;                    //目标reg地址

    i2c_data.msgs[1].len = length;                       //目标reg的地址的长度
    i2c_data.msgs[1].addr = device_address;         //i2c设备地址
    i2c_data.msgs[1].flags = I2C_M_RD;              //read flag
    i2c_data.msgs[1].buf = result;                 //分配内存

    int try = 0;
    time_t s = time(NULL);
    while(1) {
        if(ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data) < 0) {
            usleep(100);
            try++;
            LOGI(">>> SC_I2C_DRIVER_read: retry %d", try);
        } else {
//            LOGI(">>> SC_I2C_DRIVER_read: done.");
            break;
        }
        if(time(NULL)-s > 3) {
            LOGE(">>> SC_I2C_DRIVER_read: Read data failed. %s", strerror(errno));
            return -1;
        }
    }

    // Print received data
    char buf[1024] = {0x00};
    toHexString(result, buf, length, ',');
    LOGD(">>> SC_I2C_DRIVER_read: %d bytes read from 0x%02X.\n[%s]", length, reg, buf);

    free(i2c_data.msgs);
    SC_I2C_DRIVER_close(fd);

    return length;
}
*/
// 第二种实现方法，注意，这个方法还是需要实现打开文件
int SC_I2C_DRIVER_read(int group_id, int device_address, uint8_t reg, uint8_t *result, int length) {
//int SC_I2C_DRIVER_read(int fd, uint8_t reg, uint8_t *result, int length) {
//    LOGI(">>> SC_I2C_DRIVER_read: Read %d bytes from 0x%02X to buffer [0x%08X]", length, reg, (unsigned int )result);

    // 检查参数
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

    int fd;

    fd = SC_I2C_DRIVER_open(group_id, device_address);

    if(fd < 0) {
        return -1;
    }

    if(check_read_i2c_block_data(fd) < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: I2C_FUNC_SMBUS_READ_I2C_BLOCK right failed. %s", strerror(errno));
        return -1;
    }

    int read_length = -1;
    int recv_length = -1;

    // Write register address to I2C device
    char reg_buf[1];
    reg_buf[0] = reg;

    int try = 0;
    while (try < 100) {
        ssize_t ret = write(fd, reg_buf, 1);
        if(ret < 0) {
//            LOGE(">>> SC_I2C_DRIVER_read: Write register address failed. %s", strerror(errno));
            usleep(90);
        } else {
            // Read data from indicated register of the device
            usleep(10);
            recv_length = read(fd, result, length);
            if(recv_length >= 0) {
                break;
            } else {
//                LOGE(">>> SC_I2C_DRIVER_read: Read data failed. %s", strerror(errno));
                usleep(90);
            }
/*            recv_length = 0;
            int try1 = 0;
            while(recv_length < length && try1 < 100) {
                usleep(100);
                read_length = read(fd, result + recv_length, length - recv_length);
                recv_length += read_length;
                while(recv_length > 0 && *(result + recv_length - 1) == 0xFF) {
                    recv_length--;
                }
                LOGD(">>> SC_I2C_DRIVER_read: recv_length = %d!", recv_length);
                try1++;
            }
//            if(recv_length >= 0) {
                LOGD(">>> SC_I2C_DRIVER_read: Read data retry %d.", try1);
                break;
//            } else {
//                LOGE(">>> SC_I2C_DRIVER_read: Read data failed. %s", strerror(errno));
//            }
*/
        }
        try++;
    }

    if(recv_length < 0) {
        LOGE(">>> SC_I2C_DRIVER_read: Read data failed. %s", strerror(errno));
        return -1;
    }

/*    recv_length = length;*/

    // Print received data
    char buf[1024] = {0x00};
    toHexString(result, buf, recv_length, ',');
    LOGD(">>> SC_I2C_DRIVER_read: %d bytes read from 0x%02X. Retry = %d\n[%s]", recv_length, reg, try, buf);
//    LOGD(">>> SC_I2C_DRIVER_read: [%s](%d bytes) read from 0x%02X.", buf, read_length, reg);

    SC_I2C_DRIVER_close(fd);

    return recv_length;
}

