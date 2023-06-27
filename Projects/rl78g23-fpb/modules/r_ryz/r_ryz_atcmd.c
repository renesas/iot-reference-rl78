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
 * File Name    : r_ryz_atcmd.c
 * Version      : 1.0
 * Description  : AT command functions definition.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : DD.MM.YYYY 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_ryz_private.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define CMD_BUF_MAX     (RYZ_AT_CMD_BUF_MAX)
#define RESP_BUF_MAX    (RYZ_AT_RESP_BUF_MAX)
#define PREFIX_LEN_MAX  (128)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    uint8_t  WIFI_FAR * p_text;
    e_rslt_code_t       rslt_cd;
} st_rslt_code_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/* User callback */
#if(1 == WIFI_CFG_USE_CALLBACK_FUNCTION)
void WIFI_CFG_CALLBACK_FUNCTION_NAME(void * p_args);
void (*const p_wifi_callback)(void *p_args) = WIFI_CFG_CALLBACK_FUNCTION_NAME;
#else
void (* const p_wifi_callback)(void *p_args) = NULL;
#endif

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/* AT commands return result code */
static const st_rslt_code_t s_rtncode[] =
{
    {"OK\r\n"             , AT_OK          },
    {"ERROR\r\n"          , AT_ERROR       },
    {"CONNECT\r\n"        , AT_CONNECT     },
    {"RING\r\n"           , AT_RING        },
//    {"NO CARRIER\r\n"     , AT_NOCARRIER   },
    {"NO ANSWER\r\n"      , AT_NOANSER     },
    {NULL                 , AT_MAX         }
};

/* Buffer control */
static uint8_t  s_cmd_buf[CMD_BUF_MAX];
static uint8_t  s_resp_buf[RESP_BUF_MAX];
static char     s_resp_prefix[PREFIX_LEN_MAX];
static uint32_t s_rcv_cnt = 0;
static uint32_t s_read_cnt = 0;
static uint32_t s_start_pos = 0;
static uint32_t s_timeout_ms = ATCMD_RESP_TIMEOUT;

/**********************************************************************************************************************
 * Function Name: at_init_memory
 * Description  : Initialize using memory.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void at_init_memory(void)
{
    /* Initialize */
    s_rcv_cnt = 0;
    s_read_cnt = 0;
    s_start_pos = 0;
    g_uart_tbl.tx_end_flag = 0;
    R_SCI_Control(g_uart_tbl.sci_hdl, SCI_CMD_RX_Q_FLUSH, NULL);
    R_SCI_Control(g_uart_tbl.sci_hdl, SCI_CMD_TX_Q_FLUSH, NULL);
    memset (s_resp_buf, 0, sizeof(s_resp_buf));
}
/**********************************************************************************************************************
 * End of function at_init_memory
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_send
 * Description  : Send AT commands on UART.
 * Arguments    : response_fmt
 * Return Value : None
 *********************************************************************************************************************/
void at_send(const char *cmd, ...)
{
    va_list   args;

    /* Initialize */
    at_init_memory();
    memset (s_cmd_buf, 0, sizeof(s_cmd_buf));

    /* Make AT command */
    va_start(args, cmd);
    vsnprintf((char __far *)s_cmd_buf, sizeof(s_cmd_buf), cmd, args);
    va_end(args);

    /* Send AT command */
    R_SCI_Send(g_uart_tbl.sci_hdl, s_cmd_buf, strlen((const char *)s_cmd_buf));

    /* wait for transmission end */
    while (0 == g_uart_tbl.tx_end_flag)
    {
        os_wrap_sleep(1, UNIT_TICK);
    }
#if DEBUG_ATCMD != 0
    DBG_PRINTF("%s", s_cmd_buf);
#endif
}
/**********************************************************************************************************************
 * End of function at_send
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_recv
 * Description  : Receive response and return RESULT CODE.
 * Arguments    : port
 * Return Value : AT_OK
 *                AT_CONNECT
 *                AT_RING
 *                AT_NOCARRIER
 *                AT_ERROR
 *                AT_NODIALTONE
 *                AT_BUSY
 *                AT_NOANSER
 *                AT_TIMEOUT
 *                AT_INTERNAL_TIMEOUT
 *********************************************************************************************************************/
e_rslt_code_t at_recv(void)
{
    uint8_t   data;
    uint32_t  i;
    uint32_t  line_start = 0;
    uint16_t  cnt;
    e_rslt_code_t ret = AT_INTERNAL_ERROR;

    /* Receive and store AT command response */
    tick_count_start(at_resp_timeout_get(), NULL);
    while (1)
    {
        /* read data from SCI's BYTEQ */
        if (SCI_SUCCESS == R_SCI_Receive(g_uart_tbl.sci_hdl, &data, 1))
        {
            /* Buffer full? */
            if (RESP_BUF_MAX <= s_rcv_cnt)
            {
#if DEBUG_ATCMD != 0
                DBG_PRINTF("Buffer over flow!!\n");
#endif
                post_err_event(WIFI_EVENT_RCV_TASK_RXB_OVF_ERR, 0);
                break;
            }
            s_resp_buf[s_rcv_cnt++] = data;
            s_resp_buf[s_rcv_cnt]   = 0;

            /* End of line? */
            if (('\r' == s_resp_buf[s_rcv_cnt - 2]) && ('\n' == s_resp_buf[s_rcv_cnt - 1]))
            {
#if DEBUG_ATCMD != 0
                DBG_PRINTF("%s", &s_resp_buf[start_pos]);
#endif
                /* Matching response code. */
                for (i = 0;; i++ )
                {
                    /* Not match? */
                    if (AT_MAX == s_rtncode[i].rslt_cd)
                    {
                        break;
                    }
                    if (0 == strncmp((const char *)&s_resp_buf[line_start],
                                     (const char *)s_rtncode[i].p_text,
                                     strlen((const char *)s_rtncode[i].p_text)))
                    {
                        /* return sx-ulpgn result code */
                        tick_count_stop();
                        ret = s_rtncode[i].rslt_cd;
                        goto FUNC_END;
                    }
                }
                /* set pointer to next line */
                line_start = s_rcv_cnt;
            }
            else
            {
                if (4 == s_rcv_cnt)
                {
                    if (('\r' == s_resp_buf[0]) && ('\n' == s_resp_buf[1]) &&  ('>' == s_resp_buf[2]) && (' ' == s_resp_buf[3]))
                    {
                        /* CHECK FOR SUFFICIENT DATA IN QUEUE, AND FETCH IF AVAILABLE */
                        R_SCI_Control(g_uart_tbl.sci_hdl, SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, &cnt);
                        tick_count_stop();
                        ret = AT_SEND_DATA;
                        goto FUNC_END;
                    }
                }
            }
        }

        /* timeout? */
        if (TICK_EXPIERD == tick_count_check())
        {
            ret = AT_INTERNAL_TIMEOUT;
            break;
        }
    }

FUNC_END:
    s_timeout_ms = ATCMD_RESP_TIMEOUT;
    return ret;
}
/**********************************************************************************************************************
 * End of function at_recv
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_exec
 * Description  : Send AT commands and Get response on UART.
 * Arguments    : cmd
 * Return Value : Same as at_recv()
 *********************************************************************************************************************/
e_rslt_code_t at_exec(const char *cmd, ...)
{
    va_list args;

    /* Initialize */
    at_init_memory();

    memset (s_cmd_buf, 0, sizeof(s_cmd_buf));
    /* Make AT command */
    va_start(args, cmd);
    vsnprintf((char __far *)s_cmd_buf, sizeof(s_cmd_buf), cmd, args);
    va_end(args);

    /* Send AT command */
    R_SCI_Send(g_uart_tbl.sci_hdl, s_cmd_buf, strlen((const char *)s_cmd_buf));

#if DEBUG_ATCMD != 0
    DBG_PRINTF("%s", s_cmd_buf);
#endif
    /* wait for transmission end */
    while (0 == g_uart_tbl.tx_end_flag)
    {
        os_wrap_sleep(1, UNIT_TICK);
    }

    /* Receive response */
    return at_recv();
}
/**********************************************************************************************************************
 * End of function at_exec
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_read
 * Description  : Read buffer with prefix. (example at_read("abcd = %s\r\n", p_char) )
 * Arguments    : response_fmt
 * Return Value : 0     : data not found
 *                other : data found
 *********************************************************************************************************************/
uint32_t at_read(const char *response_fmt, ...)
{
    va_list   args;
    uint32_t  rtn = DATA_NOT_FOUND;
    char WIFI_FAR * sp;

    /* No received data? */
    if (0 == s_rcv_cnt)
    {
        return 0;
    }

    va_start(args, response_fmt);

    /* Make prefix */
    memset(s_resp_prefix, 0, sizeof(s_resp_prefix));
    strcpy(s_resp_prefix, response_fmt);
    sp = strstr((const char *)response_fmt, (const char WIFI_FAR *)"%");
    if (0 != sp)
    {
        s_resp_prefix[(uint32_t)sp - (uint32_t)response_fmt] = 0;
    }

    /* Read from received response */
    while (1)
    {
        if (s_read_cnt >= s_rcv_cnt)
        {
            break;
        }

        /* End of line? */
        if ('\n' == s_resp_buf[s_read_cnt++])
        {
            /* Check for match */
            if (0 == strncmp((const char WIFI_FAR *)&s_resp_buf[s_start_pos], s_resp_prefix, strlen(s_resp_prefix)))
            {
                if (EOF == vsscanf((const char WIFI_FAR *)&s_resp_buf[s_start_pos], response_fmt, args))
                {
                    rtn = DATA_NOT_FOUND;
                }
                else
                {
                    rtn = DATA_FOUND;
                }
                s_start_pos = s_read_cnt;
                break;
            }

            /* set pointer to next line */
            s_start_pos = s_read_cnt;
        }
    }

    va_end(args);
    return rtn;
}
/**********************************************************************************************************************
 * End of function at_read
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_read_wo_prefix
 * Description  : Read buffer without prefix. (example at_read_wo_prefix("%s\r\n", p_char) )
 * Arguments    : response_fmt
 * Return Value : same as vsscanf() function.
 *********************************************************************************************************************/
int32_t at_read_wo_prefix(const char *response_fmt, ...)
{
    int32_t ret;
    va_list args;

    va_start(args, response_fmt);
    ret = vsscanf((const char WIFI_FAR *)&s_resp_buf[s_start_pos], response_fmt, args);
    va_end(args);
    return ret;
}
/**********************************************************************************************************************
 * End of function at_read_wo_prefix
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_move_to_next_line
 * Description  : Move pointer to next line of response buffer.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void at_move_to_next_line(void)
{
    while (1)
    {
        if (s_read_cnt >= s_rcv_cnt)
        {
            break;
        }

        /* Set pointer to next line */
        if ('\n' == s_resp_buf[s_read_cnt++])
        {
            s_start_pos = s_read_cnt;
            break;
        }
    }
}
/**********************************************************************************************************************
 * End of function at_move_to_next_line
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_move_to_first_line
 * Description  : Set pointer to first line of response buffer.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void at_move_to_first_line(void)
{
    s_start_pos = 0;
}
/**********************************************************************************************************************
 * End of function at_move_to_first_line
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_get_current_line
 * Description  : Get pointer on current line of response buffer.
 * Arguments    : None
 * Return Value : Pointer of current line
 *********************************************************************************************************************/
uint8_t * at_get_current_line(void)
{
    return &s_resp_buf[s_start_pos];
}
/**********************************************************************************************************************
 * End of function at_get_current_line
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: post_err_event
 * Description  : Post error event to user callback function.
 * Arguments    : err_event - error event
 *                sock_idx  - socket index
 * Return Value : None
 *********************************************************************************************************************/
void post_err_event(wifi_err_event_enum_t err_event, uint32_t sock_idx)
{
    wifi_err_event_t evt;

    evt.event = err_event;
    evt.socket_number = sock_idx;
    if (NULL != p_wifi_callback)
    {
        p_wifi_callback(&evt);
    }
}
/**********************************************************************************************************************
 * End of function post_err_event
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_resp_timeout_set
 * Description  : set response timeout .
 * Arguments    : ms - timeout (msec)  0:default(ATCMD_RESP_TIMEOUT)  1-
 * Return Value : None
 *********************************************************************************************************************/
void at_resp_timeout_set(uint32_t ms)
{
    s_timeout_ms = ms;
}
/**********************************************************************************************************************
 * End of function at_resp_timeout_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: at_resp_timeout_get
 * Description  : set response timeout .
 * Arguments    : none
 * Return Value : response timeout
 *********************************************************************************************************************/
uint32_t at_resp_timeout_get(void)
{
    return s_timeout_ms;
}
/**********************************************************************************************************************
 * End of function at_resp_timeout_set
 *********************************************************************************************************************/
