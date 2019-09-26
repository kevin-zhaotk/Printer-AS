/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   © Copyright 2015-2017 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card_gpio_ifc.h
//-----------------------------------------------------------------------------
// Description: GPIO driver for Raspberry Pi with HP development board.
//
//*****************************************************************************
#include <stdio.h>

#include "internal_ifc/hp_smart_card_gpio_ifc.h"

#include "hp_smart_card.h"
#include "hp_debug_log_internal.h"

#include "bcm2835.h"

/* Modified by H.M.Wang 2019-9-22
#define RESET_PIN       RPI_V2_GPIO_P1_13
#define BUSY_PIN        RPI_V2_GPIO_P1_15
#define SUPPLY_PIN      RPI_V2_GPIO_P1_12
*/

/* A20 GPIO Definition
    1 GPIOA: 0
    6 GPIOB: 32
    7 GPIOC: 64
    8 GPIOD: 96
    2 GPIOE: 128
    3 GPIOF: 160
    4 GPIOG: 192
    5 GPIOH: 224
*/

#define RESET_PIN       198             // PG6 = PG0(192) + 6
#define BUSY_PIN        199             // PG7 = PG0(192) + 7
//#define SUPPLY_PIN      RPI_V2_GPIO_P1_12

unsigned int                bcm2835_LibVersion = 0;

static HP_SMART_CARD_bool_t Initialized = HP_SMART_CARD_FALSE;


void HP_SMART_CARD_gpio_init()
{
    // if already initialized, don't do it again
    if (Initialized)
        return;
/* Cut by H.M.Wang 2019-9-22
    // initialize lib
    if (bcm2835_init() == 0)
        return;                                                 // failure
    bcm2835_LibVersion = bcm2835_version();

    // set up GPIO pins
    bcm2835_gpio_fsel(RESET_PIN, BCM2835_GPIO_FSEL_OUTP);               // Reset output
    bcm2835_gpio_set_pud(RESET_PIN, BCM2835_GPIO_PUD_OFF);
    bcm2835_gpio_fsel(BUSY_PIN, BCM2835_GPIO_FSEL_INPT);                // Busy input
    bcm2835_gpio_fsel(SUPPLY_PIN, BCM2835_GPIO_FSEL_OUTP);              // Mux output
    bcm2835_gpio_set_pud(SUPPLY_PIN, BCM2835_GPIO_PUD_OFF);

    // set to I2C mode and baud rate
    bcm2835_i2c_begin();
    bcm2835_i2c_set_baudrate(100000);       // 100kHz
*/
    // Initialized
    Initialized = HP_SMART_CARD_TRUE;
    return;
}

uint8_t GPIOLineToPin(HP_SMART_CARD_gpio_line_t line)
{
    if (line == HP_SMART_CARD_GPIO_HOST_RESET)
    {
        return RESET_PIN;
    }
    else if (line == HP_SMART_CARD_GPIO_HOST_READY)
    {
        return BUSY_PIN;
    }
    else if (line == HP_SMART_CARD_SUPPLY_SELECT)
    {
        return SUPPLY_PIN;
    }
    return 0;
}

HP_SMART_CARD_bool_t HP_SMART_CARD_gpio_get_value(HP_SMART_CARD_gpio_line_t line)
{
    // If not initialized, return TRUE (since that is interpretted as Busy)
    if (!Initialized)
        return HP_SMART_CARD_TRUE;

    // Host Ready is the only valid line to read
    if (line != HP_SMART_CARD_GPIO_HOST_READY)
        return HP_SMART_CARD_TRUE;

    // Read busy line and return the value
    uint8_t lev = bcm2835_gpio_lev(GPIOLineToPin(line));

    //HP_DEBUG_printf("Pi",
    //	HP_DBG_LEVEL_CUSTOMER, 5,
    //	"GPIO GET from line %d returns %d\n", line, lev);

    return((lev == LOW) ? HP_SMART_CARD_FALSE : HP_SMART_CARD_TRUE);
}


void HP_SMART_CARD_gpio_set_value(HP_SMART_CARD_gpio_line_t line, HP_SMART_CARD_bool_t value)
{
    // If not initialized, return
    if (!Initialized)
        return;

    // Host Reset and Supply Select are the only valid lines to write
    if (line != HP_SMART_CARD_GPIO_HOST_RESET && line != HP_SMART_CARD_SUPPLY_SELECT)
        return;


    // Set or Clear the line
    if (value != 0)
        bcm2835_gpio_set(GPIOLineToPin(line));
    else
        bcm2835_gpio_clr(GPIOLineToPin(line));

    //HP_DEBUG_printf("Pi",
    //	HP_DBG_LEVEL_CUSTOMER, 5,
    //	"GPIO line %d is %s\n", line, (value != 0 ? "SET" : "CLR"));

    return;
}
