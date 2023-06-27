/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021, 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_UART3.c
* Component Version: 1.5.0
* Device(s)        : R7F100GSNxFB
* Description      : This file implements device driver for Config_UART3.
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "Config_UART3.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_uart3_tx_address;    /* uart3 transmit buffer address */
volatile uint16_t g_uart3_tx_count;        /* uart3 transmit data number */
volatile uint8_t * gp_uart3_rx_address;    /* uart3 receive buffer address */
volatile uint16_t g_uart3_rx_count;        /* uart3 receive data number */
uint16_t g_uart3_rx_length;                /* uart3 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Create
* Description  : This function initializes the UART3 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Create(void)
{
    SPS1 &= _000F_SAU_CK01_CLEAR;
    SPS1 |= _0010_SAU_CK01_FCLK_1;
    ST1 |= (_0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON);
    STMK3 = 1U;    /* disable INTST3 interrupt */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRMK3 = 1U;    /* disable INTSR3 interrupt */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
    SREMK3 = 1U;    /* disable INTSRE3 interrupt */
    SREIF3 = 0U;    /* clear INTSRE3 interrupt flag */
    /* Set INTST3 low priority */
    STPR13 = 1U;
    STPR03 = 1U;
    /* Set INTSR3 level2 priority */
    SRPR13 = 1U;
    SRPR03 = 0U;
    /* Set INTSRE3 low priority */
    SREPR13 = 1U;
    SREPR03 = 1U;
    SMR12 = _0020_SAU_SMRMN_INITIALVALUE | _8000_SAU_CLOCK_SELECT_CK01 | _0000_SAU_TRIGGER_SOFTWARE | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR12 = _0004_SAU_SCRMN_INITIALVALUE | _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | 
            _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR12 = _1000_SAU1_CH2_TRANSMIT_DIVISOR;
    NFEN0 |= _40_SAU_RXD3_FILTER_ON;
    SIR13 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR13 = _0020_SAU_SMRMN_INITIALVALUE | _8000_SAU_CLOCK_SELECT_CK01 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR13 = _0004_SAU_SCRMN_INITIALVALUE | _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | 
            _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR13 = _1000_SAU1_CH3_RECEIVE_DIVISOR;
    SO1 |= _0004_SAU_CH2_DATA_OUTPUT_1;
    SOL1 |= _0000_SAU_CHANNEL2_NORMAL;    /* output level normal */
    SOE1 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART3 output */
    /* Set TxD3 pin */
    P14 |= 0x10U;
    PM14 &= 0xEFU;
    /* Set RxD3 pin */
    PM14 |= 0x08U;

    R_Config_UART3_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Start
* Description  : This function starts UART3 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Start(void)
{
    SO1 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
    SOE1 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART3 output */
    SS1 |= (_0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON);    /* enable UART3 receive and transmit */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
    SREIF3 = 0U;    /* clear INTSRE3 interrupt flag */
    STMK3 = 0U;    /* enable INTST3 interrupt */
    SRMK3 = 0U;    /* enable INTSR3 interrupt */
    SREMK3 = 0U;    /* enable INTSRE3 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Stop
* Description  : This function stops UART3 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Stop(void)
{
    STMK3 = 1U;    /* disable INTST3 interrupt */
    SRMK3 = 1U;    /* disable INTSR3 interrupt */
    SREMK3 = 1U;    /* disable INTSRE3 interrupt */
    ST1 |= (_0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON);
    SOE1 &= (uint16_t)~_0004_SAU_CH2_OUTPUT_ENABLE;    /* disable UART3 output */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
    SREIF3 = 0U;    /* clear INTSRE3 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Send
* Description  : This function sends UART3 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_Config_UART3_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart3_tx_address = tx_buf;
        g_uart3_tx_count = tx_num;
        STMK3 = 1U;    /* disable INTST3 interrupt */
        TXD3 = *gp_uart3_tx_address;
        gp_uart3_tx_address++;
        g_uart3_tx_count--;
        STMK3 = 0U;    /* enable INTST3 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Receive
* Description  : This function receives UART3 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_Config_UART3_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart3_rx_count = 0U;
        g_uart3_rx_length = rx_num;
        gp_uart3_rx_address = rx_buf;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Loopback_Enable
* Description  : This function enables the UART3 loopback function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Loopback_Enable(void)
{
    ULBS3 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Loopback_Disable
* Description  : This function disables the UART3 loopback function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Loopback_Disable(void)
{
    ULBS3 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
