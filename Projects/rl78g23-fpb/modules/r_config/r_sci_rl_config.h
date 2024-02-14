/*
 * r_sci_rl_config.h
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

#ifndef R_SCI_RL_CONFIG_H
#define R_SCI_RL_CONFIG_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"

/***********************************************************************************************************************
 Configuration Options
 **********************************************************************************************************************/

/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING */
/* Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting */
/* Setting to 1 includes parameter checking; 0 compiles out parameter checking */
#define SCI_CFG_PARAM_CHECKING_ENABLE   (0)

/* SPECIFY WHETHER TO INCLUDE CODE FOR DIFFERENT SCI MODES */
/* Setting an equate to 1 includes code specific to that mode. */
#define SCI_CFG_ASYNC_INCLUDED          (1)
#define SCI_CFG_SYNC_INCLUDED           (0)   /* Not supported */
#define SCI_CFG_SSPI_INCLUDED           (0)   /* Not supported */

/* SPECIFY BYTE VALUE TO TRANSMIT WHILE CLOCKING IN DATA IN SSPI MODES */
#define SCI_CFG_DUMMY_TX_BYTE           (0xFF)

/* SPECIFY CHANNELS TO INCLUDE SOFTWARE SUPPORT FOR 1=included, 0=not */
#define SCI_CFG_CH0_INCLUDED            (0)
#define SCI_CFG_CH1_INCLUDED            (0)
#define SCI_CFG_CH2_INCLUDED            (1)
#define SCI_CFG_CH3_INCLUDED            (1)

/* SPECIFY ASYNC MODE TX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_TX_BUFSIZ           (80)
#define SCI_CFG_CH1_TX_BUFSIZ           (80)
#define SCI_CFG_CH2_TX_BUFSIZ           (128)
#define SCI_CFG_CH3_TX_BUFSIZ           (128)

/* SPECIFY ASYNC MODE RX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_RX_BUFSIZ           (80)
#define SCI_CFG_CH1_RX_BUFSIZ           (80)
#define SCI_CFG_CH2_RX_BUFSIZ           (80)
#define SCI_CFG_CH3_RX_BUFSIZ           (128)

/* 
* ENABLE TRANSMIT END INTERRUPT (ASYNCHRONOUS)
* This interrupt only occurs when the last bit of the last byte of data 
* has been sent and the transmitter has become idle. The interrupt calls
* the user's callback function specified in R_SCI_Open() and passes it an
* SCI_EVT_TEI event. A typical use of this feature is to disable an external
* transceiver to save power. It would then be up to the user's code to 
* re-enable the transceiver before sending again. Not including this feature
* reduces code space used by the interrupt. Note that this equate is only
* for including the TEI code. The interrupt itself must be enabled using an
* R_SCI_Control(hdl, SCI_CMD_EN_TEI, NULL) call.
*/
#define SCI_CFG_TEI_INCLUDED            (1)      /* 1=included, 0=not */

/*
* The following settings are used for follow control to prevent overflow of
* the receive buffer when receiving UART.
*/
/* 1=Enable , 0=Disable */
#define SCI_CFG_UART0_FLOW_CTRL_ENABLE  (0)
#define SCI_CFG_UART1_FLOW_CTRL_ENABLE  (0)
#define SCI_CFG_UART2_FLOW_CTRL_ENABLE  (0)
#define SCI_CFG_UART3_FLOW_CTRL_ENABLE  (1)

/* If SCI_CFG_UARTx_FLOW_CTRL_ENABLE is '1', need to set I/O Port. */
#define SCI_CFG_UART0_RTS_PORT
#define SCI_CFG_UART1_RTS_PORT
#define SCI_CFG_UART2_RTS_PORT          (P1_bit.no5)
#define SCI_CFG_UART3_RTS_PORT          (P14_bit.no2)

/*
 * Threshold for free receive buffer to set RTS high.
 */
#define SCI_CFG_UART_RX_BUFFER_THRESH   (20)

#endif /* R_SCI_RL_CONFIG_H */
