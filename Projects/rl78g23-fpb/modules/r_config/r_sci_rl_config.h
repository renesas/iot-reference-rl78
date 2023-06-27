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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_sci_rl_config.h
 * Version      : 0.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description

 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"

#ifndef R_SCI_RL_CONFIG_H
#define R_SCI_RL_CONFIG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/

/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING */
/* Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting */
/* Setting to 1 includes parameter checking; 0 compiles out parameter checking */
#define SCI_CFG_PARAM_CHECKING_ENABLE   (0)

/* SPECIFY WHETHER TO INCLUDE CODE FOR DIFFERENT SCI MODES */
/* Setting an equate to 1 includes code specific to that mode. */
#define SCI_CFG_ASYNC_INCLUDED  (1)
#define SCI_CFG_SYNC_INCLUDED   (0)
#define SCI_CFG_SSPI_INCLUDED   (0)

/* SPECIFY BYTE VALUE TO TRANSMIT WHILE CLOCKING IN DATA IN SSPI MODES */
#define SCI_CFG_DUMMY_TX_BYTE   (0xFF)

/* SPECIFY CHANNELS TO INCLUDE SOFTWARE SUPPORT FOR 1=included, 0=not */
#define SCI_CFG_CH0_INCLUDED    (0)
#define SCI_CFG_CH1_INCLUDED    (0)
#define SCI_CFG_CH2_INCLUDED    (1)
#define SCI_CFG_CH3_INCLUDED    (1)

/* SPECIFY ASYNC MODE TX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_TX_BUFSIZ   (80)
#define SCI_CFG_CH1_TX_BUFSIZ   (80)
#define SCI_CFG_CH2_TX_BUFSIZ   (128)
#define SCI_CFG_CH3_TX_BUFSIZ   (256)

/* SPECIFY ASYNC MODE RX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_RX_BUFSIZ   (80)
#define SCI_CFG_CH1_RX_BUFSIZ   (80)
#define SCI_CFG_CH2_RX_BUFSIZ   (128)
#define SCI_CFG_CH3_RX_BUFSIZ   (256)

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
#define SCI_CFG_TEI_INCLUDED    (1)      /* 1=included, 0=not */

/*
* The following settings are used for follow control to prevent overflow of
* the receive buffer when receiving UART.
*/
#define SCI_CFG_UART0_FLOW_CTRL_ENABLE   (0)
#define SCI_CFG_UART1_FLOW_CTRL_ENABLE   (0)
#define SCI_CFG_UART2_FLOW_CTRL_ENABLE   (0)
#define SCI_CFG_UART3_FLOW_CTRL_ENABLE   (1)

#define SCI_CFG_UART0_CTS_PORT
#define SCI_CFG_UART1_CTS_PORT
#define SCI_CFG_UART2_CTS_PORT           (P1_bit.no6)
#define SCI_CFG_UART3_CTS_PORT           (P14_bit.no1)
#define SCI_CFG_UART0_RTS_PORT
#define SCI_CFG_UART1_RTS_PORT
#define SCI_CFG_UART2_RTS_PORT           (P1_bit.no5)
#define SCI_CFG_UART3_RTS_PORT           (P14_bit.no2)

/*
 * Threshold for free receive buffer to set RTS high.
 */
#define SCI_CFG_UART_RX_BUFFER_THRESH    (20)

#endif /* R_SCI_RL_CONFIG_H */
