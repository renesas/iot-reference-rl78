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
 * File Name    : r_sci_rl.c
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
#include "r_sci_rl_private.h"
#include "r_sci_rl_platform.h"

#if (SCI_CFG_ASYNC_INCLUDED)
#include "r_byteq_if.h"
#endif

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
#if (SCI_CFG_ASYNC_INCLUDED)
static sci_err_t sci_init_async (sci_hdl_t const hdl, sci_uart_t * const p_cfg, uint8_t * const p_priority);
static sci_err_t sci_init_queues (uint8_t const  chan);
static sci_err_t sci_send_async_data (sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length);
static byteq_err_t sci_put_byte (sci_hdl_t const hdl, uint8_t const byte);
static void sci_transfer (sci_hdl_t const hdl);
static sci_err_t sci_receive_async_data (sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length);
static sci_err_t sci_receive_async_data3 (sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length);
#endif

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
static sci_err_t sci_init_sync (sci_hdl_t const hdl, sci_sync_sspi_t * const p_cfg, uint8_t * const p_priority);
static sci_err_t sci_send_sync_data (sci_hdl_t const hdl, uint8_t *p_src, uint8_t *p_dst, uint16_t const length, bool save_rx_data);
static sci_err_t sci_receive_sync_data (sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length);
#endif

static void power_on (uint8_t channel);
static void power_off (uint8_t channel);
static void sci_receive (sci_hdl_t const hdl);
static void sci_receive3 (sci_hdl_t const hdl);
static void sci_error (sci_hdl_t const hdl);

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
#if SCI_CFG_CH0_INCLUDED
static uint8_t  ch0_tx_buf[SCI_CFG_CH0_TX_BUFSIZ];
static uint8_t  ch0_rx_buf[SCI_CFG_CH0_RX_BUFSIZ];
#endif

#if SCI_CFG_CH1_INCLUDED
static uint8_t  ch1_tx_buf[SCI_CFG_CH1_TX_BUFSIZ];
static uint8_t  ch1_rx_buf[SCI_CFG_CH1_RX_BUFSIZ];
#endif

#if SCI_CFG_CH2_INCLUDED
static uint8_t  ch2_tx_buf[SCI_CFG_CH2_TX_BUFSIZ];
static uint8_t  ch2_rx_buf[SCI_CFG_CH2_RX_BUFSIZ];
#endif

#if SCI_CFG_CH3_INCLUDED
static uint8_t  ch3_tx_buf[SCI_CFG_CH3_TX_BUFSIZ];
static uint8_t  ch3_rx_buf[SCI_CFG_CH3_RX_BUFSIZ];
#endif
#endif /* #if (SCI_CFG_ASYNC_INCLUDED) */

extern const sci_hdl_t SCI_FAR g_handles[SCI_NUM_CH];

/*****************************************************************************
* Function Name: R_SCI_Open
* Description  : Initializes an SCI channel for a particular mode.
*
* NOTE: The associated port must be configured/initialized prior to
*       calling this function.
*
* Arguments    : chan -
*                    channel to initialize
*                mode -
*                    operational mode (UART, SPI, I2C, ...)
*                p_cfg -
*                    ptr to configuration union; structure specific to mode
*                p_callback -
*                    ptr to function called from interrupt when a receiver 
*                    error is detected or for transmit end (TEI) condition
*                p_hdl -
*                     pointer to a handle for channel (value set here)
* Return Value : SCI_SUCCESS -
*                    channel opened successfully
*                SCI_ERR_BAD_CHAN -
*                    channel number invalid for part
*                SCI_ERR_OMITTED_CHAN -
*                    channel not included in config.h
*                SCI_ERR_CH_NOT_CLOSED -
*                    channel already in use
*                SCI_ERR_BAD_MODE -
*                    unsupported mode
*                SCI_ERR_NULL_PTR -
*                    missing required p_cfg argument
*                SCI_ERR_INVALID_ARG -
*                    element of casted mode config structure (p_cfg) is invalid
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    cannot open transmit or receive queue or both
******************************************************************************/
sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     sci_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl)
{
    sci_err_t   err = SCI_SUCCESS;
    uint8_t     priority = 1;

    /* CHECK ARGUMENTS */
#if SCI_CFG_PARAM_CHECKING_ENABLE
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
#endif
    
    /* APPLY POWER TO CHANNEL */
    power_on(chan);

    /* INITIALIZE REGISTER */
    sci_init_register(g_handles[chan]);

    /* INITIALIZE MODE SPECIFIC FEATURES */
    g_handles[chan]->mode = mode;
    if (SCI_MODE_ASYNC == mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        /* Casting sci_cfg_t type to sci_uart_t type is valid */
        err = sci_init_async(g_handles[chan], (sci_uart_t *)p_cfg, &priority);
#endif
    }
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    else
    {
        /* Casting sci_cfg_t type to sci_sync_sspi_t type is valid */
        err = sci_init_sync(g_handles[chan], (sci_sync_sspi_t *)p_cfg, &priority);
    }
#endif

    if (SCI_SUCCESS != err)
    {
        g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }
    g_handles[chan]->callback = p_callback;

    /* INITIALIZE TX AND RX QUEUES */
#if (SCI_CFG_ASYNC_INCLUDED)
    if (SCI_MODE_ASYNC == mode)
    {
        err = sci_init_queues(chan);
        if (SCI_SUCCESS != err)
        {
            g_handles[chan]->mode = SCI_MODE_OFF;
            return err;
        }
    }
#endif

    /* ENABLE INTERRUPTS */
    sci_initialize_ints(g_handles[chan], priority);

    /* FINISH */
    *p_hdl = g_handles[chan];

    return SCI_SUCCESS;
}  /* End of function R_SCI_Open() */

/*****************************************************************************
* Function Name: power_on
* Description  : This function provides power to the channel referenced by
*                the handle by taking it out of the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_on(uint8_t channel)
{
    switch(channel)
    {
#if SCI_CFG_CH0_INCLUDED
        case 0:
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE)
            SCI_CFG_UART0_RTS_PORT = 0;
#endif
            R_Config_UART0_Start();
            break;
#endif
#if SCI_CFG_CH1_INCLUDED
        case 1:
#if (SCI_CFG_UART1_FLOW_CTRL_ENABLE)
            SCI_CFG_UART1_RTS_PORT = 0;
#endif
            R_Config_UART1_Start();
            break;
#endif
#if SCI_CFG_CH2_INCLUDED
        case 2:
#if (SCI_CFG_UART2_FLOW_CTRL_ENABLE)
            SCI_CFG_UART2_RTS_PORT = 0;
#endif
            R_Config_UART2_Start();
            break;
#endif
#if SCI_CFG_CH3_INCLUDED
        case 3:
#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE)
            SCI_CFG_UART3_RTS_PORT = 0;
#endif
            R_Config_UART3_Start();
            break;
#endif
        default:
            break;
    }

    return;
}  /* End of function power_on() */

/*****************************************************************************
* Function Name: power_off
* Description  : This function removes power to the channel referenced by
*                handle by putting it into the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_off(uint8_t channel)
{
    switch(channel)
    {
#if SCI_CFG_CH0_INCLUDED
        case 0:
            R_Config_UART0_Stop();
            break;
#endif
#if SCI_CFG_CH1_INCLUDED
        case 1:
            R_Config_UART1_Stop();
            break;
#endif
#if SCI_CFG_CH2_INCLUDED
        case 2:
            R_Config_UART2_Stop();
            break;
#endif
#if SCI_CFG_CH3_INCLUDED
        case 3:
            R_Config_UART3_Stop();
            break;
#endif
        default:
            break;
    }
    return;
}  /* End of function power_off() */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_queues
* Description  : This function attaches transmit and receive queues to the
*                channel.
*
* Arguments    : chan -
*                    channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    no queue control blocks available
******************************************************************************/
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
}  /* End of function sci_init_queues() */
#endif /* End of SCI_CFG_ASYNC_INCLUDED */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_async
* Description  : This function initializes the control block and UART 
*                registers for an SCI channel.
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
* Return Value : SCI_SUCCESS - 
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
static sci_err_t sci_init_async(sci_hdl_t const      hdl,
                                sci_uart_t * const   p_cfg,
                                uint8_t * const      p_priority)
{
    sci_err_t   err=SCI_SUCCESS;

    /* Initialize channel control block flags */
    hdl->tx_idle = true;

    if (SCI_CH3 == hdl->channel)
    {
//        R_Config_UART3_Reset( p_cfg->baud_rate );
    }

    return err;
}  /* End of function sci_init_async() */

#endif /* End of SCI_CFG_ASYNC_INCLUDED */

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_init_sync
* Description  : This function initializes the control block and SYNC/SSPI
*                registers for an SCI channel.
*
* NOTE: p_cfg is checked to be non-NULL prior to this function.
*       The TE and RE bits in SCR must be 0 prior to calling this function.
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_cfg -
*                    ptr to SSPI configuration argument structure
*                p_priority -
*                    pointer to location to load interrupt priority into
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
static sci_err_t sci_init_sync(sci_hdl_t const         hdl,
                                sci_sync_sspi_t * const p_cfg,
                                uint8_t * const         p_priority)
{
    sci_err_t   err = SCI_SUCCESS;
    int32_t     bit_err;


    /* Check arguments */

#if SCI_CFG_PARAM_CHECKING_ENABLE
    if ((SCI_MODE_SSPI == hdl->mode)
            && (SCI_SPI_MODE_0 != p_cfg->spi_mode) && (SCI_SPI_MODE_1 != p_cfg->spi_mode)
            && (SCI_SPI_MODE_2 != p_cfg->spi_mode) && (SCI_SPI_MODE_3 != p_cfg->spi_mode))
    {
        return SCI_ERR_INVALID_ARG;
    }
    else if ((SCI_MODE_SYNC == hdl->mode) && (SCI_SPI_MODE_OFF != p_cfg->spi_mode))
    {
        return SCI_ERR_INVALID_ARG;
    }
    else
    {
        /* Do Nothing */
    }

    if (0 == p_cfg->bit_rate)
    {
        return SCI_ERR_INVALID_ARG;
    }

    if ((0 == p_cfg->int_priority) || (p_cfg->int_priority > BSP_MCU_IPL_MAX))
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    /* Initialize channel control block flags */
    hdl->tx_idle = true;
    hdl->tx_dummy = false;

    /* Configure SMR for SSPI/SYNC mode */
    hdl->rom->regs->SMR.BYTE = 0x80;
    hdl->rom->regs->SCMR.BIT.SMIF = 0;          /* default */
    hdl->rom->regs->SIMR1.BIT.IICM = 0;         /* default */

    /* Configure SPI register for clock polarity/phase and single master */
    if (SCI_MODE_SSPI == hdl->mode)
    {
        hdl->rom->regs->SPMR.BYTE = p_cfg->spi_mode;
    }
    else    /* synchronous operation */
    {
        hdl->rom->regs->SPMR.BYTE = 0;
    }

    /* Configure data inversion */
    hdl->rom->regs->SCMR.BIT.SINV = (uint8_t)((true == p_cfg->invert_data) ? 1 : 0);

    /* Configure bit order */
    hdl->rom->regs->SCMR.BIT.SDIR = (uint8_t)((true == p_cfg->msb_first) ? 1 : 0);


    /* SETUP CLOCK FOR BIT RATE */

    /* Use internal clock for bit rate (master) */
    bit_err = sci_init_bit_rate(hdl, hdl->pclk_speed, p_cfg->bit_rate);
    if (1000 == bit_err)
    {
        err = SCI_ERR_INVALID_ARG;      /* impossible bit rate; 100% error */
    }

    *p_priority = p_cfg->int_priority;
    return err;
} /* End of function sci_init_sync() */
#endif /* End of SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED */


/*****************************************************************************
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
*                    requested number of bytes sent/loaded into tx queue
*                SCI_ERR_NULL_PTR -
*                    hdl or p_src is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data (Async)
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_Send(sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length)
{
    sci_err_t err = SCI_SUCCESS;

    /* Check arguments */
#if SCI_CFG_PARAM_CHECKING_ENABLE
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
#endif

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        err = sci_send_async_data(hdl, p_src, length);
#endif
    }
    else
    {
        /* SSPI or SYNC */
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
        err = sci_send_sync_data(hdl, p_src, NULL, length, false);
#endif
    }

    return err;
} /* End of function R_SCI_Send() */


/*****************************************************************************
* Function Name: sci_send_async_data
* Description  : This function determines if the tx byte queue of the channel
*                referenced by the handle is not full, and call the byte
*                transmission function.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                length -
*                    number of bytes to send and possibly receive
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data (Async)
******************************************************************************/
static sci_err_t sci_send_async_data(sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length)
{
    sci_err_t   err = SCI_SUCCESS;
    uint16_t    cnt;
    byteq_err_t byteq_err = BYTEQ_ERR_QUEUE_FULL;

    if (true != hdl->tx_idle)
    {
        return SCI_ERR_XCVR_BUSY;
    }

    /* Determine amount of space left in tx queue */
    R_BYTEQ_Unused(hdl->u_tx_data.que, &cnt);

    if (cnt < length)
    {
        /* If can't fit, return */
        return SCI_ERR_INSUFFICIENT_SPACE;
    }

    /* Else load bytes into tx queue for transmission */
    /* WAIT_LOOP */
    for (cnt = 0; cnt < length; cnt++)
    {
#if (SCI_CFG_USE_CIRCULAR_BUFFER == 1)
        byteq_err = sci_put_byte(hdl, *p_src++);

        /* Allow TX interrupt occur */
        ENABLE_TXI_INT;
#else
        R_BSP_InterruptsDisable();      //RYZ014
        byteq_err = sci_put_byte(hdl, *p_src++);
        R_BSP_InterruptsEnable();      //RYZ014
#endif
        if (BYTEQ_SUCCESS != byteq_err)
        {
            /* If the return value is not BYTEQ_SUCCESS. */
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
} /* End of function sci_send_async_data() */

/*****************************************************************************
* Function Name: sci_put_byte
* Description  : Transmits byte if channel is not busy. Otherwise, byte is
*                stored in tx queue until can transmit. If buffer is full
*                and cannot store it, an error code is returned.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                byte -
*                    byte to transmit
* Return Value : Same R_BYTEQ_Put()-
******************************************************************************/
static byteq_err_t sci_put_byte(sci_hdl_t const hdl, uint8_t const byte)
{
    return R_BYTEQ_Put(hdl->u_tx_data.que, byte);
} /* End of function sci_put_byte() */

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_send_sync_data
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and begins the data transfer process
*                (both sending and receiving data).
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                p_dst -
*                    ptr to buffer to store receive data (optional)
*                length -
*                    number of bytes to send and possibly receive
*                save_rx_data -
*                    true if data clocked in should be saved to p_dst.
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
******************************************************************************/
static sci_err_t sci_send_sync_data(sci_hdl_t const hdl, uint8_t *p_src, uint8_t *p_dst,
                                    uint16_t const length, bool save_rx_data)
{
#if SCI_CFG_FIFO_INCLUDED
    uint8_t cnt;
    uint8_t thresh_cnt;
#endif

    if (true == hdl->tx_idle)
    {
        if (true == save_rx_data)
        {
            hdl->u_rx_data.buf = p_dst;
        }
        hdl->save_rx_data  = save_rx_data;

        hdl->u_tx_data.buf = p_src;
        hdl->tx_cnt        = length;
        hdl->rx_cnt        = length;
        hdl->tx_idle       = false;
        hdl->tx_dummy      = false;
        hdl->tx_cnt--;
        SCI_TDR(*hdl->u_tx_data.buf++);    /* start transmit */
        return SCI_SUCCESS;
    }

    return SCI_ERR_XCVR_BUSY;
} /* End of function sci_send_sync_data() */
#endif /* SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED */

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: R_SCI_SendReceive
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and begins the data transfer process
*                (both sending and receiving data).
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                p_dst -
*                    ptr to buffer to store received data
*                length -
*                    number of bytes to send/receive
* Return Value : SCI_SUCCESS -
*                    data transfer started
*                SCI_ERR_NULL_PTR -
*                    hdl, p_src or p_dst is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_SendReceive(sci_hdl_t const hdl,
                            uint8_t         *p_src,
                            uint8_t         *p_dst,
                            uint16_t const  length)
{
    sci_err_t   err;

#if SCI_CFG_PARAM_CHECKING_ENABLE
    /* Check arguments */
    if ((((NULL == hdl)   || (FIT_NO_PTR == hdl))    /* Check if hdl is available or not   */
    ||   ((NULL == p_src) || (FIT_NO_PTR == p_src))) /* Check if p_src is available or not */
    ||   ((NULL == p_dst) || (FIT_NO_PTR == p_dst))) /* Check if p_dst is available or not */
    {
        return SCI_ERR_NULL_PTR;
    }

    if ((SCI_MODE_SSPI != hdl->mode) && (SCI_MODE_SYNC != hdl->mode))
    {
        return SCI_ERR_BAD_MODE;
    }

    if (0 == length)
    {
        return SCI_ERR_INVALID_ARG;
    }
#endif

    err = sci_send_sync_data(hdl, p_src, p_dst, length, true);

    return err;
} /* End of function R_SCI_SendReceive() */
#endif /* End of SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED */

#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_transfer
* Description  : Transfer for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_transfer(sci_hdl_t const hdl)
{
    uint16_t    num;
    uint8_t     byte;

    R_BYTEQ_Used(hdl->u_tx_data.que, &num);
    if (0 >= num)
    {
        hdl->tx_idle = true;    // set flag if queue empty

#if SCI_CFG_TEI_INCLUDED
        sci_cb_args_t arg;
        arg.event = SCI_EVT_TEI;

        if (NULL != hdl->callback)
        {
            arg.hdl = hdl;
            hdl->callback((void *)&arg);
        }
#endif
    }
    else
    {
        /* Get bytes from tx queue */
        (void)R_BYTEQ_Get(hdl->u_tx_data.que, (uint8_t *)&byte);

        /* TDR/FTDR register write access */
        SCI_TDR(byte);
    }
} /* End of function sci_transfer() */

/*****************************************************************************
* Function Name: txi_handler
* Description  : TXI interrupt handler for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void txi_handler(sci_hdl_t const hdl)
{
    sci_transfer(hdl);
} /* End of function txi_handler() */
#endif /* SCI_CFG_ASYNC_INCLUDED */

/*****************************************************************************
* Function Name: R_SCI_Receive
* Description  : Gets data received on an SCI channel referenced by the handle 
*                from rx queue. Function does not block if the requested 
*                number of bytes is not available. If any errors occurred 
*                during reception by hardware, they are handled by the callback 
*                function specified in R_SCI_Open() and no corresponding error 
*                code is provided here.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_NULL_PTR -
*                    hdl or p_dst is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data (Async)
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy (Sync)
*                SCI_ERR_INVALID_ARG
*                    length is out of range
******************************************************************************/
sci_err_t R_SCI_Receive(sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length)
{
    sci_err_t   err = SCI_SUCCESS;

    /* Check arguments */
#if SCI_CFG_PARAM_CHECKING_ENABLE
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

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        if(SCI_CH3 == hdl->channel)
        {
            err = sci_receive_async_data3(hdl, p_dst, length);
        }
        else
        {
            err = sci_receive_async_data(hdl, p_dst, length);
        }
#endif
    }
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    else
    {
        /* mode is SSPI/SYNC */
        err = sci_receive_sync_data(hdl, p_dst, length);
    }
#endif

    return err;
} /* End of function R_SCI_Receive() */

#if (SCI_CFG_ASYNC_INCLUDED)

/*****************************************************************************
* Function Name: sci_receive_async_data
* Description  : This function determines if the rx byte queue of the channel 
*                referenced by the handle, the requested number of bytes 
*                is available, and get the data from the rx byte queue.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data
******************************************************************************/
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
#if (SCI_CFG_USE_CIRCULAR_BUFFER == 1)
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
#else
        /* Disable RXI Interrupt */
        DISABLE_RXI_INT;
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
        ENABLE_RXI_INT;
#endif
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
} /* End of function sci_receive_async_data() */

#endif /* SCI_CFG_ASYNC_INCLUDED */

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_receive_sync_data
* Description  : This function determines if the channel referenced by the
*                handle is not busy, and dummy data send.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_XCVR_BUSY -
*                    channel currently busy
******************************************************************************/
static sci_err_t sci_receive_sync_data(sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length)
{
    /* IF TRANCEIVER NOT IN USE, START DUMMY TRANSMIT TO CLOCK IN DATA */
    if (true == hdl->tx_idle)
    {
        hdl->u_rx_data.buf = p_dst;
        hdl->save_rx_data  = true;               /* save the data clocked in */
        hdl->tx_idle       = false;
        hdl->tx_cnt        = length;
        hdl->rx_cnt        = length;
        hdl->tx_dummy      = true;
        hdl->tx_cnt--;
        SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);    /* start transfer */
        return SCI_SUCCESS;
    }

    return SCI_ERR_XCVR_BUSY;
} /* End of function sci_receive_sync_data() */
#endif /* End of SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED */

/*****************************************************************************
* Function Name: sci_receive
* Description  : Receive for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void sci_receive(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;

    /* Read byte */
    SCI_RDR(&byte);

#if defined(__CCRL__) || defined(__ICCRL78__)
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE || SCI_CFG_UART1_FLOW_CTRL_ENABLE || \
        SCI_CFG_UART2_FLOW_CTRL_ENABLE || SCI_CFG_UART3_FLOW_CTRL_ENABLE)
    uint16_t byteq_free = 0;
    R_BYTEQ_Unused(hdl->u_rx_data.que, &byteq_free);
    if (SCI_CFG_UART_RX_BUFFER_THRESH >= byteq_free)
    {
        switch(hdl->channel)
        {
#if (SCI_CFG_UART0_FLOW_CTRL_ENABLE)
            case 0:
                SCI_CFG_UART0_RTS_PORT = 1;
                break;
#endif
#if (SCI_CFG_UART1_FLOW_CTRL_ENABLE)
            case 1:
                SCI_CFG_UART1_RTS_PORT = 1;
                break;
#endif
#if (SCI_CFG_UART2_FLOW_CTRL_ENABLE)
            case 2:
                SCI_CFG_UART2_RTS_PORT = 1;
                break;
#endif
#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE)
            case 3:
                SCI_CFG_UART3_RTS_PORT = 1;
                break;
#endif
            default:
                break;
        }
    }
#endif /* SCI_CFG_UART0_FLOW_CTRL_ENABLE || SCI_CFG_UART1_FLOW_CTRL_ENABLE || SCI_CFG_UART2_FLOW_CTRL_ENABLE || SCI_CFG_UART3_FLOW_CTRL_ENABLE */
#endif /* defined__CCRL__ || defined__ICCRL78__ */

    if (SCI_MODE_ASYNC == hdl->mode)
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        /* Place byte in queue */
        if (R_BYTEQ_Put(hdl->u_rx_data.que, byte) == BYTEQ_SUCCESS)
        {
            args.event = SCI_EVT_RX_CHAR;
        }
        else
        {
            args.event = SCI_EVT_RXBUF_OVFL;
        }

        /* Do callback if available */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;

            /* Casting to void type is valid */
            hdl->callback((void *)&args);
        }
#endif
    }
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    else
    {
        hdl->rx_cnt--;

        /* Place byte in buffer if Receive() or SendReceive() */
        if (true == hdl->save_rx_data)
        {
            *hdl->u_rx_data.buf++ = byte;
        }

        /* See if more bytes to transfer */
        if (0 < hdl->rx_cnt)
        {
            if (0 < hdl->tx_cnt)
            {
                /* send another byte */
                if (true == hdl->tx_dummy)
                {
                    hdl->tx_cnt--;
                    SCI_TDR(SCI_CFG_DUMMY_TX_BYTE);
                }
                else
                {
                    hdl->tx_cnt--;
                    SCI_TDR(*hdl->u_tx_data.buf++);
                }
            }
        }
        else
        {
            hdl->tx_idle = true;

            /* Do callback if available */
            if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
            {
                args.hdl = hdl;
                args.event = SCI_EVT_XFER_DONE;

                /* Casting to void type is valid */
                hdl->callback((void *)&args);
            }
        }
    }
#endif /* End of SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED */
} /* End of function sci_receive() */

/*****************************************************************************
* Function Name: rxi_handler
* Description  : RXI interrupt handler for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void rxi_handler(sci_hdl_t const hdl)
{
    sci_receive(hdl);
} /* End of function rxi_handler() */

/*****************************************************************************
* Function Name: rxi_handler
* Description  : RXI interrupt handler for SCI
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void rxi_handler3(sci_hdl_t const hdl)
{
    sci_receive3(hdl);
} /* End of function rxi_handler3() */


/*****************************************************************************
* Function Name: sci_error
* Description  : Error for SCI
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
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
#if (SCI_CFG_ASYNC_INCLUDED)
        else if (0 != (reg & SCI_SSR_PER_MASK))
        {
            args.event = SCI_EVT_PARITY_ERR;
        }
        else if (0 != (reg & SCI_SSR_FER_MASK))
        {
            args.event = SCI_EVT_FRAMING_ERR;
        }
#endif
        else
        {
            /* Do Nothing */
        }

        /* Flush register */
        SCI_RDR(&byte);

        /* Do callback for error */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;

            /* Casting to void* type is valid */
            hdl->callback((void *)&args);
        }
    }

} /* End of function sci_error() */

/*****************************************************************************
* Function Name: eri_handler
* Description  : ERI interrupt handler for SCI UART mode
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
void eri_handler(sci_hdl_t const hdl)
{
    sci_error(hdl);
} /* End of function eri_handler() */

/*****************************************************************************
* Function Name: R_SCI_Control
* Description  : This function configures non-standard UART hardware and
*                performs special software operations.
*
* WARNING: Some commands require the transmitter and receiver to be temporarily
*          disabled for the command to execute!
*          PFS and port pins must be configured prior to calling with an
*          SCI_EN_CTS_IN command.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                cmd -
*                    command to run
*                p_args -
*                    pointer argument(s) specific to command
* Return Value : SCI_SUCCESS -
*                    Command completed successfully.
*                SCI_ERR_NULL_PTR -
*                    hdl or p_args is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INVALID_ARG -
*                    The cmd value or p_args contains an invalid value.
*                    May be due to mode channel is operating in.
******************************************************************************/
sci_err_t R_SCI_Control(sci_hdl_t const hdl, sci_cmd_t const cmd, void *p_args)
{
    sci_err_t   err = SCI_SUCCESS;
    sci_baud_t  *baud;

    /* Check argument hdl */
#if SCI_CFG_PARAM_CHECKING_ENABLE
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
#endif /* End of SCI_CFG_PARAM_CHECKING_ENABLE */
    
    /* COMMANDS COMMON TO ALL MODES */
    switch (cmd)
    {
        case (SCI_CMD_CHANGE_BAUD):
            /* Casting void* type is valid */
            baud = (sci_baud_t *)p_args;
            if (SCI_CH3 == hdl->channel)
            {
//            	R_Config_UART3_Reset(baud->rate);
            }
            break;
        case (SCI_CMD_EN_CTS_IN):
            err = SCI_ERR_INVALID_ARG;      // RL78 not support.
            break;
        default:
            if (SCI_MODE_ASYNC == hdl->mode)
            {
#if (SCI_CFG_ASYNC_INCLUDED)
                /* ASYNC-SPECIFIC COMMANDS */
                err = sci_async_cmds(hdl, cmd, p_args);
#endif
            }
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
            else
            {
                /* SSPI/SYNC-SPECIFIC COMMANDS */
                err = sci_sync_cmds(hdl, cmd, p_args);
            }
#endif
            break;
    }
    return err;
} /* End of function R_SCI_Control() */

/*****************************************************************************
* Function Name: R_SCI_Close
* Description  : Disables the SCI channel designated by the handle.
*
* WARNING: This will abort any xmt or rcv messages in progress.
* NOTE:    This does not disable the GROUP12 (rcvr err) interrupts.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel closed
*                SCI_ERR_NULL_PTR -
*                    hdl was NULL
******************************************************************************/
sci_err_t R_SCI_Close(sci_hdl_t const hdl)
{

#if SCI_CFG_PARAM_CHECKING_ENABLE
    /* Check argument hdl */
    if (NULL == hdl)
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    /* disable ICU interrupts */
    sci_disable_ints(hdl);

    /* free tx and rx queues */
#if (SCI_CFG_ASYNC_INCLUDED)
    if (SCI_MODE_ASYNC == hdl->mode)
    {
        R_BYTEQ_Close(hdl->u_tx_data.que);
        R_BYTEQ_Close(hdl->u_rx_data.que);
    }
#endif

    /* mark the channel as not in use and power down */
    hdl->mode = SCI_MODE_OFF;
    power_off(hdl->channel);

    return SCI_SUCCESS;
} /* End of function R_SCI_Close() */


/*****************************************************************************
* Function Name: R_SCI_GetVersion
* Description  : Returns the version of this module. The version number is 
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
uint32_t  R_SCI_GetVersion(void)
{
    uint32_t const version = ((uint32_t)SCI_VERSION_MAJOR << 16) | SCI_VERSION_MINOR;

    return version;
} /* End of function R_SCI_GetVersion() */


static void sci_receive3(sci_hdl_t const hdl)
{
    sci_cb_args_t   args;
    uint8_t         byte;

    /* Read byte */
    byte = RXD3;

#if defined(__CCRL__) || defined(__ICCRL78__)
#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE == 1)
    uint16_t byteq_free = 0;
    R_BYTEQ_Unused(hdl->u_rx_data.que, &byteq_free);
    if (SCI_CFG_UART_RX_BUFFER_THRESH >= byteq_free)
    {
        SCI_CFG_UART3_RTS_PORT = 1;
    }
#endif /* (SCI_CFG_UART3_FLOW_CTRL_ENABLE) */
#endif /* defined__CCRL__ || defined__ICCRL78__ */

#if (SCI_CFG_ASYNC_INCLUDED)
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

        /* Do callback if available */
        if ((NULL != hdl->callback) && (FIT_NO_FUNC != hdl->callback))
        {
            args.hdl = hdl;
            args.byte = byte;

            /* Casting to void type is valid */
            hdl->callback((void *)&args);
        }
    }
#endif /* (SCI_CFG_ASYNC_INCLUDED) */
} /* End of function sci_receive() */


#if (SCI_CFG_ASYNC_INCLUDED)
/*****************************************************************************
* Function Name: sci_receive_async_data
* Description  : This function determines if the rx byte queue of the channel
*                referenced by the handle, the requested number of bytes
*                is available, and get the data from the rx byte queue.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length -
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data
******************************************************************************/
static sci_err_t sci_receive_async_data3(sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length)
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
#if (SCI_CFG_USE_CIRCULAR_BUFFER == 1)
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
#else
        /* Disable RXI Interrupt */
        R_BSP_InterruptsDisable();
        byteq_err = R_BYTEQ_Get(hdl->u_rx_data.que, p_dst++);
        R_BSP_InterruptsEnable();
#endif

        if (BYTEQ_SUCCESS != byteq_err)
        {
            err = SCI_ERR_INSUFFICIENT_DATA;
            break;
        }

        uint16_t byteq_free = 0;
        R_BYTEQ_Unused(hdl->u_rx_data.que, &byteq_free);

#if (SCI_CFG_UART3_FLOW_CTRL_ENABLE)
        if (SCI_CFG_UART_RX_BUFFER_THRESH < byteq_free)
        {
            SCI_CFG_UART3_RTS_PORT = 0;
        }
#endif
    }

    return err;
} /* End of function sci_receive_async_data() */

#endif /* SCI_CFG_ASYNC_INCLUDED */
