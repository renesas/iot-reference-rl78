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
#include <stdio.h>

/* Cellular module includes. */
#include "r_aws_cellular_if.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#ifndef RM_CELLULAR_RYZ_PDN_ACT_PACKET_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_PDN_ACT_PACKET_REQ_TIMEOUT_MS              (150000UL)
#endif
#ifndef RM_CELLULAR_RYZ_PDN_DEACT_PACKET_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_PDN_DEACT_PACKET_REQ_TIMEOUT_MS            (40000UL)
#endif
#ifndef RM_CELLULAR_RYZ_GPRS_ATTACH_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_GPRS_ATTACH_REQ_TIMEOUT_MS                 (180000UL)
#endif
#ifndef RM_CELLULAR_RYZ_DNS_QUERY_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_DNS_QUERY_REQ_TIMEOUT_MS                   (120000UL)
#endif
#ifndef RM_CELLULAR_RYZ_SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS       (120000U)
#endif
#ifndef RM_CELLULAR_RYZ_PACKET_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_PACKET_REQ_TIMEOUT_MS                      (10000U)
#endif
#ifndef RM_CELLULAR_RYZ_DATA_SEND_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_DATA_SEND_TIMEOUT_MS                       (10000U)
#endif
#ifndef RM_CELLULAR_RYZ_DATA_SEND_INTER_DELAY_MS
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
 #define RM_CELLULAR_RYZ_DATA_SEND_INTER_DELAY_MS                   (150U)
#else
 #define RM_CELLULAR_RYZ_DATA_SEND_INTER_DELAY_MS                   (200U)
#endif
#endif
#ifndef RM_CELLULAR_RYZ_DATA_READ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_DATA_READ_TIMEOUT_MS                       (50000UL)
#endif
#ifndef RM_CELLULAR_RYZ_SOCKET_DISCONNECT_PACKET_REQ_TIMEOUT_MS
 #define RM_CELLULAR_RYZ_SOCKET_DISCONNECT_PACKET_REQ_TIMEOUT_MS    (12000UL)
#endif

#define RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN                    "+SQNSRECV: "
#define RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN_LEN                (11U)
#define RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_STRING_LENGTH            (SOCKET_DATA_PREFIX_TOKEN_LEN + 6U)
#define RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING          (21U)            // The max data prefix string is "+SQNSRECV: 1,1500\r\n"
#define RM_CELLULAR_RYZ_DATA_PREFIX_STRING_CHANGELINE_LENGTH        (2U)             // The length of the change line "\r\n".
#define RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socket_id)    ((uint16_t) socket_id + 1) // Socket ID on modem starts at index 1
#define RM_CELLULAR_RYZ_CRSM_HPLMN_RAT_LENGTH                       (9U)

#define RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX  (19)    /* Maximum Certificate Index Number  */
#define RM_CELLULAR_SECURITY_PROFILE_ID_H      (6)     /* Maximum Security Profile ID Number  */
#define RM_CELLULAR_SECURITY_PROFILE_ID_L      (1)     /* Minimum Security Profile ID Number  */
#define RM_CELLULAR_NVM_CERTIFICATE_SIZE_H     (16384) /* Maximum Certificate File Size */
#define RM_CELLULAR_NVM_CERTIFICATE_SIZE_L     (1)     /* Minimum Certificate File Size */

#define PRINTF_BINARY_PATTERN_INT4          "%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT4( i )          \
    ( ( ( ( i ) & 0x08U ) != 0U ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x04U ) != 0U ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x02U ) != 0U ) ? '1' : '0' ), \
    ( ( ( ( i ) & 0x01U ) != 0U ) ? '1' : '0' )

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct _socketDataRecv
{
    uint32_t * pDataLen;
    uint8_t  * pData;
} _socketDataRecv_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static CellularError_t _Cellular_GetContextActivationStatus(CellularHandle_t              cellularHandle,
                                                            CellularPdnContextActInfo_t * pPdpContextsActInfo);
static CellularError_t     _Cellular_GetPacketSwitchStatus(CellularHandle_t cellularHandle, bool * pPacketSwitchStatus);
static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextActState(CellularContext_t                 * pContext,
                                                                   const CellularATCommandResponse_t * pAtResp,
                                                                   void                              * pData,
                                                                   uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncPacketSwitchStatus(CellularContext_t                 * pContext,
                                                                const CellularATCommandResponse_t * pAtResp,
                                                                void                              * pData,
                                                                uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextSettings(CellularContext_t                 * pContext,
                                                                   const CellularATCommandResponse_t * pAtResp,
                                                                   void                              * pData,
                                                                   uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetSignalInfo(CellularContext_t                 * pContext,
                                                           const CellularATCommandResponse_t * pAtResp,
                                                           void                              * pData,
                                                           uint16_t                            dataLen);
static bool                _parseExtendedSignalQuality(char * pQcsqPayload, CellularSignalInfo_t * pSignalInfo);
static bool                _parseSignalQuality(char * pQcsqPayload, CellularSignalInfo_t * pSignalInfo);
static CellularPktStatus_t _Cellular_RecvFuncResolveDomainToIpAddress(CellularContext_t                 * pContext,
                                                                      const CellularATCommandResponse_t * pAtResp,
                                                                      void                              * pData,
                                                                      uint16_t                            dataLen);
static CellularPktStatus_t socketSendDataPrefix(void * pCallbackContext, char * pLine, uint32_t * pBytesRead);
static CellularPktStatus_t _Cellular_RecvFuncData(CellularContext_t                 * pContext,
                                                  const CellularATCommandResponse_t * pAtResp,
                                                  void                              * pData,
                                                  uint16_t                            dataLen);
static CellularATError_t getDataFromResp(const CellularATCommandResponse_t * pAtResp,
                                         const _socketDataRecv_t           * pDataRecv,
                                         uint32_t                            outBufSize);
static CellularPktStatus_t socketRecvDataPrefix(void     * pCallbackContext,
                                                char     * pLine,
                                                uint32_t   lineLength,
                                                char    ** ppDataStart,
                                                uint32_t * pDataLength);
static bool                _checkCrsmMemoryStatus(const char * pToken);
static bool                _checkCrsmReadStatus(const char * pToken);
static bool                _parseHplmn(char * pToken, void * pData);
static CellularPktStatus_t _Cellular_RecvFuncGetHplmn(CellularContext_t                 * pContext,
                                                      const CellularATCommandResponse_t * pAtResp,
                                                      void                              * pData,
                                                      uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetIccid(CellularContext_t                 * pContext,
                                                      const CellularATCommandResponse_t * pAtResp,
                                                      void                              * pData,
                                                      uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetImsi(CellularContext_t                 * pContext,
                                                     const CellularATCommandResponse_t * pAtResp,
                                                     void                              * pData,
                                                     uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkReg(CellularContext_t                 * pContext,
                                                           const CellularATCommandResponse_t * pAtResp,
                                                           void                              * pData,
                                                           uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncUpdateMccMnc(CellularContext_t                 * pContext,
                                                          const CellularATCommandResponse_t * pAtResp,
                                                          void                              * pData,
                                                          uint16_t                            dataLen);

static CellularPktStatus_t _Cellular_RecvFuncGetPhoneNum (CellularContext_t                 * p_context,
                                                          const CellularATCommandResponse_t * p_atresp,
                                                          void                              * p_data,
                                                          uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncPingEcho (CellularContext_t                 * pContext,
                                                       const CellularATCommandResponse_t * pAtResp,
                                                       void                              * pData,
                                                       uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetOperatorName (CellularContext_t                 * pContext,
                                                              const CellularATCommandResponse_t * pAtResp,
                                                              void                              * pData,
                                                              uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetUpgradeState (CellularContext_t                 * p_context,
                                                              const CellularATCommandResponse_t * p_atresp,
                                                              void                              * p_data,
                                                              uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetCertificate (CellularContext_t                  * pContext,
                                                              const CellularATCommandResponse_t * pAtResp,
                                                              void                              * pData,
                                                              uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetString (CellularContext_t                 * p_context,
                                                        const CellularATCommandResponse_t * p_atresp,
                                                        void                              * p_data,
                                                        uint16_t                            dataLen);
static CellularATError_t _Cellular_ATGetSpecificNextTok (uint8_t      ** pp_String,
                                                         const uint8_t * p_Delimiter,
                                                         uint8_t      ** pp_TokOutput);
static CellularPktStatus_t _Cellular_RecvFuncStartDownLink (CellularContext_t                 * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void                              * pData,
                                                            uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetSVN (CellularContext_t                 * p_context,
                                                     const CellularATCommandResponse_t * p_atresp,
                                                     void                              * p_data,
                                                     uint16_t                            dataLen);
static CellularPktStatus_t _Cellular_RecvFuncGetSocketDataSize (CellularContext_t                 * p_context,
                                                                const CellularATCommandResponse_t * p_atresp,
                                                                void                              * p_data,
                                                                uint16_t                            dataLen);
static bool _parseGetHostByName (char * pQcsqPayload, char * pSignalInfo);
static bool _parseGetPhoneNum (uint8_t * p_QcsqPayload, uint8_t * p_GetPhonenum);

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/* Implementation of Cellular_Init */
CellularError_t Cellular_Init (CellularHandle_t * pCellularHandle, const CellularCommInterface_t * pCommInterface)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    CellularTokenTable_t cellularTokenTable =
    {
        .pCellularUrcHandlerTable              = cellular_urc_handler_table,
        .cellularPrefixToParserMapSize         = cellular_urc_handler_table_size,
        .pCellularSrcTokenErrorTable           = CellularSrcTokenErrorTable,
        .cellularSrcTokenErrorTableSize        = CellularSrcTokenErrorTableSize,
        .pCellularSrcTokenSuccessTable         = CellularSrcTokenSuccessTable,
        .cellularSrcTokenSuccessTableSize      = CellularSrcTokenSuccessTableSize,
        .pCellularUrcTokenWoPrefixTable        = CellularUrcTokenWoPrefixTable,
        .cellularUrcTokenWoPrefixTableSize     = CellularUrcTokenWoPrefixTableSize,
        .pCellularSrcExtraTokenSuccessTable    = CellularSrcExtraTokenSuccessTable,
        .cellularSrcExtraTokenSuccessTableSize = CellularSrcExtraTokenSuccessTableSize
    };

    cellularStatus = Cellular_CommonInit(pCellularHandle, pCommInterface, &cellularTokenTable);

    return cellularStatus;
}

/* Implementation of Cellular_GetSimCardStatus */
CellularError_t Cellular_GetSimCardStatus (CellularHandle_t cellularHandle, CellularSimCardStatus_t * pSimCardStatus)
{
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_GetSimCardStatus : _Cellular_CheckLibraryStatus failed"));
    }
    else if (pSimCardStatus == NULL)
    {
        LogWarn(("Cellular_GetSimCardStatus : Bad input Parameter"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        /* Parameters are checked in this API. */
        pSimCardStatus->simCardState     = CELLULAR_SIM_CARD_UNKNOWN;
        pSimCardStatus->simCardLockState = CELLULAR_SIM_CARD_LOCK_UNKNOWN;

        cellularStatus = Cellular_CommonGetSimCardLockStatus(cellularHandle, pSimCardStatus);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            if ((pSimCardStatus->simCardLockState != CELLULAR_SIM_CARD_INVALID) &&
                (pSimCardStatus->simCardLockState != CELLULAR_SIM_CARD_LOCK_UNKNOWN))
            {
                pSimCardStatus->simCardState = CELLULAR_SIM_CARD_INSERTED;
            }
            else
            {
                pSimCardStatus->simCardState = CELLULAR_SIM_CARD_UNKNOWN;
            }
        }
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_GetSimCardInfo */
CellularError_t Cellular_GetSimCardInfo (CellularHandle_t cellularHandle, CellularSimCardInfo_t * pSimCardInfo)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    CellularAtReq_t     atReqGetIccid  = {0};
    CellularAtReq_t     atReqGetImsi   = {0};
    CellularAtReq_t     atReqGetHplmn  = {0};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogError(("_Cellular_CheckLibraryStatus failed"));
    }
    else if (pSimCardInfo == NULL)
    {
        LogError(("Cellular_GetSimCardInfo : Bad parameter"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
        atReqGetIccid.pAtCmd       = "AT+SQNCCID?";
        atReqGetIccid.atCmdType    = CELLULAR_AT_WITH_PREFIX;
        atReqGetIccid.pAtRspPrefix = "+SQNCCID";
        atReqGetIccid.respCallback = _Cellular_RecvFuncGetIccid;
        atReqGetIccid.pData        = pSimCardInfo->iccid;
        atReqGetIccid.dataLen      = CELLULAR_ICCID_MAX_SIZE + 1U;

        atReqGetImsi.pAtCmd       = "AT+CIMI";
        atReqGetImsi.atCmdType    = CELLULAR_AT_WO_PREFIX;
        atReqGetImsi.pAtRspPrefix = NULL;
        atReqGetImsi.respCallback = _Cellular_RecvFuncGetImsi;
        atReqGetImsi.pData        = pSimCardInfo->imsi;
        atReqGetImsi.dataLen      = CELLULAR_IMSI_MAX_SIZE + 1U;

        atReqGetHplmn.pAtCmd       = "AT+CRSM=176,28514,0,0,0"; /* READ BINARY commmand. HPLMN Selector with Access Technology( 6F62 ). */
        atReqGetHplmn.atCmdType    = CELLULAR_AT_WITH_PREFIX;
        atReqGetHplmn.pAtRspPrefix = "+CRSM";
        atReqGetHplmn.respCallback = _Cellular_RecvFuncGetHplmn;
        atReqGetHplmn.pData        = &pSimCardInfo->plmn;
        atReqGetHplmn.dataLen      = (uint16_t) sizeof(CellularPlmnInfo_t);

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        (void) memset(pSimCardInfo, 0, sizeof(CellularSimCardInfo_t));
        pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqGetImsi);

        if (pktStatus == CELLULAR_PKT_STATUS_OK)
        {
            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqGetHplmn);
        }

        if (pktStatus == CELLULAR_PKT_STATUS_OK)
        {
            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqGetIccid);
        }

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            LogDebug(("SimInfo updated: IMSI:%s, Hplmn:%s%s, ICCID:%s",
                      pSimCardInfo->imsi, pSimCardInfo->plmn.mcc, pSimCardInfo->plmn.mnc,
                      pSimCardInfo->iccid));
        }
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_GetPdnStatus */
CellularError_t Cellular_GetPdnStatus (CellularHandle_t      cellularHandle,
                                       CellularPdnStatus_t * pPdnStatusBuffers,
                                       uint8_t               numStatusBuffers,
                                       uint8_t             * pNumStatus)
{
    CellularContext_t         * pContext           = (CellularContext_t *) cellularHandle;
    CellularError_t             cellularStatus     = CELLULAR_SUCCESS;
    CellularPktStatus_t         pktStatus          = CELLULAR_PKT_STATUS_OK;
    CellularPdnContextActInfo_t pdpContextsActInfo = {0};
    CellularPdnContextInfo_t    pdpContextsInfo    = {0};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t                  semaphore_ret      = 0;
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus(pContext);
    }

    if ((pPdnStatusBuffers == NULL) || (pNumStatus == NULL) || (numStatusBuffers < 1))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
        LogWarn(("_Cellular_GetPdnStatus: Bad input Parameter "));
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        *pNumStatus = 0;

        /* Check the current <Act> status of contexts. */
        cellularStatus = _Cellular_GetContextActivationStatus(cellularHandle, &pdpContextsActInfo);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Get PDP config info */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            CellularAtReq_t atReqGetCurrentApnName;
            atReqGetCurrentApnName.pAtCmd       = "AT+CGDCONT?";
            atReqGetCurrentApnName.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
            atReqGetCurrentApnName.pAtRspPrefix = "+CGDCONT";
            atReqGetCurrentApnName.respCallback = _Cellular_RecvFuncGetPdpContextSettings;
            atReqGetCurrentApnName.pData        = &pdpContextsInfo;
            atReqGetCurrentApnName.dataLen      = sizeof(CellularPdnContextInfo_t);
#else
            CellularAtReq_t atReqGetCurrentApnName =
            {
                "AT+CGDCONT?",
                CELLULAR_AT_MULTI_WITH_PREFIX,
                "+CGDCONT",
                _Cellular_RecvFuncGetPdpContextSettings,
                &pdpContextsInfo,
                sizeof(CellularPdnContextInfo_t),
            };
#endif

            pktStatus      = _Cellular_AtcmdRequestWithCallback(pContext, atReqGetCurrentApnName);
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

            if (cellularStatus == CELLULAR_SUCCESS)
            {
                for (uint8_t i = 0; i < RM_CELLULAR_RYZ_MAX_PDP_CONTEXTS; i++)
                {
                    if (*pNumStatus == numStatusBuffers)
                    {
                        /* stop if the number of status buffers has been filled */
                        break;
                    }

                    if (pdpContextsInfo.contextsPresent[i] && pdpContextsActInfo.contextsPresent[i])
                    {
                        /* Set CID */
                        pPdnStatusBuffers[i].contextId = (uint8_t) (i + 1);

                        /* Get IP Address */
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
                        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
                        cellularStatus = Cellular_GetIPAddress(cellularHandle,
                                                               (uint8_t) (i + 1),
                                                               pPdnStatusBuffers[i].ipAddress.ipAddress,
                                                               CELLULAR_IP_ADDRESS_MAX_SIZE + 1);
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
                        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif
                        if (cellularStatus != CELLULAR_SUCCESS)
                        {
                            break;
                        }

                        /* Set IP type */
                        if (0 == strcmp(pdpContextsInfo.ipType[i], "IPV4"))
                        {
                            pPdnStatusBuffers[i].ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
                            pPdnStatusBuffers[i].pdnContextType          = CELLULAR_PDN_CONTEXT_IPV4;
                        }
                        else if (0 == strcmp(pdpContextsInfo.ipType[i], "IPV6"))
                        {
                            pPdnStatusBuffers[i].ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V6;
                            pPdnStatusBuffers[i].pdnContextType          = CELLULAR_PDN_CONTEXT_IPV6;
                        }
                        else
                        {
                            pPdnStatusBuffers[i].ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
                            pPdnStatusBuffers[i].pdnContextType          = CELLULAR_PDN_CONTEXT_IPV4V6;
                        }

                        /* Set activation state */
                        LogDebug(("Context [%d], Act State [%d]\r\n", i + 1,
                                  pdpContextsActInfo.contextActState[i]));
                        pPdnStatusBuffers[i].state = pdpContextsActInfo.contextActState[i];

                        (*pNumStatus)++;
                    }
                }
            }
        }
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_ActivatePdn */
CellularError_t Cellular_ActivatePdn (CellularHandle_t cellularHandle, uint8_t contextId)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    CellularPdnContextActInfo_t pdpContextsActInfo = {0};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqActPdn;
    atReqActPdn.pAtCmd       = cmdBuf;
    atReqActPdn.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqActPdn.pAtRspPrefix = NULL;
    atReqActPdn.respCallback = NULL;
    atReqActPdn.pData        = NULL;
    atReqActPdn.dataLen      = 0;
#else
    CellularAtReq_t atReqActPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    cellularStatus = _Cellular_IsValidPdn(contextId);

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus(pContext);
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        /* Check the current <Act> status of context. If not activated, activate the PDN context ID. */
        cellularStatus = _Cellular_GetContextActivationStatus(cellularHandle, &pdpContextsActInfo);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Activate context if not already active */
            if (pdpContextsActInfo.contextActState[contextId - 1] == false)
            {
                (void) snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s=1,%u", "AT+CGACT", contextId);
                pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                      atReqActPdn,
                                                                      RM_CELLULAR_RYZ_PDN_ACT_PACKET_REQ_TIMEOUT_MS);

                if (pktStatus != CELLULAR_PKT_STATUS_OK)
                {
                    LogError(("Cellular_ActivatePdn: can't activate PDN, PktRet: %d", pktStatus));
                    cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
                }
            }
            else
            {
                LogInfo(("Cellular_ActivatePdn: Context id [%d] is already active", contextId));
            }
        }
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_DeactivatePdn */
CellularError_t Cellular_DeactivatePdn (CellularHandle_t cellularHandle, uint8_t contextId)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
    bool                packetSwitchStatus               = false;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    CellularServiceStatus_t     serviceStatus;
    CellularPdnContextActInfo_t pdpContextsActInfo = {0};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqDeactPdn;
    atReqDeactPdn.pAtCmd       = cmdBuf;
    atReqDeactPdn.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqDeactPdn.pAtRspPrefix = NULL;
    atReqDeactPdn.respCallback = NULL;
    atReqDeactPdn.pData        = NULL;
    atReqDeactPdn.dataLen      = 0;
#else
    CellularAtReq_t atReqDeactPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

    memset(&serviceStatus, 0, sizeof(serviceStatus));

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    cellularStatus = _Cellular_IsValidPdn(contextId);

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus(pContext);
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* Get current network operator settings. */
        cellularStatus = Cellular_GetServiceStatus(cellularHandle, &serviceStatus);

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Check the current <Act> status of context. */
            cellularStatus = _Cellular_GetContextActivationStatus(cellularHandle, &pdpContextsActInfo);
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            LogDebug(("Cellular_DeactivatePdn: Listing operator and context details below."));

            /* Deactivate context if active */
            if (pdpContextsActInfo.contextActState[contextId - 1] == true)
            {
                /* Don't deactivate LTE default bearer context */
                /* Otherwise sending AT command "+CGACT=0,1" for deactivation will result in ERROR */
                if ((serviceStatus.rat >= CELLULAR_RAT_LTE) && (contextId == RM_CELLULAR_RYZ_DEFAULT_BEARER_CONTEXT_ID))
                {
                    LogInfo(("Cellular_DeactivatePdn: Default Bearer context %d Active. Not allowed to deactivate.",
                             contextId));
                    cellularStatus = CELLULAR_NOT_ALLOWED;
                }
                else
                {
                    (void) snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s=0,%u", "AT+CGACT", contextId);
                    pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                          atReqDeactPdn,
                                                                          RM_CELLULAR_RYZ_PDN_DEACT_PACKET_REQ_TIMEOUT_MS);
                    cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
                }

                if ((cellularStatus != CELLULAR_SUCCESS) && (cellularStatus != CELLULAR_NOT_ALLOWED))
                {
                    LogError(("Cellular_DeactivatePdn: can't deactivate PDN, PktRet: %d", pktStatus));

                    /* Sometimes +CGACT deactivation fails in 2G. Then check packet switch attach. If attached, detach packet switch. */
                    if ((serviceStatus.rat == CELLULAR_RAT_GSM) || (serviceStatus.rat == CELLULAR_RAT_EDGE))
                    {
                        cellularStatus = _Cellular_GetPacketSwitchStatus(cellularHandle, &packetSwitchStatus);

                        if ((cellularStatus == CELLULAR_SUCCESS) && (packetSwitchStatus == true))
                        {
                            LogError(("Deactivate Packet switch"));
                            (void) snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "%s", "AT+CGATT=0");
                            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                                  atReqDeactPdn,
                                                                                  RM_CELLULAR_RYZ_GPRS_ATTACH_REQ_TIMEOUT_MS);
                            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
                        }
                        else if (cellularStatus != CELLULAR_SUCCESS)
                        {
                            LogError(("Packet switch query failed"));
                            pktStatus      = CELLULAR_PKT_STATUS_FAILURE;
                            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
                        }
                        else
                        {
                            LogInfo(("Packet switch detached"));
                        }
                    }
                }
            }
            else
            {
                LogInfo(("Cellular_DeactivatePdn: Context id [%d] is already deactive", contextId));
            }
        }
        else
        {
            LogError(("Cellular_DeactivatePdn: Unable to list operator and context details."));
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_GetSignalInfo */
CellularError_t Cellular_GetSignalInfo (CellularHandle_t cellularHandle, CellularSignalInfo_t * pSignalInfo)
{
    CellularContext_t * pContext             = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus       = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus            = CELLULAR_PKT_STATUS_OK;
    CellularRat_t       rat                  = CELLULAR_RAT_INVALID;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret        = 0;
#endif
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQuerySignalInfo;
    atReqQuerySignalInfo.pAtCmd       = "AT+CSQ";
    atReqQuerySignalInfo.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqQuerySignalInfo.pAtRspPrefix = "+CSQ";
    atReqQuerySignalInfo.respCallback = _Cellular_RecvFuncGetSignalInfo;
    atReqQuerySignalInfo.pData        = pSignalInfo;
    atReqQuerySignalInfo.dataLen      = sizeof(CellularSignalInfo_t);

    CellularAtReq_t atReqQueryExtendedSignalInfo;
    atReqQueryExtendedSignalInfo.pAtCmd       = "AT+CESQ";
    atReqQueryExtendedSignalInfo.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqQueryExtendedSignalInfo.pAtRspPrefix = "+CESQ";
    atReqQueryExtendedSignalInfo.respCallback = _Cellular_RecvFuncGetSignalInfo;
    atReqQueryExtendedSignalInfo.pData        = pSignalInfo;
    atReqQueryExtendedSignalInfo.dataLen      = sizeof(CellularSignalInfo_t);
#else
    CellularAtReq_t     atReqQuerySignalInfo =
    {
        "AT+CSQ",
        CELLULAR_AT_WITH_PREFIX,
        "+CSQ",
        _Cellular_RecvFuncGetSignalInfo,
        pSignalInfo,
        sizeof(CellularSignalInfo_t),
    };
    CellularAtReq_t atReqQueryExtendedSignalInfo =
    {
        "AT+CESQ",
        CELLULAR_AT_WITH_PREFIX,
        "+CESQ",
        _Cellular_RecvFuncGetSignalInfo,
        pSignalInfo,
        sizeof(CellularSignalInfo_t),
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_GetSignalInfo: _Cellular_CheckLibraryStatus failed"));
    }
    else if (pSignalInfo == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
        cellularStatus = _Cellular_GetCurrentRat(pContext, &rat);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        /* Get +CSQ response */
        pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqQuerySignalInfo);

        if (pktStatus == CELLULAR_PKT_STATUS_OK)
        {
            /* Get +CESQ response */
            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqQueryExtendedSignalInfo);

            if (pktStatus == CELLULAR_PKT_STATUS_OK)
            {
                /* If the convert failed, the API will return CELLULAR_INVALID_SIGNAL_BAR_VALUE in bars field. */
                (void) _Cellular_ComputeSignalBars(rat, pSignalInfo);
            }

            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_GetHostByName */
CellularError_t Cellular_GetHostByName (CellularHandle_t cellularHandle,
                                        uint8_t          contextId,
                                        const char     * pcHostName,
                                        char           * pResolvedAddress)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQueryDns;
    atReqQueryDns.pAtCmd       = cmdBuf;
    atReqQueryDns.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
    atReqQueryDns.pAtRspPrefix = "+SQNDNSLKUP";
    atReqQueryDns.respCallback = _Cellular_RecvFuncResolveDomainToIpAddress;
    atReqQueryDns.pData        = pResolvedAddress;
    atReqQueryDns.dataLen      = CELLULAR_IP_ADDRESS_MAX_SIZE;
#else
    CellularAtReq_t atReqQueryDns =
    {
        cmdBuf,
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+SQNDNSLKUP",
        _Cellular_RecvFuncResolveDomainToIpAddress,
        pResolvedAddress,
        CELLULAR_IP_ADDRESS_MAX_SIZE,
    };
#endif

    (void)(contextId);

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_GetHostByName: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if ((pcHostName == NULL) || (pResolvedAddress == NULL))
        {
            LogError(("Cellular_GetHostByName: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif
        (void) snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNDNSLKUP=\"%s\"", pcHostName);

        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                              atReqQueryDns,
                                                              RM_CELLULAR_RYZ_DNS_QUERY_REQ_TIMEOUT_MS);

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            LogError(("Cellular_GetHostByName: couldn't resolve host name"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_SocketConnect */
CellularError_t Cellular_SocketConnect (CellularHandle_t                cellularHandle,
                                        CellularSocketHandle_t          socketHandle,
                                        CellularSocketAccessMode_t      dataAccessMode,
                                        const CellularSocketAddress_t * pRemoteSocketAddress)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSocketConnect;
    atReqSocketConnect.pAtCmd       = cmdBuf;
    atReqSocketConnect.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqSocketConnect.pAtRspPrefix = NULL;
    atReqSocketConnect.respCallback = NULL;
    atReqSocketConnect.pData        = NULL;
    atReqSocketConnect.dataLen      = 0;
#else
    CellularAtReq_t     atReqSocketConnect               =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

    CellularSocketOpenCallback_t openCallback = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_SocketConnect: _Cellular_CheckLibraryStatus failed"));
    }
    else if (pRemoteSocketAddress == NULL)
    {
        LogDebug(("Cellular_SocketConnect: Invalid socket address"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if (socketHandle == NULL)
    {
        LogDebug(("Cellular_SocketConnect: Invalid socket handle"));
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
#endif
        if (socketHandle->socketState != SOCKETSTATE_ALLOCATED)
        {
            LogError(("storeAccessModeAndAddress, bad socket state %d",
                      socketHandle->socketState));
            cellularStatus = CELLULAR_INTERNAL_FAILURE;
        }
        else if (dataAccessMode != CELLULAR_ACCESSMODE_BUFFER)
        {
            LogError(("storeAccessModeAndAddress, Access mode not supported %d",
                      dataAccessMode));
            cellularStatus = CELLULAR_UNSUPPORTED;
        }
        else
        {
            socketHandle->remoteSocketAddress.port = pRemoteSocketAddress->port;
            socketHandle->dataMode                 = dataAccessMode;
            socketHandle->remoteSocketAddress.ipAddress.ipAddressType =
                pRemoteSocketAddress->ipAddress.ipAddressType;
            (void) strncpy(socketHandle->remoteSocketAddress.ipAddress.ipAddress,
                           pRemoteSocketAddress->ipAddress.ipAddress,
                           CELLULAR_IP_ADDRESS_MAX_SIZE + 1U);

            if (socketHandle->openCallback != NULL)
            {
                openCallback = socketHandle->openCallback;
            }
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    /* Configure the socket. */
    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        /* AT+SQNSCFG=<connId>,<cid>,<pktSz>,<maxTo(1s)>,<connTo(0.1s)>,<txTo(0.1s)>*/
        snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSCFG=%u,%d,0,90,600,50",
                 RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId), socketHandle->contextId);

        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                              atReqSocketConnect,
                                                              RM_CELLULAR_RYZ_SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS);

        if (pktStatus == CELLULAR_PKT_STATUS_OK)
        {
            snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSCFGEXT=%u,0,0,0",
                     RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId));

            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                  atReqSocketConnect,
                                                                  RM_CELLULAR_RYZ_SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                LogError(("Cellular_SocketConnect: Socket configure failed, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
                cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
            }
        }
        else
        {
            LogError(("Cellular_SocketConnect: Socket configure failed, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    /* Start the connection. */
    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        uint8_t protocol = (CELLULAR_SOCKET_PROTOCOL_TCP == socketHandle->socketProtocol) ? 0 : 1;

        snprintf(cmdBuf,
                 CELLULAR_AT_CMD_MAX_SIZE,
                 "AT+SQNSD=%u,%u,%u,\"%s\",0,%d,1,0",
                 RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId),
                 protocol,
                 socketHandle->remoteSocketAddress.port,
                 socketHandle->remoteSocketAddress.ipAddress.ipAddress,
                 socketHandle->localPort);

        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                              atReqSocketConnect,
                                                              RM_CELLULAR_RYZ_SOCKET_CONNECT_PACKET_REQ_TIMEOUT_MS);

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            LogError(("Cellular_SocketConnect: Socket connect failed, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            socketHandle->socketState = SOCKETSTATE_CONNECTED;
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    /* Call callback */
    if (openCallback != NULL)
    {
        if (CELLULAR_SUCCESS == cellularStatus)
        {
            openCallback(CELLULAR_URC_SOCKET_OPENED, socketHandle, socketHandle->pOpenCallbackContext);
        }
        else
        {
            openCallback(CELLULAR_URC_SOCKET_OPEN_FAILED, socketHandle, socketHandle->pOpenCallbackContext);
        }
    }

    return cellularStatus;
}

/* Implementation of Cellular_SocketSend */
CellularError_t Cellular_SocketSend (CellularHandle_t       cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     const uint8_t        * pData,
                                     uint32_t               dataLength,
                                     uint32_t             * pSentDataLength)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint32_t            sendTimeout    = RM_CELLULAR_RYZ_DATA_SEND_TIMEOUT_MS;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSocketSend;
    atReqSocketSend.pAtCmd       = cmdBuf;
    atReqSocketSend.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqSocketSend.pAtRspPrefix = NULL;
    atReqSocketSend.respCallback = NULL;
    atReqSocketSend.pData        = NULL;
    atReqSocketSend.dataLen      = 0;

    CellularAtDataReq_t atDataReqSocketSend;
    atDataReqSocketSend.pData           = pData;
    atDataReqSocketSend.dataLen         = dataLength;
    atDataReqSocketSend.pSentDataLength = pSentDataLength;
    atDataReqSocketSend.pEndPattern     = NULL;
    atDataReqSocketSend.endPatternLen   = 0;
#else
    CellularAtReq_t     atReqSocketSend =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    CellularAtDataReq_t atDataReqSocketSend =
    {
        pData,
        dataLength,
        pSentDataLength,
        NULL,
        0
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_SocketSend: _Cellular_CheckLibraryStatus failed"));
    }
    else if (socketHandle == NULL)
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if ((pData == NULL) || (pSentDataLength == NULL) || (dataLength == 0U))
    {
        LogDebug(("Cellular_SocketSend: Invalid parameter"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else if (socketHandle->socketState != SOCKETSTATE_CONNECTED)
#else
    if (socketHandle->socketState != SOCKETSTATE_CONNECTED)
#endif
    {
        /* Check the socket connection state. */
        if ((socketHandle->socketState == SOCKETSTATE_ALLOCATED) ||
            (socketHandle->socketState == SOCKETSTATE_CONNECTING))
        {
            cellularStatus = CELLULAR_SOCKET_NOT_CONNECTED;
        }
        else
        {
            cellularStatus = CELLULAR_SOCKET_CLOSED;
        }
    }
    else
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        /* Send data length check. */
        if (dataLength > (uint32_t) CELLULAR_MAX_SEND_DATA_LEN)
        {
            atDataReqSocketSend.dataLen = (uint32_t) CELLULAR_MAX_SEND_DATA_LEN;
        }

        /* Check send timeout. If not set by setsockopt, use default value. */
        if (socketHandle->sendTimeoutMs != 0U)
        {
            sendTimeout = socketHandle->sendTimeoutMs;
        }

        snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSSENDEXT=%u,%u",
                 RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId), (uint16_t) atDataReqSocketSend.dataLen);

        pktStatus = _Cellular_AtcmdDataSend(pContext,
                                            atReqSocketSend,
                                            atDataReqSocketSend,
                                            socketSendDataPrefix,
                                            NULL,
                                            RM_CELLULAR_RYZ_PACKET_REQ_TIMEOUT_MS,
                                            sendTimeout,
                                            RM_CELLULAR_RYZ_DATA_SEND_INTER_DELAY_MS);

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            if (socketHandle->socketState == SOCKETSTATE_DISCONNECTED)
            {
                LogInfo(("Cellular_SocketSend: Data send fail, socket already closed"));
                cellularStatus = CELLULAR_SOCKET_CLOSED;
            }
            else
            {
                LogError(("Cellular_SocketSend: Data send fail, PktRet: %d", pktStatus));
                cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_SocketRecv */
CellularError_t Cellular_SocketRecv (CellularHandle_t       cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     uint8_t              * pBuffer,
                                     uint32_t               bufferLength,
                                     uint32_t             * pReceivedDataLength)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
    uint32_t            recvTimeout     = RM_CELLULAR_RYZ_DATA_READ_TIMEOUT_MS;
    uint32_t            recvLen         = bufferLength;
    _socketDataRecv_t   dataRecv        = {0};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret   = 0;
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSocketRecv;
    atReqSocketRecv.pAtCmd       = cmdBuf;
    atReqSocketRecv.atCmdType    = CELLULAR_AT_MULTI_DATA_WO_PREFIX;
    atReqSocketRecv.pAtRspPrefix = "+SQNSRECV";
    atReqSocketRecv.respCallback = _Cellular_RecvFuncData;
    atReqSocketRecv.pData        = (void *) &dataRecv;
    atReqSocketRecv.dataLen      = (uint16_t) bufferLength;
#else
    CellularAtReq_t     atReqSocketRecv =
    {
        cmdBuf,
        CELLULAR_AT_MULTI_DATA_WO_PREFIX,
        "+SQNSRECV",
        _Cellular_RecvFuncData,
        (void *) &dataRecv,
        (uint16_t) bufferLength,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("Cellular_SocketRecv: _Cellular_CheckLibraryStatus failed"));
    }
    else if (socketHandle == NULL)
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else if ((pBuffer == NULL) || (pReceivedDataLength == NULL) || (bufferLength == 0U))
    {
        LogDebug(("Cellular_SocketRecv: Bad input Param"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        dataRecv.pDataLen = pReceivedDataLength;
        dataRecv.pData    = pBuffer;

        /* Update recvLen to maximum module length. */
        if (CELLULAR_MAX_RECV_DATA_LEN <= bufferLength)
        {
            recvLen = (uint32_t) CELLULAR_MAX_RECV_DATA_LEN;
        }

        if (recvLen > 0)
        {
            /* Update receive timeout to default timeout if not set with setsocketopt. */
            if (socketHandle->recvTimeoutMs > 1U)
            {
                recvTimeout = socketHandle->recvTimeoutMs;
            }

            snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSRECV=%u,%u",
                     RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId), (uint16_t) recvLen);
            pktStatus = _Cellular_TimeoutAtcmdDataRecvRequestWithCallback(pContext,
                                                                          atReqSocketRecv,
                                                                          recvTimeout,
                                                                          socketRecvDataPrefix,
                                                                          NULL);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                /* Reset data handling parameters. */
                LogError(("Cellular_SocketRecv: Data Receive fail, pktStatus: %d", pktStatus));
                cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_SocketClose */
CellularError_t Cellular_SocketClose (CellularHandle_t cellularHandle, CellularSocketHandle_t socketHandle)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSockClose;
    atReqSockClose.pAtCmd       = cmdBuf;
    atReqSockClose.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqSockClose.pAtRspPrefix = NULL;
    atReqSockClose.respCallback = NULL;
    atReqSockClose.pData        = NULL;
    atReqSockClose.dataLen      = 0;
#else
    CellularAtReq_t     atReqSockClose =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogDebug(("_Cellular_CheckLibraryStatus failed"));
    }
    else if (socketHandle == NULL)
    {
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        if ((socketHandle->socketState == SOCKETSTATE_CONNECTING) ||
            (socketHandle->socketState == SOCKETSTATE_CONNECTED) ||
            (socketHandle->socketState == SOCKETSTATE_DISCONNECTED))
        {
            /* Form the AT command. */
            snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSH=%u",
                     RM_CELLULAR_RYZ_GET_MODEM_SOCKET_ID(socketHandle->socketId));
            pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                                  atReqSockClose,
                                                                  RM_CELLULAR_RYZ_SOCKET_DISCONNECT_PACKET_REQ_TIMEOUT_MS);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                LogError(("Cellular_SocketClose: Socket close failed, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
            }
        }

        /* Ignore the result from the info, and force to remove the socket. */
        cellularStatus = _Cellular_RemoveSocketData(pContext, socketHandle);

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_GetServiceStatus */
CellularError_t Cellular_GetServiceStatus (CellularHandle_t cellularHandle, CellularServiceStatus_t * pServiceStatus)
{
    CellularContext_t     * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t         cellularStatus = CELLULAR_SUCCESS;
    CellularServiceStatus_t serviceStatus;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t              semaphore_ret  = 0;
#endif

    (void) memset(&serviceStatus, 0, sizeof(CellularServiceStatus_t));

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* pContext is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(pContext);

    if (cellularStatus != CELLULAR_SUCCESS)
    {
        LogError(("_Cellular_CheckLibraryStatus failed"));
    }
    else if (pServiceStatus == NULL)
    {
        LogError(("Cellular_GetServiceStatus : Bad parameter"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        const cellularAtData_t * pLibAtData     = NULL;
        CellularPktStatus_t      pktStatus      = CELLULAR_PKT_STATUS_OK;
        CellularAtReq_t          atReqCommon = {0};

        atReqCommon.pAtCmd       = "AT+CEREG?";
        atReqCommon.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
        atReqCommon.pAtRspPrefix = "+CEREG";
        atReqCommon.respCallback = _Cellular_RecvFuncGetNetworkReg;
        atReqCommon.pData        = NULL;
        atReqCommon.dataLen      = 0;

        pktStatus      = _Cellular_AtcmdRequestWithCallback(pContext, atReqCommon);
        cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

        /* Get the service status from lib AT data. */
        if (cellularStatus == CELLULAR_SUCCESS)
        {
            pLibAtData = &pContext->libAtData;
            _Cellular_LockAtDataMutex(pContext);
            pServiceStatus->rat = pLibAtData->rat;
            pServiceStatus->csRegistrationStatus = pLibAtData->csRegStatus;
            pServiceStatus->psRegistrationStatus = pLibAtData->psRegStatus;
            pServiceStatus->csRejectionCause     = pLibAtData->csRejCause;
            pServiceStatus->csRejectionType      = pLibAtData->csRejectType;
            pServiceStatus->psRejectionCause     = pLibAtData->psRejCause;
            pServiceStatus->psRejectionType      = pLibAtData->psRejectType;
            _Cellular_UnlockAtDataMutex(pContext);

            atReqCommon.pAtCmd       = "AT+COPS?";
            atReqCommon.atCmdType    = CELLULAR_AT_WITH_PREFIX;
            atReqCommon.pAtRspPrefix = "+COPS";
            atReqCommon.respCallback = _Cellular_RecvFuncUpdateMccMnc;
            atReqCommon.pData        = &serviceStatus;
            atReqCommon.dataLen      = (uint16_t) sizeof(CellularServiceStatus_t);

            pktStatus      = _Cellular_AtcmdRequestWithCallback(pContext, atReqCommon);
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

            if (cellularStatus == CELLULAR_SUCCESS)
            {
                /* Service status data from operator info. */
                pServiceStatus->networkRegistrationMode = serviceStatus.networkRegistrationMode;
                pServiceStatus->plmnInfo                = serviceStatus.plmnInfo;
                (void) strncpy(pServiceStatus->operatorName, serviceStatus.operatorName, sizeof(serviceStatus.operatorName));
                pServiceStatus->operatorNameFormat = serviceStatus.operatorNameFormat;

                LogDebug(("SrvStatus: rat %d cs %d, ps %d, mode %d, csRejType %d,",
                          pServiceStatus->rat,
                          pServiceStatus->csRegistrationStatus,
                          pServiceStatus->psRegistrationStatus,
                          pServiceStatus->networkRegistrationMode,
                          pServiceStatus->csRejectionType));

                LogDebug(("csRej %d, psRejType %d, psRej %d, plmn %s%s",
                          pServiceStatus->csRejectionCause,
                          pServiceStatus->psRejectionType,
                          pServiceStatus->psRejectionCause,
                          pServiceStatus->plmnInfo.mcc,
                          pServiceStatus->plmnInfo.mnc));
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/*******************************************************************************************************************//**
 * Callback to process AT+CEREG response.
 *
 * @param[in] pContext  Pointer to cellular context.
 * @param[in] pAtResp   Pointer to AT command response.
 * @param[in] pData     Pointer to where to store parsed data.
 * @param[in] dataLen   Length of where to store parsed data.
 *
 * @retval  CELLULAR_PKT_STATUS_OK          Response processing succeeded
 * @retval  CELLULAR_PKT_STATUS_FAILURE     Parsing failure
 * @retval  CELLULAR_PKT_STATUS_BAD_PARAM   Bad parameter passed to callback
 **********************************************************************************************************************/
static CellularPktStatus_t _Cellular_RecvFuncGetNetworkReg (CellularContext_t                 * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void                              * pData,
                                                            uint16_t                            dataLen)
{
    char              * pPregLine    = NULL;
    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

    (void)(pData);
    (void)(dataLen);

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("_Cellular_RecvFuncGetNetworkReg: response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        pPregLine    = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces(&pPregLine);
        pktStatus    = _Cellular_TranslateAtCoreStatus(atCoreStatus);

        if (pktStatus == CELLULAR_PKT_STATUS_OK)
        {
            /* Assumption is that the data is null terminated so we don't need the dataLen. */
            _Cellular_LockAtDataMutex(pContext);
            pktStatus = _Cellular_ParseRegStatus(pContext, pPregLine, false, CELLULAR_REG_TYPE_CEREG);
            pContext->libAtData.csRegStatus = pContext->libAtData.psRegStatus;
            _Cellular_UnlockAtDataMutex(pContext);
        }

        LogDebug(("atcmd network register status pktStatus:%d", pktStatus));
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*******************************************************************************************************************//**
 * Callback to process AT+COPS response.
 *
 * @param[in] pContext  Pointer to cellular context.
 * @param[in] pAtResp   Pointer to AT command response.
 * @param[in] pData     Pointer to where to store parsed data.
 * @param[in] dataLen   Length of where to store parsed data.
 *
 * @retval  CELLULAR_PKT_STATUS_OK          Response processing succeeded
 * @retval  CELLULAR_PKT_STATUS_FAILURE     Parsing failure
 * @retval  CELLULAR_PKT_STATUS_BAD_PARAM   Bad parameter passed to callback
 **********************************************************************************************************************/
static CellularPktStatus_t _Cellular_RecvFuncUpdateMccMnc (CellularContext_t                 * pContext,
                                                           const CellularATCommandResponse_t * pAtResp,
                                                           void                              * pData,
                                                           uint16_t                            dataLen)
{
    char                   * pCopsResponse = NULL;
    CellularPktStatus_t      pktStatus     = CELLULAR_PKT_STATUS_OK;
    CellularATError_t        atCoreStatus  = CELLULAR_AT_SUCCESS;
    CellularServiceStatus_t * pserviceStatus = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("UpdateMccMnc: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pData == NULL) || (dataLen != sizeof(CellularServiceStatus_t)))
    {
        LogError(("UpdateMccMnc: pData is invalid or dataLen is wrong"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        pCopsResponse = pAtResp->pItm->pLine;
        pserviceStatus = (CellularServiceStatus_t *) pData;

        /* Remove COPS Prefix. */
        atCoreStatus = Cellular_ATRemovePrefix(&pCopsResponse);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing all the Quotes from the COPS response. */
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote(pCopsResponse);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing all Space from the COPS response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pCopsResponse);
        }

        /* parse all the data from cops. */
        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            char * pToken            = NULL;
            char * pTempCopsResponse = pCopsResponse;
            bool   parseStatus       = true;

            /* Getting next token from COPS response. */
            atCoreStatus = Cellular_ATGetNextTok(&pTempCopsResponse, &pToken);

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                int32_t var = 0;

                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &var);

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    if ((var >= 0) && (var < (int32_t) REGISTRATION_MODE_MAX))
                    {
                        /* Variable "var" is ensured that it is valid and within
                         * a valid range. Hence, assigning the value of the variable to
                         * networkRegMode with a enum cast. */

                        /* coverity[misra_c_2012_rule_10_5_violation] */
                        pserviceStatus->networkRegistrationMode = (CellularNetworkRegistrationMode_t) var;
                    }
                    else
                    {
                        LogError(("_parseCopsRegMode: Error in processing Network Registration mode. Token %s",
                                  pToken));
                        parseStatus = false;
                    }
                }

                if (parseStatus == false)
                {
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATGetNextTok(&pTempCopsResponse, &pToken);
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                int32_t var = 0;

                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &var);

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    if ((var >= 0) &&
                        (var < (int32_t) OPERATOR_NAME_FORMAT_MAX))
                    {
                        /* Variable "var" is ensured that it is valid and within
                         * a valid range. Hence, assigning the value of the variable to
                         * operatorNameFormat with a enum cast. */

                        /* coverity[misra_c_2012_rule_10_5_violation] */
                        pserviceStatus->operatorNameFormat = (CellularOperatorNameFormat_t) var;
                    }
                    else
                    {
                        LogError((
                                     "_parseCopsNetworkNameFormat: Error in processing Network Registration mode. Token %s",
                                     pToken));
                        parseStatus = false;
                    }
                }

                if (parseStatus == false)
                {
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATGetNextTok(&pTempCopsResponse, &pToken);
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                uint32_t mccMncLen = 0U;

                if ((pserviceStatus->operatorNameFormat == OPERATOR_NAME_FORMAT_LONG) ||
                    (pserviceStatus->operatorNameFormat == OPERATOR_NAME_FORMAT_SHORT))
                {
                    (void) strncpy(pserviceStatus->operatorName, pToken, CELLULAR_NETWORK_NAME_MAX_SIZE);
                }
                else if (pserviceStatus->operatorNameFormat == OPERATOR_NAME_FORMAT_NUMERIC)
                {
                    mccMncLen = (uint32_t) strlen(pToken);

                    if ((mccMncLen == (CELLULAR_MCC_MAX_SIZE + CELLULAR_MNC_MAX_SIZE)) ||
                        (mccMncLen == (CELLULAR_MCC_MAX_SIZE + CELLULAR_MNC_MAX_SIZE - 1U)))
                    {
                        (void) strncpy(pserviceStatus->plmnInfo.mcc, pToken, CELLULAR_MCC_MAX_SIZE);
                        pserviceStatus->plmnInfo.mcc[CELLULAR_MCC_MAX_SIZE] = '\0';
                        (void) strncpy(pserviceStatus->plmnInfo.mnc, &pToken[CELLULAR_MCC_MAX_SIZE],
                                       (uint32_t) (mccMncLen - CELLULAR_MCC_MAX_SIZE + 1U));
                        pserviceStatus->plmnInfo.mnc[CELLULAR_MNC_MAX_SIZE] = '\0';
                    }
                    else
                    {
                        LogError(("_parseCopsNetworkName: Error in processing Network MCC MNC: Length not Valid"));
                        parseStatus = false;
                    }
                }
                else
                {
                    LogError(("Error in processing Operator Name: Format Unknown"));
                    parseStatus = false;
                }

                if (parseStatus == false)
                {
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATGetNextTok(&pTempCopsResponse, &pToken);
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                int32_t var = 0;

                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &var);

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    if ((var < (int32_t) CELLULAR_RAT_MAX) && (var >= 0))
                    {
                        /* Variable "var" is ensured that it is valid and within
                         * a valid range. Hence, assigning the value of the variable to
                         * rat with a enum cast. */

                        /* coverity[misra_c_2012_rule_10_5_violation] */
                        pserviceStatus->rat = (CellularRat_t) var;
                    }
                    else
                    {
                        LogError(("_parseCopsNetworkName: Error in processing RAT. Token %s", pToken));
                        parseStatus = false;
                    }
                }

                if (parseStatus == false)
                {
                    atCoreStatus = CELLULAR_AT_ERROR;
                }
            }
        }

        if (atCoreStatus == CELLULAR_AT_ERROR)
        {
            LogError(("ERROR: COPS %s", pCopsResponse));
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*******************************************************************************************************************//**
 * Get activation status with AT+CGACT command.
 *
 * @param[in] cellularHandle        Cellular handle provided by init command.
 * @param[in] pPdpContextsActInfo   Struct to store activation info in.
 *
 * @retval  See _Cellular_AtcmdRequestWithCallback
 **********************************************************************************************************************/
static CellularError_t _Cellular_GetContextActivationStatus (CellularHandle_t              cellularHandle,
                                                             CellularPdnContextActInfo_t * pPdpContextsActInfo)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqPacketSwitchStatus;
    atReqPacketSwitchStatus.pAtCmd       = "AT+CGACT?";
    atReqPacketSwitchStatus.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
    atReqPacketSwitchStatus.pAtRspPrefix = "+CGACT";
    atReqPacketSwitchStatus.respCallback = _Cellular_RecvFuncGetPdpContextActState;
    atReqPacketSwitchStatus.pData        = pPdpContextsActInfo;
    atReqPacketSwitchStatus.dataLen      = sizeof(CellularPdnContextActInfo_t);
#else
    CellularAtReq_t atReqPacketSwitchStatus =
    {
        "AT+CGACT?",
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+CGACT",
        _Cellular_RecvFuncGetPdpContextActState,
        pPdpContextsActInfo,
        sizeof(CellularPdnContextActInfo_t),
    };
#endif

    /* Internal function. Callee check parameters. */
    pktStatus      = _Cellular_AtcmdRequestWithCallback(pContext, atReqPacketSwitchStatus);
    cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

    return cellularStatus;
}

/*******************************************************************************************************************//**
 * Callback to process AT+CGACT response.
 *
 * @param[in] pContext  Pointer to cellular context.
 * @param[in] pAtResp   Pointer to AT command response.
 * @param[in] pData     Pointer to where to store parsed data.
 * @param[in] dataLen   Length of where to store parsed data.
 *
 * @retval  CELLULAR_PKT_STATUS_OK          Response processing succeeded
 * @retval  CELLULAR_PKT_STATUS_FAILURE     Parsing failure
 * @retval  CELLULAR_PKT_STATUS_BAD_PARAM   Bad parameter passed to callback
 **********************************************************************************************************************/
static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextActState (CellularContext_t                 * pContext,
                                                                    const CellularATCommandResponse_t * pAtResp,
                                                                    void                              * pData,
                                                                    uint16_t                            dataLen)
{
    char * pRespLine = NULL;
    CellularPdnContextActInfo_t   * pPDPContextsActInfo = (CellularPdnContextActInfo_t *) pData;
    CellularPktStatus_t             pktStatus           = CELLULAR_PKT_STATUS_OK;
    CellularATError_t               atCoreStatus        = CELLULAR_AT_SUCCESS;
    const CellularATCommandLine_t * pCommandItem        = NULL;
    uint8_t tokenIndex = 0;
    uint8_t contextId  = 0;
    int32_t tempValue  = 0;
    char  * pToken     = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("_Cellular_RecvFuncGetPdpContextActState: invalid context"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pPDPContextsActInfo == NULL) || (dataLen != sizeof(CellularPdnContextActInfo_t)))
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if (pAtResp == NULL)
    {
        LogError(("_Cellular_RecvFuncGetPdpContextActState: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("_Cellular_RecvFuncGetPdpContextActState: no PDN context available"));
        pktStatus = CELLULAR_PKT_STATUS_OK;
    }
    else
    {
#endif
        pRespLine = pAtResp->pItm->pLine;

        pCommandItem = pAtResp->pItm;

        while (pCommandItem != NULL)
        {
            pRespLine = pCommandItem->pLine;
            LogDebug(("_Cellular_RecvFuncGetPdpContextActState: pRespLine [%s]", pRespLine));

            /* Removing all the Spaces in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pRespLine);

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATRemovePrefix(&pRespLine);

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    atCoreStatus = Cellular_ATRemoveAllDoubleQuote(pRespLine);
                }

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    atCoreStatus = Cellular_ATGetNextTok(&pRespLine, &pToken);
                }

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    tokenIndex = 0;

                    while ((pToken != NULL) && (atCoreStatus == CELLULAR_AT_SUCCESS))
                    {
                        switch (tokenIndex)
                        {
                            case (RM_CELLULAR_RYZ_PDN_ACT_STATUS_POS_CONTEXT_ID):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextActState: Context Id pToken: %s", pToken));
                                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

                                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                                {
                                    if ((tempValue >= (int32_t) CELLULAR_PDN_CONTEXT_ID_MIN) &&
                                        (tempValue <= (int32_t) RM_CELLULAR_RYZ_MAX_PDP_CONTEXTS))
                                    {
                                        contextId = (uint8_t) tempValue;
                                        pPDPContextsActInfo->contextsPresent[contextId - 1] = true;
                                        LogDebug(("_Cellular_RecvFuncGetPdpContextActState: Context Id: %d",
                                                  contextId));
                                    }
                                    else
                                    {
                                        LogError((
                                                     "_Cellular_RecvFuncGetPdpContextActState: Invalid Context Id. Token %s",
                                                     pToken));
                                        atCoreStatus = CELLULAR_AT_ERROR;
                                    }
                                }

                                break;
                            }

                            case (RM_CELLULAR_RYZ_PDN_ACT_STATUS_POS_CONTEXT_STATE):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextActState: Context <Act> pToken: %s", pToken));
                                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

                                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                                {
                                    pPDPContextsActInfo->contextActState[contextId - 1] = (bool) tempValue;
                                    LogDebug(("_Cellular_RecvFuncGetPdpContextActState: Context <Act>: %d",
                                              pPDPContextsActInfo->contextActState[contextId - 1]));
                                }

                                break;
                            }

                            default:
                            {
                                break;
                            }
                        }

                        tokenIndex++;

                        if (Cellular_ATGetNextTok(&pRespLine, &pToken) != CELLULAR_AT_SUCCESS)
                        {
                            break;
                        }
                    }
                }
            }

            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                LogError(("_Cellular_RecvFuncGetPdpContextActState: parse %s failed", pRespLine));
                break;
            }

            pCommandItem = pCommandItem->pNext;
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*******************************************************************************************************************//**
 * Get packet switch status with AT+CGATT?
 *
 * @param[in] cellularHandle        Cellular handle provided by init command.
 * @param[in] pPacketSwitchStatus   Location to store status.
 *
 * @retval  See _Cellular_TimeoutAtcmdRequestWithCallback
 **********************************************************************************************************************/
static CellularError_t _Cellular_GetPacketSwitchStatus (CellularHandle_t cellularHandle, bool * pPacketSwitchStatus)
{
    CellularContext_t * pContext                = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus          = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus               = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqPacketSwitchStatus;
    atReqPacketSwitchStatus.pAtCmd       = "AT+CGATT?";
    atReqPacketSwitchStatus.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqPacketSwitchStatus.pAtRspPrefix = "+CGATT";
    atReqPacketSwitchStatus.respCallback = _Cellular_RecvFuncPacketSwitchStatus;
    atReqPacketSwitchStatus.pData        = pPacketSwitchStatus;
    atReqPacketSwitchStatus.dataLen      = sizeof(bool);
#else
    CellularAtReq_t     atReqPacketSwitchStatus =
    {
        "AT+CGATT?",
        CELLULAR_AT_WITH_PREFIX,
        "+CGATT",
        _Cellular_RecvFuncPacketSwitchStatus,
        pPacketSwitchStatus,
        sizeof(bool),
    };
#endif

    /* Internal function. Callee check parameters. */
    pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(pContext,
                                                          atReqPacketSwitchStatus,
                                                          RM_CELLULAR_RYZ_PDN_ACT_PACKET_REQ_TIMEOUT_MS);
    cellularStatus = _Cellular_TranslatePktStatus(pktStatus);

    return cellularStatus;
}

/*******************************************************************************************************************//**
 * Callback to process AT+CGATT response.
 *
 * @param[in] pContext  Pointer to cellular context.
 * @param[in] pAtResp   Pointer to AT command response.
 * @param[in] pData     Pointer to where to store parsed data.
 * @param[in] dataLen   Length of where to store parsed data.
 *
 * @retval  CELLULAR_PKT_STATUS_OK          Response processing succeeded
 * @retval  CELLULAR_PKT_STATUS_FAILURE     Parsing failure
 * @retval  CELLULAR_PKT_STATUS_BAD_PARAM   Bad parameter passed to callback
 **********************************************************************************************************************/
static CellularPktStatus_t _Cellular_RecvFuncPacketSwitchStatus (CellularContext_t                 * pContext,
                                                                 const CellularATCommandResponse_t * pAtResp,
                                                                 void                              * pData,
                                                                 uint16_t                            dataLen)
{
    char              * pInputLine          = NULL;
    CellularPktStatus_t pktStatus           = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus        = CELLULAR_AT_SUCCESS;
    bool              * pPacketSwitchStatus = (bool *) pData;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("PacketSwitchStatus: Invalid handle"));
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pData == NULL) || (dataLen != sizeof(bool)))
    {
        LogError(("GetPacketSwitchStatus: Invalid param"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("GetPacketSwitchStatus: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        pInputLine = pAtResp->pItm->pLine;

        /* Remove prefix. */
        atCoreStatus = Cellular_ATRemovePrefix(&pInputLine);

        /* Remove leading space. */
        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            atCoreStatus = Cellular_ATRemoveLeadingWhiteSpaces(&pInputLine);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            if (*pInputLine == '0')
            {
                *pPacketSwitchStatus = false;
            }
            else if (*pInputLine == '1')
            {
                *pPacketSwitchStatus = true;
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*******************************************************************************************************************//**
 * Callback to process AT+CGDCONT response.
 *
 * @param[in] pContext  Pointer to cellular context.
 * @param[in] pAtResp   Pointer to AT command response.
 * @param[in] pData     Pointer to where to store parsed data.
 * @param[in] dataLen   Length of where to store parsed data.
 *
 * @retval  CELLULAR_PKT_STATUS_OK          Response processing succeeded
 * @retval  CELLULAR_PKT_STATUS_FAILURE     Parsing failure
 * @retval  CELLULAR_PKT_STATUS_BAD_PARAM   Bad parameter passed to callback
 **********************************************************************************************************************/
static CellularPktStatus_t _Cellular_RecvFuncGetPdpContextSettings (CellularContext_t                 * pContext,
                                                                    const CellularATCommandResponse_t * pAtResp,
                                                                    void                              * pData,
                                                                    uint16_t                            dataLen)
{
    char * pRespLine = NULL;
    CellularPdnContextInfo_t      * pPDPContextsInfo = (CellularPdnContextInfo_t *) pData;
    CellularPktStatus_t             pktStatus        = CELLULAR_PKT_STATUS_OK;
    CellularATError_t               atCoreStatus     = CELLULAR_AT_SUCCESS;
    const CellularATCommandLine_t * pCommandItem     = NULL;
    uint8_t tokenIndex = 0;
    uint8_t contextId  = 0;
    int32_t tempValue  = 0;
    char  * pToken     = NULL;

    (void)(dataLen);

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("_Cellular_RecvFuncGetPdpContextSettings: invalid context"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if (pPDPContextsInfo == NULL)
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if (pAtResp == NULL)
    {
        LogError(("_Cellular_RecvFuncGetPdpContextSettings: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("_Cellular_RecvFuncGetPdpContextSettings: no PDN context available"));
        pktStatus = CELLULAR_PKT_STATUS_OK;
    }
    else
    {
#endif
        pRespLine = pAtResp->pItm->pLine;

        pCommandItem = pAtResp->pItm;

        while (pCommandItem != NULL)
        {
            pRespLine = pCommandItem->pLine;
            LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: pRespLine [%s]", pRespLine));

            /* Removing all the Spaces in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pRespLine);

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATRemovePrefix(&pRespLine);

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    atCoreStatus = Cellular_ATRemoveAllDoubleQuote(pRespLine);
                }

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    atCoreStatus = Cellular_ATGetNextTok(&pRespLine, &pToken);
                }

                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                {
                    tokenIndex = 0;

                    while ((pToken != NULL) && (atCoreStatus == CELLULAR_AT_SUCCESS))
                    {
                        switch (tokenIndex)
                        {
                            case (RM_CELLULAR_RYZ_PDN_STATUS_POS_CONTEXT_ID):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: Context Id pToken: %s", pToken));
                                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

                                if (atCoreStatus == CELLULAR_AT_SUCCESS)
                                {
                                    if ((tempValue >= (int32_t) CELLULAR_PDN_CONTEXT_ID_MIN) &&
                                        (tempValue <= (int32_t) RM_CELLULAR_RYZ_MAX_PDP_CONTEXTS))
                                    {
                                        contextId = (uint8_t) tempValue;
                                        pPDPContextsInfo->contextsPresent[contextId - 1] = true;
                                        LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: Context Id: %d",
                                                  contextId));
                                    }
                                    else
                                    {
                                        LogError((
                                                     "_Cellular_RecvFuncGetPdpContextSettings: Invalid Context Id. Token %s",
                                                     pToken));
                                        atCoreStatus = CELLULAR_AT_ERROR;
                                    }
                                }

                                break;
                            }

                            case (RM_CELLULAR_RYZ_PDN_STATUS_POS_CONTEXT_TYPE):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: Context Type pToken: %s", pToken));
                                (void) memcpy((void *) pPDPContextsInfo->ipType[contextId - 1],
                                              (void *) pToken,
                                              RM_CELLULAR_RYZ_PDN_CONTEXT_TYPE_MAX_SIZE);
                                break;
                            }

                            case (RM_CELLULAR_RYZ_PDN_STATUS_POS_APN_NAME):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: Context APN name pToken: %s",
                                          pToken));
                                (void) memcpy((void *) pPDPContextsInfo->apnName[contextId - 1],
                                              (void *) pToken,
                                              CELLULAR_APN_MAX_SIZE);
                                break;
                            }

                            case (RM_CELLULAR_RYZ_PDN_STATUS_POS_IP_ADDRESS):
                            {
                                LogDebug(("_Cellular_RecvFuncGetPdpContextSettings: Context IP address pToken: %s",
                                          pToken));
                                (void) memcpy((void *) pPDPContextsInfo->ipAddress[contextId - 1],
                                              (void *) pToken,
                                              CELLULAR_IP_ADDRESS_MAX_SIZE + 1U);
                                break;
                            }

                            default:
                            {
                                break;
                            }
                        }

                        tokenIndex++;

                        if (Cellular_ATGetNextTok(&pRespLine, &pToken) != CELLULAR_AT_SUCCESS)
                        {
                            break;
                        }
                    }
                }
            }

            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                LogError(("_Cellular_RecvFuncGetPdpContextSettings: parse %s failed", pRespLine));
                break;
            }

            pCommandItem = pCommandItem->pNext;
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetSignalInfo (CellularContext_t                 * pContext,
                                                            const CellularATCommandResponse_t * pAtResp,
                                                            void                              * pData,
                                                            uint16_t                            dataLen)
{
    char                 * pInputLine      = NULL;
    CellularSignalInfo_t * pSignalInfo     = (CellularSignalInfo_t *) pData;
    bool                parseStatus        = true;
    CellularPktStatus_t pktStatus          = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus       = CELLULAR_AT_SUCCESS;
    bool                isExtendedResponse = false;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pSignalInfo == NULL) || (dataLen != sizeof(CellularSignalInfo_t)))
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("GetSignalInfo: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        pInputLine = pAtResp->pItm->pLine;

        if (strstr(pInputLine, "+CESQ"))
        {
            LogDebug(("GetSignalInfo: ExtendedResponse received."));
            isExtendedResponse = true;
        }

        atCoreStatus = Cellular_ATRemovePrefix(&pInputLine);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pInputLine);
        }

        if (atCoreStatus != CELLULAR_AT_SUCCESS)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (pktStatus == CELLULAR_PKT_STATUS_OK)
    {
        if (isExtendedResponse)
        {
            parseStatus = _parseExtendedSignalQuality(pInputLine, pSignalInfo);
        }
        else
        {
            parseStatus = _parseSignalQuality(pInputLine, pSignalInfo);
        }

        if (parseStatus != true)
        {
            pSignalInfo->rssi = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->rsrp = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->rsrq = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->ber  = CELLULAR_INVALID_SIGNAL_VALUE;
            pSignalInfo->bars = CELLULAR_INVALID_SIGNAL_BAR_VALUE;
            pktStatus         = CELLULAR_PKT_STATUS_FAILURE;
        }
    }

    return pktStatus;
}

static bool _parseExtendedSignalQuality (char * pQcsqPayload, CellularSignalInfo_t * pSignalInfo)
{
    char            * pToken          = NULL;
    char            * pTmpQcsqPayload = pQcsqPayload;
    int32_t           tempValue       = 0;
    bool              parseStatus     = true;
    CellularATError_t atCoreStatus    = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((pSignalInfo == NULL) || (pQcsqPayload == NULL))
    {
        LogError(("_parseExtendedSignalQuality: Invalid Input Parameters"));
        parseStatus = false;
    }
#endif

    /*  +CESQ: <rxlev>,<ber>,<rscp>,<ecn0>,<rsrq>,<rsrp>. */

    /* Skip <rxlev>. */
    atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);

    /* Parse <ber>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

        if ((atCoreStatus == CELLULAR_AT_SUCCESS) && (tempValue <= INT16_MAX) && (tempValue >= INT16_MIN))
        {
            /*
             * Bit Error Rate (BER):
             *  0..7: as RXQUAL values in the table in 3GPP TS 45.008 [124], subclause 8.2.4
             *  99: not known or not detectable
             */
            if ((tempValue >= 0) && (tempValue <= 7))
            {
                pSignalInfo->ber = (int16_t) tempValue;
            }
            else
            {
                pSignalInfo->ber = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            LogError(("_parseExtendedSignalQuality: Error in processing BER. Token %s", pToken));
            atCoreStatus = CELLULAR_AT_ERROR;
        }
    }

    /* Skip <rscp>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    /* Skip <ecno>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    /* Parse <rsrq>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /*
             * Reference Signal Received Quality (RSRQ):
             *  0: less than -19.5 dB
             *  1..33: from -19.5 dB to -3.5 dB with 0.5 dB steps
             *  34: -3 dB or greater
             *  255: not known or not detectable
             */
            if ((tempValue >= 0) && (tempValue <= 34))                     // NOLINT(readability-magic-numbers)
            {
                pSignalInfo->rsrq = (int16_t) ((-20) + (tempValue * 0.5)); // NOLINT(readability-magic-numbers)
            }
            else
            {
                pSignalInfo->rsrq = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            LogError(("_parseExtendedSignalQuality: Error in processing RSRP. Token %s", pToken));
            parseStatus = false;
        }
    }

    /* Parse <rsrp>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /*
             * Reference Signal Received Power(RSRP) :
             *  0 : less than - 140 dBm
             *  1..96 : from - 140 dBm to - 45 dBm with 1 dBm steps
             *  97 : -44 dBm or greater
             *  255 : not known or not detectable
             */
            if ((tempValue >= 0) && (tempValue <= 97))                // NOLINT(readability-magic-numbers)
            {
                pSignalInfo->rsrp = (int16_t) ((-141) + (tempValue)); // NOLINT(readability-magic-numbers)
            }
            else
            {
                pSignalInfo->rsrp = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            LogError(("_parseExtendedSignalQuality: Error in processing RSRP. Token %s", pToken));
            parseStatus = false;
        }
    }

    return parseStatus;
}

static bool _parseSignalQuality (char * pQcsqPayload, CellularSignalInfo_t * pSignalInfo)
{
    char            * pToken          = NULL;
    char            * pTmpQcsqPayload = pQcsqPayload;
    int32_t           tempValue       = 0;
    bool              parseStatus     = true;
    CellularATError_t atCoreStatus    = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((pSignalInfo == NULL) || (pQcsqPayload == NULL))
    {
        LogError(("_parseSignalQuality: Invalid Input Parameters"));
        parseStatus = false;
    }
#endif

    /* +CSQ: <signal_power>,<qual>. */

    /* Parse <signal_power>. */
    atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);

    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /*
             * The allowed range is 0-31 and 99.
             *  0           RSSI of the network <= -113 dBm
             *  1           -111 dBm
             *  2...30      -109 dBm <= RSSI of the network <= -53 dBm
             *  31          -51 dBm <= RSSI of the network
             *  99          Not known or not detectable
             */
            if ((tempValue >= 0) && (tempValue <= 31))                    // NOLINT(readability-magic-numbers)
            {
                pSignalInfo->rssi = (int16_t) ((-113) + (tempValue * 2)); // NOLINT(readability-magic-numbers)
            }
            else if (tempValue == 99)                                     // NOLINT(readability-magic-numbers)
            {
                pSignalInfo->rssi = -113;                                 // NOLINT(readability-magic-numbers)
            }
            else
            {
                pSignalInfo->rssi = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            LogError(("_parseSignalQuality: Error in processing RSSI. Token %s", pToken));
            parseStatus = false;
        }
    }

    /* Parse <qual>. */
    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATGetNextTok(&pTmpQcsqPayload, &pToken);
    }

    if (atCoreStatus == CELLULAR_AT_SUCCESS)
    {
        atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /*
             * The allowed range is 0-7 and 99 :
             *  In 2G RAT CS dedicated and GPRS packet transfer mode indicates the Bit Error Rate (BER) as specified in 3GPP TS 45.008
             */
            if ((tempValue >= 0) && (tempValue <= 7))
            {
                pSignalInfo->ber = (int16_t) tempValue;
            }
            else
            {
                pSignalInfo->ber = CELLULAR_INVALID_SIGNAL_VALUE;
            }
        }
        else
        {
            LogError(("_parseSignalQuality: Error in processing ber. Token %s", pToken));
            parseStatus = false;
        }
    }

    return parseStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncResolveDomainToIpAddress (CellularContext_t                 * pContext,
                                                                       const CellularATCommandResponse_t * pAtResp,
                                                                       void                              * pData,
                                                                       uint16_t                            dataLen)
{
    (void)dataLen;

    char * pInputLine = NULL;
    char pRecvInfo[16] = {0};
    bool parseStatus = true;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if( pContext == NULL )
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if( ( pAtResp == NULL ) || ( pAtResp->pItm == NULL ) || ( pAtResp->pItm->pLine == NULL ) )
    {
        LogError(( "GetHostByNameInfo: Input Line passed is NULL" ));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        pInputLine = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemovePrefix( &pInputLine );

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote( pInputLine );
        }

        if( atCoreStatus == CELLULAR_AT_SUCCESS )
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces( pInputLine );
        }

        if( atCoreStatus != CELLULAR_AT_SUCCESS )
        {
            pktStatus = _Cellular_TranslateAtCoreStatus( atCoreStatus );
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if( pktStatus == CELLULAR_PKT_STATUS_OK )
    {
        parseStatus = _parseGetHostByName( pInputLine, pRecvInfo );

        if( parseStatus == true )
        {
            ( void ) strcpy( (char *)pData, &pRecvInfo[0] );
        }
        else
        {
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }
    }

    return pktStatus;
}

static bool _parseGetHostByName( char * pQcsqPayload, char * pGetHostInfo )
{
    char * pToken          = NULL;
    char * pTmpQcsqPayload = pQcsqPayload;
    bool   parseStatus     = true;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if( ( &pGetHostInfo == NULL ) || ( pQcsqPayload == NULL ) )
    {
        LogError(( "_parseGetHostByName: Invalid Input Parameters" ));
        parseStatus = false;
    }
#endif

    if( ( parseStatus == true ) && ( Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken ) != CELLULAR_AT_SUCCESS ) )
    {
        LogError(( "_parseGetHostByName: Error in processing HostName. Token %s", pToken ));
        parseStatus = false;
    }

    if( ( parseStatus == true ) && ( Cellular_ATGetNextTok( &pTmpQcsqPayload, &pToken ) == CELLULAR_AT_SUCCESS ) )
    {
         ( void ) strcpy( pGetHostInfo, pToken );
    }
    else
    {
        LogError(( "_parseGetHostByName: Error in processing HostName. Token %s", pToken ));
        parseStatus = false;
    }

    return parseStatus;
}

static CellularPktStatus_t socketSendDataPrefix (void * pCallbackContext, char * pLine, uint32_t * pBytesRead)
{
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((pLine == NULL) || (pBytesRead == NULL))
    {
        LogError(("socketSendDataPrefix: pLine is invalid or pBytesRead is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if (pCallbackContext != NULL)
    {
        LogError(("socketSendDataPrefix: pCallbackContext is not NULL"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if (*pBytesRead != 2U)
    {
        LogDebug(("socketSendDataPrefix: pBytesRead %u %s is not 2", (unsigned int) *pBytesRead, pLine));
    }
    else
    {
#endif
        /* After the data prefix, there should not be any data in stream.
         * Cellular commmon processes AT command in lines. Add a '\0' after '@'. */
        if (strcmp(pLine, "> ") == 0)
        {
            pLine[2]    = '\n';
            *pBytesRead = 3;
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncData (CellularContext_t                 * pContext,
                                                   const CellularATCommandResponse_t * pAtResp,
                                                   void                              * pData,
                                                   uint16_t                            dataLen)
{
    CellularATError_t         atCoreStatus = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t       pktStatus    = CELLULAR_PKT_STATUS_OK;
    char                    * pInputLine   = NULL;
    char                    * pToken       = NULL;
    const _socketDataRecv_t * pDataRecv    = (_socketDataRecv_t *) pData;
    int32_t                   tempValue    = 0;

    (void) dataLen;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("Receive Data: invalid context"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("Receive Data: response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else if ((pDataRecv == NULL) || (pDataRecv->pData == NULL) || (pDataRecv->pDataLen == NULL))
    {
        LogError(("Receive Data: Bad parameters"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        pInputLine = pAtResp->pItm->pLine; /* The first item is the data prefix. */

        /* Check the data prefix token "+SQNSRECV: ". */
        if (strncmp(pInputLine, RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN,
                    RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN_LEN) != 0)
        {
            LogError(("response item error in prefix"));
            atCoreStatus = CELLULAR_AT_ERROR;
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Get the socket ID */
            atCoreStatus = Cellular_ATGetNextTok(&pInputLine, &pToken);

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                /* Get the bytes received */
                atCoreStatus = Cellular_ATGetNextTok(&pInputLine, &pToken);
            }

            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                atCoreStatus = Cellular_ATStrtoi(pToken, 10, &tempValue);
            }

            /* Process the data buffer. */
            if (atCoreStatus == CELLULAR_AT_SUCCESS)
            {
                *pDataRecv->pDataLen = (uint32_t) tempValue;
                atCoreStatus         = getDataFromResp(pAtResp, pDataRecv, *pDataRecv->pDataLen);
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

static CellularATError_t getDataFromResp (const CellularATCommandResponse_t * pAtResp,
                                          const _socketDataRecv_t           * pDataRecv,
                                          uint32_t                            outBufSize)
{
    CellularATError_t atCoreStatus  = CELLULAR_AT_SUCCESS;
    const char      * pInputLine    = NULL;
    uint32_t          dataLenToCopy = 0;

    /* Check if the received data size is greater than the output buffer size. */
    if (*pDataRecv->pDataLen > outBufSize)
    {
        LogError(("Data is truncated, received data length %u, out buffer size %u",
                  (unsigned int) *pDataRecv->pDataLen, (unsigned int) outBufSize));
        dataLenToCopy        = outBufSize;
        *pDataRecv->pDataLen = outBufSize;
    }
    else
    {
        dataLenToCopy = *pDataRecv->pDataLen;
    }

    /* Data is stored in the next intermediate response. */
    if (pAtResp->pItm->pNext != NULL)
    {
        pInputLine = pAtResp->pItm->pNext->pLine;

        if ((pInputLine != NULL) && (dataLenToCopy > 0U))
        {
            /* Copy the data to the out buffer. */
            (void) memcpy((void *) pDataRecv->pData, (const void *) pInputLine, dataLenToCopy);
        }
        else
        {
            LogError(("Receive Data: paramerter error, data pointer %p, data to copy %u",
                      pInputLine, (unsigned int) dataLenToCopy));
            atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }
    else if (*pDataRecv->pDataLen == 0U)
    {
        /* Receive command success but no data. */
        LogDebug(("Receive Data: no data"));
    }
    else
    {
        LogError(("Receive Data: Intermediate response empty"));
        atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    return atCoreStatus;
}

static CellularPktStatus_t socketRecvDataPrefix (void     * pCallbackContext,
                                                 char     * pLine,
                                                 uint32_t   lineLength,
                                                 char    ** ppDataStart,
                                                 uint32_t * pDataLength)
{
    char              * pDataStart       = NULL;
    uint32_t            prefixLineLength = 0U;
    int32_t             tempValue        = 0;
    CellularATError_t   atResult         = CELLULAR_AT_SUCCESS;
    CellularPktStatus_t pktStatus        = CELLULAR_PKT_STATUS_OK;
    uint32_t            i                = 0;
    char                pLocalLine[RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING + 1] = "\0";
    uint32_t            localLineLength = RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING >
                                          lineLength ? lineLength : RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING;

    (void) pCallbackContext;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((pLine == NULL) || (ppDataStart == NULL) || (pDataLength == NULL))
    {
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        /* Check if the message is a data response. */
        if (strncmp(pLine, RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN, RM_CELLULAR_RYZ_SOCKET_DATA_PREFIX_TOKEN_LEN) == 0)
        {
            strncpy(pLocalLine, pLine, RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING);
            pLocalLine[RM_CELLULAR_RYZ_MAX_SQNSSRECV_STRING_PREFIX_STRING] = '\0';
            pDataStart = pLocalLine;

            /* Add a '\0' char at the end of the line. */
            for (i = 0; i < localLineLength; i++)
            {
                if ((pDataStart[i] == '\r') || (pDataStart[i] == '\n'))
                {
                    pDataStart[i]    = '\0';
                    prefixLineLength = i;
                    break;
                }
            }

            if (i == localLineLength)
            {
                LogDebug(("Data prefix invalid line : %s", pLocalLine));
                pDataStart = NULL;
            }
        }

        if (pDataStart != NULL)
        {
            atResult = Cellular_ATStrtoi(&pDataStart[strlen("+SQNSRECV: 1,")], 10, &tempValue);

            if ((atResult == CELLULAR_AT_SUCCESS) && (tempValue >= 0) &&
                (tempValue <= (int32_t) CELLULAR_MAX_RECV_DATA_LEN))
            {
                if ((prefixLineLength + RM_CELLULAR_RYZ_DATA_PREFIX_STRING_CHANGELINE_LENGTH) > lineLength)
                {
                    /* More data is required. */
                    *pDataLength = 0;
                    pDataStart   = NULL;
                    pktStatus    = CELLULAR_PKT_STATUS_SIZE_MISMATCH;
                }
                else
                {
                    pDataStart    = &pLine[prefixLineLength];
                    pDataStart[0] = '\0';
                    pDataStart    = &pDataStart[RM_CELLULAR_RYZ_DATA_PREFIX_STRING_CHANGELINE_LENGTH];
                    *pDataLength  = (uint32_t) tempValue;
                }

                LogDebug(("DataLength %p at pktIo = %d", pDataStart, (unsigned int) *pDataLength));
            }
            else
            {
                *pDataLength = 0;
                pDataStart   = NULL;
                LogError(("Data response received with wrong size"));
            }
        }

        *ppDataStart = pDataStart;
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

static bool _checkCrsmMemoryStatus (const char * pToken)
{
    bool memoryStatus = true;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pToken == NULL)
    {
        LogError(("Input Parameter NULL"));
        memoryStatus = false;
    }
#endif

    if (memoryStatus == true)
    {
        /* Checking the value sw2 in AT command response for memory problem during CRSM read.
         * Refer 3GPP Spec TS 51.011 Section 9.4. */
        if (strcmp(pToken, "64") == 0) /* '40' memory problem. */
        {
            LogError(("_checkCrsmMemoryStatus: Error in Processing HPLMN: CRSM Memory Error"));
            memoryStatus = false;
        }
    }

    return memoryStatus;
}

/*-----------------------------------------------------------*/

static bool _checkCrsmReadStatus (const char * pToken)
{
    bool readStatus = true;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pToken == NULL)
    {
        LogError(("Input Parameter NULL"));
        readStatus = false;
    }
#endif

    if (readStatus == true)
    {
        /* Checking the parameter sw1 in AT command response for successful CRSM read.
         * Refer 3GPP Spec TS 51.011 Section 9.4. */
        if ((strcmp(pToken, "144") != 0) && /* '90' normal ending of the command. */
            (strcmp(pToken, "145") != 0) && /* '91' normal ending of the command, with extra information. */
            (strcmp(pToken, "146") != 0))   /* '92' command successful but after using an internal update retry routine 'X' times. */
        {
            LogError(("_checkCrsmReadStatus: Error in Processing HPLMN: CRSM Read Error"));
            readStatus = false;
        }
    }

    return readStatus;
}

/*-----------------------------------------------------------*/

static bool _parseHplmn (char * pToken, void * pData)
{
    bool                 parseStatus = true;
    CellularPlmnInfo_t * plmn        = (CellularPlmnInfo_t *) pData;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pToken == NULL)
    {
        LogError(("_parseHplmn: pToken is NULL or pData is NULL"));
        parseStatus = false;
    }
    else if (strlen(pToken) < (RM_CELLULAR_RYZ_CRSM_HPLMN_RAT_LENGTH))
    {
        LogError(("_parseHplmn: Error in processing HPLMN invalid token %s", pToken));
        parseStatus = false;
    }
    else
    {
#endif
        /* Returning only the very first HPLMN present in EFHPLMNwACT in SIM.
         * EF-HPLMNwACT can contain a maximum of 10 HPLMN entries in decreasing order of priority.
         * In this implementation, returning the very first HPLMN is the PLMN priority list. */

        /* Refer TS 51.011 Section 10.3.37 for encoding. */
        plmn->mcc[0] = pToken[1];
        plmn->mcc[1] = pToken[0];
        plmn->mcc[2] = pToken[3];
        plmn->mnc[0] = pToken[5];
        plmn->mnc[1] = pToken[4];

        if (pToken[2] != 'F')
        {
            plmn->mnc[2] = pToken[2];
            plmn->mnc[3] = '\0';
        }
        else
        {
            plmn->mnc[2] = '\0';
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return parseStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_RecvFuncGetHplmn (CellularContext_t                 * pContext,
                                                       const CellularATCommandResponse_t * pAtResp,
                                                       void                              * pData,
                                                       uint16_t                            dataLen)
{
    bool                parseStatus   = true;
    CellularPktStatus_t pktStatus     = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus  = CELLULAR_AT_SUCCESS;
    char              * pCrsmResponse = NULL;
    char              * pToken        = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("GetHplmn: pContext is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("GetHplmn: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if ((pData == NULL) || (dataLen != sizeof(CellularPlmnInfo_t)))
    {
        LogError(("GetHplmn: pData is invalid or dataLen is wrong"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        pCrsmResponse = pAtResp->pItm->pLine;
        atCoreStatus  = Cellular_ATRemoveAllWhiteSpaces(pCrsmResponse);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing the CRSM prefix in AT Response. */
            atCoreStatus = Cellular_ATRemovePrefix(&pCrsmResponse);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing All quotes in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote(pCrsmResponse);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Getting the next token separated by comma in At Response. */
            atCoreStatus = Cellular_ATGetNextTok(&pCrsmResponse, &pToken);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            parseStatus = _checkCrsmReadStatus(pToken);

            if (parseStatus == false)
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            atCoreStatus = Cellular_ATGetNextTok(&pCrsmResponse, &pToken);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            parseStatus = _checkCrsmMemoryStatus(pToken);

            if (parseStatus == false)
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            atCoreStatus = Cellular_ATGetNextTok(&pCrsmResponse, &pToken);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            parseStatus = _parseHplmn(pToken, pData);

            if (parseStatus == false)
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_RecvFuncGetIccid (CellularContext_t                 * pContext,
                                                       const CellularATCommandResponse_t * pAtResp,
                                                       void                              * pData,
                                                       uint16_t                            dataLen)
{
    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;
    char              * pRespLine    = NULL;
    char              * pToken       = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("getIccid: pContext is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) ||
             (pAtResp->pItm->pLine == NULL))
    {
        LogError(("getIccid: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if ((pData == NULL) || (dataLen != (CELLULAR_ICCID_MAX_SIZE + 1U)))
    {
        LogError(("getIccid: pData is invalid or dataLen is wrong"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        pRespLine    = pAtResp->pItm->pLine;
        atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pRespLine);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing QCCID Prefix in AT Response. */
            atCoreStatus = Cellular_ATRemovePrefix(&pRespLine);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Removing All quotes in the AT Response. */
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote(pRespLine);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Getting the next token separated by comma in At Response. */
            atCoreStatus = Cellular_ATGetNextTok(&pRespLine, &pToken);
        }

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            /* Storing the ICCID value in the AT Response. */
            if (strlen(pToken) < ((size_t) CELLULAR_ICCID_MAX_SIZE + 1U))
            {
                (void) strncpy(pData, pToken, dataLen);
            }
            else
            {
                atCoreStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/*-----------------------------------------------------------*/

static CellularPktStatus_t _Cellular_RecvFuncGetImsi (CellularContext_t                 * pContext,
                                                      const CellularATCommandResponse_t * pAtResp,
                                                      void                              * pData,
                                                      uint16_t                            dataLen)
{
    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;
    char              * pRespLine    = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pContext == NULL)
    {
        LogError(("getImsi: pContext is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((pAtResp == NULL) || (pAtResp->pItm == NULL) || (pAtResp->pItm->pLine == NULL))
    {
        LogError(("getImsi: Response is invalid"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else if ((pData == NULL) || (dataLen != (CELLULAR_IMSI_MAX_SIZE + 1U)))
    {
        LogError(("getImsi: pData is invalid or dataLen is wrong"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
        pRespLine = pAtResp->pItm->pLine;

        /* Removing all the Spaces in the AT Response. */
        atCoreStatus = Cellular_ATRemoveAllWhiteSpaces(pRespLine);

        if (atCoreStatus == CELLULAR_AT_SUCCESS)
        {
            if (strlen(pRespLine) < (CELLULAR_IMSI_MAX_SIZE + 1U))
            {
                (void) strncpy((char *) pData, pRespLine, dataLen);
            }
            else
            {
                atCoreStatus = CELLULAR_AT_ERROR;
            }
        }

        pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/* Implementation of Cellular_SetPdnConfig */
CellularError_t Cellular_SetPdnConfig (CellularHandle_t            cellularHandle,
                                       uint8_t                     contextId,
                                       const CellularPdnConfig_t * pPdnConfig)
{
    CellularContext_t * pContext       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    char cmdBuf[CELLULAR_AT_CMD_MAX_SIZE * 2] = {'\0'}; /* APN and auth info is too long */

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSetPdn;
    atReqSetPdn.pAtCmd       = cmdBuf;
    atReqSetPdn.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqSetPdn.pAtRspPrefix = NULL;
    atReqSetPdn.respCallback = NULL;
    atReqSetPdn.pData        = NULL;
    atReqSetPdn.dataLen      = 0;
#else
    CellularAtReq_t atReqSetPdn =
    {
        cmdBuf,
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (pPdnConfig == NULL)
    {
        LogError(("Cellular_ATCommandRaw: Input parameter is NULL"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        cellularStatus = _Cellular_IsValidPdn(contextId);
    }

    if (cellularStatus == CELLULAR_SUCCESS)
    {
        /* Make sure the library is open. */
        cellularStatus = _Cellular_CheckLibraryStatus(pContext);
    }
#endif

    if (cellularStatus == CELLULAR_SUCCESS)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(pContext);
#endif

        const char * pPdnType;

        /* Set PDN context IP type string for AT+CGDCONT */
        if (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV6)
        {
            pPdnType = "IPV6";
        }
        else if (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV4V6)
        {
            pPdnType = "IPV4V6";
        }
        else
        {
            pPdnType = "IP";           // default: CELLULAR_PDN_CONTEXT_IPV4
        }

        if ((pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV4V6) ||
                (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV6))
        {
            /* IPv6 address format setting */
            (void)snprintf(cmdBuf, sizeof(cmdBuf), "AT+CGPIAF=1,0,1,0");
            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqSetPdn);
        }

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            LogError(("Cellular_SetPdnConfig: can't set PDN, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            /* Build AT+CGDCONT command to set the PDN context type and APN name */
            (void) snprintf(cmdBuf, sizeof(cmdBuf), "AT+CGDCONT=%d,\"%s\",\"%s\"", contextId, pPdnType,
                            pPdnConfig->apnName);

            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqSetPdn);
        }

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            LogError(("Cellular_SetPdnConfig: can't set PDN, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            /* Check if APN authentication is specified */
            if (pPdnConfig->pdnAuthType == CELLULAR_PDN_AUTH_NONE)
            {
                (void) snprintf(cmdBuf, sizeof(cmdBuf), "AT+CGAUTH=%d,0", contextId);
            }
            else
            {
                /* Send AT+CGAUTH command to set APN authentication type, username, and password */
                (void) snprintf(cmdBuf,
                                sizeof(cmdBuf),
                                "AT+CGAUTH=%d,%d,\"%s\",\"%s\"",
                                contextId,
                                pPdnConfig->pdnAuthType,
                                pPdnConfig->username,
                                pPdnConfig->password);
            }

            pktStatus = _Cellular_AtcmdRequestWithCallback(pContext, atReqSetPdn);

            if (pktStatus != CELLULAR_PKT_STATUS_OK)
            {
                LogError(("Cellular_SetPdnConfig: can't set PDN, cmdBuf:%s, PktRet: %d", cmdBuf, pktStatus));
                cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(pContext, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_GetPhoneNum */
CellularError_t Cellular_GetPhoneNum(CellularHandle_t cellularHandle,
                                     const uint8_t  * p_phonenum)
{
    CellularContext_t * p_context       = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqGetPhonenum;
    atReqGetPhonenum.pAtCmd       = (char *)cmdBuf;
    atReqGetPhonenum.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqGetPhonenum.pAtRspPrefix = "+CNUM";
    atReqGetPhonenum.respCallback = _Cellular_RecvFuncGetPhoneNum;
    atReqGetPhonenum.pData        = (void *)p_phonenum;
    atReqGetPhonenum.dataLen      = CELLULAR_PHONENUM_MAX_SIZE;
#else
    CellularAtReq_t atReqGetPhonenum =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+CNUM",
        _Cellular_RecvFuncGetPhoneNum,
        (void *)p_phonenum, //cast
        CELLULAR_PHONENUM_MAX_SIZE,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_GetPhoneNum: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_phonenum)
        {
            LogError(("Cellular_GetPhoneNum: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS != cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CNUM");    //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetPhonenum);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_GetPhoneNum: couldn't resolve phone number"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetPhoneNum(CellularContext_t                 * p_context,
                                                         const CellularATCommandResponse_t * p_atresp,
                                                         void                              * p_data,
                                                         uint16_t                            dataLen)
{
    (void)dataLen;

    uint8_t * p_inputline                              = NULL;
    uint8_t   phonenum[CELLULAR_PHONENUM_MAX_SIZE + 1] = {0};
    bool      parseStatus                              = true;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetPhoneNum: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline  = (uint8_t *)p_atresp->pItm->pLine;                //cast
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATRemovePrefix((char **)&p_inputline);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATRemovePrefix(&(char *)p_inputline);   //cast
#endif

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (CELLULAR_PKT_STATUS_OK == pktStatus)
    {
        parseStatus = _parseGetPhoneNum(p_inputline, phonenum);

        if (true == parseStatus)
        {
            (void)strcpy((char *)p_data, (char *)phonenum); //cast
        }
        else
        {
            pktStatus = CELLULAR_PKT_STATUS_FAILURE;
        }
    }

    return pktStatus;
}

static bool _parseGetPhoneNum(uint8_t * p_QcsqPayload,
                              uint8_t * p_GetPhonenum)
{
    uint8_t * p_Token          = NULL;
    uint8_t * p_TmpQcsqPayload = p_QcsqPayload;
    bool      parseStatus      = true;

    CellularATError_t atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == p_GetPhonenum) || (NULL == p_QcsqPayload))
    {
        LogError(("_parseGetPhoneNum: Invalid Input Parameters"));
        parseStatus = false;
    }
#endif

    if (true == parseStatus)
    {
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATGetNextTok((char **)&p_TmpQcsqPayload, (char **)&p_Token);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATGetNextTok(&(char *)p_TmpQcsqPayload, &(char *)p_Token);  //cast
#endif
        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            LogError(("_parseGetPhoneNum: Error in processing Get Phone Number. Token %s", p_Token));
            parseStatus = false;
        }
    }

    if ((true == parseStatus))
    {
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATGetNextTok((char **)&p_TmpQcsqPayload, (char **)&p_Token);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATGetNextTok(&(char *)p_TmpQcsqPayload, &(char *)p_Token);  //cast
#endif
        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            LogError(("_parseGetPhoneNum: Error in processing Get Phone Number. Token %s", p_Token));
            parseStatus = false;
        }
        else
        {
            (void)strcpy((char *)p_GetPhonenum, (char *)p_Token);   //cast
        }
    }

    return parseStatus;
}

/* Implementation of Cellular_PingRequest */
CellularError_t Cellular_PingRequest(CellularHandle_t      cellularHandle,
                                     const uint8_t * const p_hostname)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqPing;
    atReqPing.pAtCmd       = (char *)cmdBuf;
    atReqPing.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
    atReqPing.pAtRspPrefix = "+PING";
    atReqPing.respCallback = _Cellular_RecvFuncPingEcho;
    atReqPing.pData        = NULL;
    atReqPing.dataLen      = 0;
#else
    CellularAtReq_t atReqPing =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+PING",
        _Cellular_RecvFuncPingEcho,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_PingRequest: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_hostname)
        {
            LogError(("Cellular_PingRequest: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                        "AT+PING=\"%s\"", p_hostname);
        pktStatus = _Cellular_TimeoutAtcmdRequestWithCallback(p_context, atReqPing,
                                                                RM_CELLULAR_RYZ_DATA_SEND_TIMEOUT_MS);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_PingRequest: couldn't resolve ping"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncPingEcho(CellularContext_t                 * p_context,
                                                      const CellularATCommandResponse_t * p_atresp,
                                                      void                              * p_data,
                                                      uint16_t                            dataLen)
{
    (void)p_data;
    (void)dataLen;

    uint8_t * p_inputline = NULL;

    CellularPktStatus_t       pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATCommandLine_t * p_line    = NULL;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncPingEcho: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline = (uint8_t *)p_atresp->pItm->pLine; //cast
        p_line      = p_atresp->pItm->pNext;
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (CELLULAR_PKT_STATUS_OK == pktStatus)
    {
        while (1)
        {
            if (NULL != p_context->cbEvents.genericCallback)
            {
                p_context->cbEvents.genericCallback((char *)p_inputline, p_context->cbEvents.pGenericCallbackContext);
            }

            if (NULL != p_line)
            {
                p_inputline = (uint8_t *)p_line->pLine; //cast
                p_line      = p_line->pNext;
            }
            else
            {
                break;
            }
        }
    }

    return pktStatus;
}

/* Implementation of Cellular_SetOperator */
CellularError_t Cellular_SetOperator(CellularHandle_t      cellularHandle,
                                     const uint8_t * const p_operator_name)
{
    CellularContext_t      * p_context  = (CellularContext_t *) cellularHandle;                  //cast
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;    //cast

    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint8_t             resBuf[20]     = {'\0'};
    uint8_t             count          = 0;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqGetOpName;
    atReqGetOpName.pAtCmd       = (char *)cmdBuf;
    atReqGetOpName.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqGetOpName.pAtRspPrefix = "+SQNCTM";
    atReqGetOpName.respCallback = _Cellular_RecvFuncGetOperatorName;
    atReqGetOpName.pData        = resBuf;
    atReqGetOpName.dataLen      = sizeof(resBuf);

    CellularAtReq_t atReqother;
    atReqother.pAtCmd       = (char *)cmdBuf;
    atReqother.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqother.pAtRspPrefix = NULL;
    atReqother.respCallback = NULL;
    atReqother.pData        = NULL;
    atReqother.dataLen      = 0;
#else
    CellularAtReq_t atReqGetOpName =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SQNCTM",
        _Cellular_RecvFuncGetOperatorName,
        resBuf,
        sizeof(resBuf),
    };

    CellularAtReq_t atReqother =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_SetOperator: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_operator_name)
        {
            LogError(("Cellular_SetOperator: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNCTM?"); //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetOpName);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetOperator: couldn't resolve get operator name"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if ((CELLULAR_SUCCESS == cellularStatus) &&
            (strcmp((const char *)p_operator_name, (const char *)resBuf) != 0))  //cast
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNCTM=\"%s\"", p_operator_name);  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqother);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetOperator: couldn't resolve set operator name"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            count = 0;

            while (1)
            {
                if (AWS_CELLULAR_MODULE_START_FLG == p_aws_ctrl->module_flg)
                {
                    p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;
                    break;
                }
                else
                {
                    count++;
                    Platform_Delay(1000);   //cast
                }

                if (count > AWS_CELLULAR_RESTART_LIMIT)
                {
                    cellularStatus = CELLULAR_TIMEOUT;
                    break;
                }
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_SetBand */
CellularError_t Cellular_SetBand(CellularHandle_t      cellularHandle,
                                 const uint8_t * const p_band)
{
    CellularContext_t      * p_context  = (CellularContext_t *) cellularHandle;                  //cast
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;    //cast

    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint8_t             resBuf[20]     = {'\0'};
    uint8_t             rat            = 0;    // 0 = M1, 1 = NB1
    uint8_t             count          = 0;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqGetOpName;
    atReqGetOpName.pAtCmd       = (char *)cmdBuf;
    atReqGetOpName.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqGetOpName.pAtRspPrefix = "+SQNCTM";
    atReqGetOpName.respCallback = _Cellular_RecvFuncGetOperatorName;
    atReqGetOpName.pData        = resBuf;
    atReqGetOpName.dataLen      = sizeof(resBuf);

    CellularAtReq_t atReqother;
    atReqother.pAtCmd       = (char *)cmdBuf;
    atReqother.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqother.pAtRspPrefix = NULL;
    atReqother.respCallback = NULL;
    atReqother.pData        = NULL;
    atReqother.dataLen      = 0;
#else
    CellularAtReq_t atReqGetOpName =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SQNCTM",
        _Cellular_RecvFuncGetOperatorName,
        resBuf,
        sizeof(resBuf),
    };

    CellularAtReq_t atReqother =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_SetBand: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_band)
        {
            LogError(("Cellular_SetBand: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNCTM?"); //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetOpName);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetBand: couldn't resolve get operator name"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,    //cast
                        "AT+SQNBANDSEL=%d,\"%s\",\"%s\"", rat, resBuf, p_band);
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqother);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetBand: couldn't resolve set band"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT^RESET");   //cast
            pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqother);

            if (CELLULAR_PKT_STATUS_OK != pktStatus)
            {
                LogError(("Cellular_SetBand: couldn't resolve reset"));
                cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
            }
            else
            {
                count = 0;

                while (1)
                {
                    if (AWS_CELLULAR_MODULE_START_FLG == p_aws_ctrl->module_flg)
                    {
                        p_aws_ctrl->module_flg = AWS_CELLULAR_MODULE_FLG_INIT;
                        break;
                    }
                    else
                    {
                        count++;
                        Platform_Delay(1000);   //cast
                    }

                    if (count > AWS_CELLULAR_RESTART_LIMIT)
                    {
                        cellularStatus = CELLULAR_TIMEOUT;
                        break;
                    }
                }
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetOperatorName(CellularContext_t                 * p_context,
                                                             const CellularATCommandResponse_t * p_atresp,
                                                             void                              * p_data,
                                                             uint16_t                            dataLen)
{
    (void)dataLen;

    uint8_t * p_token     = NULL;
    uint8_t * p_inputline = NULL;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetOperatorName: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline  = (uint8_t *)p_atresp->pItm->pLine;    //cast
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATRemovePrefix((char **)&p_inputline);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATRemovePrefix(&(char *)p_inputline);   //cast
#endif

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token); /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if ((CELLULAR_PKT_STATUS_OK == pktStatus) && (NULL != p_data) && (NULL != p_token))
    {
        (void)strcpy((char *)p_data, (char *)p_token);  //cast
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }

    return pktStatus;
}

CellularError_t Cellular_FirmUpgrade(CellularHandle_t                    cellularHandle,
                                     const uint8_t * const               p_url,
                                     const e_aws_cellular_firmupgrade_t  command,
                                     const uint8_t                       spid)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqFirmUpgrade;
    atReqFirmUpgrade.pAtCmd       = (char *)cmdBuf;
    atReqFirmUpgrade.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqFirmUpgrade.pAtRspPrefix = NULL;
    atReqFirmUpgrade.respCallback = NULL;
    atReqFirmUpgrade.pData        = NULL;
    atReqFirmUpgrade.dataLen      = 0;
#else
    CellularAtReq_t atReqFirmUpgrade =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_FirmUpgrade: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_url)
        {
            LogError(("Cellular_FirmUpgrade: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        if (0 == spid)
        {
            (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,    //cast
                            "AT+SQNSUPGRADE=\"%s\",0,5,%d", p_url, command);
        }
        else
        {
            (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,    //cast
                            "AT+SQNSUPGRADE=\"%s\",0,5,%d,%d", p_url, command, spid);
        }
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFirmUpgrade);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_FirmUpgrade: couldn't resolve firm upgrade"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

CellularError_t Cellular_GetUpgradeState(CellularHandle_t   cellularHandle,
                                         const uint8_t    * p_state,
                                         const uint32_t     size)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqFirmUpgrade;
    atReqFirmUpgrade.pAtCmd       = (char *)cmdBuf;
    atReqFirmUpgrade.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqFirmUpgrade.pAtRspPrefix = "+SQNSUPGRADE";
    atReqFirmUpgrade.respCallback = _Cellular_RecvFuncGetUpgradeState;
    atReqFirmUpgrade.pData        = (void *)p_state;
    atReqFirmUpgrade.dataLen      = size;
#else
    CellularAtReq_t atReqFirmUpgrade =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SQNSUPGRADE",
        _Cellular_RecvFuncGetUpgradeState,
        (void *)p_state,    //cast
        size,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_GetUpgradeState: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_state)
        {
            LogError(("Cellular_GetUpgradeState: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSUPGRADE?");

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFirmUpgrade);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_GetUpgradeState: couldn't resolve AT+SQNSUPGRADE?"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetUpgradeState(CellularContext_t                  * p_context,
                                                              const CellularATCommandResponse_t * p_atresp,
                                                              void                              * p_data,
                                                              uint16_t                            dataLen)
{
    (void)dataLen;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetUpgradeState: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        if (dataLen > strlen(p_atresp->pItm->pLine))
        {
            sprintf(p_data, "%s", p_atresp->pItm->pLine);
        }
        else
        {
            LogError(("_Cellular_RecvFuncGetUpgradeState: Buffer size is small"));
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

CellularError_t Cellular_WriteCertificate(CellularHandle_t                 cellularHandle,
                                           const e_aws_cellular_nvm_type_t data_type,
                                           const uint8_t                   index,
                                           const uint8_t * const           p_data,
                                           const uint32_t                  size)
{
    CellularContext_t * p_context      = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint32_t            timeout        = 1000;
    uint32_t            p_len          = 0;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqWriteCert;
    atReqWriteCert.pAtCmd       = (char *)cmdBuf;
    atReqWriteCert.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqWriteCert.pAtRspPrefix = NULL;
    atReqWriteCert.respCallback = NULL;
    atReqWriteCert.pData        = NULL;
    atReqWriteCert.dataLen      = 0;

    CellularAtDataReq_t atDataReqWriteCert;
    atDataReqWriteCert.pData           = p_data;
    atDataReqWriteCert.dataLen         = size;
    atDataReqWriteCert.pSentDataLength = &p_len;
    atDataReqWriteCert.pEndPattern     = NULL;
    atDataReqWriteCert.endPatternLen   = 0;
#else
    CellularAtReq_t     atReqWriteCert =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    CellularAtDataReq_t atDataReqWriteCert =
    {
        p_data,
        size,
        &p_len,
        NULL,
        0
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_WriteCertificate: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if ((NULL == p_data) ||
            ((AWS_CELLULAR_NVM_TYPE_CERTIFICATE != data_type) && (AWS_CELLULAR_NVM_TYPE_PRIVATEKEY != data_type)) ||
                (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX < index) ||
                (RM_CELLULAR_NVM_CERTIFICATE_SIZE_L > size) || (RM_CELLULAR_NVM_CERTIFICATE_SIZE_H < size))
        {
            LogError(("Cellular_WriteCertificate: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        if (AWS_CELLULAR_NVM_TYPE_CERTIFICATE == data_type)
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //casst
                            "AT+SQNSNVW=\"%s\",%u,%u", "certificate", index, size);
        }
        else
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                            "AT+SQNSNVW=\"%s\",%u,%u", "privatekey", index, size);
        }

        pktStatus = _Cellular_AtcmdDataSend(p_context,
                                            atReqWriteCert,
                                            atDataReqWriteCert,
                                            socketSendDataPrefix,
                                            NULL,
                                            timeout,
                                            timeout,
                                            RM_CELLULAR_RYZ_DATA_SEND_INTER_DELAY_MS);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_WriteCertificate: couldn't resolve certificate write."));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

CellularError_t Cellular_EraseCertificate(CellularHandle_t                 cellularHandle,
                                           const e_aws_cellular_nvm_type_t data_type,
                                           const uint8_t                   index)
{
    CellularContext_t * p_context      = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqFirmUpgrade;
    atReqFirmUpgrade.pAtCmd       = (char *)cmdBuf;
    atReqFirmUpgrade.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqFirmUpgrade.pAtRspPrefix = NULL;
    atReqFirmUpgrade.respCallback = NULL;
    atReqFirmUpgrade.pData        = NULL;
    atReqFirmUpgrade.dataLen      = 0;
#else
    CellularAtReq_t atReqFirmUpgrade =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_EraseCertificate: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (((AWS_CELLULAR_NVM_TYPE_CERTIFICATE != data_type) && (AWS_CELLULAR_NVM_TYPE_PRIVATEKEY != data_type)) ||
                (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX < index))
        {
            LogError(("Cellular_EraseCertificate: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        if (AWS_CELLULAR_NVM_TYPE_CERTIFICATE == data_type)
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                                "AT+SQNSNVW=\"%s\",%d,0\r", "certificate", index);
        }
        else
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                                "AT+SQNSNVW=\"%s\",%d,0\r", "privatekey", index);
        }

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFirmUpgrade);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_EraseCertificate: couldn't resolve erace certificate"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

CellularError_t Cellular_GetCertificate(CellularHandle_t                 cellularHandle,
                                         const e_aws_cellular_nvm_type_t data_type,
                                         const uint8_t                   index,
                                         const uint8_t                 * p_buff,
                                         const uint32_t                  buff_length)
{
    CellularContext_t * p_context      = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqGetCert;
    atReqGetCert.pAtCmd       = (char *)cmdBuf;
    atReqGetCert.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqGetCert.pAtRspPrefix = "+SQNSNVR";
    atReqGetCert.respCallback = _Cellular_RecvFuncGetCertificate;
    atReqGetCert.pData        = (void *)p_buff;
    atReqGetCert.dataLen      = buff_length;
#else
    CellularAtReq_t atReqGetCert =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SQNSNVR",
        _Cellular_RecvFuncGetCertificate,
        (void *)p_buff,    //cast
        buff_length,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_GetCertificate: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (((AWS_CELLULAR_NVM_TYPE_CERTIFICATE != data_type) && (AWS_CELLULAR_NVM_TYPE_PRIVATEKEY != data_type)) ||
                (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX < index))
        {
            LogError(("Cellular_GetCertificate: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        if (AWS_CELLULAR_NVM_TYPE_CERTIFICATE == data_type)
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                                "AT+SQNSNVR=\"%s\",%d", "certificate", index);
        }
        else
        {
            (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                                "AT+SQNSNVR=\"%s\",%d", "privatekey", index);
        }

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetCert);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_GetCertificate: couldn't resolve get certificate"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetCertificate(CellularContext_t                  * p_context,
                                                             const CellularATCommandResponse_t * p_atresp,
                                                             void                              * p_data,
                                                             uint16_t                            dataLen)
{
    (void)dataLen;

    uint8_t       * p_inputline = NULL;
    uint8_t       * p_Token     = NULL;
    const uint8_t * p_Delimiter = ",";

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetCertificate: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline = (uint8_t *)p_atresp->pItm->pLine; //cast

        atCoreStatus = _Cellular_ATGetSpecificNextTok(&p_inputline,
                                                      p_Delimiter,
                                                      &p_Token);

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = _Cellular_ATGetSpecificNextTok(&p_inputline,
                                                          p_Delimiter,
                                                          &p_Token);
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (CELLULAR_PKT_STATUS_OK == pktStatus)
    {
        if (dataLen > strlen((char *)p_inputline))  //cast
        {
            sprintf(p_data, "%s", p_inputline);
        }
        else
        {
            LogError(("_Cellular_RecvFuncGetCertificate: Buffer size is small"));
        }
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }

    return pktStatus;
}

static CellularATError_t _Cellular_ATGetSpecificNextTok(uint8_t      ** pp_String,
                                                        const uint8_t * p_Delimiter,
                                                        uint8_t      ** pp_TokOutput)
{
    uint16_t  tokStrLen  = 0;
    uint16_t  dataStrlen = 0;
    uint8_t * p_tok      = NULL;

    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((NULL == pp_String) || (NULL == p_Delimiter) || (NULL == pp_TokOutput))
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }
#endif

    if (CELLULAR_AT_SUCCESS == atStatus)
    {
        dataStrlen = (uint16_t)strlen((char *)*pp_String);  //cast

        if ((**pp_String) == (*p_Delimiter))
        {
            **pp_String = '\0';
            p_tok       = *pp_String;
        }
        else
        {
            p_tok = (uint8_t *)strtok((char *)*pp_String, (char *)p_Delimiter);    //cast
        }
    }

    if (CELLULAR_AT_SUCCESS == atStatus)
    {
        tokStrLen = (uint16_t)strlen((char *)p_tok);    //cast

        if ((tokStrLen < dataStrlen) && ('\0' != (*pp_String)[tokStrLen + 1U]))
        {
            *pp_String = &p_tok[strlen((char *)p_tok) + 1U];    //cast
        }
        else
        {
            *pp_String = &p_tok[strlen((char *)p_tok)];    //cast
        }

        *pp_TokOutput = p_tok;
    }

    return atStatus;
}

CellularError_t Cellular_ConfigSSLProfile(CellularHandle_t                       cellularHandle,
                                           const uint8_t                         security_profile_id,
                                           const e_aws_cellular_validate_level_t cert_valid_level,
                                           const uint8_t                         ca_certificate_id,
                                           const uint8_t                         client_certificate_id,
                                           const uint8_t                         client_privatekey_id)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};
    uint8_t             arg[3][3]                        = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqFirmUpgrade;
    atReqFirmUpgrade.pAtCmd       = (char *)cmdBuf;
    atReqFirmUpgrade.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqFirmUpgrade.pAtRspPrefix = NULL;
    atReqFirmUpgrade.respCallback = NULL;
    atReqFirmUpgrade.pData        = NULL;
    atReqFirmUpgrade.dataLen      = 0;
#else
    CellularAtReq_t atReqFirmUpgrade =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_ConfigSSLProfile: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if ((RM_CELLULAR_SECURITY_PROFILE_ID_L > security_profile_id) ||
            (RM_CELLULAR_SECURITY_PROFILE_ID_H < security_profile_id) ||
                ((AWS_CELLULAR_NO_CERT_VALIDATE         != cert_valid_level) &&
                 (AWS_CELLULAR_VALIDATE_CERT_EXPDATE    != cert_valid_level) &&
                 (AWS_CELLULAR_VALIDATE_CERT_EXPDATE_CN != cert_valid_level)))
        {
            LogError(("Cellular_ConfigSSLProfile: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        if (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX >= ca_certificate_id)
        {
            sprintf((char *)arg[0], "%d", ca_certificate_id);   //cast
        }
        else
        {
            arg[0][0] = '\0';
        }

        if (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX >= client_certificate_id)
        {
            sprintf((char *)arg[1], "%d", client_certificate_id); //cast
        }
        else
        {
            arg[1][0] =  '\0';
        }

        if (RM_CELLULAR_MAX_NVM_CERTIFICATE_INDEX >= client_privatekey_id)
        {
            sprintf((char *)arg[2], "%d", client_privatekey_id); //cast
        }
        else
        {
            arg[2][0] =  '\0';
        }

        (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                        "AT+SQNSPCFG=%d,2,,%d,%s,%s,%s,\"\"",
                        security_profile_id,
                        cert_valid_level,
                        arg[0], arg[1], arg[2]);

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFirmUpgrade);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_ConfigSSLProfile: couldn't resolve config ssl profile"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
CellularError_t Cellular_ConfigTlslSocket(CellularHandle_t cellularHandle,
                                          const uint8_t    socket_id,
                                          const bool       enable,
                                          const uint8_t    security_profile_id)
{
    CellularContext_t * p_context = (CellularContext_t *) cellularHandle;
    CellularError_t     cellularStatus;
    CellularPktStatus_t pktStatus;
    char                cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

    CellularAtReq_t atReq;
    atReq.pAtCmd       = cmdBuf;
    atReq.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReq.pAtRspPrefix = NULL;
    atReq.respCallback = NULL;
    atReq.pData        = NULL;
    atReq.dataLen      = 0;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if ((RM_CELLULAR_SECURITY_PROFILE_ID_L > security_profile_id) ||
        (RM_CELLULAR_SECURITY_PROFILE_ID_H < security_profile_id))
    {
        LogError(("Cellular_ConfigTlslSocket: Bad input Parameter "));
        return (CELLULAR_BAD_PARAMETER);
    }

    if ((0 == socket_id) || (CELLULAR_NUM_SOCKET_MAX < socket_id))
    {
        LogError(("Cellular_ConfigTlslSocket: Bad input Parameter "));
        return (CELLULAR_BAD_PARAMETER);
    }

    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_EraseCertificate: _Cellular_CheckLibraryStatus failed"));
        return (cellularStatus);
    }
#endif

    (void)snprintf(cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSSCFG=%d,%d,%d\r",
                   socket_id, enable, security_profile_id);
    pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReq);
    if (CELLULAR_PKT_STATUS_OK != pktStatus)
    {
        LogError(("Cellular_ConfigTlslSocket: couldn't resolve erase certificate"));
    }

    cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
    return cellularStatus;
}
#endif

/* Implementation of Cellular_AutoAPConnectConfig */
CellularError_t Cellular_AutoAPConnectConfig(CellularHandle_t              cellularHandle,
                                             const e_aws_cellular_auto_connect_t type)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqAutoConfig;
    atReqAutoConfig.pAtCmd       = (char *)cmdBuf;
    atReqAutoConfig.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqAutoConfig.pAtRspPrefix = NULL;
    atReqAutoConfig.respCallback = NULL;
    atReqAutoConfig.pData        = NULL;
    atReqAutoConfig.dataLen      = 0;
#else
    CellularAtReq_t atReqAutoConfig =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_AutoAPConnectConfig: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if ((AWS_CELLULAR_DISABLE_AUTO_CONNECT != type) && (AWS_CELLULAR_ENABLE_AUTO_CONNECT != type))
        {
            LogError(("Cellular_AutoAPConnectConfig: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                        "AT+SQNAUTOCONNECT=%s", type);
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqAutoConfig);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_AutoAPConnectConfig: couldn't resolve auto ap connect config"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_UnlockSIM */
CellularError_t Cellular_UnlockSIM(CellularHandle_t      cellularHandle,
                                   const uint8_t * const p_pass)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    uint8_t             resBuf[20]     = {'\0'};
    int32_t             funclevel      = 0;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqFuncGetString;
    atReqFuncGetString.pAtCmd       = (char *)cmdBuf;
    atReqFuncGetString.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqFuncGetString.pAtRspPrefix = "+CFUN";
    atReqFuncGetString.respCallback = _Cellular_RecvFuncGetString;
    atReqFuncGetString.pData        = resBuf;
    atReqFuncGetString.dataLen      = sizeof(resBuf);

    CellularAtReq_t atReqCommon;
    atReqCommon.pAtCmd       = (char *)cmdBuf;
    atReqCommon.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqCommon.pAtRspPrefix = NULL;
    atReqCommon.respCallback = NULL;
    atReqCommon.pData        = NULL;
    atReqCommon.dataLen      = 0;
#else
    CellularAtReq_t atReqFuncGetString =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+CFUN",
        _Cellular_RecvFuncGetString,
        resBuf,
        sizeof(resBuf),
    };

    CellularAtReq_t atReqCommon =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_UnlockSIM: _Cellular_CheckLibraryStatus failed"));
    }
    else
    {
        if (NULL == p_pass)
        {
            LogError(("Cellular_UnlockSIM: Bad input Parameter "));
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN?");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFuncGetString);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_UnlockSIM: couldn't resolve func check"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            sscanf((char *)resBuf, "%ld", &funclevel);  //cast
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if ((CELLULAR_SUCCESS == cellularStatus) &&
            ((AWS_CELLULAR_MODULE_OPERATING_LEVEL1 != funclevel) &&
             (AWS_CELLULAR_MODULE_OPERATING_LEVEL4 != funclevel)))
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=4");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqCommon);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_UnlockSIM: couldn't resolve set func"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        Platform_Delay(1000);   //cast

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CPIN?");  //cast
        atReqFuncGetString.pAtRspPrefix = "+CPIN";

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqFuncGetString);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_UnlockSIM: couldn't resolve get lock state"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if ((CELLULAR_SUCCESS == cellularStatus) && (NULL == strstr((char *)resBuf, "READY")))  //cast
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CPIN=\"%s\"", (char *)p_pass);  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqCommon);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_UnlockSIM: couldn't resolve unlock sim"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetString(CellularContext_t                 * p_context,
                                                       const CellularATCommandResponse_t * p_atresp,
                                                       void                              * p_data,
                                                       uint16_t                            dataLen)
{
    (void)dataLen;

    uint8_t * p_token     = NULL;
    uint8_t * p_inputline = NULL;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetString: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline  = (uint8_t *)p_atresp->pItm->pLine;    //cast
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATRemovePrefix((char **)&p_inputline);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATRemovePrefix(&(char *)p_inputline);   //cast
#endif

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token);  /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if ((CELLULAR_PKT_STATUS_OK == pktStatus) && (NULL != p_data) && (NULL != p_token))
    {
        (void)strcpy((char *)p_data, (char *)p_token);  //cast
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }

    return pktStatus;
}

/* Implementation of Cellular_FactoryReset */
CellularError_t Cellular_FactoryReset(CellularHandle_t cellularHandle)
{
    CellularContext_t      * p_context       = (CellularContext_t *)cellularHandle;
    CellularError_t          cellularStatus  = CELLULAR_SUCCESS;
    CellularPktStatus_t      pktStatus       = CELLULAR_PKT_STATUS_OK;
    CellularPdnContextInfo_t pdpContextsInfo = {0};
    uint8_t                  cnt             = 0;
    uint8_t                  cnt_res         = RM_CELLULAR_RYZ_MAX_PDP_CONTEXTS;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t               semaphore_ret   = 0;
#endif

    uint8_t cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqCommon;
    atReqCommon.pAtCmd       = (char *)cmdBuf;
    atReqCommon.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqCommon.pAtRspPrefix = NULL;
    atReqCommon.respCallback = NULL;
    atReqCommon.pData        = NULL;
    atReqCommon.dataLen      = 0;

    CellularAtReq_t atReqGetCurrentApnName;
    atReqGetCurrentApnName.pAtCmd       = "AT+CGDCONT?";
    atReqGetCurrentApnName.atCmdType    = CELLULAR_AT_MULTI_WITH_PREFIX;
    atReqGetCurrentApnName.pAtRspPrefix = "+CGDCONT";
    atReqGetCurrentApnName.respCallback = _Cellular_RecvFuncGetPdpContextSettings;
    atReqGetCurrentApnName.pData        = &pdpContextsInfo;
    atReqGetCurrentApnName.dataLen      = sizeof(CellularPdnContextInfo_t);
#else
    CellularAtReq_t atReqCommon =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    CellularAtReq_t atReqGetCurrentApnName =
    {
        "AT+CGDCONT?",
        CELLULAR_AT_MULTI_WITH_PREFIX,
        "+CGDCONT",
        _Cellular_RecvFuncGetPdpContextSettings,
        &pdpContextsInfo,
        sizeof(CellularPdnContextInfo_t),
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_FactoryReset: _Cellular_CheckLibraryStatus failed"));
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CGDCONT?");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetCurrentApnName);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_FactoryReset: couldn't resolve AT+CGDCONT?"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            for (cnt = RM_CELLULAR_RYZ_MAX_PDP_CONTEXTS; cnt > 0; cnt--)
            {
                if (false == pdpContextsInfo.contextsPresent[cnt - 1])
                {
                    cnt_res = cnt;
                    cnt     = 1;    /* Exit this For Loop to set "1" */
                }
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,    //cast
                        "AT+CGDCONT=%d,\"IPV4V6\",\"%s\"", cnt_res, "RESET");
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqCommon);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_FactoryReset: couldn't resolve AT+CGDCONT"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        cellularStatus = aws_cellular_psm_config(p_context, 0);

        if (CELLULAR_SUCCESS != cellularStatus)
        {
            LogError(("Cellular_FactoryReset: couldn't resolve psm config"));
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        cellularStatus = aws_cellular_closesocket(p_context);

        if (CELLULAR_SUCCESS == cellularStatus)
        {
            (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSFACTORYRESET");   //cast
            (void)_Cellular_AtcmdRequestWithCallback(p_context, atReqCommon);
            cellularStatus = aws_cellular_hardreset(p_context);
        }
        else
        {
            LogError(("Cellular_FactoryReset: couldn't resolve AT+SQNSFACTORYRESET"));
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        memset((void *)&pdpContextsInfo, 0x00, sizeof(CellularPdnContextInfo_t));   //cast
        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CGDCONT?");    //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqGetCurrentApnName);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_FactoryReset: couldn't resolve AT+CGDCONT?"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            if ((false != pdpContextsInfo.contextsPresent[cnt_res - 1]) &&
                (0     == strncmp((char *)pdpContextsInfo.apnName[cnt_res - 1], "RESET", 5)))  //cast
            {
                LogError(("Cellular_FactoryReset: couldn't resolve factory reset"));
                cellularStatus = CELLULAR_UNKNOWN;
            }
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_SetBaudrate */
CellularError_t Cellular_SetBaudrate(CellularHandle_t cellularHandle,
                                     const uint32_t   baudrate)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
    sci_err_t           sci_ret        = SCI_SUCCESS;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQuery;
    atReqQuery.pAtCmd       = (char *)cmdBuf;
    atReqQuery.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqQuery.pAtRspPrefix = NULL;
    atReqQuery.respCallback = NULL;
    atReqQuery.pData        = NULL;
    atReqQuery.dataLen      = 0;
#else
    CellularAtReq_t atReqQuery =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_SetBaudrate: _Cellular_CheckLibraryStatus failed"));
    }
    else if (0 == baudrate)
    {
        LogError(("Cellular_SetBaudrate: Invalid argument"));
        pktStatus = CELLULAR_PKT_STATUS_BAD_PARAM;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=5");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQuery);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetBaudrate: couldn't resolve %s", cmdBuf));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,
                        "AT+SQNHWCFG=\"uart0\",\"enable\",\"rtscts\",\"%ld\",\"8\",\"none\",\"1\",\"at\"", baudrate);
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQuery);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetBaudrate: couldn't resolve %s", cmdBuf));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        sci_ret = aws_cellular_temp_close(cellularHandle);

        if (SCI_SUCCESS == sci_ret)
        {
            sci_ret = aws_cellular_serial_reopen(cellularHandle, baudrate);
        }
        else
        {
            cellularStatus = CELLULAR_UNKNOWN;
        }

        if (SCI_SUCCESS == sci_ret)
        {
            cellularStatus = aws_cellular_hardreset(p_context);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_StartUpLink */
CellularError_t Cellular_StartUpLink(CellularHandle_t    cellularHandle,
                                     const uint8_t       enable,
                                     const uint16_t      earfcn,
                                     const uint32_t      level)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQueryStartUpLink;
    atReqQueryStartUpLink.pAtCmd       = (char *)cmdBuf;
    atReqQueryStartUpLink.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqQueryStartUpLink.pAtRspPrefix = NULL;
    atReqQueryStartUpLink.respCallback = NULL;
    atReqQueryStartUpLink.pData        = NULL;
    atReqQueryStartUpLink.dataLen      = 0;
#else
    CellularAtReq_t atReqQueryStartUpLink =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_StartUpLink: _Cellular_CheckLibraryStatus failed"));
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=5");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQueryStartUpLink);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_StartUpLink: couldn't resolve Start Up Link"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void) snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,   //cast
                        "AT+SMCWTX=%u,%u,%ld", enable, earfcn, level);
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQueryStartUpLink);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_StartUpLink: couldn't resolve Start Up Link"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

/* Implementation of Cellular_StartDownLink */
CellularError_t Cellular_StartDownLink(CellularHandle_t    cellularHandle,
                                       const uint16_t      earfcn)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReq;
    atReq.pAtCmd       = (char *)cmdBuf;
    atReq.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReq.pAtRspPrefix = NULL;
    atReq.respCallback = NULL;
    atReq.pData        = NULL;
    atReq.dataLen      = 0;

    CellularAtReq_t atReqQueryStartDownLink;
    atReqQueryStartDownLink.pAtCmd       = (char *)cmdBuf;
    atReqQueryStartDownLink.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqQueryStartDownLink.pAtRspPrefix = "+SMCWRX";
    atReqQueryStartDownLink.respCallback = _Cellular_RecvFuncStartDownLink;
    atReqQueryStartDownLink.pData        = NULL;
    atReqQueryStartDownLink.dataLen      = 0;
#else
    CellularAtReq_t atReq =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    CellularAtReq_t atReqQueryStartDownLink =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SMCWRX",
        _Cellular_RecvFuncStartDownLink,
        NULL,  //cast
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_StartDownLink: _Cellular_CheckLibraryStatus failed"));
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+CFUN=5");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReq);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_StartDownLink: couldn't resolve Start Down Link"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SMCWRX=%d", earfcn);   //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQueryStartDownLink);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_StartDownLink: couldn't resolve Start Down Link"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncStartDownLink(CellularContext_t                 * p_context,
                                                           const CellularATCommandResponse_t * p_atresp,
                                                           void                              * p_data,
                                                           uint16_t                            dataLen)
{
    (void)dataLen;

    uint8_t * p_inputline = NULL;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncStartDownLink: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline = (uint8_t *)p_atresp->pItm->pLine; //cast
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATRemovePrefix((char **)&p_inputline);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATRemovePrefix(&(char *)p_inputline);   //cast
#endif

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    if (CELLULAR_PKT_STATUS_OK == pktStatus)
    {
        *(int32_t *)p_data = atoi((char *)p_inputline); //cast
    }
    else
    {
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }

    return pktStatus;
}

/* Implementation of Cellular_SetPsmSettings */
CellularError_t Cellular_SetPsmSettings(CellularHandle_t              cellularHandle,
                                        const CellularPsmSettings_t * pPsmSettings)
{
    CellularContext_t      * p_context  = (CellularContext_t *)cellularHandle;                  //cast
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;   //cast

    CellularError_t cellularStatus = CELLULAR_SUCCESS;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t      semaphore_ret  = 0;
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* Make sure the library is open. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("_Cellular_CheckLibraryStatus failed"));
    }
    else if (NULL == pPsmSettings)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        cellularStatus = aws_cellular_psm_config(p_context, pPsmSettings->mode);

        if (CELLULAR_SUCCESS == cellularStatus)
        {
            cellularStatus = Cellular_CommonSetPsmSettings(cellularHandle, pPsmSettings);
        }

        if ((CELLULAR_SUCCESS == cellularStatus) && (1 == pPsmSettings->mode))
        {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
            aws_cellular_rts_hw_flow_disable();
#endif
            aws_cellular_rts_ctrl(1);
            p_aws_ctrl->psm_mode = 1;
        }
        else
        {
            aws_cellular_psm_config(p_context, 0);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_GetSVN */
CellularError_t Cellular_GetSVN(CellularHandle_t    cellularHandle,
                                const st_aws_cellular_svn_t * p_svn)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQueryStartUpLink;
    atReqQueryStartUpLink.pAtCmd       = (char *)cmdBuf;
    atReqQueryStartUpLink.atCmdType    = CELLULAR_AT_MULTI_WO_PREFIX;
    atReqQueryStartUpLink.pAtRspPrefix = NULL;
    atReqQueryStartUpLink.respCallback = _Cellular_RecvFuncGetSVN;
    atReqQueryStartUpLink.pData        = (void *)p_svn;
    atReqQueryStartUpLink.dataLen      = sizeof(st_aws_cellular_svn_t);
#else
    CellularAtReq_t atReqQueryStartUpLink =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_MULTI_WO_PREFIX,
        NULL,
        _Cellular_RecvFuncGetSVN,
        (void *)p_svn,  //cast
        sizeof(st_aws_cellular_svn_t),
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_GetSVN: _Cellular_CheckLibraryStatus failed"));
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "ATI1");  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQueryStartUpLink);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_GetSVN: couldn't resolve ATI1 command"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetSVN(CellularContext_t                 * p_context,
                                                    const CellularATCommandResponse_t * p_atresp,
                                                    void                              * p_data,
                                                    uint16_t                            dataLen)
{
    (void)dataLen;

    st_aws_cellular_svn_t * p_svn       = p_data;
    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetSVN: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        (void)snprintf((char *)p_svn->ue_svn, AWS_CELLULAR_MAX_UE_SVN_LENGTH, p_atresp->pItm->pLine);           //cast
        (void)snprintf((char *)p_svn->lr_svn, AWS_CELLULAR_MAX_LR_SVN_LENGTH, p_atresp->pItm->pNext->pLine);    //cast
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}

/* Implementation of Cellular_SetEidrxSettingsEXT */
CellularError_t Cellular_SetEidrxSettingsEXT(CellularHandle_t                cellularHandle,
                                             const CellularEidrxSettings_t * p_eidrxsettings)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqSetEidrx;
    atReqSetEidrx.pAtCmd       = (char *)cmdBuf;
    atReqSetEidrx.atCmdType    = CELLULAR_AT_NO_RESULT;
    atReqSetEidrx.pAtRspPrefix = NULL;
    atReqSetEidrx.respCallback = NULL;
    atReqSetEidrx.pData        = NULL;
    atReqSetEidrx.dataLen      = 0;
#else
    CellularAtReq_t atReqSetEidrx =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError(("_Cellular_CheckLibraryStatus failed"));
    }
    else if (NULL == p_eidrxsettings)
    {
        LogError(("Cellular_CommonSetEidrxSettings : Bad parameter"));
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
#endif
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE,    //cast
                        "%s%d,%d,\"" PRINTF_BINARY_PATTERN_INT4 "\",\"" PRINTF_BINARY_PATTERN_INT4 "\"",
                        "AT+SQNEDRX=",
                        p_eidrxsettings->mode,
                        p_eidrxsettings->rat,
                        PRINTF_BYTE_TO_BINARY_INT4(p_eidrxsettings->requestedEdrxVaue),
                        PRINTF_BYTE_TO_BINARY_INT4(p_eidrxsettings->pagingTimeWindow));

        LogDebug(("Eidrx setting: %s ", cmdBuf));
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqSetEidrx);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("_Cellular_SetEidrxSettings: couldn't set Eidrx settings"));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return cellularStatus;
}

/* Implementation of Cellular_GetSocketDataSize */
CellularError_t Cellular_GetSocketDataSize(CellularHandle_t        cellularHandle,
                                            CellularSocketHandle_t socketHandle,
                                            const uint16_t       * p_size)
{
    CellularContext_t * p_context      = (CellularContext_t *)cellularHandle;
    CellularError_t     cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    BaseType_t          semaphore_ret  = 0;
#endif

    uint8_t             cmdBuf[CELLULAR_AT_CMD_MAX_SIZE] = {'\0'};

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularAtReq_t atReqQueryStartUpLink;
    atReqQueryStartUpLink.pAtCmd       = (char *)cmdBuf;
    atReqQueryStartUpLink.atCmdType    = CELLULAR_AT_WITH_PREFIX;
    atReqQueryStartUpLink.pAtRspPrefix = "+SQNSI";
    atReqQueryStartUpLink.respCallback = _Cellular_RecvFuncGetSocketDataSize;
    atReqQueryStartUpLink.pData        = (void *)p_size;
    atReqQueryStartUpLink.dataLen      = sizeof(uint16_t);
#else
    CellularAtReq_t atReqQueryStartUpLink =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_WITH_PREFIX,
        "+SQNSI",
        _Cellular_RecvFuncGetSocketDataSize,
        (void *)p_size,  //cast
        sizeof(uint16_t),
    };
#endif

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    /* p_context is checked in _Cellular_CheckLibraryStatus function. */
    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogDebug(("Cellular_GetSocketDataSize: _Cellular_CheckLibraryStatus failed"));
    }

    if ((NULL == socketHandle) || (NULL == p_size))
    {
        LogDebug(("Cellular_GetSocketDataSize: Invalid argument"));
        cellularStatus = CELLULAR_INVALID_HANDLE;
    }
#endif

    if (CELLULAR_SUCCESS == cellularStatus)
    {
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        semaphore_ret = aws_cellular_rts_deactive(p_context);
#endif

        (void)snprintf((char *)cmdBuf, CELLULAR_AT_CMD_MAX_SIZE, "AT+SQNSI=%ld", socketHandle->socketId + 1);  //cast
        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqQueryStartUpLink);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_GetSocketDataSize: couldn't resolve %s command", cmdBuf));
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
        aws_cellular_rts_active(p_context, semaphore_ret);
#endif
    }

    return cellularStatus;
}

static CellularPktStatus_t _Cellular_RecvFuncGetSocketDataSize(CellularContext_t                 * p_context,
                                                               const CellularATCommandResponse_t * p_atresp,
                                                               void                              * p_data,
                                                               uint16_t                            dataLen)
{
    (void)dataLen;

    uint16_t * p_ret       = p_data;
    uint8_t  * p_inputline = NULL;
    uint8_t  * p_token     = NULL;

    CellularPktStatus_t pktStatus    = CELLULAR_PKT_STATUS_OK;
    CellularATError_t   atCoreStatus = CELLULAR_AT_SUCCESS;

#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    if (NULL == p_context)
    {
        pktStatus = CELLULAR_PKT_STATUS_INVALID_HANDLE;
    }
    else if ((NULL == p_atresp) || (NULL == p_atresp->pItm) || (NULL == p_atresp->pItm->pLine))
    {
        LogError(("_Cellular_RecvFuncGetSVN: Input Line passed is NULL"));
        pktStatus = CELLULAR_PKT_STATUS_FAILURE;
    }
    else
    {
#endif
        p_inputline  = (uint8_t *)p_atresp->pItm->pLine;                //cast
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        atCoreStatus = Cellular_ATRemovePrefix((char **)&p_inputline);  /* Fixed build error on CC-RL. */
#else
        atCoreStatus = Cellular_ATRemovePrefix(&(char *)p_inputline);   //cast
#endif

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllDoubleQuote((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            atCoreStatus = Cellular_ATRemoveAllWhiteSpaces((char *)p_inputline);    //cast
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            /* Get connId */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token);  /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            /* Get sent */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token);  /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            /* Get received */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token);  /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if (CELLULAR_AT_SUCCESS == atCoreStatus)
        {
            /* Get buff_in */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
            atCoreStatus = Cellular_ATGetNextTok((char **)&p_inputline, (char **)&p_token);  /* Fixed build error on CC-RL. */
#else
            atCoreStatus = Cellular_ATGetNextTok(&(char *)p_inputline, &(char *)p_token);   //cast
#endif
        }

        if ((CELLULAR_AT_SUCCESS == atCoreStatus) && (NULL != p_token))
        {
            *p_ret = (uint16_t)atoi((char *)p_token);    //cast
        }

        if (CELLULAR_AT_SUCCESS != atCoreStatus)
        {
            pktStatus = _Cellular_TranslateAtCoreStatus(atCoreStatus);
        }
#if AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE == 1
    }
#endif

    return pktStatus;
}
