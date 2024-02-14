/*
 * r_trng.h
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
