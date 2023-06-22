/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : serial_term_uart.h
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : DD.MM.YYYY 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "r_smc_entry.h"
#include "rl78_wrapper_com.h"
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

#define SERIAL_TERM_CH      	(2)

#if (SERIAL_TERM_CH == 1)
#define REG_SSR             	(SSR02)
#define REG_STMK            	(STMK1)
#define REG_TXD             	(TXD1)
#define UART_START_FUNC     	(R_Config_UART1_Start)
#endif
#if (SERIAL_TERM_CH == 2)
#define REG_SSR             	(SSR10)
#define REG_STMK            	(STMK2)
#define REG_TXD             	(TXD2)
#define UART_START_FUNC     	(R_Config_UART2_Start)
#endif

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void uart_config (void);
void uart_string_printf (RL78_FAR char * pString);
void uart_printf (RL78_FAR char * pString, ...);

#endif /* SERIAL_TERM_UART_H_ */
