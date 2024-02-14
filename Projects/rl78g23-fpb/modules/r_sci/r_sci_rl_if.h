/*
 * r_sci_rl_if.h
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

#ifndef R_SCI_RL_IF_H
#define R_SCI_RL_IF_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_sci_rl_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#if defined(__CCRL__)
#define SCI_FAR         __far
#define SCI_FAR_FUNC    __far
#elif defined(__ICCRL78__)
#define SCI_FAR         __far
#define SCI_FAR_FUNC    __far_func
#endif

#if (SCI_CFG_USE_CIRCULAR_BUFFER == 1)
#if (BYTEQ_CFG_PROTECT_QUEUE == 0)
    #error "Circular buffer must be protected."
#endif
#endif

/* Version Number of API. */
#define SCI_VERSION_MAJOR   (0)
#define SCI_VERSION_MINOR   (01)

#define SCI_CLK_INT         (0x00U) /* use internal clock for baud generation */
#define SCI_CLK_EXT8X       (0x03U) /* use external clock 8x baud rate (ASYNC) */
#define SCI_CLK_EXT16X      (0x02U) /* use external clock 16x baud rate (ASYNC) */
#define SCI_DATA_7BIT       (0x40U)
#define SCI_DATA_8BIT       (0x00U)
#define SCI_PARITY_ON       (0x20U)
#define SCI_PARITY_OFF      (0x00U)
#define SCI_ODD_PARITY      (0x10U)
#define SCI_EVEN_PARITY     (0x00U)
#define SCI_STOPBITS_2      (0x08U)
#define SCI_STOPBITS_1      (0x00U)

/*****************************************************************************
Typedef definitions
******************************************************************************/
/* SCI channel numbers */
typedef enum e_sci_ch
{
    SCI_CH0=0,
    SCI_CH1,
    SCI_CH2,
    SCI_CH3,
    SCI_NUM_CH
} sci_ch_t;

/* SCI operational modes */
typedef enum e_sci_mode
{
    SCI_MODE_OFF=0,             /* channel not in use */
    SCI_MODE_ASYNC,             /* Asynchronous */
    SCI_MODE_SSPI,              /* Simple SPI */
    SCI_MODE_SYNC,              /* Synchronous */
    SCI_MODE_MAX,               /* End of modes currently supported */
} sci_mode_t;

/* SCI API error codes */
typedef enum e_sci_err
{
    SCI_SUCCESS=0,
    SCI_ERR_BAD_CHAN,           /* non-existent channel number */
    SCI_ERR_OMITTED_CHAN,       /* SCI_CHx_INCLUDED is 0 in config.h */
    SCI_ERR_CH_NOT_CLOSED,      /* chan still running in another mode */
    SCI_ERR_BAD_MODE,           /* unsupported or incorrect mode for channel */
    SCI_ERR_INVALID_ARG,        /* argument is not one of the predefined values */
    SCI_ERR_NULL_PTR,           /* received null ptr; missing required argument */
    SCI_ERR_XCVR_BUSY,          /* cannot start data transfer; transceiver busy */

    /* Asynchronous mode only */
    SCI_ERR_QUEUE_UNAVAILABLE,  /* can't open tx or rx queue or both */
    SCI_ERR_INSUFFICIENT_SPACE, /* not enough space in transmit queue */
    SCI_ERR_INSUFFICIENT_DATA,  /* not enough data in receive queue */

    /* Synchronous/SSPI modes only */
    SCI_ERR_XFER_NOT_DONE       /* data transfer still in progress */
} sci_err_t;

/* CHANNEL CONTROL BLOCK HANDLE */
typedef struct st_sci_ch_ctrl * sci_hdl_t;

/* SCI_OPEN() ARGUMENT DEFINITIONS (do NOT change values) */
typedef enum e_sci_spi_mode
{
    SCI_SPI_MODE_OFF = 1,   /* channel is in synchronous mode */

    SCI_SPI_MODE_0 = 0x80,  /* SPMR Register CKPH=1, CKPOL=0
                               Mode 0: 00 CPOL=0 resting lo, CPHA=0 leading edge/rising */
    SCI_SPI_MODE_1 = 0x40,  /* SPMR Register CKPH=0, CKPOL=1
                               Mode 1: 01 CPOL=0 resting lo, CPHA=1 trailing edge/falling */
    SCI_SPI_MODE_2 = 0xC0,  /* SPMR Register CKPH=1, CKPOL=1
                               Mode 2: 10 CPOL=1 resting hi, CPHA=0 leading edge/falling */
    SCI_SPI_MODE_3 = 0x00   /* SPMR Register CKPH=0, CKPOL=0
                               Mode 3: 11 CPOL=1 resting hi, CPHA=1 trailing edge/rising */
} sci_spi_mode_t;


/* Open() p_cfg structure when mode=SCI_MODE_ASYNC */
typedef struct st_sci_uart
{
    uint32_t    baud_rate;      /* ie 9600, 19200, 115200 */
    uint8_t     clk_src;        /* use SCI_CLK_INT/EXT8X/EXT16X */
    uint8_t     data_size;      /* use SCI_DATA_nBIT */
    uint8_t     parity_en;      /* use SCI_PARITY_ON/OFF */
    uint8_t     parity_type;    /* use SCI_ODD/EVEN_PARITY */
    uint8_t     stop_bits;      /* use SCI_STOPBITS_1/2 */
    uint8_t     int_priority;   /* interrupt priority; 1=low, 15=high */
} sci_uart_t;

/* Open() p_cfg structure when mode = SCI_MODE_SYNC or SCI_MODE_SSPI */
typedef struct st_sci_sync_sspi
{
    sci_spi_mode_t  spi_mode;       /* clock polarity and phase; unused for sync */
    uint32_t        bit_rate;       /* ie 1000000 for 1Mbps */
    bool            msb_first;
    bool            invert_data;
    uint8_t         int_priority;   /* interrupt priority; 1=low, 15=high */
} sci_sync_sspi_t;

typedef union
{
    sci_uart_t      async;
    sci_sync_sspi_t sync;
    sci_sync_sspi_t sspi;
} sci_cfg_t;

/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */
typedef enum e_sci_cb_evt   // callback function events
{
    /* Async Events */
    SCI_EVT_TEI,            /* TEI interrupt occurred; transmitter is idle */
    SCI_EVT_RX_CHAR,        /* received a character; already placed in queue */
    SCI_EVT_RX_CHAR_MATCH,  /* received a matched character; already placed in queue */
    SCI_EVT_RXBUF_OVFL,     /* rx queue is full; can't save anymore data */
    SCI_EVT_FRAMING_ERR,    /* receiver hardware framing error */
    SCI_EVT_PARITY_ERR,     /* receiver hardware parity error */

    /* SSPI/Sync Events */
    SCI_EVT_XFER_DONE,      /* transfer completed */
    SCI_EVT_XFER_ABORTED,   /* transfer aborted */

    /* Common Events */
    SCI_EVT_OVFL_ERR        /* receiver hardware overrun error */
} sci_cb_evt_t;

/* callback arguments */
typedef struct st_sci_cb_args
{
    sci_hdl_t       hdl;    /* SCI handle */
    sci_cb_evt_t    event;  /* event */
    uint8_t         byte;   /* byte read when error occurred (unused for TEI, XFER_DONE) */
    uint8_t         num;    /* Number of bytes were stored to queue (used only async(FIFO)) */
} sci_cb_args_t;

/* commands */
typedef enum e_sci_cmd
{
    /* All modes */
    SCI_CMD_CHANGE_BAUD,              /* change baud/bit rate */

    /* Async commands */
    SCI_CMD_EN_NOISE_CANCEL,          /* enable noise cancellation */
    SCI_CMD_EN_TEI,                   /* SCI_CMD_EN_TEI is obsolete command,
                                         but it exists only for compatibility with older version. */
    SCI_CMD_OUTPUT_BAUD_CLK,          /* output baud clock on the SCK pin */
    SCI_CMD_START_BIT_EDGE,           /* detect start bit as falling edge of RXDn pin
                                         (default detect as low level on RXDn pin) */
    SCI_CMD_GENERATE_BREAK,           /* generate break condition */
    SCI_CMD_TX_Q_FLUSH,               /* flush transmit queue */
    SCI_CMD_RX_Q_FLUSH,               /* flush receive queue */
    SCI_CMD_TX_Q_BYTES_FREE,          /* get count of unused transmit queue bytes */
    SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, /* get num bytes ready for reading */
    SCI_CMD_COMPARE_RECEIVED_DATA,    /* Compare received data with comparison data */

    /* Async/Sync commands */
    SCI_CMD_EN_CTS_IN,                /* enable CTS input (default RTS output) */

    /* SSPI/Sync commands */
    SCI_CMD_CHECK_XFER_DONE,          /* see if send, rcv, or both are done; SCI_SUCCESS if yes */
    SCI_CMD_ABORT_XFER,
    SCI_CMD_XFER_LSB_FIRST,           /* start from LSB bit when sending */
    SCI_CMD_XFER_MSB_FIRST,           /* start from MSB bit when sending */
    SCI_CMD_INVERT_DATA,              /* logic level of send/receive data is invert */

    /* SSPI commands */
    SCI_CMD_CHANGE_SPI_MODE           /* change clock polarity and phase in SSPI mode */
} sci_cmd_t;

/* SCI_CMD_CHANGE_BAUD/CHANGE_BITRATE take a ptr to this structure for *p_args */
typedef struct st_sci_baud
{
    uint32_t    pclk;       /* peripheral clock speed; e.g. 24000000 is 24MHz */
    uint32_t    rate;       /* e.g. 9600, 19200, 115200 */
} sci_baud_t;

/* SCI_CMD_TX_Q_BYTES_FREE and SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ take a pointer
   to a uint16_t for p_args */

/* SCI_CMD_SET_RXI_PRIORITY and SCI_CMD_SET_TXI_PRIORITY take a pointer to a
   uint8_t for p_args */

/* SCI_CMD_CHANGE_SPI_MODE takes a pointer to an sci_spi_mode_t for p_args */

/*****************************************************************************
Public Functions
******************************************************************************/
sci_err_t R_SCI_Open (uint8_t const chan, sci_mode_t const mode, sci_cfg_t * const p_cfg,
                      void (* const p_callback)(void *p_args), sci_hdl_t * const p_hdl);
sci_err_t R_SCI_Send (sci_hdl_t const hdl, uint8_t SCI_FAR *p_src, uint16_t const length);
sci_err_t R_SCI_SendReceive (sci_hdl_t const hdl, uint8_t *p_src, uint8_t *p_dst, uint16_t const length);
sci_err_t R_SCI_Receive (sci_hdl_t const hdl, uint8_t *p_dst, uint16_t const length);
sci_err_t R_SCI_Control (sci_hdl_t const hdl, sci_cmd_t const cmd, void *p_args);
sci_err_t R_SCI_Close (sci_hdl_t const hdl);
uint32_t  R_SCI_GetVersion (void);

#endif /* R_SCI_RL_IF_H */

