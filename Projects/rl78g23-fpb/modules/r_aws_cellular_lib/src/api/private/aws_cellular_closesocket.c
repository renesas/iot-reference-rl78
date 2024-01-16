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
 * File Name    : aws_cellular_closesocket.c
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
 * Function Name  @fn            aws_cellular_closesocket
 ************************************************************************************************/
CellularError_t aws_cellular_closesocket(CellularHandle_t cellularHandle)
{
    CellularContext_t * p_context      = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint8_t             cnt            = 0;

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

    CellularAtReq_t     atReqSockClose =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS == cellularStatus)
    {
        /* WAIT_LOOP */
        for (cnt = 0; cnt < CELLULAR_NUM_SOCKET_MAX; cnt++)
        {
            if (NULL != p_context->pSocketData[cnt])
            {
                if (SOCKETSTATE_CONNECTING == p_context->pSocketData[cnt]->socketState)
                {
                    /* Form the AT command. */
                    snprintf((char *)cmdBuf, sizeof(cmdBuf), "AT+SQNSH=%u",   //cast
                             p_context->pSocketData[cnt]->socketId + 1);
                    pktStatus = _Cellular_AtcmdRequestWithCallback(p_context,
                                                                   atReqSockClose);

                    if (CELLULAR_PKT_STATUS_OK != pktStatus)
                    {
                        LogError(("aws_cellular_closesocket: Socket close failed, cmdBuf:%s, PktRet: %d", 
                                    cmdBuf, pktStatus));
                        cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
                    }
                    else
                    {
                        /* Ignore the result from the info, and force to remove the socket. */
                        cellularStatus = _Cellular_RemoveSocketData(p_context, p_context->pSocketData[cnt]);
                    }
                }
            }
        }
    }

    return cellularStatus;
}
/**********************************************************************************************************************
 * End of function aws_cellular_closesocket
 *********************************************************************************************************************/
