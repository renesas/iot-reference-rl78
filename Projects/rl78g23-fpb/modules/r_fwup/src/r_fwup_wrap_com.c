/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_wrap_com.c
 * Description  : Functions for the Firmware update module.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.03.2023 2.00    First Release
 *         : 20.11.2023 2.01    Fixed log messages.
 *                              Add parameter checking.
 *                              Added arguments to R_FWUP_WriteImageProgram API.
 *         : 18.04.2025 2.03    V203 Release.
 *         : 27.08.2025 2.04    V204 Release.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_fwup_wrap_com.h"
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/*
 * Common functions
 */
/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_disable_interrupt
 * Description  : wrapper function for disable interrupts
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void r_fwup_wrap_disable_interrupt(void)
{
    /**** Start user code ****/
#if defined(__RX)
    R_BSP_InterruptsDisable();
#else
    BSP_DISABLE_INTERRUPT();
#endif
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_disable_interrupt
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_enable_interrupt
 * Description  : wrapper function for enable interrupts
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void r_fwup_wrap_enable_interrupt(void)
{
    /**** Start user code ****/
#if defined(__RX)
    R_BSP_InterruptsEnable();
#else
    BSP_ENABLE_INTERRUPT();
#endif
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_enable_interrupt
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_software_reset
 * Description  : wrapper function for execute software reset.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void r_fwup_wrap_software_reset(void)
{
    /**** Start user code ****/

    r_fwup_wrap_disable_interrupt();
#if (FWUP_CFG_PRINTF_DISABLE == 0)
    r_fwup_wrap_software_delay(1000, FWUP_DELAY_MILLISECS);
#endif /* (FWUP_CFG_PRINTF_DISABLE == 0) */
    /* SW Reset */
#if defined(__RX)
    R_BSP_SoftwareReset();
#else
    /* Unauthorized access detection enabled */
    IAWCTL=0x80;

    /* White at address 0x00 */
    *(__far volatile char *)0x00=0x00;
    BSP_NOP();
    while(1);
#endif

    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_software_reset
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_software_delay
 * Description  : wrapper function for execute software delay.
 * Arguments    : delay : delay count
 *                units : FWUP_DELAY_MICROSECS/FWUP_DELAY_MILLISECS/FWUP_DELAY_SECS
 * Return Value : 0     : success
 *                other : error
 *********************************************************************************************************************/
uint32_t r_fwup_wrap_software_delay(uint32_t delay, e_fwup_delay_units_t units)
{
    /**** Start user code ****/
#if defined(__RX)
    uint32_t  time_units;

    if (FWUP_DELAY_MICROSECS == units)
    {
        time_units = BSP_DELAY_MICROSECS;
    }
    else if (FWUP_DELAY_MILLISECS == units)
    {
        time_units = BSP_DELAY_MILLISECS;
    }
    else
    {
        time_units = BSP_DELAY_SECS;
    }
    return ((uint32_t)R_BSP_SoftwareDelay(delay, (bsp_delay_units_t)time_units));
#else
    uint32_t time;
    uint32_t fclk = 32;

    switch (units)
    {
        case FWUP_DELAY_MICROSECS:
            time = delay;
            fclk = (uint32_t)(fclk / 20U);
            break;
        case FWUP_DELAY_MILLISECS:
            time = (uint32_t)(delay * 100U);
            break;
        case FWUP_DELAY_SECS:
            time = (uint32_t)(delay * 100000U);
            break;
        default:
            break;
    }

    while(0 < time)
    {
        /* Delay 1us */
        for(uint16_t cnt = fclk; 0 < cnt; cnt--)
        {
            BSP_NOP();
            BSP_NOP();
        }
        time--;
    }
    return (0);
#endif
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_software_delay
 *********************************************************************************************************************/
