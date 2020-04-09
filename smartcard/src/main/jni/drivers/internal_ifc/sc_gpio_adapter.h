/*********************************************************************************
    sc_gpio_adapter.h

    @功能
        实现对GPIO端口的控制和状态读取的管理的适配器。基于赵工的ext-gpio驱动
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

#ifndef _SC_GPIO_ADAPTER_H_
#define _SC_GPIO_ADAPTER_H_

#include <stddef.h>
#include "../internal_ifc/hp_smart_card_gpio_ifc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOW   0
#define HIGH  1

#define SC_GPIO_ADAPTER_SUCCESS   0
#define SC_GPIO_ADAPTER_FAIL     -1

// Added by H.M.Wang 2019-10-17
//#define SELECT_ITEM_0   FALSE, FALSE, FALSE
/*
// 高电平有效，589排列
#define SELECT_LEVEL_1  FALSE, FALSE, TRUE
#define SELECT_CARD_1   FALSE, TRUE, FALSE
#define SELECT_LEVEL_2  FALSE, TRUE, TRUE
#define SELECT_CARD_2   TRUE, FALSE, FALSE

// 高电平有效，985排列
#define SELECT_LEVEL_1  TRUE, FALSE, FALSE
#define SELECT_CARD_1   FALSE, TRUE, FALSE
#define SELECT_LEVEL_2  TRUE, TRUE, FALSE
#define SELECT_CARD_2   FALSE, FALSE, TRUE
*/
// 低电平有效，589排列
#define SELECT_LEVEL_1  0x00, 0x00, 0x00
#define SELECT_CARD_1   0x00, 0x01, 0x00
#define SELECT_LEVEL_2  TRUE, FALSE, FALSE
#define SELECT_CARD_2   FALSE, TRUE, TRUE
/*
// 低电平有效，985排列
#define SELECT_LEVEL_1  FALSE, TRUE, TRUE
#define SELECT_CARD_1   TRUE, FALSE, TRUE
#define SELECT_LEVEL_2  FALSE, FALSE, TRUE
#define SELECT_CARD_2   TRUE, TRUE, FALSE
*/
//#define SELECT_ITEM_5   TRUE, FALSE, TRUE
//#define SELECT_ITEM_6   TRUE, TRUE, FALSE
//#define SELECT_ITEM_7   TRUE, TRUE, TRUE
// Added by H.M.Wang 2019-10-17 end

//-----------------------------------------------------------------------------
// Function Prototypes

/*********************************************************************************
    SC_GPIO_DRIVER_ioctl
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口发送输入输出控制字。
    @参数
        fd: GPIO端口的文件描述符
        cmd: 控制字名称
        arg1: 控制字值
    @返回值
        >0: 写入数据的长度
        -1：失败
**********************************************************************************/

uint8_t SC_GPIO_ADAPTER_set_value(HP_SMART_CARD_gpio_line_t line, int value);

/*********************************************************************************
    SC_GPIO_DRIVER_read
----------------------------------------------------------------------------------
    @描述
        从一个GPIO端口读取数值。
    @参数
        line: GPIO line
    @返回值
        >0: 读取的数值
        -1：失败
**********************************************************************************/

uint8_t SC_GPIO_ADAPTER_read_value(HP_SMART_CARD_gpio_line_t line);

/*********************************************************************************
    SC_GPIO_ADAPTER_select_38_xlater
----------------------------------------------------------------------------------
    @描述
        选择38译码器的通路。
    @参数
        pg5 - 38译码器的输入第一根线
        pg8 - 38译码器的输入第二根线
        pg9 - 38译码器的输入第三根线
    @返回值
        0: 相应的端口为低电平
        1: 相应的端口为高电平
        -1：失败
**********************************************************************************/

uint8_t SC_GPIO_ADAPTER_select_38_xlater(int pg5, int pg8, int pg9);


#ifdef __cplusplus
}
#endif

#endif /*  _SC_GPIO_ADAPTER_H_ */
