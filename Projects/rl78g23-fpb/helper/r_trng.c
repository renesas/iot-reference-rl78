/*
 * r_trng.c
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

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdint.h>
#include "r_trng.h"
#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 * Function Name: R_TRNG_GenerateRandomNumber
 * @brief         .
 * @param         None
 * @retval        0: TRNG_SUCCESS
 * @details       This function enable the random number generator and start the random number generation.
 *********************************************************************************************************************/
int8_t R_TRNG_GenerateRandomNumber(void)
{
    TRNGSCR0 = 0x0C; /* Start random number generation */
    return TRNG_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_TRNG_GenerateRandomNumber
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 * Function Name: R_TRNG_GetRandomNumber
 * @brief         .
 * @param [in]    random: top address of random number
 * @param [in]    size m: size of of random number
 * @retval         0: TRNG_SUCCESS
 * @retval        -1: TRNG_BUSY
 * @details       This function gets random numbers.
 *********************************************************************************************************************/
int8_t R_TRNG_GetRandomNumber(uint8_t *random, uint16_t size)
{
    uint16_t i;

    if (TRNGSCR0 & TRNGRDY_CHK) /* Check the TRNGSCR0 */
    {
        /* Store the Ramdom Number */
        for (i = 0; i < size; i++)
        {
            *(random + i) = TRNGSDR;
        }
        TRNGSCR0 = 0x00; /* Stop TRNG */
        return TRNG_SUCCESS;
    }
    else
    {
        return TRNG_BUSY; /* Generating random numbers */
    }
}
/**********************************************************************************************************************
 * End of function R_TRNG_GetRandomNumber
 **********************************************************************************************************************/

