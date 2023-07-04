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
 * File Name    : r_ryz_private.h
 * Version      : 1.0
 * Description  : Private functions definition for module.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : dd.mm.2023 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "r_ryz_os_wrap.h"
#include "r_byteq_if.h"
#include "r_ryz_if.h"
#include "r_sci_rl_if.h"
#include "rl78_bsp_wrapper.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef R_RYZ_PRIVATE_H
#define R_RYZ_PRIVATE_H

#if !defined(RYZ_CFG_SCI_CHANNEL)
#error "Error! Need to define RYZ_CFG_SCI_CHANNEL in r_wifi_sx_ulpgn_config.h"
#endif

#if SCI_CFG_TEI_INCLUDED != 1
#error "Error! Need to set SCI_CFG_TEI_INCLUDED is '1' in r_sci_rx_config.h"
#endif

/* Configuration */
#define SOCK_TBL_MAX                   (RYZ_CFG_MAX_SOCKETS)            // Socket table (default:4)
#define CERT_PROFILE_MAX               (5)       // Number of Certificate profiles
#define CERT_HOSTNAME_MAX              (256)     // Number of Certificate host name

#define ATCMD_RESP_TIMEOUT             (20000)   // Timeout threshold for AT command response (msec)
#define RYZ_AT_CMD_BUF_MAX             (256)
#define RYZ_AT_RESP_BUF_MAX            (1500)
#define RYZ_BAUD_DEFAULT               (921600)

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
/* Reset port pin macros.  */
#define WIFI_RESET_DDR(x, y)           (WIFI_RESET_DDR_PREPROC(x, y))
#define WIFI_RESET_DDR_PREPROC(x, y)   ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RESET_DR(x, y)            (WIFI_RESET_DR_PREPROC(x, y))
#define WIFI_RESET_DR_PREPROC(x, y)    ((PORT ## x .PODR.BIT.B ## y))

/* RTS port pin macros.  */
#define WIFI_RTS_DDR(x, y)             (WIFI_RTS_DDR_PREPROC(x, y))
#define WIFI_RTS_DDR_PREPROC(x, y)     ((PORT ## x .PDR.BIT.B ## y))
#define WIFI_RTS_DR(x, y)              (WIFI_RTS_DR_PREPROC(x, y))
#define WIFI_RTS_DR_PREPROC(x, y)      ((PORT ## x .PODR.BIT.B ## y))
#else
#define WIFI_FAR                       __far

/* Reset port pin macros.  */
#define WIFI_RESET_DDR(x, y)           (WIFI_RESET_DDR_PREPROC(x, y))
#define WIFI_RESET_DDR_PREPROC(x, y)   ((PM ## x ## _bit.no ## y))
#define WIFI_RESET_DR(x, y)            (WIFI_RESET_DR_PREPROC(x, y))
#define WIFI_RESET_DR_PREPROC(x, y)    ((P ## x ## _bit.no ## y))

/* RTS port pin macros.  */
#define WIFI_RTS_DDR(x, y)             (WIFI_RTS_DDR_PREPROC(x, y))
#define WIFI_RTS_DDR_PREPROC(x, y)     ((PM ## x ## _bit.no ## y))
#define WIFI_RTS_DR(x, y)              (WIFI_RTS_DR_PREPROC(x, y))
#define WIFI_RTS_DR_PREPROC(x, y)      ((P ## x ## _bit.no ## y))
#endif

/* IP address(xxx.xxx.xxx.xxx) into ULONG */
#define IPADR_UB_TO_UL(adr1, adr2, adr3, adr4) \
    ((((adr1) & 0x000000FF) << 24) | (((adr2) & 0x000000FF) << 16) |\
    (((adr3) & 0x000000FF) << 8)  | (((adr4) & 0x000000FF)))

/* Debug mode */
#define DEBUGLOG                        (0)
#define DEBUG_ATCMD                     (0)

/* Debug print mode */
#define FREERTOS_IDT                    (1)
#if FREERTOS_IDT != 0
#define DBG_PRINTF                      vLoggingPrintf
#else
#define DBG_PRINTF                      printf
#endif

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/* UART table information */
typedef struct
{
    sci_hdl_t   sci_hdl;
    sci_cfg_t   sci_config;
    volatile uint8_t tx_end_flag;
} st_uart_tbl_t;

/* Socket Timer */
typedef struct
{
    OS_TICK   threshold;        /* Timeout threshold */
    OS_TICK   tick_sta;         /* Tick of Timer start  */
} st_sock_timer_t;

/* Socket table information */
typedef struct
{
    uint8_t     status;
    uint8_t     ipver;
    uint8_t     prtcl;
    uint32_t    put_err_cnt;
    st_sock_timer_t timer_tx;
    st_sock_timer_t timer_rx;
    struct
    {
        uint8_t    enable;
        uint8_t    spid;
    } ssl;
} st_sock_tbl_t;

/* Certificate profile */
typedef struct
{
    uint8_t     ca_cert_id;        /* CA certificate id */
    uint8_t     cli_cert_id;       /* Client certificate id */
    uint8_t     cli_privatekey_id; /* Client private key id */
} st_cert_profile_t;

/* WIFI FIT module status */
typedef enum
{
    MODULE_DISCONNECTED = 0,    /* Disconnected WIFI module */
    MODULE_CONNECTED,           /* Connected WIFI module    */
    MODULE_ACCESSPOINT,         /* Connected access point   */
} e_module_status_t;

/* return code for common functions */
typedef enum
{
    E_OK      = 0,              /* return = OK      */
    E_FAIL    = -1              /* return = failure */
} e_func_result_t;

/* UART HW flow control */
typedef enum
{
    RTS_OFF   = 0,              /* RTS = ON         */
    RTS_ON                      /* RTS = OFF        */
} e_flow_ctrl_t;

/* Result code */
typedef enum
{
    AT_OK               = 0,    /* OK               */
    AT_CONNECT          = 1,    /* CONNECT          */
    AT_RING             = 2,    /* RING             */
    AT_NOCARRIER        = 3,    /* NO CARRIER       */
    AT_ERROR            = 4,    /* ERROR            */
    AT_NODIALTONE       = 6,    /* NO DIALTONE      */
    AT_BUSY             = 7,    /* BUSY             */
    AT_NOANSER          = 8,    /* NO ANSWER        */
    AT_SEND_DATA        = 9,    /* Send binary data */
    AT_INTERNAL_TIMEOUT = 253,  /* INTERNAL TIMEOUT */
    AT_INTERNAL_ERROR   = 254,  /* INTERNAL ERROR   */
    AT_MAX              = 255   /* STOPPER          */
} e_rslt_code_t;

typedef enum
{
    DATA_NOT_FOUND = 0,
    DATA_FOUND,
} e_atcmd_read_t;

/* Cast uint32_t -> uint8_t */
typedef union
{
    uint32_t    ul;
    struct
    {
        uint8_t hh;
        uint8_t hl;
        uint8_t lh;
        uint8_t ll;
    } b;
} u_cast_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern st_uart_tbl_t g_uart_tbl;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/* r_ryz_atcmd.c */
/**********************************************************************************************************************
 * Function Name: at_init_memory
 * Description  : Initialize using memory.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void at_init_memory (void);

/**********************************************************************************************************************
 * Function Name: at_send
 * Description  : Send AT commands on UART.
 * Arguments    : cmd
 * Return Value : None
 *********************************************************************************************************************/
void at_send (const char *cmd, ...);

/**********************************************************************************************************************
 * Function Name: at_recv
 * Description  : Receive response and return RESULT CODE.
 * Arguments    : none
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
 *                AT_INTERNAL_ERROR
 *********************************************************************************************************************/
e_rslt_code_t at_recv (void);

/**********************************************************************************************************************
 * Function Name: at_exec
 * Description  : Send AT commands and Receive response on UART.
 * Arguments    : cmd
 * Return Value : Same as at_recv() function.
 *********************************************************************************************************************/
e_rslt_code_t at_exec (const char *cmd, ...);

/**********************************************************************************************************************
 * Function Name: at_read
 * Description  : Read buffer with prefix. (example at_read("abcd = %s\r\n", p_char) )
 * Arguments    : response_fmt
 * Return Value : 0     : data not found
 *                other : data found
 *********************************************************************************************************************/
uint32_t at_read (const char *response_fmt, ...);

/**********************************************************************************************************************
 * Function Name: at_read_wo_prefix
 * Description  : Read buffer without prefix. (example at_read_wo_prefix("%s\r\n", p_char) )
 * Arguments    : response_fmt
 * Return Value : same as vsscanf() function.
 *********************************************************************************************************************/
int32_t at_read_wo_prefix (const char *response_fmt, ...);

/**********************************************************************************************************************
 * Function Name: at_move_to_next_line
 * Description  : Move pointer to next line of response buffer.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void at_move_to_next_line (void);

/**********************************************************************************************************************
 * Function Name: at_move_to_first_line
 * Description  : Set pointer to first line of response buffer.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void at_move_to_first_line (void);

/**********************************************************************************************************************
 * Function Name: at_get_current_line
 * Description  : Get pointer on current line of response buffer.
 * Arguments    : None
 * Return Value : Pointer of current line
 *********************************************************************************************************************/
uint8_t * at_get_current_line (void);

/**********************************************************************************************************************
 * Function Name: post_err_event
 * Description  : Post error event to user callback function.
 * Arguments    : err_event - error event
 *                sock_idx  - socket index
 * Return Value : None
 *********************************************************************************************************************/
void post_err_event (wifi_err_event_enum_t err_event, uint32_t sock_idx);

/**********************************************************************************************************************
 * Function Name: at_resp_timeout_set
 * Description  : set response timeout .
 * Arguments    : ms - timeout (msec)
 * Return Value : None
 *********************************************************************************************************************/
void at_resp_timeout_set (uint32_t ms);

/**********************************************************************************************************************
 * Function Name: at_resp_timeout_get
 * Description  : set response timeout .
 * Arguments    : none
 * Return Value : response timeout
 *********************************************************************************************************************/
uint32_t at_resp_timeout_get (void);
#endif /* R_RYZ_PRIVATE_H */
