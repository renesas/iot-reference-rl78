/*
 * r_sci_rl.c
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

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"
#include "r_sci_rl_private.h"
#include "r_sci_rl_platform.h"

#if (SCI_CFG_ASYNC_INCLUDED == 1)
#include "r_byteq_if.h"
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define UART_RTS_ENABLE     (SCI_CFG_UART0_FLOW_CTRL_ENABLE || SCI_CFG_UART1_FLOW_CTRL_ENABLE ||  \
                             SCI_CFG_UART2_FLOW_CTRL_ENABLE || SCI_CFG_UART3_FLOW_CTRL_ENABLE)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
#if (SCI_CFG_ASYNC_INCLUDED == 1)
static sci_err_t sci_init_async (sci_hdl_t const hdl, sci_uart_t * const p_cfg, uint8_t * const p_priority);
static sci_err_t sci_init_queues (uint8_t const  chan);
static sci_err_t sci_send_async_data (sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length);
static byteq_err_t sci_put_byte (sci_hdl_t const hdl, uint8_t const byte);
static void sci_transfer (sci_hdl_t const hdl);
static sci_err_t sci_receive_async_data (sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length);
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

static void power_on (uint8_t channel);
static void power_off (uint8_t channel);
static void sci_receive (sci_hdl_t const hdl);
static void sci_error (sci_hdl_t const hdl);

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED == 1)
#if (SCI_CFG_CH0_INCLUDED == 1)
static uint8_t  ch0_tx_buf[SCI_CFG_CH0_TX_BUFSIZ];
static uint8_t  ch0_rx_buf[SCI_CFG_CH0_RX_BUFSIZ];
#endif /* (SCI_CFG_CH0_INCLUDED == 1) */

#if (SCI_CFG_CH1_INCLUDED == 1)
static uint8_t  ch1_tx_buf[SCI_CFG_CH1_TX_BUFSIZ];
static uint8_t  ch1_rx_buf[SCI_CFG_CH1_RX_BUFSIZ];
#endif /* (SCI_CFG_CH1_INCLUDED == 1) */

#if (SCI_CFG_CH2_INCLUDED == 1)
static uint8_t  ch2_tx_buf[SCI_CFG_CH2_TX_BUFSIZ];
static uint8_t  ch2_rx_buf[SCI_CFG_CH2_RX_BUFSIZ];
#endif /* (SCI_CFG_CH2_INCLUDED == 1) */

#if (SCI_CFG_CH3_INCLUDED == 1)
static uint8_t  ch3_tx_buf[SCI_CFG_CH3_TX_BUFSIZ];
static uint8_t  ch3_rx_buf[SCI_CFG_CH3_RX_BUFSIZ];
#endif /* (SCI_CFG_CH3_INCLUDED == 1) */
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

extern const sci_hdl_t SCI_FAR g_handles[SCI_NUM_CH];

/**********************************************************************************************************************
 * Function Name: R_SCI_Open
 * Description  : Initializes an SCI channel for a particular mode.
 *
 * Arguments    : chan       channel to initialize
 *                mode       operational mode (UART, SPI, I2C, ...)
 *                p_cfg      ptr to configuration union; structure specific to mode
 *                p_callback ptr to function called from interrupt when a receiver error is detected or for transmit end (TEI) condition
 *                p_hdl      pointer to a handle for channel (value set here)
 * Return Value : SCI_SUCCESS
 *                SCI_ERR_BAD_CHAN
 *                SCI_ERR_OMITTED_CHAN
 *                SCI_ERR_CH_NOT_CLOSED
 *                SCI_ERR_BAD_MODE
 *                SCI_ERR_NULL_PTR
 *                SCI_ERR_INVALID_ARG
 *                SCI_ERR_QUEUE_UNAVAILABLE
 *********************************************************************************************************************/
sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     sci_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl)
{
    sci_err_t   err = SCI_SUCCESS;
    uint8_t     priority = 1;

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    /* Check arguments */
    err = sci_mcu_param_check(chan);
    if (SCI_SUCCESS != err)
    {
        return err;
    }

    /* Check argument g_handles */
    if (NULL == g_handles[chan])
    {
        return SCI_ERR_OMITTED_CHAN;
    }
    if (SCI_MODE_OFF != g_handles[chan]->mode)
    {
        return SCI_ERR_CH_NOT_CLOSED;
    }
    if ((SCI_MODE_OFF == mode) || (SCI_MODE_MAX <= mode))
    {
        return SCI_ERR_BAD_MODE;
    }

    /* Check argument p_cfg, p_hdl */
    if ((NULL == p_cfg) || (NULL == p_hdl))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */
    
    /* Apply power to channel */
    power_on(chan);

    /* Initialize registers */
    sci_init_register(g_handles[chan]);

    /* Initialize mode specific features */
    g_handles[chan]->mode = mode;
#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == mode)
    {
        /* Casting sci_cfg_t type to sci_uart_t type is valid */
        err = sci_init_async(g_handles[chan], (sci_uart_t *)p_cfg, &priority);
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

    if (SCI_SUCCESS != err)
    {
        g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }
    g_handles[chan]->callback = p_callback;

    /* Initialize TX and RX queues */
#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == mode)
    {
        err = sci_init_queues(chan);
        if (SCI_SUCCESS != err)
        {
            g_handles[chan]->mode = SCI_MODE_OFF;
            return err;
        }
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

    /* Enable interrupts */
    sci_initialize_ints(g_handles[chan], priority);

    /* Finish */
    *p_hdl = g_handles[chan];

    return SCI_SUCCESS;
}

/**********************************************************************************************************************
 * Function Name: power_on
 * Description  : This function provides power to the channel referenced by
 *                the handle by taking it out of the module stop state.
 * Arguments    : hdl - handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
static void power_on(uint8_t channel)
{
    switch (channel)
    {
#if (SCI_CFG_CH0_INCLUDED)
        case 0:
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE)
            SCI_CFG_UART0_RTS_PORT = 0;
#endif
            R_Config_UART0_Start();
            break;
#endif /* (SCI_CFG_CH0_INCLUDED) */

#if (SCI_CFG_CH1_INCLUDED)
        case 1:
#if (SCI_CFG_UART1_FLOW_CTRL_ENABLE)
            SCI_CFG_UART1_RTS_PORT = 0;
#endif
            R_Config_UART1_Start();
            break;
#endif /* (SCI_CFG_CH1_INCLUDED) */

#if (SCI_CFG_CH2_INCLUDED)
        case 2:
#if (SCI_CFG_UART2_FLOW_CTRL_ENABLE)
            SCI_CFG_UART2_RTS_PORT = 0;
#endif
            R_Config_UART2_Start();
            break;
#endif /* (SCI_CFG_CH2_INCLUDED) */

#if (SCI_CFG_CH3_INCLUDED)
        case 3:
#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE)
            SCI_CFG_UART3_RTS_PORT = 0;
#endif
            R_Config_UART3_Start();
            break;
#endif /* (SCI_CFG_CH3_INCLUDED) */

        default:
            break;
    }

    return;
}

/**********************************************************************************************************************
 * Function Name: power_off
 * Description  : This function removes power to the channel referenced by handle.
 * Arguments    : hdl - handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
static void power_off(uint8_t channel)
{
    switch(channel)
    {
#if (SCI_CFG_CH0_INCLUDED)
        case 0:
            R_Config_UART0_Stop();
            break;
#endif

#if (SCI_CFG_CH1_INCLUDED)
        case 1:
            R_Config_UART1_Stop();
            break;
#endif

#if (SCI_CFG_CH2_INCLUDED)
        case 2:
            R_Config_UART2_Stop();
            break;
#endif

#if (SCI_CFG_CH3_INCLUDED)
        case 3:
            R_Config_UART3_Stop();
            break;
#endif

        default:
            break;
    }
    return;
}

#if (SCI_CFG_ASYNC_INCLUDED == 1)
/**********************************************************************************************************************
 * Function Name: sci_init_queues
 * Description  : This function attaches transmit and receive queues to the channel.
 *
 * Arguments    : chan - channel (ptr to chan control block)
 * Return Value : SCI_SUCCESS -
 *                SCI_ERR_QUEUE_UNAVAILABLE -
 *********************************************************************************************************************/
static sci_err_t sci_init_queues(uint8_t const chan)
{
    byteq_err_t q_err1 = BYTEQ_ERR_INVALID_ARG;
    byteq_err_t q_err2 = BYTEQ_ERR_INVALID_ARG;
    sci_err_t   err = SCI_SUCCESS;

    /* channel number verified as legal prior to calling this function */
    switch (chan)
    {
#if SCI_CFG_CH0_INCLUDED
        case (SCI_CH0):
            q_err1 = R_BYTEQ_Open(ch0_tx_buf, SCI_CFG_CH0_TX_BUFSIZ, &g_handles[SCI_CH0]->u_tx_data.que);
            q_err2 = R_BYTEQ_Open(ch0_rx_buf, SCI_CFG_CH0_RX_BUFSIZ, &g_handles[SCI_CH0]->u_rx_data.que);
            break;
#endif

#if SCI_CFG_CH1_INCLUDED
        case (SCI_CH1):
            q_err1 = R_BYTEQ_Open(ch1_tx_buf, SCI_CFG_CH1_TX_BUFSIZ, &g_handles[SCI_CH1]->u_tx_data.que);
            q_err2 = R_BYTEQ_Open(ch1_rx_buf, SCI_CFG_CH1_RX_BUFSIZ, &g_handles[SCI_CH1]->u_rx_data.que);
            break;
#endif

#if SCI_CFG_CH2_INCLUDED
        case (SCI_CH2):
            q_err1 = R_BYTEQ_Open(ch2_tx_buf, SCI_CFG_CH2_TX_BUFSIZ, &g_handles[SCI_CH2]->u_tx_data.que);
            q_err2 = R_BYTEQ_Open(ch2_rx_buf, SCI_CFG_CH2_RX_BUFSIZ, &g_handles[SCI_CH2]->u_rx_data.que);
            break;
#endif

#if SCI_CFG_CH3_INCLUDED
        case (SCI_CH3):
            q_err1 = R_BYTEQ_Open(ch3_tx_buf, SCI_CFG_CH3_TX_BUFSIZ, &g_handles[SCI_CH3]->u_tx_data.que);
            q_err2 = R_BYTEQ_Open(ch3_rx_buf, SCI_CFG_CH3_RX_BUFSIZ, &g_handles[SCI_CH3]->u_rx_data.que);
            break;
#endif

        default:
            err = SCI_ERR_QUEUE_UNAVAILABLE;
            break;
    }

    if ((BYTEQ_SUCCESS != q_err1) || (BYTEQ_SUCCESS != q_err2))
    {
        err = SCI_ERR_QUEUE_UNAVAILABLE;
    }
    return err;
}

/**********************************************************************************************************************
 * Function Name: sci_init_async
 * Description  : This function initializes the control block and UART registers for an SCI channel.
 *
 * NOTE: p_cfg is checked to be non-NULL prior to this function.
 *       The TE and RE bits in SCR must be 0 prior to calling this function.
 *
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                p_cfg -
 *                    ptr to Uart configuration argument structure
 *                p_priority -
 *                    pointer to location to load interrupt priority into
 * Return Value : SCI_SUCCESS
 *                SCI_ERR_INVALID_ARG
 *********************************************************************************************************************/
static sci_err_t sci_init_async(sci_hdl_t const      hdl,
                                sci_uart_t * const   p_cfg,
                                uint8_t * const      p_priority)
{
    sci_err_t err = SCI_SUCCESS;

    /* Initialize channel control block flags */
    hdl->tx_idle = true;
    return err;
}
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

/**********************************************************************************************************************
 * Function Name: R_SCI_Send
 * Description  : This function is used to start transmitting data. For Async,
 *                the data is loaded into a queue and transmission started if
 *                not already in progress. For SSPI/Sync, the channel is
 *                checked to see if busy, and if not, transmission of the
 *                source buffer is started.
 *
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                p_src -
 *                    ptr to data to transmit
 *                length -
 *                    number of bytes to send
 * Return Value : SCI_SUCCESS -
 *                SCI_ERR_NULL_PTR -
 *                SCI_ERR_BAD_MODE -
 *                SCI_ERR_INSUFFICIENT_SPACE -
 *                SCI_ERR_XCVR_BUSY -
 *                SCI_ERR_INVALID_ARG
 *********************************************************************************************************************/
sci_err_t R_SCI_Send(sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length)
{
    sci_err_t err = SCI_SUCCESS;

    /* Check arguments */
#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((NULL == hdl) || (NULL == p_src))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */

#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        err = sci_send_async_data(hdl, p_src, length);
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

    return err;
}

/**********************************************************************************************************************
 * Function Name: sci_send_async_data
 * Description  : This function determines if the tx byte queue of the channel referenced by the handle is not full,
 *                and call the byte transmission function.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                p_src -
 *                    ptr to data to transmit
 *                length -
 *                    number of bytes to send and possibly receive
 * Return Value : SCI_SUCCESS
 *                SCI_ERR_XCVR_BUSY
 *                SCI_ERR_INSUFFICIENT_SPACE
 *********************************************************************************************************************/
static sci_err_t sci_send_async_data(sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length)
{
    sci_err_t   err = SCI_SUCCESS;
    uint16_t    cnt;
    byteq_err_t byteq_err;

    if (true != hdl->tx_idle)
    {
        return SCI_ERR_XCVR_BUSY;
    }

    /* How many bytes of free space in the TX queue. */
    R_BYTEQ_Unused(hdl->u_tx_data.que, &cnt);

    if (cnt < length)
    {
        /* If can't fit, return */
        return SCI_ERR_INSUFFICIENT_SPACE;
    }

    /* Else load bytes into tx queue for transmission */
    for (cnt = 0; cnt < length; cnt++)
    {
        R_BSP_InterruptsDisable();
        byteq_err = sci_put_byte(hdl, *p_src++);
        R_BSP_InterruptsEnable();

        /* If the return value is not BYTEQ_SUCCESS. */
        if (BYTEQ_SUCCESS != byteq_err)
        {
            err = SCI_ERR_INSUFFICIENT_SPACE;
            break;
        }
    }

    if (SCI_SUCCESS == err)
    {
        hdl->tx_idle = false;
        sci_transfer(hdl);
    }

    return err;
}

/**********************************************************************************************************************
 * Function Name: sci_put_byte
 * Description  : Transmits byte if channel is not busy. Otherwise, byte is stored in tx queue until can transmit.
 *                If buffer is full and cannot store it, an error code is returned.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                byte -
 *                    byte to transmit
 * Return Value : Same R_BYTEQ_Put()-
 *********************************************************************************************************************/
static byteq_err_t sci_put_byte(sci_hdl_t const hdl, uint8_t const byte)
{
    return R_BYTEQ_Put(hdl->u_tx_data.que, byte);
}

#if (SCI_CFG_ASYNC_INCLUDED == 1)
/**********************************************************************************************************************
 * Function Name: sci_transfer
 * Description  : Transfer for SCI
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
static void sci_transfer(sci_hdl_t const hdl)
{
    uint16_t    num;
    uint8_t     byte;

    R_BYTEQ_Used(hdl->u_tx_data.que, &num);
    if (0 >= num)
    {
        hdl->tx_idle = true;

#if (SCI_CFG_TEI_INCLUDED == 1)
        sci_cb_args_t arg;
        arg.event = SCI_EVT_TEI;

        if (NULL != hdl->callback)
        {
            arg.hdl = hdl;
            hdl->callback((void *)&arg);
        }
#endif /* (SCI_CFG_TEI_INCLUDED == 1) */
    }
    else
    {
        /* Get a byte from tx queue */
        (void)R_BYTEQ_Get(hdl->u_tx_data.que, (uint8_t *)&byte);

        /* TDR/FTDR register write access */
        SCI_TDR(byte);
    }
}

/**********************************************************************************************************************
 * Function Name: txi_handler
 * Description  : TXI interrupt handler for SCI
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
void txi_handler(sci_hdl_t const hdl)
{
    sci_transfer(hdl);
}
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

/**********************************************************************************************************************
 * Function Name: R_SCI_Receive
 * Description  : Gets data received on an SCI channel referenced by the handle from rx queue.
 *                Function does not block if the requested number of bytes is not available.
 *                If any errors occurred during reception by hardware, they are handled by the callback
 *                function specified in R_SCI_Open() and no corresponding error code is provided here.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                p_dst -
 *                    ptr to buffer to load data into
 *                length -
 *                    number of bytes to read
 * Return Value : SCI_SUCCESS
 *                SCI_ERR_NULL_PTR
 *                SCI_ERR_BAD_MODE
 *                SCI_ERR_INSUFFICIENT_DATA
 *                SCI_ERR_XCVR_BUSY
 *                SCI_ERR_INVALID_ARG
 *********************************************************************************************************************/
sci_err_t R_SCI_Receive(sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length)
{
    sci_err_t   err = SCI_SUCCESS;

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    /* Check arguments */
    if ((NULL == hdl) || (NULL == p_dst))
    {
        return SCI_ERR_NULL_PTR;
    }
    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        err = sci_receive_async_data(hdl, p_dst, length);
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */
    return err;
}

#if (SCI_CFG_ASYNC_INCLUDED == 1)
/**********************************************************************************************************************
 * Function Name: sci_receive_async_data
 * Description  : This function determines if the rx byte queue of the channel referenced by the handle,
 *                the requested number of bytes is available, and get the data from the rx byte queue.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                p_dst -
 *                    ptr to buffer to load data into
 *                length -
 *                    number of bytes to read
 * Return Value : SCI_SUCCESS -
 *                SCI_ERR_INSUFFICIENT_DATA -
 *********************************************************************************************************************/
static sci_err_t sci_receive_async_data(sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length)
{
    sci_err_t   err = SCI_SUCCESS;
    uint16_t    cnt;
    byteq_err_t byteq_err = BYTEQ_SUCCESS;

    /* CHECK FOR SUFFICIENT DATA IN QUEUE, AND FETCH IF AVAILABLE */
    R_BYTEQ_Used(hdl->u_rx_data.que, &cnt);

    if (cnt < length)
    {
        return SCI_ERR_INSUFFICIENT_DATA;
    }

    /* Get bytes from rx queue */
    /* WAIT_LOOP */
    for (cnt = 0; cnt < length; cnt++)
    {
        /* Disable RXI Interrupt */
        R_BSP_InterruptsDisable();
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
        R_BSP_InterruptsEnable();

        if (BYTEQ_SUCCESS != byteq_err)
        {
            err = SCI_ERR_INSUFFICIENT_DATA;
            break;
        }

        uint16_t byteq_free = 0;
        R_BYTEQ_Unused(hdl->u_rx_data.que, &byteq_free);
        if (SCI_CFG_UART_RX_BUFFER_THRESH < byteq_free)
        {
            switch(hdl->channel)
            {
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE)
                case 0:
                    SCI_CFG_UART0_RTS_PORT = 0;
                    break;
#endif

#if (SCI_CFG_UART1_FLOW_CTRL_ENABLE)
                case 1:
                    SCI_CFG_UART1_RTS_PORT = 0;
                    break;
#endif

#if (SCI_CFG_UART2_FLOW_CTRL_ENABLE)
                case 2:
                    SCI_CFG_UART2_RTS_PORT = 0;
                    break;
#endif

#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE)
                case 3:
                    SCI_CFG_UART3_RTS_PORT = 0;
                    break;
#endif
                default:
                    break;
            }
        }
    }
    return err;
}
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

/**********************************************************************************************************************
 * Function Name: sci_receive
 * Description  : Receive for SCI
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
static void sci_receive(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;

    /* Read byte */
    SCI_RDR(&byte);

#if defined(__CCRL__) || defined(__ICCRL78__)
#if (UART_RTS_ENABLE != 0)
    uint16_t byteq_free = 0;
    R_BYTEQ_Unused(hdl->u_rx_data.que, &byteq_free);
    if (SCI_CFG_UART_RX_BUFFER_THRESH >= byteq_free)
    {
        switch (hdl->channel)
        {
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE == 1)
            case 0:
                SCI_CFG_UART0_RTS_PORT = 1;
                break;
#endif /* (SCI_CFG_UART0_FLOW_CTRL_ENABLE == 1) */

#if (SCI_CFG_UART1_FLOW_CTRL_ENABLE == 1)
            case 1:
                SCI_CFG_UART1_RTS_PORT = 1;
                break;
#endif /* (SCI_CFG_UART1_FLOW_CTRL_ENABLE == 1) */

#if (SCI_CFG_UART2_FLOW_CTRL_ENABLE == 1)
            case 2:
                SCI_CFG_UART2_RTS_PORT = 1;
                break;
#endif /* (SCI_CFG_UART2_FLOW_CTRL_ENABLE == 1) */

#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE == 1)
            case 3:
                SCI_CFG_UART3_RTS_PORT = 1;
                break;
#endif /* (SCI_CFG_UART3_FLOW_CTRL_ENABLE == 1) */

            default:
                break;
        }
    }
#endif /* (SCI_RTS_ENABLE != 0) */
#endif /* defined__CCRL__ || defined__ICCRL78__ */

#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        /* Place byte in queue */
        if (R_BYTEQ_Put(hdl->u_rx_data.que, byte) == BYTEQ_SUCCESS)
        {
            args.event = SCI_EVT_RX_CHAR;
        }
        else
        {
            args.event = SCI_EVT_RXBUF_OVFL;
        }

        /* Post event to callback */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;
            hdl->callback((void *)&args);
        }
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */
}

/**********************************************************************************************************************
 * Function Name: rxi_handler
 * Description  : RXI interrupt handler for SCI
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
void rxi_handler(sci_hdl_t const hdl)
{
    sci_receive(hdl);
}

/**********************************************************************************************************************
 * Function Name: sci_error
 * Description  : Post SCI Error event to call back function.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
static void sci_error(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;
    uint8_t         reg;

    reg = SCI_SSR;
    if (0 != (reg & SCI_RCVR_ERR_MASK))
    {
        if (0 != (reg & SCI_SSR_ORER_MASK))
        {
            args.event = SCI_EVT_OVFL_ERR;
        }
#if (SCI_CFG_ASYNC_INCLUDED == 1)
        else if (0 != (reg & SCI_SSR_PER_MASK))
        {
            args.event = SCI_EVT_PARITY_ERR;
        }
        else if (0 != (reg & SCI_SSR_FER_MASK))
        {
            args.event = SCI_EVT_FRAMING_ERR;
        }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */
        else
        {
            /* Do Nothing */
        }

        /* Flush register */
        SCI_RDR(&byte);

        /* Post event to callback */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;
            hdl->callback((void *)&args);
        }
    }
}

/**********************************************************************************************************************
 * Function Name: eri_handler
 * Description  : ERI interrupt handler for SCI UART mode
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : none
 *********************************************************************************************************************/
void eri_handler(sci_hdl_t const hdl)
{
    sci_error(hdl);
}

/**********************************************************************************************************************
 * Function Name: R_SCI_Control
 * Description  : This function configures non-standard UART hardware and performs special software operations.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 *                cmd -
 *                    command to run
 *                p_args -
 *                    pointer argument(s) specific to command
 * Return Value : SCI_SUCCESS -
 *                SCI_ERR_NULL_PTR
 *                SCI_ERR_BAD_MODE
 *                SCI_ERR_INVALID_ARG
 *********************************************************************************************************************/
sci_err_t R_SCI_Control(sci_hdl_t const hdl, sci_cmd_t const cmd, void *p_args)
{
    sci_err_t   err = SCI_SUCCESS;

    /* Check parameters */
#if (SCI_CFG_PARAM_CHECKING_ENABLE ==1)
    if (NULL == hdl)
    {
        return SCI_ERR_NULL_PTR;
    }

    /* Check argument p_args*/
    if (NULL == p_args)
    {
        if (SCI_CMD_CHANGE_BAUD == cmd)
        {
            return SCI_ERR_NULL_PTR;
        }
    }

    if ((SCI_MODE_OFF == hdl->mode) || (SCI_MODE_MAX <= hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE ==1) */
    
    switch (cmd)
    {
        case (SCI_CMD_CHANGE_BAUD):
        case (SCI_CMD_EN_CTS_IN):
            /* Not supported. */
            break;
        default:
#if (SCI_CFG_ASYNC_INCLUDED == 1)
            /* Async-specific commands */
            if (SCI_MODE_ASYNC == hdl->mode)
            {
                err = sci_async_cmds(hdl, cmd, p_args);
            }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */
            break;
    }
    return err;
}

/**********************************************************************************************************************
 * Function Name: R_SCI_Close
 * Description  : Disables the SCI channel designated by the handle.
 * Arguments    : hdl -
 *                    handle for channel (ptr to chan control block)
 * Return Value : SCI_SUCCESS -
 *                SCI_ERR_NULL_PTR -
 *********************************************************************************************************************/
sci_err_t R_SCI_Close(sci_hdl_t const hdl)
{
#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    /* Check argument */
    if (NULL == hdl)
    {
        return SCI_ERR_NULL_PTR;
    }
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */

    /* Disable interrupts */
    sci_disable_ints(hdl);

#if (SCI_CFG_ASYNC_INCLUDED == 1)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        /* Close BYTEQ */
        R_BYTEQ_Close(hdl->u_tx_data.que);
        R_BYTEQ_Close(hdl->u_rx_data.que);
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

    /* mark the channel as not in use and power down */
    hdl->mode = SCI_MODE_OFF;
    power_off(hdl->channel);

    return SCI_SUCCESS;
}

/**********************************************************************************************************************
 * Function Name: R_SCI_GetVersion
 * Description  : Returns the version of this module.
 * Arguments    : none
 * Return Value : version number
 *********************************************************************************************************************/
uint32_t  R_SCI_GetVersion(void)
{
    uint32_t const version = ((uint32_t)SCI_VERSION_MAJOR << 16) | SCI_VERSION_MINOR;
    return version;
}
