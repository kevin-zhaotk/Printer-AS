/*********************************************************************************
    smfuncs.c

    @功能
        i2c总线支持功能检测程序
    @创建
        2020.1.17
    @作者
        H.M. Wang
    @版权
        Copyrights 2019 - 2029, Allrights reserved.
    @地点
        北京
    @变更履历
        2020.1.17 依据i2c_tools的功能改写
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "smfuncs.h"

int check_read_byte(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE)) {
        return -1;
    }

    return 0;
}

int check_write_byte(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
        return -1;
    }

    return 0;
}

int check_read_byte_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA)) {
        return -1;
    }

    return 0;
}

int check_write_byte_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA)) {
        return -1;
    }

    return 0;
}

int check_read_word_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA)) {
        return -1;
    }

    return 0;
}

int check_write_word_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_WORD_DATA)) {
        return -1;
    }

    return 0;
}

int check_read_block_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_READ_BLOCK_DATA)) {
        return -1;
    }

    return 0;
}

int check_write_block_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)) {
        return -1;
    }

    return 0;
}

int check_read_i2c_block_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_READ_I2C_BLOCK)) {
        return -1;
    }

    return 0;
}

int check_write_i2c_block_data(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)) {
        return -1;
    }

    return 0;
}

int check_process_call(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_SMBUS_PROC_CALL)) {
        return -1;
    }

    return 0;
}

int check_block_process_call(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & I2C_SMBUS_BLOCK_PROC_CALL)) {
        return -1;
    }

    return 0;
}

int check_pec(int file) {
    unsigned long funcs;

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        return -1;
    }

    if (!(funcs & (I2C_FUNC_SMBUS_PEC | I2C_FUNC_I2C))) {
        return -1;
    }

    return 0;
}

