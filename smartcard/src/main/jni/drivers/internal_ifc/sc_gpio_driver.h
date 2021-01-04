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

// H.M.Wang 2020-9-25 为了使用rfidSwitch的方法选通38译码器做的追加
#define GPIO_RFID_CARD1         0x03        // Print Cartridge. PG5-9(011)，PG9未使用
#define GPIO_RFID_CARD2         0x04
#define GPIO_RFID_CARD3         0x05
#define GPIO_RFID_CARD4         0x06        // Bulk Cartridge. PG5-9(110)，PG9未使用
#define GPIO_RFID_CARD5         0x07
#define GPIO_RFID_CARD6         0x08
#define GPIO_RFID_CARD7         0x09
#define GPIO_RFID_CARD8         0x0A
#define GPIO_IDLE               0x0B        // Level. PG5-9(000)，PG9未使用

// 参照原来的定义，是一样的
//#define SELECT_LEVEL        0x00, 0x00, 0x00
//#define SELECT_PRNT_CTRG    0x00, 0x01, 0x00
//#define SELECT_BULK_CTRG    0x01, 0x01, 0x00
// End of H.M.Wang 2020-9-25 为了使用rfidSwitch的方法选通38译码器做的追加

/*
// H.M.Wang 2020-12-29 增加7寸屏的卡定义
#define GPIO_RFID_CARD1         0x03
#define GPIO_RFID_CARD2         0x04          // Bulk Cartridge. PG985(001)
#define GPIO_RFID_CARD3         0x05          // Print2 Cartridge. PG985(010)
#define GPIO_RFID_CARD4         0x06
#define GPIO_RFID_CARD5         0x07          // Print1 Cartridge. PG985(100)
#define GPIO_RFID_CARD6         0x08          // Output1 Cartridge. PG985(101)
#define GPIO_RFID_CARD7         0x09          // Output2 Cartridge. PG985(110)
#define GPIO_RFID_CARD8         0x0A
// End of H.M.Wang 2020-12-29 增加7寸屏的卡定义
*/

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
    SP_GPIO_DRIVER_select_card
----------------------------------------------------------------------------------
    @描述
        通过RFID选卡的方式，等效实现38译码器的选通。暂时是为了规避Smart卡专用img的问题而采取
        的尝试措施
    @参数
        card: 与选择的卡号
    @返回值
        SC_GPIO_DRIVER_SUCCESS(0): 选卡成功
        SC_GPIO_DRIVER_FAIL(-1)：失败。错误信息保存在errno
**********************************************************************************/

int SP_GPIO_DRIVER_select_card(int card);

#ifdef __cplusplus
}
#endif

#endif /*  _SC_GPIO_DRIVER_H_ */
