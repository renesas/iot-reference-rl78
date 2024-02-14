/*
 * putchar_rl78.h
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
#include <stdio.h>
#include "r_smc_entry.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef PUTCHAR_RL78_H_
#define PUTCHAR_RL78_H_

#if defined(__CCRL__)
#define PUTCHAR_RL78_FAR_FUNC                __far
#elif defined(__ICCRL78__)
#define PUTCHAR_RL78_FAR_FUNC                __far_func
#endif

#define USE_UART_CH         (2)

#if (USE_UART_CH == 1)
#define REG_SSR             (SSR02)
#define REG_STMK            (STMK1)
#define REG_TXD             (TXD1)
#define UART_START_FUNC     (R_Config_UART1_Start)
#define UART_STOP_FUNC      (R_Config_UART1_Stop)
#endif
#if (USE_UART_CH == 2)
#define REG_SSR             (SSR10)
#define REG_STMK            (STMK2)
#define REG_TXD             (TXD2)
#define UART_START_FUNC     (R_Config_UART2_Start)
#define UART_STOP_FUNC      (R_Config_UART2_Stop)
#endif

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* PUTCHAR_RL78_H_ */
