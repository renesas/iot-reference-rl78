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
* File Name        : Config_UART3_user.c
* Component Version: 1.6.0
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
#ifdef DELETE
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_Config_UART3_interrupt_send(vect=INTST3)
#pragma interrupt r_Config_UART3_interrupt_receive(vect=INTSR3)
#pragma interrupt r_Config_UART3_interrupt_error(vect=INTSRE3)
/* Start user code for pragma. Do not edit comment generated here */
#endif
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_uart3_tx_address;    /* uart3 transmit buffer address */
extern volatile uint16_t g_uart3_tx_count;        /* uart3 transmit data number */
extern volatile uint8_t * gp_uart3_rx_address;    /* uart3 receive buffer address */
extern volatile uint16_t g_uart3_rx_count;        /* uart3 receive data number */
extern uint16_t g_uart3_rx_length;                /* uart3 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_UART3_Create_UserInit
* Description  : This function adds user code after initializing UART3.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Config_UART3_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_callback_sendend
* Description  : This function is a callback function when UART3 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_Config_UART3_callback_sendend(void)
{
    /* Start user code for r_Config_UART3_callback_sendend. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_callback_receiveend
* Description  : This function is a callback function when UART3 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_Config_UART3_callback_receiveend(void)
{
    /* Start user code for r_Config_UART3_callback_receiveend. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_callback_error
* Description  : This function is a callback function when UART3 reception error occurs.
* Arguments    : err_type -
*                    error type info
* Return Value : None
***********************************************************************************************************************/
static void r_Config_UART3_callback_error(uint8_t err_type)
{
    /* Start user code for r_Config_UART3_callback_error. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_callback_softwareoverrun
* Description  : This function is a callback when UART3 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_Config_UART3_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code for r_Config_UART3_callback_softwareoverrun. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_interrupt_send
* Description  : This function is UART3 send interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_Config_UART3_interrupt_send(void)
{
    if (g_uart3_tx_count > 0U)
    {
        TXD3 = *gp_uart3_tx_address;
        gp_uart3_tx_address++;
        g_uart3_tx_count--;
    }
    else
    {
        r_Config_UART3_callback_sendend();
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_interrupt_receive
* Description  : This function is UART3 receive interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_Config_UART3_interrupt_receive(void)
{
    uint8_t rx_data;

    rx_data = RXD3;

    if (g_uart3_rx_length > g_uart3_rx_count)
    {
        *gp_uart3_rx_address = rx_data;
        gp_uart3_rx_address++;
        g_uart3_rx_count++;

        if (g_uart3_rx_length == g_uart3_rx_count)
        {
            r_Config_UART3_callback_receiveend();
        }
    }
    else
    {
        r_Config_UART3_callback_softwareoverrun(rx_data);
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_UART3_interrupt_error
* Description  : This function is UART3 error interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_Config_UART3_interrupt_error(void)
{
    uint8_t err_type;

    *gp_uart3_rx_address = RXD3;
    err_type = (uint8_t)(SSR13 & 0x0007U);
    SIR13 = (uint16_t)err_type;
    r_Config_UART3_callback_error(err_type);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
