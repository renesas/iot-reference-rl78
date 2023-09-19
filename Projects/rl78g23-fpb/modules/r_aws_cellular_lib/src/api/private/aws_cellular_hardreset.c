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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : aws_cellular_hardreset.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_aws_cellular_if.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_hardreset
 ************************************************************************************************/
CellularError_t aws_cellular_hardreset(CellularHandle_t cellularHandle)
{
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)cellularHandle->pModueContext;   //cast

    uint8_t         cnt            = 0;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;

    AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RESET_PORT, AWS_CELLULAR_CFG_RESET_PIN) = AWS_CELLULAR_CFG_RESET_SIGNAL_ON;
    AWS_CELLULAR_SET_PDR(AWS_CELLULAR_CFG_RESET_PORT, AWS_CELLULAR_CFG_RESET_PIN)  = 1;
    Platform_Delay(1);   /* hold reset signal time for cellular module */
    AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RESET_PORT, AWS_CELLULAR_CFG_RESET_PIN) = AWS_CELLULAR_CFG_RESET_SIGNAL_OFF;

    while (1)
    {
        if (AWS_CELLULAR_MODULE_START_FLG == p_aws_ctrl->module_flg)
        {
           p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;
           break;
        }
        else
        {
           cnt++;
           Platform_Delay(1000);   //cast
        }

        if (cnt > AWS_CELLULAR_RESTART_LIMIT)
        {
            cellularStatus = CELLULAR_MODEM_NOT_READY;
           break;
        }
    }

    return cellularStatus;
}
/**********************************************************************************************************************
 * End of function aws_cellular_hardreset
 *********************************************************************************************************************/
