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
#include <unistd.h>
#include "internal_ifc/sc_gpio_adapter.h"
#include "internal_ifc/sc_gpio_driver.h"
#include "../common_log.h"

// Added by H.M.Wang 2019-10-17

// Added by H.M.Wang 2019-10-17 end
int16_t GPIOLineToPin(HP_SMART_CARD_gpio_line_t line) {
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

// H.M.Wang 2020-9-27 通过切换RFID的方式实现38译码器的功能
    if(pg5 == 0x00 && pg8 == 0x01 && pg9 == 0x00) {     // Print Cartridge
// H.M.Wang 2020-11-27 修改38译码器出口，新的墨盒插槽已经将墨盒和墨位的38译码器出口调整为相同的一个了
        return SP_GPIO_DRIVER_select_card(GPIO_RFID_CARD1);     // 墨位和墨盒走38译码器的不同出口
//        return SP_GPIO_DRIVER_select_card(GPIO_IDLE);     // 墨位和墨盒走38译码器的相同出口
// End of H.M.Wang 2020-11-27 修改38译码器出口，新的墨盒插槽已经将墨盒和墨位的38译码器出口调整为相同的一个了
    } else if(pg5 == 0x01 && pg8 == 0x01 && pg9 == 0x00) {  // Bulk Cartridge
        return SP_GPIO_DRIVER_select_card(GPIO_RFID_CARD4);
    } else if(pg5 == 0x00 && pg8 == 0x00 && pg9 == 0x00) {  // Level
        return SP_GPIO_DRIVER_select_card(GPIO_IDLE);
    } else {
        return SP_GPIO_DRIVER_select_card(GPIO_RFID_CARD2);
    }
/*
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
    if(!pg9) {
        ret3 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG9, GPIO_ENABLE);
    } else {
        ret3 = SP_GPIO_DRIVER_set_value(GPIO_PORT_PG9, GPIO_DISABLE);
    }

    usleep(500);

    return ((ret1 == -1 || ret2 == -1 || ret3 == -1) ? SC_GPIO_ADAPTER_FAIL : SC_GPIO_ADAPTER_SUCCESS);
*/
// End of H.M.Wang 2020-9-27 通过切换RFID的方式实现38译码器的功能
}

