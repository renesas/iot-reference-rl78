/*
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name    : rl78_serial_term_uart.c
* Description  : Header file for UART terminal interface on RL78, providing declarations for FreeRTOS entry functions
*                and related serial communication routines
***********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Renesas includes. */
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

const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5000);

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint8_t  s_cmd_buf[256];

/* Used to guard access to the UART in case messages are sent to the UART from
more than one task. */
static SemaphoreHandle_t xTransmitMutex = NULL;

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

    /* Create the semaphore used to access the UART Tx. */
    xTransmitMutex = xSemaphoreCreateMutex();
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

    str_length = (uint16_t)strlen(pString);    /* Cast to (uint16_t) */

    if (xSemaphoreTake(xTransmitMutex, xMaxBlockTime) == pdPASS)
    {
        while ((retry > 0) && (str_length > 0))
        {
            R_SCI_Control(sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &transmit_length);

            if (transmit_length > str_length)
            {
                transmit_length = str_length;
            }

            sci_error = R_SCI_Send(sci_handle, (uint8_t SCI_FAR*)pString, transmit_length);
            if ((SCI_ERR_XCVR_BUSY == sci_error) || (SCI_ERR_INSUFFICIENT_SPACE == sci_error))
            {
                /* retry if previous transmission still in progress or tx buffer is insufficient. */
                retry--;
                continue;
            }
            else
            {
                str_length -= transmit_length;
                pString    += transmit_length;
            }
        }

        /* Must ensure to give the mutex back. */
        xSemaphoreGive(xTransmitMutex);
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

    uint16_t  retry = 0xffff;
    uint16_t  str_length;
    uint16_t  transmit_length;
    uint8_t * p_cmd_buf = s_cmd_buf;
    va_list   args;

    va_start(args, pString);
    vsnprintf((char __far *)p_cmd_buf, sizeof(p_cmd_buf), pString, args);   /* Cast to (char __far *) */
    va_end(args);

    /* Cast first params to (char __far *) */
    /* Cast return of strlen() to (uint16_t) */
    str_length = (uint16_t)strlen((const char __far *)p_cmd_buf);

    while ((retry > 0) && (str_length > 0))
    {
        R_SCI_Control(sci_handle, SCI_CMD_TX_Q_BYTES_FREE, &transmit_length);

        if (transmit_length > str_length)
        {
            transmit_length = str_length;
        }

        sci_error = R_SCI_Send(sci_handle, (uint8_t SCI_FAR *)p_cmd_buf, transmit_length);
        if ((SCI_ERR_XCVR_BUSY == sci_error) || (SCI_ERR_INSUFFICIENT_SPACE == sci_error))
        {
            /* retry if previous transmission still in progress or tx buffer is insufficient. */
            retry--;
        }
        else
        {
            str_length -= transmit_length;
            p_cmd_buf  += transmit_length;
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
    while (0 != (REG_SSR & 0x20))
    {
        NOP();
    }
    REG_STMK = 0;
    REG_TXD  = (unsigned char)c; /* Cast to (unsigned char) */
    REG_STMK = 1;
    return c;
}
/**********************************************************************************************************************
 * End of function putchar
 *********************************************************************************************************************/
