/***********************************************************************************************************************
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/******************************************************************************************************************//**
 * @file          r_trng.c
 * @brief         TRNG driver
 *********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

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
 * @retval         0: TRNG_SUCCESS
 * @retval        -1: TRNG_BUSY
 * @details       This function gets random numbers.
 *********************************************************************************************************************/
int8_t R_TRNG_GetRandomNumber(uint8_t *random)
{
    uint8_t i;

    if (TRNGSCR0 & TRNGRDY_CHK) /* Check the TRNGSCR0 */
    {
        /* Store the Ramdom Number */
        for (i = 0; i < RANDOM_LENGTH; i++)
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

