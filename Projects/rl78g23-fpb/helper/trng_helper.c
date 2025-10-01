/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : trng_helper.c
 * Description  : Functions for the TRNG.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 27.12.2023 1.00    First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "trng_helper.h"

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
 * function name: xTrngGenerateRandomNumber
 *********************************************************************************************************************/
BaseType_t xTrngGenerateRandomNumber(uint8_t * pusRandomNumBuffer, size_t xBufferLength)
{
    R_TRNG_GenerateRandomNumber();
    BSP_NOP();
    while (TRNG_SUCCESS != R_TRNG_GetRandomNumber(pusRandomNumBuffer, xBufferLength))
    {
        ;
    }

    return pdPASS;
}/* End of function xTrngGenerateRandomNumber()*/

/**********************************************************************************************************************
 * function name: uxRand
 *********************************************************************************************************************/
uint32_t uxRand(void)
{
    static uint32_t ulRandomNum = 0;

    /* Cast to proper datatype to avoid warning */
    xTrngGenerateRandomNumber((uint8_t *)&ulRandomNum, sizeof(ulRandomNum));
    return ulRandomNum;
}/* End of function uxRand()*/
