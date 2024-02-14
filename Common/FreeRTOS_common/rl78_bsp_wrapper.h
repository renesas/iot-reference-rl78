/*
 * rl78_bsp_wrapper.h
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
#include "platform.h"
#include "stdbool.h"

#ifndef RL78_WRPPER_COM_H_
#define RL78_WRPPER_COM_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/* This file is for compatibility with RX. */
#ifndef R_BSP_NOP
#define R_BSP_NOP                     BSP_NOP
#endif
#define R_BSP_InterruptsDisable       BSP_DISABLE_INTERRUPT
#define R_BSP_InterruptsEnable        BSP_ENABLE_INTERRUPT

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
//bool R_BSP_SoftwareDelay (uint32_t delay, bsp_delay_units_t units);
void R_BSP_SoftwareReset (void);
uint16_t R_BSP_GET_PSW (void);

#endif /* RL78_WRPPER_COM_H_ */
