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
 * File Name    : r_ryz_api.c
 * Version      : 1.0
 * Description  : API functions definition for RYZ014A.
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
#define MUTEX_TX             (0x01)     // WIFI API(TX)
#define MUTEX_RX             (0x02)     // WIFI API(RX)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
/* SCI configuration */
typedef struct
{
    sci_ch_t ch;           /* SCI channel */
    void (*func)(void);    /* Function name of SCI Port configuration */
    uint16_t tx_size;      /* TX buffer size */
    uint16_t rx_size;      /* RX buffer size */
} st_sci_conf_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
uint8_t       g_uart_port;
st_uart_tbl_t g_uart_tbl;
st_sock_tbl_t g_sock_tbl[SOCK_TBL_MAX];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/* Initialize */
static void initialize_memory (void);

/* System state control */
static void cellular_state_set (e_module_status_t state);
static e_module_status_t cellular_state_get (void);

/* Sub functions */
static cellular_err_t disconnect_ap_sub (void);
static cellular_err_t shutdown_socket_sub (uint8_t sock_idx);
static uint32_t get_ipaddr_by_name (RYZ_C_CH_FAR name);

/* Mutex control */
static cellular_err_t mutex_create (void);
static int32_t mutex_take (uint8_t mutex_flag);
static void mutex_give (uint8_t mutex_flag);

/* WIFI module control */
static void module_hw_reset (void);
static void module_close (void);

/* FIT module Wrapper */
static sci_err_t wrap_sci_send (uint8_t __far *p_src, uint16_t const length);
static sci_err_t wrap_sci_ctrl (sci_cmd_t const cmd, void * p_args);
static sci_err_t wrap_sci_recv (uint8_t * p_dst, uint16_t const length);

/* Port configurations */
static void flow_ctrl_init (void);
static void flow_ctrl_set (uint32_t flow);
static st_sci_conf_t WIFI_FAR * get_port_config (uint8_t port);
static int32_t uart_open (uint8_t port, void (* const p_cb)(void *p_args));
static void uart_port_close (void);

/* SCI callback functions for HSUART */
static void uart_callback (void * pArgs);
//static void cb_sci_err (sci_cb_evt_t event);
static void cb_sci_err(sci_cb_args_t * p_args);

/* SCI configurations */
static const st_sci_conf_t s_sci_cfg[] =
{
#if SCI_CFG_CH0_INCLUDED == 1
    {SCI_CH0 , R_SCI_PinSet_SCI0  ,SCI_CFG_CH0_TX_BUFSIZ  ,SCI_CFG_CH0_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH1_INCLUDED == 1
    {SCI_CH1 , R_SCI_PinSet_SCI1  ,SCI_CFG_CH1_TX_BUFSIZ  ,SCI_CFG_CH1_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH2_INCLUDED == 1
    {SCI_CH2 , NULL  ,SCI_CFG_CH2_TX_BUFSIZ  ,SCI_CFG_CH2_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH3_INCLUDED == 1
    {SCI_CH3 , NULL  ,SCI_CFG_CH3_TX_BUFSIZ  ,SCI_CFG_CH3_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH4_INCLUDED == 1
    {SCI_CH4 , R_SCI_PinSet_SCI4  ,SCI_CFG_CH4_TX_BUFSIZ  ,SCI_CFG_CH4_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH5_INCLUDED == 1
    {SCI_CH5 , R_SCI_PinSet_SCI5  ,SCI_CFG_CH5_TX_BUFSIZ  ,SCI_CFG_CH5_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH6_INCLUDED == 1
    {SCI_CH6 , R_SCI_PinSet_SCI6  ,SCI_CFG_CH6_TX_BUFSIZ  ,SCI_CFG_CH6_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH7_INCLUDED == 1
    {SCI_CH7 , R_SCI_PinSet_SCI7  ,SCI_CFG_CH7_TX_BUFSIZ  ,SCI_CFG_CH7_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH8_INCLUDED == 1
    {SCI_CH8 , R_SCI_PinSet_SCI8  ,SCI_CFG_CH8_TX_BUFSIZ  ,SCI_CFG_CH8_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH9_INCLUDED == 1
    {SCI_CH9 , R_SCI_PinSet_SCI9  ,SCI_CFG_CH9_TX_BUFSIZ  ,SCI_CFG_CH9_RX_BUFSIZ  },
#endif
#if SCI_CFG_CH10_INCLUDED == 1
    {SCI_CH10, R_SCI_PinSet_SCI10 ,SCI_CFG_CH10_TX_BUFSIZ ,SCI_CFG_CH10_RX_BUFSIZ },
#endif
#if SCI_CFG_CH11_INCLUDED == 1
    {SCI_CH11, R_SCI_PinSet_SCI11 ,SCI_CFG_CH11_TX_BUFSIZ ,SCI_CFG_CH11_RX_BUFSIZ },
#endif
#if SCI_CFG_CH12_INCLUDED == 1
    {SCI_CH12, R_SCI_PinSet_SCI12 ,SCI_CFG_CH12_TX_BUFSIZ ,SCI_CFG_CH12_RX_BUFSIZ },
#endif
    {SCI_NUM_CH, NULL ,0 ,0 }
};

/* Max sockets */
static uint8_t  s_sockets_max;

/* WIFI system state */
static e_module_status_t s_cellular_state = MODULE_DISCONNECTED;

/* certificate profiles */
static st_cert_profile_t s_cert_profile[CERT_PROFILE_MAX];
static wifi_certificate_infomation_t s_cert_info;

/* OS parameters */
static OS_MUTEX s_binary_sem_tx;
static OS_MUTEX s_binary_sem_rx;
static const OS_TICK s_sem_block_time = OS_WRAP_MS_TO_TICKS(10000UL);

/**********************************************************************************************************************
 * Function Name: initialize_memory
 * Description  : Initialize memory and tables.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void initialize_memory(void)
{
    /* Tables */
    memset(&g_uart_tbl, 0, sizeof(g_uart_tbl));
    memset(g_sock_tbl, 0, sizeof(g_sock_tbl));

    g_uart_port = RYZ_CFG_SCI_CHANNEL;

    /* Max sockets */
    s_sockets_max = RYZ_CFG_MAX_SOCKETS;
}
/**********************************************************************************************************************
 * End of function initialize_memory
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_Open
 * Description  : Open Cellular Module.
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_SERIAL_OPEN
 *                WIFI_ERR_BYTEQ_OPEN
 *                WIFI_ERR_ALREADY_OPEN
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_Open(void)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Disconnected WiFi module? */
    if (MODULE_DISCONNECTED != cellular_state_get())
    {
        return WIFI_ERR_ALREADY_OPEN;
    }

    /* Memory initialize */
    initialize_memory();

    /* Mutex initialize */
    if (WIFI_SUCCESS != mutex_create())
    {
        api_ret = WIFI_ERR_TAKE_MUTEX;
        goto END_INITIALIZE;
    }

    /* Reset WIFI module */
    module_hw_reset();

    /* Open command port */
    if (E_OK != uart_open(g_uart_port, uart_callback))
    {
        api_ret = WIFI_ERR_SERIAL_OPEN;
        goto END_INITIALIZE;
    }

    /* Set flow control for data port */
    flow_ctrl_init();

    /* Disable echo back */
    at_exec("ATE0\r");

    /* Disconnect from currently connected Access Point */
    at_exec("AT+SQNAUTOCONNECT=0\r");

    /* UART transmission flow control busy retry timeout = 0-255 (x10msec) */
    at_exec("AT+SQNSIMST=0\r");

    /* Escape(+++) guard time = 10 (x20msec) */
    at_exec("AT+CEREG=0\r");

    /* Escape(+++) guard time = 10 (x20msec) */
    at_exec("AT+CFUN=4\r");

    at_exec("AT+CFUN?\r");

    os_wrap_sleep(500, UNIT_MSEC);

    /* Enable PIN? */
    if (AT_OK != at_exec("AT+CPIN?\r"))
    {
        os_wrap_sleep(1000, UNIT_MSEC);
        at_exec("AT+CPIN?\r");
    }

    /* socket settings */
    at_exec("AT+SQNSCFG=1,1,0,60,200,10\r");
    at_exec("AT+SQNSCFG=2,1,0,60,200,10\r");
    at_exec("AT+SQNSCFG=3,1,0,60,200,10\r");
    at_exec("AT+SQNSCFG=4,1,0,60,200,10\r");
    at_exec("AT+SQNSCFG=5,1,0,60,200,10\r");
    at_exec("AT+SQNSCFG=6,1,0,60,200,10\r");

    os_wrap_sleep(100, UNIT_MSEC);

END_INITIALIZE:
    if (WIFI_SUCCESS == api_ret)
    {
        /* Set WIFI State to "Connected WiFi module" */
        cellular_state_set(MODULE_CONNECTED);
    }
    else
    {
        module_close();
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_Open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_Close
 * Description  : Close Cellular Module.
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_TAKE_MUTEX
 *********************************************************************************************************************/
cellular_err_t R_RYZ_Close(void)
{
    /* Disconnected WiFi module? */
    if (MODULE_DISCONNECTED == cellular_state_get())
    {
        return WIFI_SUCCESS;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    disconnect_ap_sub();

    /* Give mutex */
    mutex_give(MUTEX_TX | MUTEX_RX);

    /* Close module */
    module_close();

    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_RYZ_Close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectAP
 * Description  : Connect to Access Point.
 * Arguments    : ap_name
 *                user_id
 *                passwd
 *                auth_type (1:PAP, 2:CHAP)
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectAP(RYZ_C_CH_FAR ap_name, RYZ_C_CH_FAR user_id, RYZ_C_CH_FAR passwd, uint8_t auth_type)
{
    uint8_t retry_count;
    static uint8_t retry_max = 30;
    static uint32_t cgatt_val = 0;
    cellular_err_t api_ret = WIFI_ERR_MODULE_COM;

    /* Disconnected WiFi module? */
    if (MODULE_DISCONNECTED == cellular_state_get())
    {
        return WIFI_ERR_NOT_OPEN;
    }

    /* Already connected access point? */
    if (0 == R_RYZ_IsConnected())
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Check parameters */
    if ((NULL == ap_name) || (NULL == user_id) || (NULL == passwd))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    /* APN Setting */
    if (AT_OK != at_exec("AT+CGDCONT=1,\"IPV4V6\",\"%s\",,,,0,0,0,0,0,0,1,,0\r", ap_name))
    {
        api_ret = WIFI_ERR_MODULE_COM;
        goto RELEASE_MUTEX;
    }

    /* APN authentication setting */
    if (AT_OK != at_exec("AT+CGAUTH=1,%d,\"%s\",\"%s\"\r", auth_type, user_id, passwd))
    {
        api_ret = WIFI_ERR_MODULE_COM;
        goto RELEASE_MUTEX;
    }

    /* Function mode 1 */
    if (AT_OK != at_exec("AT+CFUN=1\r"))
    {
        api_ret = WIFI_ERR_MODULE_COM;
        goto RELEASE_MUTEX;
    }

    /* Connect to APN */
    for (retry_count = 0; retry_count < retry_max; retry_count++ )
    {
        if (AT_OK == at_exec("AT+CGATT=1\r"))
        {
            api_ret = WIFI_SUCCESS;
            break;
        }
        os_wrap_sleep(2000, UNIT_MSEC);
    }
    if (WIFI_SUCCESS != api_ret)
    {
        goto RELEASE_MUTEX;
    }

    os_wrap_sleep(1000, UNIT_MSEC);

    /* Connect access point */
    api_ret = WIFI_ERR_MODULE_COM;
    for (retry_count = 0; retry_count < retry_max; retry_count++ )
    {
        if (AT_OK == at_exec("AT+CGATT?\r"))
        {
            at_read("+CGATT: %d\r\n", &cgatt_val);
            if (1 == cgatt_val)
            {
                api_ret = WIFI_SUCCESS;
                break;
            }
        }
        os_wrap_sleep(5000, UNIT_MSEC);
    }
    if (WIFI_SUCCESS != api_ret)
    {
        goto RELEASE_MUTEX;
    }

    /* Set Cellular state to "Connected access point" */
    cellular_state_set(MODULE_ACCESSPOINT);

RELEASE_MUTEX:
    mutex_give(MUTEX_TX | MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ConnectAP
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_DisconnectAP
 * Description  : Disconnect from Access Point.
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_DisconnectAP(void)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Disconnected module? */
    if (MODULE_DISCONNECTED == cellular_state_get())
    {
        return WIFI_ERR_NOT_OPEN;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    if (WIFI_SUCCESS != disconnect_ap_sub())
    {
        api_ret = WIFI_ERR_MODULE_COM;
        goto RELEASE_MUTEX;
    }

    /* Set state to "Connected module" */
    cellular_state_set(MODULE_CONNECTED);

RELEASE_MUTEX:
    /* Give mutex */
    mutex_give(MUTEX_TX | MUTEX_RX);

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_DisconnectAP
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_IsConnected
 * Description  : Check connected access point.
 * Arguments    : none
 * Return Value : 0  - connected
 *                -1 - disconnected
 *********************************************************************************************************************/
int32_t R_RYZ_IsConnected(void)
{
    /* Connected access point? */
    if (MODULE_ACCESSPOINT == cellular_state_get())
    {
        return 0;
    }
    else
    {
        return (-1);
    }
}
/**********************************************************************************************************************
 * End of function R_RYZ_IsConnected
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_CreateSocket
 * Description  : Create TCP/UDP socket
 * Arguments    : type
 *                ip_version
 * Return Value : Positive number - created socket index
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_CREATE
 *********************************************************************************************************************/
int32_t R_RYZ_CreateSocket(e_ryz_prtcl_type_t prtcl_type, uint32_t ip_version)
{
    uint16_t i;
    int32_t  ret = WIFI_ERR_SOCKET_CREATE;

    /* Connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if (RYZ_IP_VER4 != ip_version)
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex */
    if (0 != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    for (i = 0; i < s_sockets_max; i++ )
    {
        if (SOCKET_STATUS_CLOSED == g_sock_tbl[i].status)
        {
            g_sock_tbl[i].status = SOCKET_STATUS_SOCKET;     /* socket status   */
            g_sock_tbl[i].ipver = (uint8_t)ip_version;       /* ip_version      */
            g_sock_tbl[i].prtcl = (uint8_t)prtcl_type;       /* protocol type   */
            g_sock_tbl[i].ssl.enable  = 0;                   /* SSL enable flag */
            g_sock_tbl[i].ssl.spid = 0;                      /* Security Profile identifier */
            ret = (int32_t) i;
            break;
        }
    }

    /* Give mutex. */
    mutex_give(MUTEX_TX | MUTEX_RX);

    return ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_CreateSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectSocket
 * Description  : Open client mode TCP/UDP socket.
 * Arguments    : sock_id
 *                ip_address
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectSocket(uint8_t sock_id, e_ryz_prtcl_type_t tx_prtcl,
        uint16_t remote_port, uint32_t ipaddr, uint16_t local_port, uint8_t conn_mode)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Connect access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if ((sock_id >= s_sockets_max) || (0 == ipaddr))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* socket created? */
    if (SOCKET_STATUS_SOCKET != g_sock_tbl[sock_id].status)
    {
        return WIFI_ERR_SOCKET_NUM;
    }


    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }


    api_ret = WIFI_ERR_SOCKET_CONNECT;
    for (int i = 0; i < 10; i++)
    {
        at_resp_timeout_set(40000);
        if (AT_OK == at_exec("AT+SQNSD=%d,%d,%d,\"%d.%d.%d.%d\",0,%d,%d\r",
                sock_id + 1,
                tx_prtcl,
                remote_port,
                (uint16_t)((ipaddr >> 24) & 0x00FF), (uint16_t)((ipaddr >> 16) & 0x00FF), (uint16_t)((ipaddr >>  8) & 0x00FF), (uint16_t)(ipaddr & 0x00FF),
                local_port,
                conn_mode))
        {
            break;
        }
    }

    /* Socket Configuration Extended */
    if (WIFI_SUCCESS != api_ret)
    {
        api_ret = WIFI_ERR_SOCKET_CONNECT;
        goto RELEASE_MUTEX;
    }

    g_sock_tbl[sock_id].status = SOCKET_STATUS_CONNECTED;

RELEASE_MUTEX:
    mutex_give(MUTEX_TX | MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ConnectSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectSocketByHostName
 * Description  : Open client mode TCP/UDP socket.
 * Arguments    : sock_id
 *                host_name
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectSocketByHostName(uint8_t sock_id, e_ryz_prtcl_type_t tx_prtcl,
        uint16_t remote_port, RYZ_C_CH_FAR host_name, uint16_t local_port, uint8_t conn_mode)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Connect access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if ((sock_id >= s_sockets_max) || (NULL == host_name))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* socket created? */
    if (SOCKET_STATUS_SOCKET != g_sock_tbl[sock_id].status)
    {
        return WIFI_ERR_SOCKET_NUM;
    }


    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }


    api_ret = WIFI_ERR_SOCKET_CONNECT;
    for (int i = 0; i < 10; i++)
    {
        at_resp_timeout_set(40000);
        if (AT_OK == at_exec("AT+SQNSD=%d,%d,%d,\"%s\",0,%d,%d\r",
                sock_id + 1,
                tx_prtcl,
                remote_port,
                host_name,
                local_port,
                conn_mode))
        {
            api_ret = WIFI_SUCCESS;
            break;
        }
    }
    /* Socket Configuration Extended */
    if (WIFI_SUCCESS != api_ret)
    {
        api_ret = WIFI_ERR_SOCKET_CONNECT;
        goto RELEASE_MUTEX;
    }

    g_sock_tbl[sock_id].status = SOCKET_STATUS_CONNECTED;

RELEASE_MUTEX:
    mutex_give(MUTEX_TX | MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ConnectSocketByHostName
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_SendSocket
 * Description  : Send data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of sent data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_RYZ_SendSocket(uint8_t socket_number, uint8_t * p_data, uint32_t length, uint32_t timeout_ms)
{
    int32_t     send_idx;
    int32_t     send_length;
    int32_t     api_ret = WIFI_ERR_SOCKET_TIMEOUT;
    OS_TICK     tick_tmp;
    uint32_t    txsize_max = get_port_config(g_uart_port)->tx_size;

    /* Connect access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if ((socket_number >= s_sockets_max) || (NULL == p_data))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX|MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    if (AT_OK != at_exec("AT+SQNSI=%d\r", socket_number + 1))
    {
        DBG_PRINTF("R_RYZ_SendSocket() : AT+SQNSI failed");
        api_ret = WIFI_ERR_SOCKET_CONNECT;
        goto RELEASE_MUTEX;
    }

    /*
     *  Send
     */
    /* received "> " ? */
    if (AT_SEND_DATA != at_exec("AT+SQNSSENDEXT=%d,%ld\r", socket_number + 1, length))
    {
        if (AT_SEND_DATA != at_exec("AT+SQNSSENDEXT=%d,%ld\r", socket_number + 1, length))
        {
            api_ret = WIFI_ERR_SOCKET_TIMEOUT;
            goto RELEASE_MUTEX;
        }
    }

    send_idx = 0;
    at_init_memory();
    g_sock_tbl[socket_number].timer_tx.threshold = OS_WRAP_MS_TO_TICKS(timeout_ms);
    if (0 < timeout_ms)
    {
        g_sock_tbl[socket_number].timer_tx.tick_sta = os_wrap_tickcount_get();
    }
    while (send_idx < length)
    {
        if ((length - send_idx) > txsize_max)
        {
            send_length = txsize_max;
        }
        else
        {
            send_length = length - send_idx;
        }

        /* data send */
        if (SCI_SUCCESS != wrap_sci_send((uint8_t *)p_data + send_idx, send_length))
        {
            break;
        }

        /* Wait for transmit end */
        while (1)
        {
            if (0 != g_uart_tbl.tx_end_flag)
            {
                break;
            }
            os_wrap_sleep(1, UNIT_TICK);
        }
        send_idx += send_length;

        /* timeout? */
        if (0 < timeout_ms)
        {
            tick_tmp = os_wrap_tickcount_get() - g_sock_tbl[socket_number].timer_tx.tick_sta;
            if (g_sock_tbl[socket_number].timer_tx.threshold <= tick_tmp)
            {
                break;
            }
        }
        os_wrap_sleep(1, UNIT_TICK);
    }
    api_ret = send_idx;

    /* Receive response */
    if (AT_OK != at_recv())
    {
        api_ret = WIFI_ERR_SOCKET_TIMEOUT;
    }

RELEASE_MUTEX:
    mutex_give(MUTEX_TX|MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_SendSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ReceiveSocket
 * Description  : Receive data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of received data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_RYZ_ReceiveSocket(uint8_t socket_number, uint8_t * p_data, uint32_t length, uint32_t timeout_ms)
{
    int32_t         api_ret;
    OS_TICK         tick_tmp;
    static uint32_t sockid = 0;
    static uint32_t send_len = 0;
    static uint32_t received_len = 0;
    static uint32_t ring_len = 0;
    static uint32_t ack_wait_len = 0;
    static uint32_t read_len = 0;

    /* Connect access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if ((socket_number >= s_sockets_max) || (NULL == p_data) || (0 == length))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX|MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    /*
     * Receive
     */
    g_sock_tbl[socket_number].timer_rx.threshold = OS_WRAP_MS_TO_TICKS(timeout_ms);
    if (0 < timeout_ms)
    {
        g_sock_tbl[socket_number].timer_rx.tick_sta = os_wrap_tickcount_get();
    }

    while (1)
    {
        if (AT_OK == at_exec("AT+SQNSI=%d\r", socket_number + 1))
        {
            if (DATA_FOUND == at_read("+SQNSI: %d,%lu,%lu,%lu,%lu\r\n",
                                      &sockid, &send_len, &received_len, &ring_len, &ack_wait_len))
            {
                if (0 < ring_len)
                {
                    if (length <= ring_len)
                    {
                        read_len = length;
                        break;
                    }
                }
            }
        }

        /* timeout? */
        if (0 < timeout_ms)
        {
            tick_tmp = os_wrap_tickcount_get() - g_sock_tbl[socket_number].timer_rx.tick_sta;
            if (g_sock_tbl[socket_number].timer_rx.threshold <= tick_tmp)
            {
                /* timeout */
                read_len = ring_len;
                break;
            }
        }
        os_wrap_sleep(50, UNIT_TICK);
    }

    api_ret = 0;
    if (AT_OK == at_exec("AT+SQNSRECV=%d,%d\r", socket_number + 1, read_len))
    {
        if (DATA_FOUND == at_read("+SQNSRECV: %lu,%lu\r\n", &sockid, &read_len))
        {
            memcpy(p_data, at_get_current_line(), read_len);
            api_ret = read_len;
        }
    }

    mutex_give(MUTEX_TX|MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ReceiveSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ShutdownSocket
 * Description  : Shutdown connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ShutdownSocket(uint8_t socket_number)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if (socket_number >= s_sockets_max)
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    api_ret = shutdown_socket_sub(socket_number);

    mutex_give(MUTEX_TX | MUTEX_RX);
    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ShutdownSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_CloseSocket
 * Description  : Disconnect connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_CloseSocket(uint8_t socket_number)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if (socket_number >= s_sockets_max)
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    /* Already closed? */
    if (SOCKET_STATUS_CLOSED == g_sock_tbl[socket_number].status)
    {
        return WIFI_SUCCESS;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    if (SOCKET_STATUS_SOCKET != g_sock_tbl[socket_number].status)
    {
        api_ret = shutdown_socket_sub(socket_number);
    }

    g_sock_tbl[socket_number].put_err_cnt = 0;
    g_sock_tbl[socket_number].ssl.enable  = 0;
    g_sock_tbl[socket_number].ssl.spid    = 0;
    g_sock_tbl[socket_number].status = SOCKET_STATUS_CLOSED;

    os_wrap_sleep(100, UNIT_MSEC);

    /* Give mutex */
    mutex_give(MUTEX_TX | MUTEX_RX);

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_CloseSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_DnsQuery
 * Description  : Execute DNS query.
 * Arguments    : domain_name
 *                ip_address
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_DnsQuery(RYZ_C_CH_FAR domain_name, uint32_t * ip_address)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    /* Connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if ((NULL == domain_name) || (NULL == ip_address))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    /* Get IP address by domain name */
    *ip_address = get_ipaddr_by_name(domain_name);
    if (0 == (*ip_address))
    {
        api_ret = WIFI_ERR_MODULE_COM;
    }

    /* Give mutex */
    mutex_give(MUTEX_TX | MUTEX_RX);

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_DnsQuery
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_GetVersion
 * Description  : Get FIT module version.
 * Arguments    : none
 * Return Value : FIT module version
 *********************************************************************************************************************/
uint32_t R_RYZ_GetVersion(void)
{
    /* These version macros are defined in r_wifi_sx_ulpgn_if.h. */
    return ((((uint32_t) WIFI_SX_ULPGN_CFG_VERSION_MAJOR) << 16) | (uint32_t) WIFI_SX_ULPGN_CFG_VERSION_MINOR);
}
/**********************************************************************************************************************
 * End of function R_RYZ_GetVersion
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_GetSocketStatus
 * Description  : Get TCP socket status.
 * Arguments    : socket_number (0-)
 * Return Value : -1    - not exist
 *                other - socket status
 *********************************************************************************************************************/
int32_t R_RYZ_GetSocketStatus(uint8_t socket_number)
{
    int32_t  ret = (-1);
    uint16_t i;
    static uint32_t u32_1;
    static uint32_t u32_2;

    /* Check parameters */
    if (socket_number >= s_sockets_max)
    {
        return (-1);
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    /* Query current socket status. */
    if (AT_OK != at_exec("AT+SQNSS?\r"))
    {
        ret = (-1);
        goto RELEASE_MUTEX;
    }

    at_move_to_next_line(); // Skip "\r\n"
    for (i = 0; i < socket_number ; i++)
    {
        at_move_to_next_line();
    }
    at_read("+SQNSS: %d,%d\r\n", &u32_1, &u32_2);
    ret = (int32_t)u32_2;

RELEASE_MUTEX:
    mutex_give(MUTEX_TX | MUTEX_RX);

    return ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_GetSocketStatus
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConfigTlslSocket
 * Description  : Request TLS socket communication.
 * Arguments    : sock_id    Socket connection identifier.[0-5]
 *                enable     Security enable status.      [0/1]
 *                spid       Security profile identifier. [0-5]
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConfigTlslSocket(uint8_t sock_id, e_ryz_ssl_enable_t enable, uint8_t spid)
{
    /* Connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_ERR_NOT_CONNECT;
    }

    /* Check parameters */
    if (sock_id >= s_sockets_max)
    {
        return WIFI_ERR_SOCKET_NUM;
    }

    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX|MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    at_exec("AT+SQNSSCFG=%d,%d,%d\r", sock_id + 1, enable, spid + 1);
    g_sock_tbl[sock_id].ssl.enable = enable;

    mutex_give(MUTEX_TX | MUTEX_RX);
    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_RYZ_ConfigTslSocket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_WriteCertificate
 * Description  : Write server certificate to module.
 * Arguments    : type    (0:certificate, 1:private key)
 *                index
 *                p_data
 *                length  (0:remove ,1-:writing size)
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_WriteCertificate(e_ryz_write_type_t type, uint32_t index, RYZ_C_U8_FAR p_data, uint32_t length)
{
    cellular_err_t api_ret = WIFI_ERR_MODULE_COM;
    uint32_t send_idx;
    uint32_t send_length;
    uint32_t txsize_max = get_port_config(g_uart_port)->tx_size;

    /* Disconnected WiFi module? */
    if (MODULE_DISCONNECTED == cellular_state_get())
    {
        return WIFI_ERR_NOT_OPEN;
    }

    /* Check parameters */
    if ((NULL == p_data) || (0 == length))
    {
        return WIFI_ERR_PARAMETER;
    }

    /* Take mutex. */
    if (E_OK != mutex_take(MUTEX_TX | MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    /* Send AT command : ATNSSLCERT */
    e_rslt_code_t rslt;
    if (WRITE_CERTIFICATE == type)
    {
        rslt = at_exec("AT+SQNSNVW=\"certificate\",%lu,%lu\r",index , length);
    }
    else
    {
        rslt = at_exec("AT+SQNSNVW=\"privatekey\",%lu,%lu\r",index , length);
    }
    if (AT_SEND_DATA != rslt)
    {
        goto RELEASE_MUTEX;
    }

    /* Send certificate data */
    send_idx = 0;
    at_init_memory();
    while (send_idx < length)
    {
        if ((length - send_idx) > txsize_max)
        {
            send_length = txsize_max;
        }
        else
        {
            send_length = length - send_idx;
        }

        /* data send */
        if (SCI_SUCCESS != wrap_sci_send((uint8_t __far *)p_data + send_idx, send_length))
        {
            goto RELEASE_MUTEX;
        }

        /* Wait for transmit end */
        while (1)
        {
            if (0 != g_uart_tbl.tx_end_flag)
            {
                break;
            }
            os_wrap_sleep(1, UNIT_TICK);
        }
        send_idx += send_length;
    }

    /* Receive response */
    if (AT_OK == at_recv())
    {
        api_ret = WIFI_SUCCESS;
    }

RELEASE_MUTEX:
    mutex_give(MUTEX_TX | MUTEX_RX);

    return api_ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_WriteCertificate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_SetTlsProfile
 * Description  : Associate server information to certificate.
 * Arguments    : spid                   Security Profile identifier, integer in range [0-5].
 *                ca_cert_id             Trusted Certificate Authority certificate ID, integer in range [0-19].
 *                client_cert_id         Client certificate ID, integer in range [0-19].
 *                client_privateKey_id   Client private key ID, integer in range [0-19].
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *********************************************************************************************************************/
cellular_err_t R_RYZ_SetTlsProfile(uint8_t spid, uint8_t ca_cert_id, uint8_t cli_cert_id, uint8_t cli_privateKey_id)
{
    cellular_err_t ret = WIFI_ERR_MODULE_COM;

    /* Check parameter */
    if (CERT_PROFILE_MAX <= spid)
    {
        return WIFI_ERR_PARAMETER;
    }
    /* Take mutex */
    if (E_OK != mutex_take(MUTEX_TX|MUTEX_RX))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }

    if (AT_OK == at_exec("AT+SQNSPCFG=%d,2,,5,%d,%d,%d,\"\"\r", spid + 1, ca_cert_id, cli_cert_id, cli_privateKey_id))
    {
        s_cert_profile[spid].ca_cert_id = ca_cert_id;
        s_cert_profile[spid].cli_cert_id = cli_cert_id;
        s_cert_profile[spid].cli_privatekey_id = cli_privateKey_id;
        ret = WIFI_SUCCESS;
    }
    mutex_give(MUTEX_TX|MUTEX_RX);
    return ret;
}
/**********************************************************************************************************************
 * End of function R_RYZ_SetTlsProfile
 *********************************************************************************************************************/

/*
 * System state control
 */
/**********************************************************************************************************************
 * Function Name: cellular_state_set
 * Description  : Set WIFI system state.
 * Arguments    : state
 * Return Value : none
 *********************************************************************************************************************/
static void cellular_state_set(e_module_status_t state)
{
    s_cellular_state = state;
}
/**********************************************************************************************************************
 * End of function cellular_state_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cellular_state_get
 * Description  : Get Cellular system state.
 * Arguments    : none
 * Return Value : wifi_system_status_t Cellular system state
 *********************************************************************************************************************/
static e_module_status_t cellular_state_get(void)
{
    return s_cellular_state;
}
/**********************************************************************************************************************
 * End of function cellular_state_get
 *********************************************************************************************************************/

/*
 * Sub functions for API
 */
/**********************************************************************************************************************
 * Function Name: disconnect_ap_sub
 * Description  : Disconnect access point (sub function).
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
static cellular_err_t disconnect_ap_sub(void)
{
    /* Not connected access point? */
    if (0 != R_RYZ_IsConnected())
    {
        return WIFI_SUCCESS;
    }

    /* Disconnect from APN */
    if (AT_OK != at_exec("AT+CGATT=0\r"))
    {
        return WIFI_ERR_MODULE_COM;
    }

    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function disconnect_ap_sub
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: shutdown_socket_sub
 * Description  : Closing socket (sub function)
 * Arguments    : sock_idx
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
static cellular_err_t shutdown_socket_sub(uint8_t sock_idx)
{
    /* Close network socket. */
    if (AT_OK != at_exec("AT+SQNSH=%d\r", sock_idx + 1))
    {
        return WIFI_ERR_MODULE_COM;
    }
    g_sock_tbl[sock_idx].status = SOCKET_STATUS_SOCKET;
    return WIFI_SUCCESS;
}
/**********************************************************************************************************************
 * End of function shutdown_socket_sub
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_ipaddr_by_name
 * Description  : Get host IP address by host name.
 * Arguments    : domain_name
 * Return Value : IP address (0:fail , other:assigned IP address)
 *********************************************************************************************************************/
static uint32_t get_ipaddr_by_name(RYZ_C_CH_FAR name)
{
    uint32_t ret = 0;
    static uint32_t ip[4];
    char buf[128];

    /* Perform DNS query. */
    for (uint8_t i = 0; i < 5; i++)
    {
        if (AT_OK == at_exec("AT+SQNDNSLKUP=\"%s\",0\r", name))
        {
            snprintf(buf, sizeof(buf), "+SQNDNSLKUP: %s,%s.%s.%s.%s\r\n", name, "%d", "%d", "%d", "%d");
            at_read(buf, &ip[0], &ip[1], &ip[2], &ip[3]);
            ret = IPADR_UB_TO_UL(ip[0], ip[1], ip[2], ip[3]);
            break;
        }
    }

    return ret;
}
/**********************************************************************************************************************
 * End of function get_ipaddr_by_name
 *********************************************************************************************************************/

/*
 * Mutex control
 */
/**********************************************************************************************************************
 * Function Name: mutex_create
 * Description  : Create Module mutex.
 * Arguments    : none
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_TAKE_MUTEX
 *********************************************************************************************************************/
static cellular_err_t mutex_create(void)
{
    cellular_err_t api_ret = WIFI_SUCCESS;

    os_wrap_mutex_delete(&s_binary_sem_tx);
    if (OS_WRAP_SUCCESS != os_wrap_mutex_create(&s_binary_sem_tx, (char WIFI_FAR *)"SX-ULPGN tx mutex"))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }
    os_wrap_mutex_delete(&s_binary_sem_rx);
    if (OS_WRAP_SUCCESS != os_wrap_mutex_create(&s_binary_sem_rx, (char WIFI_FAR *)"SX-ULPGN rx mutex"))
    {
        return WIFI_ERR_TAKE_MUTEX;
    }
    return api_ret;
}
/**********************************************************************************************************************
 * End of function mutex_create
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mutex_take
 * Description  : Take mutex for module.
 * Arguments    : mutex_flag
 * Return Value : E_OK    success
 *                E_FAIL  failed
 *********************************************************************************************************************/
static int32_t mutex_take(uint8_t mutex_flag)
{
    if (0 != (mutex_flag & MUTEX_TX))
    {
        if (OS_WRAP_SUCCESS != os_wrap_mutex_take(&s_binary_sem_tx, s_sem_block_time, CALL_NOT_ISR))
        {
            return E_FAIL;
        }
    }

    if (0 != (mutex_flag & MUTEX_RX))
    {
        if (OS_WRAP_SUCCESS != os_wrap_mutex_take(&s_binary_sem_rx, s_sem_block_time, CALL_NOT_ISR))
        {
            return E_FAIL;
        }
    }

    return E_OK;
}
/**********************************************************************************************************************
 * End of function mutex_take
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: mutex_give
 * Description  : Give mutex for module.
 * Arguments    : mutex_flg
 * Return Value : none
 *********************************************************************************************************************/
static void mutex_give(uint8_t mutex_flg)
{
    if (0 != (mutex_flg & MUTEX_RX))
    {
        os_wrap_mutex_give(&s_binary_sem_rx, CALL_NOT_ISR);
        os_wrap_sleep(1, UNIT_TICK);
    }
    if (0 != (mutex_flg & MUTEX_TX))
    {
        os_wrap_mutex_give(&s_binary_sem_tx, CALL_NOT_ISR);
        os_wrap_sleep(1, UNIT_TICK);
    }
}
/**********************************************************************************************************************
 * End of function mutex_give
 *********************************************************************************************************************/

/*
 * WIFI module control
 */
/**********************************************************************************************************************
 * Function Name: module_hw_reset
 * Description  : Reset module.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void module_hw_reset(void)
{
    /* Phase 3 WIFI Module hardware reset   */
    WIFI_RESET_DR(RYZ_CFG_RESET_PORT, RYZ_CFG_RESET_PIN)  = 1;  /* low    */
    WIFI_RESET_DDR(RYZ_CFG_RESET_PORT, RYZ_CFG_RESET_PIN) = 0;  /* output */
    R_BSP_SoftwareDelay(30, BSP_DELAY_MILLISECS);
    WIFI_RESET_DR(RYZ_CFG_RESET_PORT, RYZ_CFG_RESET_PIN)  = 0;  /* high   */
    R_BSP_SoftwareDelay(2000, BSP_DELAY_MILLISECS);
}
/**********************************************************************************************************************
 * End of function module_hw_reset
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: module_close
 * Description  : Close module.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void module_close(void)
{
    uart_port_close();
    os_wrap_mutex_delete(&s_binary_sem_rx);
    os_wrap_mutex_delete(&s_binary_sem_tx);
    cellular_state_set(MODULE_DISCONNECTED);
}
/**********************************************************************************************************************
 * End of function module_close
 *********************************************************************************************************************/

/*
 * Port configuration
 */
/**********************************************************************************************************************
 * Function Name: flow_ctrl_init
 * Description  : Initialize HW flow control.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void flow_ctrl_init(void)
{
    wrap_sci_ctrl(SCI_CMD_EN_CTS_IN, NULL);
//    WIFI_RTS_DDR(RYZ_CFG_RTS_PORT, RYZ_CFG_RTS_PIN) = 0;  /* Output */
    WIFI_RTS_DR(RYZ_CFG_RTS_PORT, RYZ_CFG_RTS_PIN)  = 0;  /* low    */
}
/**********************************************************************************************************************
 * End of function flow_ctrl_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: flow_ctrl_set
 * Description  : Set HW flow control.
 * Arguments    : flow (RTS_OFF, RTS_ON)
 * Return Value : none
 *********************************************************************************************************************/
static void flow_ctrl_set(uint32_t flow)
{
    WIFI_RTS_DR(RYZ_CFG_RTS_PORT, RYZ_CFG_RTS_PIN) = flow;
}
/**********************************************************************************************************************
 * End of function flow_ctrl_set
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_port_config
 * Description  : get port(HSUART1 or HSUART2) configuration table pointer.
 * Arguments    : port - (0:HSUART1, 1:HSUART2)
 * Return Value : SUCCESS : SCI configuration table(st_sci_conf_t) pointer by port.
 *                FAIL    : NULL
 *********************************************************************************************************************/
static st_sci_conf_t WIFI_FAR * get_port_config(uint8_t ch)
{
    uint16_t i;
    st_sci_conf_t WIFI_FAR * p_tbl = NULL;

    /* Set table pointer */
    for (i = 0;; i++ )
    {
        if (SCI_NUM_CH == s_sci_cfg[i].ch)
        {
            break;
        }
        if (ch == s_sci_cfg[i].ch)
        {
            p_tbl = (st_sci_conf_t WIFI_FAR *)&s_sci_cfg[i];
            break;
        }
    }

    return p_tbl;
}
/**********************************************************************************************************************
 * End of function get_port_config
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: uart_open
 * Description  : Initialize SCI for Command port.
 * Arguments    : port  - Command port number
 *                p_cb  - Callback function of SCI interrupts
 * Return Value : E_OK    success
 *                E_FAIL  failed
 *********************************************************************************************************************/
static int32_t uart_open(uint8_t ch, void (* const p_cb)(void *p_args))
{
    st_uart_tbl_t * p_uart = &g_uart_tbl;
    st_sci_conf_t WIFI_FAR * p_cfg = get_port_config(ch);

    if (NULL == p_cfg)
    {
        return E_FAIL;
    }

    /* Port settings */
    if (NULL != p_cfg->func)
    {
        p_cfg->func();
    }

    memset(&p_uart->sci_hdl , 0, sizeof(sci_hdl_t));
    p_uart->sci_config.async.baud_rate    = RYZ_BAUD_DEFAULT;
    p_uart->sci_config.async.clk_src      = SCI_CLK_INT;
    p_uart->sci_config.async.data_size    = SCI_DATA_8BIT;
    p_uart->sci_config.async.parity_en    = SCI_PARITY_OFF;
    p_uart->sci_config.async.parity_type  = SCI_EVEN_PARITY;
    p_uart->sci_config.async.stop_bits    = SCI_STOPBITS_1;
    p_uart->sci_config.async.int_priority = RYZ_CFG_SCI_INTERRUPT_LEVEL;
    if (SCI_SUCCESS != R_SCI_Open(p_cfg->ch, SCI_MODE_ASYNC, &p_uart->sci_config, p_cb, &p_uart->sci_hdl))
    {
        return E_FAIL;
    }
    return E_OK;
}
/**********************************************************************************************************************
 * End of function uart_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: uart_port_close
 * Description  : Close serial port.
 * Arguments    : port  - HSUART port number
 * Return Value : none
 *********************************************************************************************************************/
static void uart_port_close(void)
{
    if (0 != g_uart_tbl.sci_hdl)
    {
        R_SCI_Control(g_uart_tbl.sci_hdl, SCI_CMD_RX_Q_FLUSH, NULL);
        R_SCI_Control(g_uart_tbl.sci_hdl, SCI_CMD_TX_Q_FLUSH, NULL);
        R_SCI_Close(g_uart_tbl.sci_hdl);
    }
}
/**********************************************************************************************************************
 * End of function uart_port_close
 *********************************************************************************************************************/

/*
 * Callback functions
 */
/**********************************************************************************************************************
 * Function Name: uart_callback
 * Description  : SCI callback function of serial secondary port.
 * Arguments    : pArgs
 * Return Value : none
 *********************************************************************************************************************/
static void uart_callback(void * pArgs)
{
    sci_cb_args_t * p_args = (sci_cb_args_t *) pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        R_BSP_NOP();
    }
    else if (SCI_EVT_TEI == p_args->event)
    {
        g_uart_tbl.tx_end_flag = 1;
    }
    else
    {
        cb_sci_err(p_args);
    }
}
/**********************************************************************************************************************
 * End of function uart_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cb_sci_err
 * Description  : SCI callback function of error event.
 * Arguments    : event
 * Return Value : none
 *********************************************************************************************************************/
//static void cb_sci_err(sci_cb_evt_t event)
static void cb_sci_err(sci_cb_args_t * p_args)
{
    if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full */
        R_BSP_NOP();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt */
        R_BSP_NOP();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt */
        R_BSP_NOP();
    }
    else
    {
        /* Do nothing */
    }
}
/**********************************************************************************************************************
 * End of function cb_sci_err
 *********************************************************************************************************************/

/*
 * Wrapper
 */
/**********************************************************************************************************************
 * Function Name: wrap_sci_send
 * Description  : Wrapped R_SCI_Send().
 * Arguments    : p_src
 *                length
 * Return Value : Same as R_SCI_Send() function
 *********************************************************************************************************************/
static sci_err_t wrap_sci_send(uint8_t __far *p_src, uint16_t const length)
{
    g_uart_tbl.tx_end_flag = 0;
    return R_SCI_Send(g_uart_tbl.sci_hdl, (uint8_t __far *)p_src, length);
}
/**********************************************************************************************************************
 * End of function wrap_sci_send
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wrap_sci_ctrl
 * Description  : Wrapped R_SCI_Control().
 * Arguments    : port  - HSUART port number
 *                cmd
 *                p_args
 * Return Value : Same as R_SCI_Control() function
 *********************************************************************************************************************/
static sci_err_t wrap_sci_ctrl(sci_cmd_t const cmd, void * p_args)
{
    return R_SCI_Control(g_uart_tbl.sci_hdl, cmd, p_args);
}
/**********************************************************************************************************************
 * End of function wrap_sci_ctrl
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wrap_sci_recv
 * Description  : Wrapped R_SCI_Receive().
 * Arguments    : p_dst
 *                length
 * Return Value : Same as R_SCI_Receive() function
 *********************************************************************************************************************/
static sci_err_t wrap_sci_recv(uint8_t * p_dst, uint16_t const length)
{
    return R_SCI_Receive(g_uart_tbl.sci_hdl, p_dst, length);
}
/**********************************************************************************************************************
 * End of function wrap_sci_recv
 *********************************************************************************************************************/
