/*********************************************************************************
    sc_gpio_driver.h

    @功能
        实现对GPIO端口的控制和状态读取。本驱动的前提是使用赵工的ext-gpio驱动
    @创建
        2019.10.15
    @作者
        H.M. Wang
    @版权
        Copyrights 2019 - 2029, Allrights reserved.
    @地点
        北京
    @变更履历
        2019.10.15 创建 Version 1.0
*********************************************************************************/

#ifndef _SC_GPIO_DRIVER_H_
#define _SC_GPIO_DRIVER_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_GPIO_DRIVER_SUCCESS   0
#define SC_GPIO_DRIVER_FAIL     -1

#define GPIO_PORT_PG5           154
#define GPIO_PORT_PG6           155
#define GPIO_PORT_PG7           156
#define GPIO_PORT_PG8           157
#define GPIO_PORT_PG9           158

#define GPIO_ENABLE             0x00
#define GPIO_DISABLE            0x01

#define GPIO_DEVICE "/dev/ext-gpio"

#define GPIO_DEVICE_IOCTL_WRITE (0x10)

#define GPIO_PIN_PG5_ENABLE     (0x6050)
#define GPIO_PIN_PG5_DIAABLE    (0x6051)

#define GPIO_PIN_PG6_ENABLE     (0x6060)
#define GPIO_PIN_PG6_DIAABLE    (0x6061)

#define GPIO_PIN_PG7_ENABLE     (0x6070)
#define GPIO_PIN_PG7_DIAABLE    (0x6071)

#define GPIO_PIN_PG8_ENABLE     (0x6080)
#define GPIO_PIN_PG8_DIAABLE    (0x6081)

#define GPIO_PIN_PG9_ENABLE     (0x6090)
#define GPIO_PIN_PG9_DIAABLE    (0x6091)

//-----------------------------------------------------------------------------
// Function Prototypes

/*********************************************************************************
    SP_GPIO_DRIVER_set_value
----------------------------------------------------------------------------------
    @描述
        给GPIO端口设置值。
    @参数
        port: GPIO端口
        value: 设置值
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 关闭成功
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_set_value(int port, uint8_t value);

/*********************************************************************************
    SP_GPIO_DRIVER_get_value
----------------------------------------------------------------------------------
    @描述
        从GPIO端口读取值。
    @参数
        port: GPIO端口
    @返回值
        value: 读取的值
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_get_value(int port);

/*********************************************************************************
    SC_GPIO_DRIVER_open(取消)
----------------------------------------------------------------------------------
    @描述
        打开一个GPIO端口。
    @参数
        dev: GPIO驱动设备命
    @返回值
        >0: 文件描述符
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_open(char *dev);

/*********************************************************************************
    SC_GPIO_DRIVER_write(取消)
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口写数据。
    @参数
        fd: GPIO端口的文件描述符
        data_buf: 向GPIO端口写入的数据
        count: 数据长度
    @返回值
        >0: 写入数据的长度
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

//int SC_GPIO_DRIVER_write(int fd, char *data_buf, int count);

/*********************************************************************************
    SC_GPIO_DRIVER_ioctl(取消)
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口发送输入输出控制字。
    @参数
        fd: GPIO端口的文件描述符
        cmd: 控制字名称
        arg1: 控制字值
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 设置成功
        SC_GPIO_DRIVER_FAIL(<=-1)：失败，及失败原因
**********************************************************************************/

int SC_GPIO_DRIVER_ioctl(int fd, int cmd, long arg1);

/*********************************************************************************
    SC_GPIO_DRIVER_read(取消)
----------------------------------------------------------------------------------
    @描述
        从一个GPIO端口读取数值。
    @参数
        fd: GPIO端口的文件描述符
        pin: GPIO pin
    @返回值
        >0: 读取的数值
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_read(int fd, uint8_t pin);

/*********************************************************************************
    SC_GPIO_DRIVER_close(取消)
----------------------------------------------------------------------------------
    @描述
        关闭一个GPIO端口。
    @参数
        fd: GPIO端口的文件描述符
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 关闭成功
        SC_GPIO_DRIVER_FAIL(-1)：失败
**********************************************************************************/

int SC_GPIO_DRIVER_close(int fd);

#ifdef __cplusplus
}
#endif

#endif /*  _SC_GPIO_DRIVER_H_ */
