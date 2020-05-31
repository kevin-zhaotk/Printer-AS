//
// Created by hmwan on 2020/4/2.
//

#include <drivers/internal_ifc/sc_i2c_driver.h>
#include <common_log.h>
#include <unistd.h>
#include "level_memory_access.h"

#define LEVEL_I2C_ADDRESS       0x2b

#define DATA_CHANNEL_0_MSB_ADDR     0x00
#define DATA_CHANNEL_0_LSB_ADDR     0x01
#define DATA_CHANNEL_1_MSB_ADDR     0x02
#define DATA_CHANNEL_1_LSB_ADDR     0x03
#define DATA_CHANNEL_2_MSB_ADDR     0x04
#define DATA_CHANNEL_2_LSB_ADDR     0x05
#define DATA_CHANNEL_3_MSB_ADDR     0x06
#define DATA_CHANNEL_3_LSB_ADDR     0x07

#define RCOUNT_0_ADDR           0x08
#define RCOUNT_1_ADDR           0x09
#define RCOUNT_2_ADDR           0x0A
#define RCOUNT_3_ADDR           0x0B

#define OFFSET_0_ADDR           0x0C
#define OFFSET_1_ADDR           0x0D
#define OFFSET_2_ADDR           0x0E
#define OFFSET_3_ADDR           0x0F

#define SETTLECOUNT_0_ADDR      0x10
#define SETTLECOUNT_1_ADDR      0x11
#define SETTLECOUNT_2_ADDR      0x12
#define SETTLECOUNT_3_ADDR      0x13

#define CLOCK_DIVIDER_0_ADDR    0x14
#define CLOCK_DIVIDER_1_ADDR    0x15
#define CLOCK_DIVIDER_2_ADDR    0x16
#define CLOCK_DIVIDER_3_ADDR    0x17

#define STATUS_ADDR             0x18
#define ERROR_CONFIG_ADDR       0x19
#define CONFIG_ADDR             0x1A
#define MUX_CONFIG_ADDR         0x1B
#define RESET_ADDR              0x1C

#define DRIVE_CURRENT_0_ADDR    0x1E
#define DRIVE_CURRENT_1_ADDR    0x1F
#define DRIVE_CURRENT_2_ADDR    0x20
#define DRIVE_CURRENT_3_ADDR    0x21

#define MANUFACTURE_ID_ADDR     0x7E        // Default 0x5449
#define DEVICE_ID_ADDR          0x7F        // Default 0x3055

static int _writeI2Cdata(uint8_t reg, uint8_t *data, int length) {
    int fd;
    int ret;

    fd = SC_I2C_DRIVER_open(0x01, LEVEL_I2C_ADDRESS);

    if(fd < 0) {
        LOGE("I2C device open error!");
        return LEVEL_I2C_FAILED;
    }

    ret = SC_I2C_DRIVER_write(fd, reg, data, length);

    SC_I2C_DRIVER_close(fd);

    if(ret < 0) {
        LOGE("Write data error!");
        return LEVEL_I2C_FAILED;
    }

    return LEVEL_I2C_OK;
}

static int _readI2Cdata(int reg, uint8_t *data, int length) {
    int fd;
    int read_length;

    fd = SC_I2C_DRIVER_open(0x01, LEVEL_I2C_ADDRESS);

    if(fd < 0) {
        LOGE("I2C device open error!");
        return LEVEL_I2C_FAILED;
    }

    read_length = SC_I2C_DRIVER_read(fd, reg, data, length);

    SC_I2C_DRIVER_close(fd);

    if(read_length < 0) {
        LOGE("Read data error!");
        return LEVEL_I2C_FAILED;
    }

    return LEVEL_I2C_OK;
}

static int _read2Bytes(int reg, uint16_t *data) {
    uint8_t buf[2];

    int ret = _readI2Cdata(reg, buf, 2);

    if(ret == LEVEL_I2C_OK) {
        *data = 0x0000;
        (*data) <<= 8;
        *data |= buf[0];
        (*data) <<= 8;
        *data |= buf[1];
    }

    return ret;
}

static int _write2Bytes(uint8_t reg, uint16_t *data) {
    uint8_t buf[2];

    buf[0] = (int8_t )(((*data) >> 8) & 0x00FF);
    buf[1] = (int8_t )((*data) & 0x00FF);

    return _writeI2Cdata(reg, buf, 2);
}

static int _read4Bytes(int reg, uint32_t *data) {
    uint8_t buf[2];

    *data = 0x00000000;

    int ret = _readI2Cdata(reg, buf, 2);

    if(ret == LEVEL_I2C_OK) {
        (*data) <<= 8;
        *data |= buf[0];
        (*data) <<= 8;
        *data |= buf[1];

        ret = _readI2Cdata(reg+1, buf, 2);

        if(ret == LEVEL_I2C_OK) {
            (*data) <<= 8;
            *data |= buf[0];
            (*data) <<= 8;
            *data |= buf[1];
        }
    }

    return ret;
}

// --- Functional Functions ---------------------------------------------------------

int readChannelData0(uint32_t *data) {
    return _read4Bytes(DATA_CHANNEL_0_MSB_ADDR, data);
}

int readChannelData1(uint32_t *data) {
    return _read4Bytes(DATA_CHANNEL_1_MSB_ADDR, data);
}

int readChannelData2(uint32_t *data) {
    return _read4Bytes(DATA_CHANNEL_2_MSB_ADDR, data);
}

int readChannelData3(uint32_t *data) {
    return _read4Bytes(DATA_CHANNEL_3_MSB_ADDR, data);
}

// Conversion Time T(c0) = (RCOUNT0 * 16) / f(ref0)
int readRCount0(uint16_t *data) {
    return _read2Bytes(RCOUNT_0_ADDR, data);
}

int writeRCount0(uint16_t *data) {
    return _write2Bytes(RCOUNT_0_ADDR, data);
}

// Conversion Time T(c1) = (RCOUNT1 * 16) / f(ref1)
int readRCount1(uint16_t *data) {
    return _read2Bytes(RCOUNT_1_ADDR, data);
}

int writeRCount1(uint16_t *data) {
    return _write2Bytes(RCOUNT_1_ADDR, data);
}

// Conversion Time T(c2) = (RCOUNT2 * 16) / f(ref2)
int readRCount2(uint16_t *data) {
    return _read2Bytes(RCOUNT_2_ADDR, data);
}

int writeRCount2(uint16_t *data) {
    return _write2Bytes(RCOUNT_2_ADDR, data);
}

// Conversion Time T(c3) = (RCOUNT3 * 16) / f(ref3)
int readRCount3(uint16_t *data) {
    return _read2Bytes(RCOUNT_3_ADDR, data);
}

int writeRCount3(uint16_t *data) {
    return _write2Bytes(RCOUNT_3_ADDR, data);
}

// Conversion Time f(offset0) = (OFFSET0 * 2^16) * f(ref0)
int readOffset0(uint16_t *data) {
    return _read2Bytes(OFFSET_0_ADDR, data);
}

int writeOffset0(uint16_t *data) {
    return _write2Bytes(OFFSET_0_ADDR, data);
}

// Conversion Time f(offset1) = (OFFSET1 * 2^16) * f(ref1)
int readOffset1(uint16_t *data) {
    return _read2Bytes(OFFSET_1_ADDR, data);
}

int writeOffset1(uint16_t *data) {
    return _write2Bytes(OFFSET_1_ADDR, data);
}

// Conversion Time f(offset2) = (OFFSET2 * 2^16) * f(ref2)
int readOffset2(uint16_t *data) {
    return _read2Bytes(OFFSET_2_ADDR, data);
}

int writeOffset2(uint16_t *data) {
    return _write2Bytes(OFFSET_2_ADDR, data);
}

// Conversion Time f(offset3) = (OFFSET3 * 2^16) * f(ref3)
int readOffset3(uint16_t *data) {
    return _read2Bytes(OFFSET_3_ADDR, data);
}

int writeOffset3(uint16_t *data) {
    return _write2Bytes(OFFSET_3_ADDR, data);
}

// 0x0000: Settle Time (tS0)= 32 ÷ ƒREF0
// 0x0001: Settle Time (tS0)= 32 ÷ ƒREF0
// 0x0002 - 0xFFFF: Settle Time (tS0)= (SETTLECOUNT0ˣ16) ÷ ƒREF0
int readSettleCount0(uint16_t *data) {
    return _read2Bytes(SETTLECOUNT_0_ADDR, data);
}

int writeSettleCount0(uint16_t *data) {
    return _write2Bytes(SETTLECOUNT_0_ADDR, data);
}

// 0x0000: Settle Time (tS1)= 32 ÷ ƒREF1
// 0x0001: Settle Time (tS1)= 32 ÷ ƒREF1
// 0x0002 - 0xFFFF: Settle Time (tS1)= (SETTLECOUNT1ˣ16) ÷ ƒREF1
int readSettleCount1(uint16_t *data) {
    return _read2Bytes(SETTLECOUNT_1_ADDR, data);
}

int writeSettleCount1(uint16_t *data) {
    return _write2Bytes(SETTLECOUNT_1_ADDR, data);
}

// 0x0000: Settle Time (tS2)= 32 ÷ ƒREF2
// 0x0001: Settle Time (tS2)= 32 ÷ ƒREF2
// 0x0002 - 0xFFFF: Settle Time (tS2)= (SETTLECOUNT2ˣ16) ÷ ƒREF2
int readSettleCount2(uint16_t *data) {
    return _read2Bytes(SETTLECOUNT_2_ADDR, data);
}

int writeSettleCount2(uint16_t *data) {
    return _write2Bytes(SETTLECOUNT_2_ADDR, data);
}

// 0x0000: Settle Time (tS3)= 32 ÷ ƒREF3
// 0x0001: Settle Time (tS3)= 32 ÷ ƒREF3
// 0x0002 - 0xFFFF: Settle Time (tS3)= (SETTLECOUNT3ˣ16) ÷ ƒREF3
int readSettleCount3(uint16_t *data) {
    return _read2Bytes(SETTLECOUNT_3_ADDR, data);
}

int writeSettleCount3(uint16_t *data) {
    return _write2Bytes(SETTLECOUNT_3_ADDR, data);
}

// 15:12 FIN_DIVIDER0 R/W 0000 Channel 0 Input Divider
//                              Sets the divider for Channel 0 input. Must be set to ≥2 if the
//                              Sensor frequency is ≥ 8.75MHz
//                              b0000: Reserved. Do not use.
//                              FIN_DIVIDER0 ≥ b0001:
//                              ƒin0 = ƒSENSOR0/FIN_DIVIDER0
// 11:10 RESERVED R/W 00 Reserved. Set to b00.
// 9:0 FREF_DIVIDER0 R/W 0x000 Channel 0 Reference Divider
//                              Sets the divider for Channel 0 reference. Use this to scale the
//                              maximum conversion frequency.
//                              0x000: Reserved. Do not use.
//                              FREF_DIVIDER0 ≥ 0x001:
//                              ƒREF0 = ƒCLK/FREF_DIVIDER0
int readClockDivider0(uint16_t *data) {
    return _read2Bytes(CLOCK_DIVIDER_0_ADDR, data);
}

int writeClockDivider0(uint16_t *data) {
    return _write2Bytes(CLOCK_DIVIDER_0_ADDR, data);
}

int readClockDivider1(uint16_t *data) {
    return _read2Bytes(CLOCK_DIVIDER_1_ADDR, data);
}

int writeClockDivider1(uint16_t *data) {
    return _write2Bytes(CLOCK_DIVIDER_1_ADDR, data);
}

int readClockDivider2(uint16_t *data) {
    return _read2Bytes(CLOCK_DIVIDER_2_ADDR, data);
}

int writeClockDivider2(uint16_t *data) {
    return _write2Bytes(CLOCK_DIVIDER_2_ADDR, data);
}

int readClockDivider3(uint16_t *data) {
    return _read2Bytes(CLOCK_DIVIDER_3_ADDR, data);
}

int writeClockDivider3(uint16_t *data) {
    return _write2Bytes(CLOCK_DIVIDER_3_ADDR, data);
}

int readStatus(uint16_t *data) {
    return _read2Bytes(STATUS_ADDR, data);
}

int readErrorConfig(uint16_t *data) {
    return _read2Bytes(ERROR_CONFIG_ADDR, data);
}

int writeErrorConfig(uint16_t *data) {
    return _write2Bytes(ERROR_CONFIG_ADDR, data);
}

int readConfig(uint16_t *data) {
    return _read2Bytes(CONFIG_ADDR, data);
}

int writeConfig(uint16_t *data) {
    return _write2Bytes(CONFIG_ADDR, data);
}

int readMuxConfig(uint16_t *data) {
    return _read2Bytes(MUX_CONFIG_ADDR, data);
}

int writeMuxConfig(uint16_t *data) {
    return _write2Bytes(MUX_CONFIG_ADDR, data);
}

int readReset(uint16_t *data) {
    return _read2Bytes(RESET_ADDR, data);
}

int writeReset(uint16_t *data) {
    return _write2Bytes(RESET_ADDR, data);
}

int readDriveCurrent0(uint16_t *data) {
    return _read2Bytes(DRIVE_CURRENT_0_ADDR, data);
}

int writeDriveCurrent0(uint16_t *data) {
    return _write2Bytes(DRIVE_CURRENT_0_ADDR, data);
}

int readDriveCurrent1(uint16_t *data) {
    return _read2Bytes(DRIVE_CURRENT_1_ADDR, data);
}

int writeDriveCurrent1(uint16_t *data) {
    return _write2Bytes(DRIVE_CURRENT_1_ADDR, data);
}

int readDriveCurrent2(uint16_t *data) {
    return _read2Bytes(DRIVE_CURRENT_2_ADDR, data);
}

int writeDriveCurrent2(uint16_t *data) {
    return _write2Bytes(DRIVE_CURRENT_2_ADDR, data);
}

int readDriveCurrent3(uint16_t *data) {
    return _read2Bytes(DRIVE_CURRENT_3_ADDR, data);
}

int writeDriveCurrent3(uint16_t *data) {
    return _write2Bytes(DRIVE_CURRENT_3_ADDR, data);
}

int readManufactureID(uint16_t *data) {
    return _read2Bytes(MANUFACTURE_ID_ADDR, data);
}

int readDeviceID(uint16_t *data) {
    return _read2Bytes(DEVICE_ID_ADDR, data);
}
