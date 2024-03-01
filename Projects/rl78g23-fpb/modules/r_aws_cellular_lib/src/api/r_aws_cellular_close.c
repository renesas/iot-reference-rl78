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
 * File Name    : r_aws_cellular_close.c
 * Description  : Function to terminate communication with the module.
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
static void cellular_shutdown (CellularHandle_t cellularHandle);

/*************************************************************************************************
 * Function Name  @fn            R_AWS_CELLULAR_Close
 ************************************************************************************************/
CellularError_t R_AWS_CELLULAR_Close(CellularHandle_t cellularHandle)
{
    CellularContext_t      * p_context  = (CellularContext_t *)cellularHandle;
    st_aws_cellular_ctrl_t * p_aws_ctrl = NULL;
    CellularError_t          ret        = CELLULAR_SUCCESS;
    uint8_t                  cnt        = 0;
    BaseType_t               semaphore_ret = 0;

    if (NULL != cellularHandle)
    {
        if (true != p_context->bLibOpened)
        {
            ret = CELLULAR_LIBRARY_NOT_OPEN;
        }
        else
        {
            semaphore_ret = aws_cellular_rts_deactive(p_context);

            aws_cellular_closesocket(cellularHandle);
            aws_cellular_psm_config(p_context, 0);

            ret = aws_cellular_hardreset(cellularHandle);

            if (CELLULAR_SUCCESS == ret)
            {
                cellular_shutdown(cellularHandle);
                p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;    //cast

                /* WAIT_LOOP */
                while (1)
                {
                    if (AWS_CELLULAR_MODULE_SHUTDOWN_FLG == p_aws_ctrl->module_flg)
                    {
                       p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;
                       break;
                    }
                    else
                    {
                       cnt++;
                       Platform_Delay(1000);   //cast
                    }

                    if (cnt >= AWS_CELLULAR_RESTART_LIMIT)
                    {
                       break;
                    }
                }
            }

            aws_cellular_rts_active(p_context, semaphore_ret);

            ret = Cellular_Cleanup(cellularHandle);
        }
    }
    else
    {
        ret = CELLULAR_INVALID_HANDLE;
    }

    return ret;
}
/**********************************************************************************************************************
 * End of function R_AWS_CELLULAR_Close
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            cellular_shutdown
 ************************************************************************************************/
static void cellular_shutdown(CellularHandle_t cellularHandle)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

    CellularAtReq_t atReqShutdown =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS == cellularStatus)
    {
        (void)snprintf((char *)cmdBuf, sizeof(cmdBuf), "AT+SQNSSHDN");    //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqShutdown);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("cellular_shutdown: couldn't resolve shutdown"));
        }
    }
    else
    {
        LogDebug(("cellular_shutdown: _Cellular_CheckLibraryStatus failed"));
    }

    return;
}
/**********************************************************************************************************************
 * End of function cellular_shutdown
 *********************************************************************************************************************/
