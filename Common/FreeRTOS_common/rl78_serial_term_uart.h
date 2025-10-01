/*
 * rl78_serial_term_uart.h
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
#include <string.h>
#include <stdarg.h>
#include "r_smc_entry.h"
#include "rl78_bsp_wrapper.h"
#include "r_sci_rl_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef SERIAL_TERM_UART_H_
#define SERIAL_TERM_UART_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
#define RL78_FAR                __far
#if defined(__CCRL__)
#define RL78_FAR_FUNC           __far
#elif defined(__ICCRL78__)
#define RL78_FAR_FUNC           __far_func
#endif

#define SERIAL_TERM_CH          (2)

#if (SERIAL_TERM_CH == 1)
#define REG_SSR                 (SSR02)
#define REG_STMK                (STMK1)
#define REG_TXD                 (TXD1)
#define UART_START_FUNC         (R_Config_UART1_Start)
#endif
#if (SERIAL_TERM_CH == 2)
#define REG_SSR                 (SSR10)
#define REG_STMK                (STMK2)
#define REG_TXD                 (TXD2)
#define UART_START_FUNC         (R_Config_UART2_Start)
#endif

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * function name: uart_config
 *********************************************************************************************************************/
void uart_config (void);

/**********************************************************************************************************************
 * function name: uart_string_printf
 *********************************************************************************************************************/
void uart_string_printf (RL78_FAR char * pString);

/**********************************************************************************************************************
 * function name: uart_printf
 *********************************************************************************************************************/
void uart_printf (RL78_FAR char * pString, ...);

#endif /* SERIAL_TERM_UART_H_ */
