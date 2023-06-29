/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021, 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_PORT.c
* Component Version: 1.4.0
* Device(s)        : R7F100GSNxFB
* Description      : This file implements device driver for Config_PORT.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_PORT.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_PORT_Create
* Description  : This function initializes the port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PORT_Create(void)
{
    volatile uint16_t w_count;

    CCDE = _00_P63_OUTPUT_CURRENT_OFF | _00_P62_OUTPUT_CURRENT_OFF | _00_P61_OUTPUT_CURRENT_OFF | 
           _00_P60_OUTPUT_CURRENT_OFF;

    /* Wait for stable time (10us) */
    for (w_count = 0U; w_count < PORT_STABLE_WAITTIME; w_count++)
    {
        NOP();
    }
 
    /* Set PORT6 registers */
    P6 = _00_Pn7_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | _00_Pn3_OUTPUT_0 | 
         _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;
    PMCT6 = _00_PMCTn7_NOT_USE | _00_PMCTn6_NOT_USE | _00_PMCTn5_NOT_USE | _00_PMCTn4_NOT_USE;
    PMCE6 = _00_PMCEn1_DIGITAL_ON | _00_PMCEn0_NOT_USE;
    PM6 = _80_PMn7_NOT_USE | _40_PMn6_NOT_USE | _20_PMn5_NOT_USE | _10_PMn4_NOT_USE | _08_PMn3_NOT_USE | 
          _04_PMn2_NOT_USE | _00_PMn1_MODE_OUTPUT | _01_PMn0_NOT_USE;

    R_Config_PORT_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_PORT_ReadPmnValues
* Description  : This function specifies the value in the output latch for a port is read when the pin is in output mode.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_PORT_ReadPmnValues(void)
{
    PMS = _00_PMN_VALUES;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
