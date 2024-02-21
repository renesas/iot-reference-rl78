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
 * @file          r_trng.h
 * @brief         TRNG driver header file
 *********************************************************************************************************************/
#ifndef R_TRNG_H_
#define R_TRNG_H_

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define TRNGRDY_CHK         (0x80)  /* TRNGRDY check */

/*----------- Definitions of return value ------------*/
#define TRNG_SUCCESS        (0)    /* Successful operation */
#define TRNG_BUSY           (-1)   /* Busy operation */

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_TRNG_GenerateRandomNumber
 * @brief         .
 * @param         None
 * @retval        0: TRNG_SUCCESS
 * @details       This function generates random numbers.
 *********************************************************************************************************************/
int8_t R_TRNG_GenerateRandomNumber (void);

/**********************************************************************************************************************
 * Function Name: R_TRNG_GetRandomNumber
 * @brief         .
 * @param [in]    random: top address of random number
 * @param [in]    size  : size of of random number
 * @retval         0: TRNG_SUCCESS
 * @retval        -1: TRNG_BUSY
 * @details       This function gets random numbers.
 *********************************************************************************************************************/
int8_t R_TRNG_GetRandomNumber (uint8_t *random, uint16_t size);

#endif /* R_TRNG_H_ */
