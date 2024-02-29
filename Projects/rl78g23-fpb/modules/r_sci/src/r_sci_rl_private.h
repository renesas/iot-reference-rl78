/*
 * r_sci_rl_private.h
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

#ifndef R_SCI_RL_PRIVATE_H
#define R_SCI_RL_PRIVATE_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_sci_rl_if.h"
#include "r_sci_rl_platform.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Bit position masks */
#define BIT0_MASK   (0x00000001U)
#define BIT1_MASK   (0x00000002U)
#define BIT2_MASK   (0x00000004U)
#define BIT3_MASK   (0x00000008U)
#define BIT4_MASK   (0x00000010U)
#define BIT5_MASK   (0x00000020U)
#define BIT6_MASK   (0x00000040U)
#define BIT7_MASK   (0x00000080U)
#define BIT8_MASK   (0x00000100U)
#define BIT9_MASK   (0x00000200U)
#define BIT10_MASK  (0x00000400U)
#define BIT11_MASK  (0x00000800U)
#define BIT12_MASK  (0x00001000U)
#define BIT13_MASK  (0x00002000U)
#define BIT14_MASK  (0x00004000U)
#define BIT15_MASK  (0x00008000U)
#define BIT16_MASK  (0x00010000U)
#define BIT17_MASK  (0x00020000U)
#define BIT18_MASK  (0x00040000U)
#define BIT19_MASK  (0x00080000U)
#define BIT20_MASK  (0x00100000U)
#define BIT21_MASK  (0x00200000U)
#define BIT22_MASK  (0x00400000U)
#define BIT23_MASK  (0x00800000U)
#define BIT24_MASK  (0x01000000U)
#define BIT25_MASK  (0x02000000U)
#define BIT26_MASK  (0x04000000U)
#define BIT27_MASK  (0x08000000U)
#define BIT28_MASK  (0x10000000U)
#define BIT29_MASK  (0x20000000U)
#define BIT30_MASK  (0x40000000U)
#define BIT31_MASK  (0x80000000U)

#ifndef NULL    /* Resolves e2studio code analyzer false error message. */
    #define NULL (0)
#endif

#if ((SCI_CFG_CH0_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH1_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH2_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH3_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH4_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH5_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH6_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH7_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH8_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH9_DATA_MATCH_INCLUDED)  ||   \
        (SCI_CFG_CH10_DATA_MATCH_INCLUDED) ||   \
        (SCI_CFG_CH11_DATA_MATCH_INCLUDED))
    #define SCI_CFG_DATA_MATCH_INCLUDED (1)
#endif

/* SSR register read access */
#define SCI_SSR          GetReg_SSR(hdl->channel)

/* TDR/FTDR register write access */
#define SCI_TDR(byte)    SetReg_TXD(hdl->channel, (byte));

/* RDR/FRDR register read access */
#define SCI_RDR(byte)    GetReg_RXD(hdl->channel, (byte));

/* SCR register dummy read */
#define SCI_SCR_DUMMY_READ                \
    if (0x00 == hdl->rom->regs->SCR.BYTE) \
    {                                     \
        R_BSP_NOP();                      \
    }

/* Interrupt Request register flag clear */
#define SCI_IR_TXI_CLEAR (*hdl->rom->ir_txi = 0)

/* Reserved space on RX */
#define FIT_NO_FUNC      ((void (*)(void *))0x10000000)
#define FIT_NO_PTR       ((void *)0x10000000)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* R_SCI_RL_PRIVATE_H */

