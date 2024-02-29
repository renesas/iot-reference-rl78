/*
 * r_sci_rl78_private.h
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

#ifndef R_SCI_RL78_PRIVATE_H
#define R_SCI_RL78_PRIVATE_H

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "rl78_bsp_wrapper.h"
#include "r_sci_rl_if.h"

#if (SCI_CFG_ASYNC_INCLUDED ==1)
#include "r_byteq_if.h"
#endif /* (SCI_CFG_ASYNC_INCLUDED ==1) */

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Mask of all active channels */
#define SCI_CFG_CH_INCLUDED_MASK   ((SCI_CFG_CH0_INCLUDED << 0)   |   \
                                    (SCI_CFG_CH1_INCLUDED << 1)   |   \
                                    (SCI_CFG_CH2_INCLUDED << 2)   |   \
                                    (SCI_CFG_CH3_INCLUDED << 3)   |   \
                                    (SCI_CFG_CH4_INCLUDED << 4)   |   \
                                    (SCI_CFG_CH5_INCLUDED << 5)   |   \
                                    (SCI_CFG_CH6_INCLUDED << 6)   |   \
                                    (SCI_CFG_CH7_INCLUDED << 7)   |   \
                                    (SCI_CFG_CH8_INCLUDED << 8)   |   \
                                    (SCI_CFG_CH9_INCLUDED << 9)   |   \
                                    (SCI_CFG_CH10_INCLUDED << 10) |   \
                                    (SCI_CFG_CH11_INCLUDED << 11) |   \
                                    (SCI_CFG_CH12_INCLUDED << 12))

/* SCI SSR register receiver error masks */
#define SCI_SSR_ORER_MASK           (0x01U)     /* overflow error */
#define SCI_SSR_FER_MASK            (0x04U)     /* framing error */
#define SCI_SSR_PER_MASK            (0x02U)     /* parity err */
#define SCI_RCVR_ERR_MASK           (SCI_SSR_ORER_MASK | SCI_SSR_FER_MASK | SCI_SSR_PER_MASK)

#define ENABLE_RXI_INT              ( SetReg_SRMKn(hdl->channel, 0) )
#define DISABLE_RXI_INT             ( SetReg_SRMKn(hdl->channel, 1) )
#define ENABLE_TXI_INT
#define DISABLE_TXI_INT

#if (__CCRL__) || defined(__ICCRL78__)
#define NUM_DIVISORS_ASYNC          (5)
#endif
#define NUM_DIVISORS_SYNC           (4)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/* ROM INFO */
typedef struct st_sci_ch_rom
{
    volatile uint16_t channel;
    volatile uint16_t* SPSm;
    volatile uint16_t *SSRmn;
    volatile uint16_t* SDRmn;
    volatile uint16_t* SIRmn;
    volatile uint16_t* SMRmn;
    volatile uint16_t* SCRmn;
    volatile uint16_t* SOLm;
    volatile uint16_t* SOm;
    volatile uint16_t* SOEm;
    volatile uint16_t* SSm;
    volatile uint16_t *TxD;
    volatile uint16_t *RxD;
} sci_ch_rom_t;

/* CHANNEL CONTROL BLOCK */
typedef struct st_sci_ch_ctrl       /* SCI channel control (for handle) */
{
    uint16_t        channel;
    sci_mode_t      mode;           /* operational mode */
    uint32_t        baud_rate;      /* baud rate */
    void          (*callback)(void *p_args); /* function ptr for rcvr errs */
    union
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        byteq_hdl_t     que;        /* async transmit queue handle */
#endif
        uint8_t         *buf;       /* sspi/sync tx buffer ptr */
    } u_tx_data;
    union
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        byteq_hdl_t     que;        /* async receive queue handle */
#endif
        uint8_t         *buf;       /* sspi/sync rx buffer ptr */
    } u_rx_data;
    bool            tx_idle;        /* TDR is empty (async); TSR is empty (sync/sspi) */
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    bool            save_rx_data;   /* save the data that is clocked in */
    uint16_t        tx_cnt;         /* number of bytes to transmit */
    uint16_t        rx_cnt;         /* number of bytes to receive */
    bool            tx_dummy;       /* transmit dummy byte, not buffer */
#endif
#if (false)                         /* Not compatible */
    uint32_t        pclk_speed;     /* saved peripheral clock speed for break generation */
#endif
} sci_ch_ctrl_t;


/* BAUD DIVISOR INFO */
typedef struct st_baud_divisor
{
    uint16_t    divisor;  // clock divisor
    uint16_t    exponent;
    uint16_t    denominator;
} baud_divisor_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern const sci_hdl_t g_sci_handles[];

#if (SCI_CFG_ASYNC_INCLUDED)
extern const SCI_FAR baud_divisor_t async_baud[];
#endif

#if (SCI_CFG_CH0_INCLUDED)
extern sci_ch_ctrl_t ch0_ctrl;
#endif

#if (SCI_CFG_CH1_INCLUDED)
extern sci_ch_ctrl_t ch1_ctrl;
#endif

#if (SCI_CFG_CH2_INCLUDED)
extern sci_ch_ctrl_t ch2_ctrl;
#endif

#if (SCI_CFG_CH3_INCLUDED)
extern sci_ch_ctrl_t ch3_ctrl;
#endif

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
#if (SCI_CFG_TEI_INCLUDED)
extern void tei_handler(sci_hdl_t const hdl);
extern void sci0_tei0_isr(void *cb_args);
extern void sci1_tei1_isr(void *cb_args);
extern void sci2_tei2_isr(void *cb_args);
extern void sci3_tei3_isr(void *cb_args);
#endif

extern void SetReg_TXD(uint8_t channel, uint8_t byte);
extern void GetReg_RXD(uint8_t channel, uint8_t *byte);
extern void SetReg_SRMKn(uint8_t channel, uint8_t flag);
extern void SetReg_SREMKn(uint8_t channel, uint8_t flag);
extern void sci_init_register(sci_hdl_t const hdl);
extern uint16_t GetReg_SSR(uint8_t channel);

#if (SCI_CFG_ASYNC_INCLUDED == 1)
extern sci_err_t sci_async_cmds(sci_hdl_t const hdl, sci_cmd_t const cmd, void *p_args);
extern void txi_handler(sci_hdl_t const hdl);
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
extern sci_err_t sci_mcu_param_check(uint8_t const chan);
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */

extern void sci_initialize_ints(sci_hdl_t const hdl, uint8_t const priority);
extern void sci_disable_ints(sci_hdl_t const hdl);
extern void rxi_handler(sci_hdl_t const hdl);
extern void eri_handler(sci_hdl_t const hdl);

#endif /* R_SCI_RL78_PRIVATE_H */

