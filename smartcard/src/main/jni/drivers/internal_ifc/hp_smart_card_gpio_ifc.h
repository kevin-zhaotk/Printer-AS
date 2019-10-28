/*
   Confidential computer software. Valid license from HP required for possession, use or copying.  Consistent with FAR 12.211 and 12.212, Commercial Computer Software, Computer Software Documentation, and Technical Data for Commercial Items are licensed to the U.S. Government under vendor's standard commercial license.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY DESCRIPTION.  HP SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  YOU ASSUME THE ENTIRE RISK RELATING TO THE USE OR PERFORMANCE OF THE LICENSED SOFTWARE.

   HP Company Confidential
   � Copyright 2009-2016 HP Development Company, L.P.
   Made in U.S.A.
 */

//*****************************************************************************
// File : hp_smart_card_gpio_ifc.h
//-----------------------------------------------------------------------------
// Description: This is the GPIO interface expected by smart card library.
//
//*****************************************************************************

#ifndef _HP_SMART_CARD_GPIO_IFC_H_
#define _HP_SMART_CARD_GPIO_IFC_H_

#include "hp_types.h"

typedef enum
{
    HP_SMART_CARD_GPIO_HOST_RESET,                          /* GPIO line to reset host smart card */
    HP_SMART_CARD_GPIO_HOST_READY,                          /* GPIO line to check if host smart card is ready */
// Removed by H.M.Wang 2019-10-17
//    HP_SMART_CARD_SUPPLY_SELECT,                            /* GPIO line to select supply component */

    /* Last entry */
//    HP_SMART_CARD_NUM_GPIO
// Removed by H.M.Wang 2019-10-17 end
} HP_SMART_CARD_gpio_line_t;

//-----------------------------------------------------------------------------
// Library Function Prototypes
//-----------------------------------------------------------------------------

//*****************************************************************************
// HP_SMART_CARD_gpio_init -
//-----------------------------------------------------------------------------
// Description - This function initializes HP smart card GPIO library. Will be
//               called by smart card library to initialize GPIO library.
//
// Inputs: None
//
// Return: None
//
//*****************************************************************************
void HP_SMART_CARD_gpio_init(void);

//*****************************************************************************
// HP_SMART_CARD_gpio_get_value -
//-----------------------------------------------------------------------------
// Description - This function returns requested GPIO value
//
// Inputs: line - Which GPIO line to read.
//
// Return: HP_SMART_CARD_TRUE  - If requested GPIO value is 1
//         HP_SMART_CARD_FALSE - If requested GPIO value is 0
//
//*****************************************************************************
HP_SMART_CARD_bool_t HP_SMART_CARD_gpio_get_value(HP_SMART_CARD_gpio_line_t line);


//*****************************************************************************
// HP_SMART_CARD_gpio_set_value -
//-----------------------------------------------------------------------------
// Description - This function sets/resets the given GPIO line.
//
// Inputs: line - which GPIO line to set
//         value - HP_SMART_CARD_FALSE or HP_SMART_CARD_TRUE.
//
// Return: HP_SMART_CARD_TRUE  - If success
//         HP_SMART_CARD_FALSE - If failed
//
//*****************************************************************************
HP_SMART_CARD_bool_t HP_SMART_CARD_gpio_set_value(HP_SMART_CARD_gpio_line_t line, HP_SMART_CARD_bool_t value);

#endif /* _HP_SMART_CARD_GPIO_IFC_H_ */
