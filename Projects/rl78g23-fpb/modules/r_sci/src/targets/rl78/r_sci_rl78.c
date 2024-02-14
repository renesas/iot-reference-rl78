/*
 * r_sci_rl78.c
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
#include "r_sci_rl78_private.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
/**********************************************************************************************************************
* Function Name: sci_mcu_param_check
* Description  : This function parameters check on MCU.
* Arguments    : chan - channel to check
* Return Value : SCI_SUCCESS
*                SCI_ERR_BAD_CHAN
*                SCI_ERR_INVALID_ARG
 *********************************************************************************************************************/
sci_err_t sci_mcu_param_check(uint8_t const chan)
{
    /* Check parameters */
    if (SCI_NUM_CH <= chan)
    {
        return SCI_ERR_BAD_CHAN;
    }
    return SCI_SUCCESS;
}
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */

/**********************************************************************************************************************
* Function Name: sci_init_register
* Description  : This function initializes the register for SCI.
* Arguments    : hdl - handle for channel (ptr to chan control block)
* Return Value : none
 *********************************************************************************************************************/
void sci_init_register(sci_hdl_t const hdl)
{
    return;
}

/**********************************************************************************************************************
* Function Name: sci_initialize_ints
* Description  : This function sets priority, clears flags, and sets interrupts in both the ICU and SCI peripheral.
* Arguments    : hdl      - handle for channel (ptr to chan control block)
*                priority - priority for interrupts
* Return Value : none
 *********************************************************************************************************************/
void sci_initialize_ints(sci_hdl_t const hdl, uint8_t const priority)
{
    return;
}

/**********************************************************************************************************************
* Function Name: sci_disable_ints
* Description  : This function disable interrupts in both the ICU and SCI peripheral.
* Arguments    : hdl - handle for channel (ptr to chan control block)
* Return Value : none
 *********************************************************************************************************************/
void sci_disable_ints(sci_hdl_t const hdl)
{
    return;
}

#if (SCI_CFG_ASYNC_INCLUDED == 1)
/**********************************************************************************************************************
* Function Name: sci_async_cmds
* Description  : This function configures non-standard UART hardware and performs special software operations.
* Arguments    : hdl    - handle for channel (ptr to chan control block)
*                cmd    - command to run
*                p_args - pointer argument(s) specific to command
* Return Value : SCI_SUCCESS         - Command completed successfully.
*                SCI_ERR_NULL_PTR    - p_args is NULL when required for cmd
*                SCI_ERR_INVALID_ARG -The cmd value or p_args contains an invalid value.
 *********************************************************************************************************************/
sci_err_t sci_async_cmds(sci_hdl_t const hdl, sci_cmd_t const cmd, void *p_args)
{
    sci_err_t   err = SCI_SUCCESS;

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    /* Check parameters */
    if ((NULL == p_args)
            && ((SCI_CMD_TX_Q_BYTES_FREE == cmd) ||
                (SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ == cmd)||
                (SCI_CMD_COMPARE_RECEIVED_DATA == cmd)))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif /* (SCI_CFG_PARAM_CHECKING_ENABLE == 1) */

    switch(cmd)
    {
#if (SCI_CFG_TEI_INCLUDED == 1)
        case (SCI_CMD_EN_TEI):
            break;
#endif /* (SCI_CFG_TEI_INCLUDED == 1) */
        case (SCI_CMD_TX_Q_FLUSH):
            DISABLE_TXI_INT;
            R_BYTEQ_Flush(hdl->u_tx_data.que);
            ENABLE_TXI_INT;
            break;
        case (SCI_CMD_RX_Q_FLUSH):
            DISABLE_RXI_INT;
            R_BYTEQ_Flush(hdl->u_rx_data.que);
            ENABLE_RXI_INT;
            break;
        case (SCI_CMD_TX_Q_BYTES_FREE):
            R_BYTEQ_Unused(hdl->u_tx_data.que, (uint16_t *) p_args);
            break;
        case (SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ):
            R_BYTEQ_Used(hdl->u_rx_data.que, (uint16_t *) p_args);
            break;
        case (SCI_CMD_EN_NOISE_CANCEL):
        case (SCI_CMD_OUTPUT_BAUD_CLK):
        case (SCI_CMD_START_BIT_EDGE):
        case (SCI_CMD_GENERATE_BREAK):
            /* Not supported. */
            break;
        default:
            err = SCI_ERR_INVALID_ARG;
            break;
    }
    return err;
}
#endif /* (SCI_CFG_ASYNC_INCLUDED == 1) */

/*
 * ISRs
 */
#if (SCI_CFG_ASYNC_INCLUDED == 1)
#if (SCI_CFG_CH0_INCLUDED == 1)
#if (USE_BOOTLOADER_V2 == 0)
#pragma interrupt sci0_txi0_isr(vect=INTST0)
#pragma interrupt sci0_rxi0_isr(vect=INTSR0)
#pragma interrupt sci0_eri0_isr(vect=INTSRE0)
#endif /* (USE_BOOTLOADER_V2 == 0) */
/**********************************************************************************************************************
 * Function Name: sci0_txi0_isr
 * Description  : TXI interrupt routines for SCI0 channel
 *********************************************************************************************************************/
void sci0_txi0_isr(void)
{
    txi_handler(&ch0_ctrl);
}

/**********************************************************************************************************************
 * Function Name: sci0_rxi0_isr
 * Description  : RXI interrupt routines for SCI0 channel
 *********************************************************************************************************************/
void sci0_rxi0_isr(void)
{
    rxi_handler(&ch0_ctrl);
}

/**********************************************************************************************************************
 * Function name: sci0_eri0_isr
 * Description  : ERI interrupt routines for SCI0 channel.
 *********************************************************************************************************************/
void sci0_eri0_isr(void)
{
    eri_handler(&ch0_ctrl);
}
#endif /* (SCI_CFG_CH0_INCLUDED == 1) */

#if (SCI_CFG_CH1_INCLUDED == 1)
#if (USE_BOOTLOADER_V2 == 0)
#pragma interrupt sci1_txi1_isr(vect=INTST1)
#pragma interrupt sci1_rxi1_isr(vect=INTSR1)
#pragma interrupt sci1_eri1_isr(vect=INTSRE1)
#endif /* (USE_BOOTLOADER_V2 == 0) */
/**********************************************************************************************************************
 * Function Name: sci1_txi1_isr
 * Description  : TXI interrupt routines for SCI1 channel
 *********************************************************************************************************************/
void sci1_txi1_isr(void)
{
    txi_handler(&ch1_ctrl);
}

/**********************************************************************************************************************
 * Function Name: sci1_rxi1_isr
 * Description  : RXI interrupt routines for SCI1 channel
 *********************************************************************************************************************/
void sci1_rxi1_isr(void)
{
    rxi_handler(&ch1_ctrl);
}

/**********************************************************************************************************************
 * Function name: sci1_eri1_isr
 * Description  : ERI interrupt routines for SCI1 channel.
 *********************************************************************************************************************/
void sci1_eri1_isr(void)
{
    eri_handler(&ch1_ctrl);
}
#endif /* (SCI_CFG_CH1_INCLUDED == 1) */

#if (SCI_CFG_CH2_INCLUDED == 1)
#if (USE_BOOTLOADER_V2 == 0)
#pragma interrupt sci2_txi2_isr(vect=INTST2)
#pragma interrupt sci2_rxi2_isr(vect=INTSR2)
#pragma interrupt sci2_eri2_isr(vect=INTSRE2)
#endif /* (USE_BOOTLOADER_V2 == 0) */
/**********************************************************************************************************************
 * Function Name: sci2_txi2_isr
 * Description  : TXI interrupt routines for SCI2 channel
 *********************************************************************************************************************/
void sci2_txi2_isr(void)
{
    txi_handler(&ch2_ctrl);
}

/**********************************************************************************************************************
 * Function Name: sci2_rxi2_isr
 * Description  : RXI interrupt routines for SCI2 channel
 *********************************************************************************************************************/
void sci2_rxi2_isr(void)
{
    rxi_handler(&ch2_ctrl);
}

/**********************************************************************************************************************
 * Function name: sci2_eri2_isr
 * Description  : ERI interrupt routines for SCI2 channel.
 *********************************************************************************************************************/
void sci2_eri2_isr(void)
{
    eri_handler(&ch2_ctrl);
}
#endif /* (SCI_CFG_CH2_INCLUDED == 1) */

#if (SCI_CFG_CH3_INCLUDED == 1)
#if (USE_BOOTLOADER_V2 == 0)
#pragma interrupt sci3_txi3_isr(vect=INTST3)
#pragma interrupt sci3_rxi3_isr(vect=INTSR3)
#pragma interrupt sci3_eri3_isr(vect=INTSRE3)
#endif /* (USE_BOOTLOADER_V2 == 0) */
/**********************************************************************************************************************
 * Function Name: sci3_txi3_isr
 * Description  : TXI interrupt routines for SCI3 channel
 *********************************************************************************************************************/
void sci3_txi3_isr(void)
{
    txi_handler(&ch3_ctrl);
}

/**********************************************************************************************************************
 * Function Name: sci3_rxi3_isr
 * Description  : RXI interrupt routines for SCI3 channel
 *********************************************************************************************************************/
void sci3_rxi3_isr(void)
{
    rxi_handler(&ch3_ctrl);
}

/**********************************************************************************************************************
 * Function name: sci3_eri3_isr
 * Description  : ERI interrupt routines for SCI3 channel.
 *********************************************************************************************************************/
void sci3_eri3_isr(void)
{
    eri_handler(&ch3_ctrl);
}
#endif /* (SCI_CFG_CH3_INCLUDED == 1) */
#endif /* (SCI_CFG_ASYNC_INCLUDED) */

