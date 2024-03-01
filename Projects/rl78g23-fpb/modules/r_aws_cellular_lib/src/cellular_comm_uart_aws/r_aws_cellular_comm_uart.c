/*
 * Amazon FreeRTOS CELLULAR Preview Release
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* The config header is always included first. */

/* Standard includes. */
#include <string.h>

#include "FreeRTOS.h"
#include "event_groups.h"
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "platform.h"
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "rl78_bsp_wrapper.h"
#include "r_sci_rl_config.h"
#else
#include "r_sci_rx_pinset.h"
#endif

/* Cellular includes. */
#include "r_aws_cellular_if.h"


#define AWS_CELLULAR_DEFAULT_SEND_WAIT      (1)
#define AWS_CELLULAR_DEFAULT_RECV_WAIT      (1)
#define AWS_CELLULAR_RECV_LIMIT             (50)

#define AWS_CELLUALR_COMM_EVT_RX_DATA       (0x01u << 0)
#define AWS_CELLUALR_COMM_EVT_RX_ERROR      (0x01u << 1)
#define AWS_CELLUALR_COMM_EVT_RX_ABORTED    (0x01u << 2)
#define AWS_CELLUALR_COMM_EVT_TX_DONE       (0x01u << 3)
#define AWS_CELLUALR_COMM_EVT_TX_ERROR      (0x01u << 4)
#define AWS_CELLUALR_COMM_EVT_TX_ABORTED    (0x01u << 5)

#ifndef AWS_CELLULAR_CFG_SCI_CH
#error "Error! Need to define AWS_CELLULAR_CFG_SCI_CH in r_aws_cellular_config.h"
#elif AWS_CELLULAR_CFG_SCI_CH == (0)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI0())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH0)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH0_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH0_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (1)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI1())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH1)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH1_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH1_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (2)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI2())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH2)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH2_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH2_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (3)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI3())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH3)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH3_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH3_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (4)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI4())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH4)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH4_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH4_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (5)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI5())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH5)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH5_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH5_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (6)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI6())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH6)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH6_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH6_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (7)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI7())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH7)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH7_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH7_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (8)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI8())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH8)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH8_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH8_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (9)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI9())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH9)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH9_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH9_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (10)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI10())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH10)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH10_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH10_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (11)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI11())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH11)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH11_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH11_RX_BUFSIZ)
#elif AWS_CELLULAR_CFG_SCI_CH == (12)
/*  */
#define AWS_CELLULAR_SERIAL_PINSET()    (R_SCI_PinSet_SCI12())
#define AWS_CELLULAR_SERIAL_CH          (SCI_CH12)
#define AWS_CELLULAR_TX_BUFSIZE         (SCI_CFG_CH12_TX_BUFSIZ)
#define AWS_CELLULAR_RX_BUFSIZE         (SCI_CFG_CH12_RX_BUFSIZ)
#else
#error "Error! Invalid setting for AWS_CELLULAR_CFG_SCI_CH in r_aws_cellular_config.h"
#endif /* AWS_CELLULAR_CFG_SCI_CH */

typedef struct aws_cellular_uart_ctrl
{
    volatile uint8_t    rx_fifo_reading_flg;
    uint32_t            tx_buff_size;
    uint32_t            rx_buff_size;
    bool                if_open;
    sci_cb_evt_t        sci_err_flg;
    sci_hdl_t           sci_hdl;
    EventGroupHandle_t  p_event_hdl;
    void *              p_user_data;
    CellularCommInterfaceReceiveCallback_t p_recv_cb;
} st_aws_cellular_uart_ctrl_t;

/*-----------------------------------------------------------*/

static sci_err_t aws_cellular_serial_open (st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl);
static sci_err_t aws_cellular_serial_close (st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl);
static void      aws_cellular_uart_callback (void * const p_Args);

static CellularCommInterfaceError_t aws_cellular_open (CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                       void * p_user_data,
                                                       CellularCommInterfaceHandle_t * pCommInterfaceHandle);
static CellularCommInterfaceError_t aws_cellular_close (CellularCommInterfaceHandle_t commInterfaceHandle);
static CellularCommInterfaceError_t aws_cellular_recv (CellularCommInterfaceHandle_t commInterfaceHandle,
                                                        uint8_t * pBuffer,
                                                        uint32_t bufferLength,
                                                        uint32_t timeoutMilliseconds,
                                                        uint32_t * pDataReceivedLength);
static CellularCommInterfaceError_t aws_cellular_send (CellularCommInterfaceHandle_t commInterfaceHandle,
                                                       const uint8_t * pData,
                                                       uint32_t dataLength,
                                                       uint32_t timeoutMilliseconds,
                                                       uint32_t * pDataSentLength);
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
static uint32_t                     aws_cellular_timeout_reconfig (st_aws_cellular_timeout_ctrl_t * const p_timeout);
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
static CellularCommInterfaceError_t aws_cellular_wait_bits (EventGroupHandle_t hdl, uint32_t timeout);
#endif
#endif

/*-----------------------------------------------------------*/

static st_aws_cellular_uart_ctrl_t s_aws_cellular_uart_ctrl = {0};

CellularCommInterface_t g_cellular_comm_interface =
{
    .open = aws_cellular_open,
    .send = aws_cellular_send,
    .recv = aws_cellular_recv,
    .close = aws_cellular_close
};

/*-----------------------------------------------------------*/

/************************************************************************
 * Function Name  @fn            aws_cellular_uart_callback
 ***********************************************************************/
static void aws_cellular_uart_callback(void * const p_Args)
{
    sci_cb_args_t * const p_args                   = (sci_cb_args_t *)p_Args; //cast
    BaseType_t            xHigherPriorityTaskWoken = pdFALSE;

    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = &s_aws_cellular_uart_ctrl;
    CellularCommInterfaceError_t  ret                      = IOT_COMM_INTERFACE_SUCCESS;

    switch (p_args->event)
    {
        case SCI_EVT_RX_CHAR:
        {
            if (0 == p_aws_cellular_uart_ctrl->rx_fifo_reading_flg)
            {
                ret = p_aws_cellular_uart_ctrl->p_recv_cb(p_aws_cellular_uart_ctrl->p_user_data,
                            (CellularCommInterfaceHandle_t)p_aws_cellular_uart_ctrl);
                if (IOT_COMM_INTERFACE_SUCCESS == ret)
                {
                    portYIELD_FROM_ISR(pdTRUE);
                }
            }

            xEventGroupSetBitsFromISR( p_aws_cellular_uart_ctrl->p_event_hdl,
                                       AWS_CELLUALR_COMM_EVT_RX_DATA,
                                       & xHigherPriorityTaskWoken );
            if (pdTRUE == xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR(pdTRUE);
            }
            break;
        }
#if SCI_CFG_TEI_INCLUDED == 1
        case SCI_EVT_TEI:
        {
            xEventGroupSetBitsFromISR(p_aws_cellular_uart_ctrl->p_event_hdl,
                                      AWS_CELLUALR_COMM_EVT_TX_DONE,
                                      &xHigherPriorityTaskWoken);
            if (pdTRUE == xHigherPriorityTaskWoken)
            {
                portYIELD_FROM_ISR(pdTRUE);
            }
            break;
        }
#endif
        case SCI_EVT_RXBUF_OVFL:
        {
            p_aws_cellular_uart_ctrl->sci_err_flg = SCI_EVT_RXBUF_OVFL;
            break;
        }
        case SCI_EVT_OVFL_ERR:
        {
            p_aws_cellular_uart_ctrl->sci_err_flg = SCI_EVT_OVFL_ERR;
            break;
        }
        case SCI_EVT_FRAMING_ERR:
        {
            p_aws_cellular_uart_ctrl->sci_err_flg = SCI_EVT_FRAMING_ERR;
            break;
        }
        case SCI_EVT_PARITY_ERR:
        {
            p_aws_cellular_uart_ctrl->sci_err_flg = SCI_EVT_PARITY_ERR;
            break;
        }
        default:
        {
            break;
        }
    }
}
/***********************************************************************
 * End of function aws_cellular_uart_callback
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_serial_open
 ***********************************************************************/
static sci_err_t aws_cellular_serial_open(st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl)
{
    sci_err_t   sci_ret     = SCI_SUCCESS;
    sci_cfg_t   sci_cfg     = {0};
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    uint8_t     tx_priority = AWS_CELLULAR_CFG_SCI_PRIORITY - 1;
#endif

    p_aws_cellular_uart_ctrl->tx_buff_size = AWS_CELLULAR_TX_BUFSIZE;
    p_aws_cellular_uart_ctrl->rx_buff_size = AWS_CELLULAR_RX_BUFSIZE;
    sci_cfg.async.baud_rate                = AWS_CELLULAR_CFG_BAUDRATE;
    sci_cfg.async.clk_src                  = SCI_CLK_INT;
    sci_cfg.async.data_size                = SCI_DATA_8BIT;
    sci_cfg.async.parity_en                = SCI_PARITY_OFF;
    sci_cfg.async.parity_type              = SCI_EVEN_PARITY;
    sci_cfg.async.stop_bits                = SCI_STOPBITS_1;
    sci_cfg.async.int_priority             = AWS_CELLULAR_CFG_SCI_PRIORITY;    // 1=lowest, 15=highest

    sci_ret = R_SCI_Open(AWS_CELLULAR_SERIAL_CH, SCI_MODE_ASYNC, &sci_cfg,
                         aws_cellular_uart_callback, &p_aws_cellular_uart_ctrl->sci_hdl);

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    if (SCI_SUCCESS == sci_ret)
    {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 0
        R_SCI_Control(p_aws_cellular_uart_ctrl->sci_hdl, SCI_CMD_EN_CTS_IN, FIT_NO_PTR);
#endif
#if AWS_CELLULAR_CFG_START_BIT_EDGE == 1
        R_SCI_Control(p_aws_cellular_uart_ctrl->sci_hdl, SCI_CMD_START_BIT_EDGE, FIT_NO_PTR);
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        R_SCI_Control(p_aws_cellular_uart_ctrl->sci_hdl, SCI_CMD_SET_TXI_PRIORITY, &tx_priority);
#else
#error  BUILD_TIME_ERROR_MESSAGE_MCU_NOT_SUPPORT
#endif
        AWS_CELLULAR_SERIAL_PINSET();
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 0
        AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 0;
        AWS_CELLULAR_SET_PDR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN)  = 1;
#endif
        LogInfo(("R_SCI_Open successfully. Baud rate is %d.", AWS_CELLULAR_CFG_BAUDRATE));
    }
#endif

    return sci_ret;
}
/***********************************************************************
 * End of function aws_cellular_serial_open
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_serial_reopen
 ***********************************************************************/
sci_err_t aws_cellular_serial_reopen(CellularHandle_t cellularHandle, const uint32_t baudrate)
{
    CellularContext_t           * p_context   = (CellularContext_t *) cellularHandle;                       //cast
    st_aws_cellular_uart_ctrl_t * p_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)p_context->hPktioCommIntf;   //cast

    sci_err_t   sci_ret     = SCI_SUCCESS;
    sci_cfg_t   sci_cfg     = {0};
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    uint8_t     tx_priority = AWS_CELLULAR_CFG_SCI_PRIORITY - 1;
#endif

    sci_cfg.async.baud_rate                = baudrate;
    sci_cfg.async.clk_src                  = SCI_CLK_INT;
    sci_cfg.async.data_size                = SCI_DATA_8BIT;
    sci_cfg.async.parity_en                = SCI_PARITY_OFF;
    sci_cfg.async.parity_type              = SCI_EVEN_PARITY;
    sci_cfg.async.stop_bits                = SCI_STOPBITS_1;
    sci_cfg.async.int_priority             = AWS_CELLULAR_CFG_SCI_PRIORITY;    // 1=lowest, 15=highest

    sci_ret = R_SCI_Open(AWS_CELLULAR_SERIAL_CH, SCI_MODE_ASYNC, &sci_cfg,
                         aws_cellular_uart_callback, &p_uart_ctrl->sci_hdl);

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    if (SCI_SUCCESS == sci_ret)
    {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 0
        R_SCI_Control(p_uart_ctrl->sci_hdl, SCI_CMD_EN_CTS_IN, FIT_NO_PTR);
#endif
#if AWS_CELLULAR_CFG_START_BIT_EDGE == 1
        R_SCI_Control(p_uart_ctrl->sci_hdl, SCI_CMD_START_BIT_EDGE, FIT_NO_PTR);
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        R_SCI_Control(p_uart_ctrl->sci_hdl, SCI_CMD_SET_TXI_PRIORITY, &tx_priority);
#else
#error  BUILD_TIME_ERROR_MESSAGE_MCU_NOT_SUPPORT
#endif
        AWS_CELLULAR_SERIAL_PINSET();
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 0
        AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 0;
        AWS_CELLULAR_SET_PDR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN)  = 1;
#endif
        LogInfo(("R_SCI_Open successfully. Baud rate is %d.", baudrate));
    }
#endif

    return sci_ret;
}
/***********************************************************************
 * End of function aws_cellular_serial_reopen
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_temp_close
 ***********************************************************************/
sci_err_t aws_cellular_temp_close(CellularHandle_t cellularHandle)
{
    CellularContext_t           * p_context   = (CellularContext_t *) cellularHandle;                       //cast
    st_aws_cellular_uart_ctrl_t * p_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)p_context->hPktioCommIntf;   //cast

    sci_err_t ret;

    ret = R_SCI_Close(p_uart_ctrl->sci_hdl);

    return ret;
}
/***********************************************************************
 * End of function aws_cellular_temp_close
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_serial_close
 ***********************************************************************/
static sci_err_t aws_cellular_serial_close(st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl)
{
    sci_err_t ret;

    ret = R_SCI_Close(p_aws_cellular_uart_ctrl->sci_hdl);

    return ret;
}
/***********************************************************************
 * End of function aws_cellular_serial_close
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_open
 ***********************************************************************/
static CellularCommInterfaceError_t aws_cellular_open(CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                       void * p_user_data,
                                                       CellularCommInterfaceHandle_t * pCommInterfaceHandle)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = &s_aws_cellular_uart_ctrl;

    sci_err_t                    sci_ret;
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;

    /* check input parameter. */
    if ((NULL == pCommInterfaceHandle) || (NULL == receiveCallback))
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if (true == p_aws_cellular_uart_ctrl->if_open)
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        /* Initialize the context structure. */
        (void)memset(p_aws_cellular_uart_ctrl, 0, sizeof(st_aws_cellular_uart_ctrl_t));
    }

    /* Setup the read FIFO. */
    if (IOT_COMM_INTERFACE_SUCCESS == ret)
    {
        p_aws_cellular_uart_ctrl->p_event_hdl = xEventGroupCreate();
        if (NULL == p_aws_cellular_uart_ctrl->p_event_hdl)
        {
            LogError(("EventGroup create failed"));
            ret = IOT_COMM_INTERFACE_NO_MEMORY;
        }
    }

    if (IOT_COMM_INTERFACE_SUCCESS == ret)
    {
        sci_ret = aws_cellular_serial_open(p_aws_cellular_uart_ctrl);
        if (SCI_SUCCESS != sci_ret)
        {
            ret     = IOT_COMM_INTERFACE_DRIVER_ERROR;
            sci_ret = aws_cellular_serial_close(p_aws_cellular_uart_ctrl);
            vEventGroupDelete(p_aws_cellular_uart_ctrl->p_event_hdl);
        }
    }

    /* setup callback function and userdata. */
    if (IOT_COMM_INTERFACE_SUCCESS == ret)
    {
        p_aws_cellular_uart_ctrl->p_recv_cb   = receiveCallback;
        p_aws_cellular_uart_ctrl->p_user_data = p_user_data;
        *pCommInterfaceHandle                 = (CellularCommInterfaceHandle_t)p_aws_cellular_uart_ctrl;
        p_aws_cellular_uart_ctrl->if_open     = true;
    }

    return ret;
}
/***********************************************************************
 * End of function aws_cellular_open
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_close
 ***********************************************************************/
static CellularCommInterfaceError_t aws_cellular_close(CellularCommInterfaceHandle_t commInterfaceHandle)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)commInterfaceHandle;    //cast

    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_FAILURE;

    if (NULL == p_aws_cellular_uart_ctrl)
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if (false == p_aws_cellular_uart_ctrl->if_open)
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        if (NULL != p_aws_cellular_uart_ctrl->sci_hdl)
        {
            aws_cellular_serial_close(p_aws_cellular_uart_ctrl);
            p_aws_cellular_uart_ctrl->sci_hdl = NULL;
        }

        /* Clean event group. */
        if (NULL != p_aws_cellular_uart_ctrl->p_event_hdl)
        {
            vEventGroupDelete(p_aws_cellular_uart_ctrl->p_event_hdl);
            p_aws_cellular_uart_ctrl->p_event_hdl = NULL;
        }

        /* Set the device open status to false. */
        p_aws_cellular_uart_ctrl->if_open = false;

        ret = IOT_COMM_INTERFACE_SUCCESS;
    }

    return ret;
}
/***********************************************************************
 * End of function aws_cellular_close
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_send
 ***********************************************************************/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static CellularCommInterfaceError_t aws_cellular_send(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      const uint8_t * pData,
                                                      uint32_t dataLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataSentLength)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)commInterfaceHandle;    //cast

    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;
    sci_err_t                    err    = SCI_SUCCESS;
    EventBits_t                  uxBits = 0;
    uint32_t                     send_length;
    uint32_t                     tick_sta;
    uint32_t                     tick_timeout;
    uint32_t                     tick_tmp;

    tick_timeout = pdMS_TO_TICKS(timeoutMilliseconds);
    tick_sta  = pdMS_TO_TICKS(xTaskGetTickCount());

    if ((NULL == p_aws_cellular_uart_ctrl) || (NULL == pData) || (NULL == pDataSentLength) || (0 == dataLength))
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else
    {
        if (false == p_aws_cellular_uart_ctrl->if_open)
        {
            ret = IOT_COMM_INTERFACE_FAILURE;
        }
    }

    if (IOT_COMM_INTERFACE_SUCCESS == ret)
    {
        (*pDataSentLength) = 0;

        while ((*pDataSentLength) < dataLength)
        {
            ( void ) xEventGroupClearBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                           AWS_CELLUALR_COMM_EVT_TX_DONE |
                                           AWS_CELLUALR_COMM_EVT_TX_ERROR |
                                           AWS_CELLUALR_COMM_EVT_TX_ABORTED);

            /* Calculate Send length */
            if ((dataLength - (*pDataSentLength)) > p_aws_cellular_uart_ctrl->tx_buff_size)
            {
                send_length = p_aws_cellular_uart_ctrl->tx_buff_size;
            }
            else
            {
                send_length = dataLength - (*pDataSentLength);
            }

            /* Send data */
            err = R_SCI_Send(p_aws_cellular_uart_ctrl->sci_hdl, (uint8_t __far *)pData + (*pDataSentLength), send_length);
            if (SCI_SUCCESS != err)
            {
                ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                break;
            }

            /* Wait for transmit end */
            uxBits = xEventGroupWaitBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                          AWS_CELLUALR_COMM_EVT_TX_DONE |
                                          AWS_CELLUALR_COMM_EVT_TX_ERROR |
                                          AWS_CELLUALR_COMM_EVT_TX_ABORTED,
                                          pdTRUE,
                                          pdFALSE,
                                          pdMS_TO_TICKS(timeoutMilliseconds));

            /* No events? */
            if (0U == uxBits)
            {
                ret = IOT_COMM_INTERFACE_TIMEOUT;
                break;
            }

            /* Error events? */
            if (0U != (uxBits & (AWS_CELLUALR_COMM_EVT_TX_ERROR | AWS_CELLUALR_COMM_EVT_TX_ABORTED)))
            {
                ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                break;
            }

            (*pDataSentLength) += send_length;

            /* timeout? */
            if (0 < timeoutMilliseconds)
            {
                tick_tmp = pdMS_TO_TICKS(xTaskGetTickCount()) - tick_sta;
                if (tick_timeout <= tick_tmp)
                {
                    ret = IOT_COMM_INTERFACE_TIMEOUT;
                    break;
                }
            }
        }
    }

    return ret;
}
#else
static CellularCommInterfaceError_t aws_cellular_send(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                       const uint8_t * pData,
                                                       uint32_t dataLength,
                                                       uint32_t timeoutMilliseconds,
                                                       uint32_t * pDataSentLength)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)commInterfaceHandle;    //cast

    CellularCommInterfaceError_t ret    = IOT_COMM_INTERFACE_SUCCESS;
    sci_err_t                    err    = SCI_SUCCESS;
    EventBits_t                  uxBits = 0;
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
    st_aws_cellular_timeout_ctrl_t timeout;

    uint32_t send_size  = 0;
    uint32_t waitTimeMs = timeoutMilliseconds;

    timeout.over_flg    = 0;
    timeout.timeout_flg = 0;
    timeout.start_time  = pdMS_TO_TICKS(xTaskGetTickCount());
    timeout.end_time    = timeout.start_time + timeoutMilliseconds;

    if (timeout.end_time < timeout.start_time)
    {
        timeout.over_flg = 1;
    }
#endif

    if ((NULL == p_aws_cellular_uart_ctrl) || (NULL == pData) || (NULL == pDataSentLength) || (0 == dataLength))
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else
    {
        if (false == p_aws_cellular_uart_ctrl->if_open)
        {
            ret = IOT_COMM_INTERFACE_FAILURE;
        }
    }

    if (IOT_COMM_INTERFACE_SUCCESS == ret)
    {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 0
        ( void ) xEventGroupClearBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                        AWS_CELLUALR_COMM_EVT_TX_DONE |
                                        AWS_CELLUALR_COMM_EVT_TX_ERROR |
                                        AWS_CELLUALR_COMM_EVT_TX_ABORTED);

        err = R_SCI_Send(p_aws_cellular_uart_ctrl->sci_hdl, (uint8_t *)pData, (uint16_t)dataLength);    //cast
        if (SCI_SUCCESS == err)
        {
            uxBits = xEventGroupWaitBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                            AWS_CELLUALR_COMM_EVT_TX_DONE |
                                            AWS_CELLUALR_COMM_EVT_TX_ERROR |
                                            AWS_CELLUALR_COMM_EVT_TX_ABORTED,
                                            pdTRUE,
                                            pdFALSE,
                                            pdMS_TO_TICKS(timeoutMilliseconds));

            if (0U != (uxBits & (AWS_CELLUALR_COMM_EVT_TX_ERROR | AWS_CELLUALR_COMM_EVT_TX_ABORTED)))
            {
                ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
            }
            else if (0 == (uxBits & AWS_CELLUALR_COMM_EVT_TX_DONE))
            {
                ret = IOT_COMM_INTERFACE_TIMEOUT;
            }
            else
            {
                (*pDataSentLength) += dataLength;
                ret                 = IOT_COMM_INTERFACE_SUCCESS;
            }
        }
        else
        {
            ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
        }
#else /* AWS_CELLULAR_CFG_CTS_SW_CTRL == 0 */

        /* WAIT_LOOP */
        while (send_size < dataLength)
        {
            if (1 != AWS_CELLULAR_GET_PIDR(AWS_CELLULAR_CFG_CTS_PORT, AWS_CELLULAR_CFG_CTS_PIN))
            {
                (void)xEventGroupClearBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                            AWS_CELLUALR_COMM_EVT_TX_DONE |
                                            AWS_CELLUALR_COMM_EVT_TX_ERROR |
                                            AWS_CELLUALR_COMM_EVT_TX_ABORTED);

                err = R_SCI_Send(p_aws_cellular_uart_ctrl->sci_hdl, (uint8_t *)&pData[send_size], 1);    //cast
                if (SCI_SUCCESS == err)
                {
                    ret = aws_cellular_wait_bits(p_aws_cellular_uart_ctrl->p_event_hdl, waitTimeMs);
                    if (IOT_COMM_INTERFACE_SUCCESS == ret)
                    {
                        send_size++;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                    break;
                }
            }

            waitTimeMs = aws_cellular_timeout_reconfig(&timeout);
            if (1 == timeout.timeout_flg)
            {
                ret = IOT_COMM_INTERFACE_TIMEOUT;
                break;
            }
        }

        *pDataSentLength = send_size;
#endif  /* AWS_CELLULAR_CFG_CTS_SW_CTRL == 0 */
    }

    return ret;
}
#endif /* defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) */
/***********************************************************************
 * End of function aws_cellular_send
 **********************************************************************/

/************************************************************************
 * Function Name  @fn            aws_cellular_recv
 ***********************************************************************/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static CellularCommInterfaceError_t aws_cellular_recv(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      uint8_t * pBuffer,
                                                      uint32_t bufferLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataReceivedLength)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)commInterfaceHandle;    //cast

    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;

    EventBits_t uxBits       = 0;
    sci_err_t   sci_ret      = SCI_SUCCESS;
    uint8_t     rxChar       = 0;
    uint32_t    rxCount      = 0;
    uint32_t    wait_timems  = AWS_CELLULAR_DEFAULT_RECV_WAIT;
    uint32_t    tick_sta;
    uint32_t    tick_timeout;
    uint32_t    tick_tmp;

    tick_timeout = pdMS_TO_TICKS(timeoutMilliseconds);
    tick_sta  = pdMS_TO_TICKS(xTaskGetTickCount());

    if ((NULL == p_aws_cellular_uart_ctrl) || (NULL == pBuffer) ||
            (NULL == pDataReceivedLength) || (0 == bufferLength))
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if (false == p_aws_cellular_uart_ctrl->if_open)
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        (*pDataReceivedLength) = 0;

        /* Set this flag to inform interrupt handler to stop calling callback function. */
        p_aws_cellular_uart_ctrl->rx_fifo_reading_flg = 1U;

        ( void ) xEventGroupClearBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                        AWS_CELLUALR_COMM_EVT_RX_DATA |
                                        AWS_CELLUALR_COMM_EVT_RX_ERROR |
                                        AWS_CELLUALR_COMM_EVT_RX_ABORTED);

        /* WAIT_LOOP */
        while (rxCount < bufferLength)
        {
            sci_ret = R_SCI_Receive(p_aws_cellular_uart_ctrl->sci_hdl, &rxChar, 1);
            if (SCI_SUCCESS == sci_ret)
            {
                pBuffer[rxCount] = rxChar;
                rxCount++;

                if (((' ' == rxChar) && ('>' == pBuffer[rxCount - 2])) ||
                        ((rxCount > 2) && ('\n' == rxChar)))
                {
                    break;
                }
            }
            else
            {
                uxBits = xEventGroupWaitBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                                AWS_CELLUALR_COMM_EVT_RX_DATA |
                                                AWS_CELLUALR_COMM_EVT_RX_ERROR |
                                                AWS_CELLUALR_COMM_EVT_RX_ABORTED,
                                                pdTRUE,
                                                pdFALSE,
                                                pdMS_TO_TICKS(wait_timems));  //cast

                if (0U != (uxBits & (AWS_CELLUALR_COMM_EVT_RX_ERROR | AWS_CELLUALR_COMM_EVT_RX_ABORTED)))
                {
                    ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                    break;
                }
            }

            /* timeout? */
            if (0 < timeoutMilliseconds)
            {
                tick_tmp = pdMS_TO_TICKS(xTaskGetTickCount()) - tick_sta;
                if (tick_timeout <= tick_tmp)
                {
                    ret = IOT_COMM_INTERFACE_TIMEOUT;
                    break;
                }
            }
        }

        /* Clear this flag to inform interrupt handler to call callback function. */
        p_aws_cellular_uart_ctrl->rx_fifo_reading_flg = 0U;

        *pDataReceivedLength = rxCount;

        /* Return success if bytes received. Even if timeout or RX error. */
        if (rxCount > 0)
        {
            ret = IOT_COMM_INTERFACE_SUCCESS;
        }
    }

    return ret;
}
#else
static CellularCommInterfaceError_t aws_cellular_recv(CellularCommInterfaceHandle_t commInterfaceHandle,
                                                      uint8_t * pBuffer,
                                                      uint32_t bufferLength,
                                                      uint32_t timeoutMilliseconds,
                                                      uint32_t * pDataReceivedLength)
{
    st_aws_cellular_uart_ctrl_t * p_aws_cellular_uart_ctrl = (st_aws_cellular_uart_ctrl_t *)commInterfaceHandle;    //cast

    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;

    EventBits_t uxBits       = 0;
    sci_err_t   sci_ret      = SCI_SUCCESS;
    uint8_t     err_count    = 0;
    uint8_t     rxChar       = 0;
    uint32_t    rxCount      = 0;
    uint32_t    remainTimeMs = timeoutMilliseconds;
    uint32_t    wait_timems  = AWS_CELLULAR_DEFAULT_RECV_WAIT;

    st_aws_cellular_timeout_ctrl_t timeout;

    timeout.over_flg    = 0;
    timeout.timeout_flg = 0;
    timeout.start_time  = pdMS_TO_TICKS(xTaskGetTickCount());
    timeout.end_time    = timeout.start_time + timeoutMilliseconds;

    if (timeout.end_time < timeout.start_time)
    {
        timeout.over_flg = 1;
    }

    if ((NULL == p_aws_cellular_uart_ctrl) || (NULL == pBuffer) ||
            (NULL == pDataReceivedLength) || (0 == bufferLength))
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if (false == p_aws_cellular_uart_ctrl->if_open)
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        /* Set this flag to inform interrupt handler to stop callling callback function. */
        p_aws_cellular_uart_ctrl->rx_fifo_reading_flg = 1U;

        ( void ) xEventGroupClearBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                        AWS_CELLUALR_COMM_EVT_RX_DATA |
                                        AWS_CELLUALR_COMM_EVT_RX_ERROR |
                                        AWS_CELLUALR_COMM_EVT_RX_ABORTED);

        /* WAIT_LOOP */
        while (rxCount < bufferLength)
        {
            sci_ret = R_SCI_Receive(p_aws_cellular_uart_ctrl->sci_hdl, &rxChar, 1);
            if (SCI_SUCCESS == sci_ret)
            {
                pBuffer[rxCount] = rxChar;
                rxCount++;

                if (((' ' == rxChar) && ('>' == pBuffer[rxCount - 2])) ||
                        ((rxCount > 2) && ('\n' == rxChar)))
                {
                    break;
                }
            }
            else
            {
                err_count++;
                if (err_count >= AWS_CELLULAR_RECV_LIMIT)
                {
                    break;
                }

                if (rxCount > 0)
                {
                    wait_timems = remainTimeMs;
                }
                else
                {
                    wait_timems = AWS_CELLULAR_DEFAULT_RECV_WAIT;
                }

                uxBits = xEventGroupWaitBits(p_aws_cellular_uart_ctrl->p_event_hdl,
                                                AWS_CELLUALR_COMM_EVT_RX_DATA |
                                                AWS_CELLUALR_COMM_EVT_RX_ERROR |
                                                AWS_CELLUALR_COMM_EVT_RX_ABORTED,
                                                pdTRUE,
                                                pdFALSE,
                                                pdMS_TO_TICKS(wait_timems));  //cast

                if (0U != (uxBits & (AWS_CELLUALR_COMM_EVT_RX_ERROR | AWS_CELLUALR_COMM_EVT_RX_ABORTED)))
                {
                    ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                    break;
                }

                remainTimeMs = aws_cellular_timeout_reconfig(&timeout);
                if (1 == timeout.timeout_flg)
                {
                    ret = IOT_COMM_INTERFACE_TIMEOUT;
                    break;
                }
            }
        }

        /* Clear this flag to inform interrupt handler to call callback function. */
        p_aws_cellular_uart_ctrl->rx_fifo_reading_flg = 0U;

        *pDataReceivedLength = rxCount;

        /* Return success if bytes received. Even if timeout or RX error. */
        if (rxCount > 0)
        {
            ret = IOT_COMM_INTERFACE_SUCCESS;
        }
    }

    return ret;
}
#endif /* defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) */
/***********************************************************************
 * End of function aws_cellular_recv
 **********************************************************************/

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
/************************************************************************
 * Function Name  @fn            aws_cellular_timeout_reconfig
 ***********************************************************************/
static uint32_t aws_cellular_timeout_reconfig(st_aws_cellular_timeout_ctrl_t * const p_timeout)
{
    uint32_t remain_time = 0;
    uint32_t this_time   = pdMS_TO_TICKS(xTaskGetTickCount());

    if (0 == p_timeout->over_flg)
    {
        if ((this_time >= p_timeout->end_time) || (this_time < p_timeout->start_time))
        {
            p_timeout->timeout_flg = 1;
        }
        else
        {
            remain_time = p_timeout->end_time - this_time;
        }
    }
    else
    {
        if ((this_time < p_timeout->start_time) && (this_time >= p_timeout->end_time))
        {
            p_timeout->timeout_flg = 1;
        }
        else
        {
            if (this_time < p_timeout->start_time)
            {
                remain_time = p_timeout->end_time - this_time;
            }
            else
            {
                remain_time = p_timeout->end_time + (portMAX_DELAY - this_time);
            }
        }
    }

    return remain_time;
}
/***********************************************************************
 * End of function aws_cellular_timeout_reconfig
 **********************************************************************/

#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
/************************************************************************
 * Function Name  @fn            aws_cellular_wait_bits
 ***********************************************************************/
static CellularCommInterfaceError_t aws_cellular_wait_bits(EventGroupHandle_t hdl, uint32_t timeout)
{
    EventBits_t                  bits;
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;

    bits = xEventGroupWaitBits(hdl,
                              AWS_CELLUALR_COMM_EVT_TX_DONE |
                              AWS_CELLUALR_COMM_EVT_TX_ERROR |
                              AWS_CELLUALR_COMM_EVT_TX_ABORTED,
                              pdTRUE,
                              pdFALSE,
                              pdMS_TO_TICKS(timeout));  //cast

    if (0U != (bits & (AWS_CELLUALR_COMM_EVT_TX_ERROR | AWS_CELLUALR_COMM_EVT_TX_ABORTED)))
    {
        ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
    }
    else
    {
        if (0 == (bits & AWS_CELLUALR_COMM_EVT_TX_DONE))
        {
            ret = IOT_COMM_INTERFACE_TIMEOUT;
        }
    }

    return ret;
}
/***********************************************************************
 * End of function aws_cellular_wait_bits
 **********************************************************************/
#endif /* AWS_CELLULAR_CFG_CTS_SW_CTRL == 1 */
#endif /* defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__) */
