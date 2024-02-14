/*
 * rl78_bsp_wrapper.c
 * Copyright (C) Renesas Electronics Corporation and/or its affiliates.
 * All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rl78_bsp_wrapper.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * function name: R_BSP_SoftwareReset
 *********************************************************************************************************************/
void R_BSP_SoftwareReset(void)
{
    IAWCTL=0x80;                        // Unauthorized access detection enabled

    *(__far volatile char *)0x00=0x00;  // White at address 0x00
    R_BSP_NOP();
    while(1)
    {
        ;                               // Reset occurs before coming here
    }
}
/**********************************************************************************************************************
 * End of function R_BSP_SoftwareReset
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * function name: R_BSP_GET_PSW
 *********************************************************************************************************************/
uint16_t R_BSP_GET_PSW(void)
{
    uint16_t ret;

    ret = __get_psw();

    return ret;
}
/**********************************************************************************************************************
 * End of function R_BSP_GET_PSW
 *********************************************************************************************************************/
