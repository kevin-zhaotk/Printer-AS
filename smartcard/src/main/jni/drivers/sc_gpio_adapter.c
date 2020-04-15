/*********************************************************************************
    sc_gpio_adapter.c

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
        2020.3.20 取消ext-gpio的引用，改为通过shell进行访问
*********************************************************************************/

#include <stdio.h>
#include "internal_ifc/sc_gpio_adapter.h"
#include "internal_ifc/sc_gpio_driver.h"
#include "../common_log.h"

// Added by H.M.Wang 2019-10-17

#define GPIO_PORT_PG5           154
#define GPIO_PORT_PG6           155
#define GPIO_PORT_PG7           156
#define GPIO_PORT_PG8           157
#define GPIO_PORT_PG9           158

#define GPIO_ENABLE             0x00
#define GPIO_DISABLE            0x01

/*
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
*/

// Added by H.M.Wang 2019-10-17 end
int16_t GPIOLineToPin(HP_SMART_CARD_gpio_line_t line)
{
	if (line == HP_SMART_CARD_GPIO_HOST_RESET)
	{
		return GPIO_PORT_PG6;
	}
	else if (line == HP_SMART_CARD_GPIO_HOST_READY)
	{
		return GPIO_PORT_PG7;
	}
	return SC_GPIO_DRIVER_FAIL;
}

/*********************************************************************************
    SC_GPIO_ADAPTER_set_value
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口写数据。
    @参数
        line: 需要读值的GPIO线名称，该名称需要换算成ext-gpio驱动识别的端口信息
    @返回值
        SC_GPIO_ADAPTER_SUCCESS(0): 成功
        SC_GPIO_ADAPTER_FAIL(-1)：失败
**********************************************************************************/

uint8_t SC_GPIO_ADAPTER_set_value(HP_SMART_CARD_gpio_line_t line, int value) {
//    LOGI(">>> SC_GPIO_ADAPTER_set_value [%d] to line: %d", value, line);

    uint8_t lev = SP_GPIO_DRIVER_set_value(GPIOLineToPin(line), value);

    return ((lev < SC_GPIO_DRIVER_SUCCESS) ? SC_GPIO_ADAPTER_FAIL : SC_GPIO_ADAPTER_SUCCESS);
}

/*********************************************************************************
    SC_GPIO_ADAPTER_read_value
----------------------------------------------------------------------------------
    @描述
        向一个GPIO端口写数据。
    @参数
        line: 需要读值的GPIO线名称，该名称需要换算成ext-gpio驱动识别的端口信息
    @返回值
        LOW(0): 相应的端口为低电平
        HIGH(1): 相应的端口为高电平
        SC_GPIO_ADAPTER_FAIL(-1)：失败
**********************************************************************************/

uint8_t SC_GPIO_ADAPTER_read_value(HP_SMART_CARD_gpio_line_t line) {
//    LOGI(">>> SC_GPIO_ADAPTER_read_value from line: %d", line);

	uint8_t lev = SP_GPIO_DRIVER_get_value(GPIOLineToPin(line));

    return ((lev < SC_GPIO_DRIVER_SUCCESS) ? SC_GPIO_ADAPTER_FAIL : lev);
}

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

uint8_t SC_GPIO_ADAPTER_select_38_xlater(int pg5, int pg8, int pg9) {
//    LOGI(">>> SC_GPIO_ADAPTER_select_38_xlater: %d, %d, %d", pg5, pg8, pg9);

    int ret1 = -1, ret2 = -1, ret3 = -1;

    if(!pg5) {
        ret1 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG5, GPIO_ENABLE);
    } else {
        ret1 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG5, GPIO_DISABLE);
    }
    if(!pg8) {
        ret2 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG8, GPIO_ENABLE);
    } else {
        ret2 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG8, GPIO_DISABLE);
    }
    if(!pg8) {
        ret3 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG9, GPIO_ENABLE);
    } else {
        ret3 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG9, GPIO_DISABLE);
    }

    return ((ret1 == -1 || ret2 == -1 || ret3 == -1) ? SC_GPIO_ADAPTER_FAIL : SC_GPIO_ADAPTER_SUCCESS);
}

