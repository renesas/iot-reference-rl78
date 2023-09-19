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
 * File Name    : r_aws_cellular_open.c
 * Description  : Function to establish the communication state with the module.
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
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
st_aws_cellular_ctrl_t g_aws_cellular_ctrl = {AWS_CELLULAR_MODULE_FLG_INIT,
                                              0,
                                              NULL,
                                              NULL,
                                              NULL};
#else
st_aws_cellular_ctrl_t g_aws_cellular_ctrl = {AWS_CELLULAR_MODULE_FLG_INIT,
                                              0,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL};
#endif /* defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) */

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            R_AWS_CELLULAR_Open
 ************************************************************************************************/
CellularError_t R_AWS_CELLULAR_Open(CellularHandle_t * cellularHandle)
{
    CellularContext_t       * p_context  = (CellularContext_t *)cellularHandle;
    CellularError_t           ret        = CELLULAR_SUCCESS;
    CellularCommInterface_t * p_comm_if  = &g_cellular_comm_interface;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL != cellularHandle)
    {
#endif
        if (true == p_context->bLibOpened)
        {
            ret = CELLULAR_LIBRARY_ALREADY_OPEN;
        }
        else
        {
            ret = Cellular_Init(cellularHandle, p_comm_if);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
    else
    {
        ret = CELLULAR_INVALID_HANDLE;
    }
#endif

    return ret;
}
/**********************************************************************************************************************
 * End of function R_AWS_CELLULAR_Open
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_modemevent
 ************************************************************************************************/
void aws_cellular_modemevent(CellularModemEvent_t   modemEvent,
                             void                 * pCallbackContext)
{
    st_aws_cellular_ctrl_t * p_ctrl = (st_aws_cellular_ctrl_t *)pCallbackContext;  //cast

    switch (modemEvent)
    {
        case CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT:
        {
            p_ctrl->module_flg = AWS_CELLULAR_MODULE_START_FLG;
            break;
        }
        case CELLULAR_MODEM_EVENT_POWERED_DOWN:
        {
            p_ctrl->module_flg = AWS_CELLULAR_MODULE_SHUTDOWN_FLG;
            break;
        }
        case CELLULAR_MODEM_EVENT_PSM_ENTER:
        {
            break;
        }
        default:
            break;
    }
}
/**********************************************************************************************************************
 * End of function aws_cellular_modemevent
 *********************************************************************************************************************/
