/*********************************************************************************
    sc_i2c_driver.h

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

#ifndef _SC_I2C_DRIVER_H_
#define _SC_I2C_DRIVER_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_I2C_DRIVER_RESULT_OK                           = 0,            /* Success */
    SC_I2C_DRIVER_RESULT_FAILED                       = 1,            /* Failure */
    SC_I2C_DRIVER_RESULT_FAILED_INVALID_GROUPID       = 10000,        /* Invalid group id */
    SC_I2C_DRIVER_RESULT_FAILED_INVALID_ADDRESS       = 10001,        /* Invalid address */
    SC_I2C_DRIVER_RESULT_FAILED_DATA_EMPTY            = 10002,        /* Empty data */
    SC_I2C_DRIVER_RESULT_FAILED_OPEN_FILE             = 10003,        /* Opening file failed */
    SC_I2C_DRIVER_RESULT_FAILED_OPEN_DEVICE           = 10004,        /* Opening device failed */
    SC_I2C_DRIVER_RESULT_FAILED_SEND_WRITE_COMMAND    = 10005,        /* Write command failed */
    SC_I2C_DRIVER_RESULT_FAILED_INVALID_NUMBER        = 10006,        /* Invalid number to recerve */
    SC_I2C_DRIVER_RESULT_FAILED_NULL_BUFFER           = 10007,        /* Null buffer */
    SC_I2C_DRIVER_RESULT_FAILED_SEND_READ_COMMAND     = 10008         /* Read command failed */
} SC_I2C_DRIVER_RESULT;

//-----------------------------------------------------------------------------
// Function Prototypes

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

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_open(int group_id, int device_address);

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

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_write(int reg, char* data);

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

SC_I2C_DRIVER_RESULT SC_I2C_DRIVER_read(int num, int reg, uint8_t *result);

#ifdef __cplusplus
}
#endif

#endif /*  _SC_I2C_DRIVER_H_ */
