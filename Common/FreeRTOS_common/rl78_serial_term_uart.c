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
 * File Name    : serial_term_uart.c
 * Description  : .
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : DD.MM.YYYY 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rl78_serial_term_uart.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
sci_hdl_t sci_handle;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint8_t  s_cmd_buf[256];

/**********************************************************************************************************************
 * Function Name: uart_config
 * Description  : Peripheral UART operation for log outpt.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void uart_config(void)
{
    sci_cfg_t sci_config;

    if (SCI_SUCCESS != R_SCI_Open(SERIAL_TERM_CH, SCI_MODE_ASYNC, &sci_config, NULL, &sci_handle))
    {
        R_BSP_NOP();
    }
}
/**********************************************************************************************************************
 * End of function uart_config
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: uart_string_printf
 * Description  : Sends the specified character string on the set UART channel.
 * Arguments    : pString-
 *                  Pointer type log output character string.
 * Return Value : none
 *********************************************************************************************************************/
void uart_string_printf(RL78_FAR char *pString)
{
    sci_err_t sci_error;

    uint16_t retry = 0xffff;
    uint16_t str_length;
    uint16_t transmit_length;

    str_length = (uint16_t) strlen(pString);

    while ((retry > 0) && (str_length > 0))
    {
        R_SCI_Control(sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &transmit_length);

        if (transmit_length > str_length)
        {
            transmit_length = str_length;
        }

        sci_error = R_SCI_Send(sci_handle, (uint8_t SCI_FAR*) pString, transmit_length);
        if ((SCI_ERR_XCVR_BUSY == sci_error) || (SCI_ERR_INSUFFICIENT_SPACE == sci_error))
        {
            /* retry if previous transmission still in progress or tx buffer is insufficient. */
            retry--;
        }
        else
        {
            str_length -= transmit_length;
            pString += transmit_length;
        }
    }

    if (SCI_SUCCESS != sci_error)
    {
        R_BSP_NOP();
    }
}
/**********************************************************************************************************************
 * End of function uart_string_printf
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: uart_printf
 * Description  : Sends the specified character string on the set UART channel.
 * Arguments    : pString-
 *                  Pointer type log output character string.
 *                ...
 *                  format
 * Return Value : none
 *********************************************************************************************************************/
void uart_printf(RL78_FAR char *pString, ...)
{
    sci_err_t sci_error;

    uint16_t retry = 0xffff;
    uint16_t str_length;
    uint16_t transmit_length;
    uint8_t * p_cmd_buf = s_cmd_buf;
    va_list  args;

    va_start(args, pString);
    vsnprintf((char __far *)p_cmd_buf, sizeof(p_cmd_buf), pString, args);
    va_end(args);

    str_length = (uint16_t) strlen((const char __far *)p_cmd_buf);

    while ((retry > 0) && (str_length > 0))
    {
        R_SCI_Control(sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &transmit_length);

        if (transmit_length > str_length)
        {
            transmit_length = str_length;
        }

        sci_error = R_SCI_Send(sci_handle, (uint8_t SCI_FAR *) p_cmd_buf, transmit_length);
        if ((SCI_ERR_XCVR_BUSY == sci_error) || (SCI_ERR_INSUFFICIENT_SPACE == sci_error))
        {
            /* retry if previous transmission still in progress or tx buffer is insufficient. */
            retry--;
        }
        else
        {
            str_length -= transmit_length;
            p_cmd_buf += transmit_length;
        }
    }

    if (SCI_SUCCESS != sci_error)
    {
        R_BSP_NOP();
    }
}
/**********************************************************************************************************************
 * End of function uart_printf
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: putchar
 * Description  : none
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
RL78_FAR_FUNC int putchar(int c)
{
    while(0 != (REG_SSR & 0x20))
    {
        NOP();
    }
    REG_STMK = 0;
    REG_TXD = (unsigned char)c;
    REG_STMK = 1;
    return c;
}
/**********************************************************************************************************************
 * End of function putchar
 *********************************************************************************************************************/
