/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_aws_cellular_if.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define RM_CELLULAR_RYZ_ENABLE_MODULE_UE_RETRY_COUNT      (3U)
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define RM_CELLULAR_RYZ_ENABLE_MODULE_UE_RETRY_TIMEOUT    (5000U)
#else
#define RM_CELLULAR_RYZ_ENABLE_MODULE_UE_RETRY_TIMEOUT    AWS_CELLULAR_CFG_AT_COMMAND_TIMEOUT
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

static CellularError_t sendAtCommandWithRetryTimeout(CellularContext_t * pContext, const CellularAtReq_t * pAtReq);

const char * CellularSrcTokenErrorTable[] =
{"ERROR", "BUSY", "NO CARRIER", "+CMS ERROR", "+CME ERROR"};
uint32_t CellularSrcTokenErrorTableSize = sizeof(CellularSrcTokenErrorTable) / sizeof(char *);

const char * CellularSrcTokenSuccessTable[]   = {"OK"};
uint32_t     CellularSrcTokenSuccessTableSize = sizeof(CellularSrcTokenSuccessTable) / sizeof(char *);

const char * CellularSrcExtraTokenSuccessTable[]   = {"> "};
uint32_t     CellularSrcExtraTokenSuccessTableSize = sizeof(CellularSrcExtraTokenSuccessTable) / sizeof(char *);

const char * CellularUrcTokenWoPrefixTable[]   = {"RDY", "+SHUTDOWN", "+SYSSTART"};
uint32_t     CellularUrcTokenWoPrefixTableSize = sizeof(CellularUrcTokenWoPrefixTable) / sizeof(char *);

/*******************************************************************************************************************//**
 * Implements Cellular_ModuleInit
 **********************************************************************************************************************/
CellularError_t Cellular_ModuleInit (const CellularContext_t * pContext, void ** ppModuleContext)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularError_t cellular_status = CELLULAR_SUCCESS;

    *ppModuleContext = &g_aws_cellular_ctrl;
    cellular_status  = Cellular_CommonRegisterModemEventCallback((CellularHandle_t)pContext,   //cast
                                                                  aws_cellular_modemevent,
                                                                  pContext->pModueContext);

#if AWS_CELLULAR_CFG_URC_CHARGET_ENABLED == 1
    if (CELLULAR_SUCCESS == cellular_status)
    {
        cellular_status = Cellular_RegisterUrcGenericCallback((CellularHandle_t)pContext,   //cast
                                                                AWS_CELLULAR_CFG_URC_CHARGET_FUNCTION,
                                                                NULL);
    }
#endif

    if (CELLULAR_SUCCESS == cellular_status)
    {
        cellular_status = aws_cellular_hardreset((CellularHandle_t)pContext);   //cast
    }

    return cellular_status;
#else
    CellularError_t cellular_status = CELLULAR_SUCCESS;

    if (NULL == pContext)
    {
        cellular_status = CELLULAR_INVALID_HANDLE;
    }
    else if (NULL == ppModuleContext)
    {
        cellular_status = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        *ppModuleContext = &g_aws_cellular_ctrl;
    }
#if AWS_CELLULAR_CFG_URC_CHARGET_ENABLED == 1
    if (CELLULAR_SUCCESS == cellular_status)
    {
        cellular_status = Cellular_RegisterUrcGenericCallback((CellularHandle_t)pContext,   //cast
                                                                AWS_CELLULAR_CFG_URC_CHARGET_FUNCTION,
                                                                NULL);
    }
#endif

    if (CELLULAR_SUCCESS == cellular_status)
    {
        cellular_status = aws_cellular_hardreset((CellularHandle_t)pContext);   //cast
    }

    return cellular_status;
#endif
}

/*******************************************************************************************************************//**
 * Implements Cellular_ModuleCleanUp
 **********************************************************************************************************************/
CellularError_t Cellular_ModuleCleanUp (const CellularContext_t * pContext)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return Cellular_CommonRegisterModemEventCallback((CellularHandle_t)pContext,   //cast
                                                      NULL,
                                                      NULL);
#else
    CellularError_t cellular_status = CELLULAR_SUCCESS;

    if (NULL == pContext)
    {
        cellular_status = CELLULAR_INVALID_HANDLE;
    }
    else
    {
        cellular_status = aws_cellular_psm_config((CellularContext_t *)pContext, 0);    //cast
    }

    return cellular_status;
#endif
}

/*******************************************************************************************************************//**
 * Implements Cellular_ModuleEnableUE
 **********************************************************************************************************************/
CellularError_t Cellular_ModuleEnableUE (CellularContext_t * pContext)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularError_t cellular_status = CELLULAR_SUCCESS;

    CellularAtReq_t at_req;
    at_req.pAtCmd       = NULL;
    at_req.atCmdType    = CELLULAR_AT_NO_RESULT;
    at_req.pAtRspPrefix = NULL;
    at_req.respCallback = NULL;
    at_req.pData        = NULL;
    at_req.dataLen      = 0;

    /* Disable echo. */
    at_req.pAtCmd = "ATE0";
    at_req.atCmdType = CELLULAR_AT_MULTI_WO_PREFIX;
    cellular_status  = sendAtCommandWithRetryTimeout(pContext, &at_req);
    if (CELLULAR_SUCCESS == cellular_status)
    {
        /* Enable RTS/CTS hardware flow control. */
        at_req.pAtCmd = "AT+IFC=2,2";
        at_req.atCmdType = CELLULAR_AT_NO_RESULT;
        cellular_status  = sendAtCommandWithRetryTimeout(pContext, &at_req);
    }
    return cellular_status;
#else
    CellularError_t cellular_status = CELLULAR_SUCCESS;

    CellularAtReq_t at_req_no_result =
    {
        NULL,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0
    };
    CellularAtReq_t at_req_with_result =
    {
        NULL,
        CELLULAR_AT_MULTI_WO_PREFIX,
        NULL,
        NULL,
        NULL,
        0
    };

    if (NULL != pContext)
    {
        /* Disable echo. */
        at_req_with_result.pAtCmd = "ATE0";
        cellular_status           = sendAtCommandWithRetryTimeout(pContext, &at_req_with_result);

        if (CELLULAR_SUCCESS == cellular_status)
        {
            /* Enable RTS/CTS hardware flow control. */
            at_req_no_result.pAtCmd = "AT+IFC=2,2";
            cellular_status         = sendAtCommandWithRetryTimeout(pContext, &at_req_no_result);
        }
    }
    else
    {
        cellular_status = CELLULAR_INVALID_HANDLE;
    }

    return cellular_status;
#endif
}

/*******************************************************************************************************************//**
 * Implements Cellular_ModuleEnableUrc
 **********************************************************************************************************************/
CellularError_t Cellular_ModuleEnableUrc (CellularContext_t * pContext)
{
    CellularError_t     cellular_status  = CELLULAR_SUCCESS;
    CellularPktStatus_t api_ret          = CELLULAR_PKT_STATUS_OK;

    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t at_req_no_result;
    at_req_no_result.pAtCmd       = NULL;
    at_req_no_result.atCmdType    = CELLULAR_AT_NO_RESULT;
    at_req_no_result.pAtRspPrefix = NULL;
    at_req_no_result.respCallback = NULL;
    at_req_no_result.pData        = NULL;
    at_req_no_result.dataLen      = 0;
#else
    CellularAtReq_t at_req_no_result =
    {
        NULL,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL != pContext)
    {
#endif
        /* Set operator format for PLMN command to numeric */
        at_req_no_result.pAtCmd = "AT+COPS=3,2";
        api_ret = _Cellular_AtcmdRequestWithCallback(pContext, at_req_no_result);

        if (CELLULAR_PKT_STATUS_OK == api_ret)
        {
            /* Enable CEREG URC with format +CEREG: <stat>[,[<tac>],[<ci>],[<AcT>]] */
            (void)snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s=%u", "AT+CEREG", AWS_CELLULAR_CFG_NETWORK_NOTIFY_LEVEL);
            at_req_no_result.pAtCmd = cmdBuf;
            api_ret = _Cellular_AtcmdRequestWithCallback(pContext, at_req_no_result);
        }

        if (CELLULAR_PKT_STATUS_OK == api_ret)
        {
            /* Enable CTZR */
            at_req_no_result.pAtCmd = "AT+CTZR=1";
            api_ret = _Cellular_AtcmdRequestWithCallback(pContext, at_req_no_result);
        }

        cellular_status = _Cellular_TranslatePktStatus(api_ret);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
    else
    {
        cellular_status = CELLULAR_INVALID_HANDLE;
    }
#endif

    return cellular_status;
}

/*******************************************************************************************************************//**
 * Send AT command with retry timeout function.
 *
 * @param[in] pContext  Cellular context pointer
 * @param[in] pAtReq    Pointer to AT request
 *
 * @retval  CELLULAR_SUCCESS        AT command succeeded
 * @retval  CELLULAR_BAD_PARAMETER  Bad paramter passed in
 **********************************************************************************************************************/
static CellularError_t sendAtCommandWithRetryTimeout (CellularContext_t * pContext, const CellularAtReq_t * pAtReq)
{
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint8_t             tryCount       = 0;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pAtReq == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
        for ( ; tryCount < RM_CELLULAR_RYZ_ENABLE_MODULE_UE_RETRY_COUNT; tryCount++)
        {
            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                  *pAtReq,
                                                                  RM_CELLULAR_RYZ_ENABLE_MODULE_UE_RETRY_TIMEOUT);
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

            if (cellularStatus == CELLULAR_SUCCESS)
            {
                break;
            }
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}
