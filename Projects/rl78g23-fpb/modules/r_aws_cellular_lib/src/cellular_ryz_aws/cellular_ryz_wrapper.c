/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_aws_cellular_if.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Cellular_Cleanup wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_Cleanup (CellularHandle_t cellularHandle)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonCleanup(cellularHandle);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RegisterUrcNetworkRegistrationEventCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RegisterUrcNetworkRegistrationEventCallback (
    CellularHandle_t                         cellularHandle,
    CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback,
    void                                   * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret =  Cellular_CommonRegisterUrcNetworkRegistrationEventCallback(cellularHandle,
                                                                      networkRegistrationCallback,
                                                                      pCallbackContext);
    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RegisterUrcPdnEventCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RegisterUrcPdnEventCallback (CellularHandle_t              cellularHandle,
                                                      CellularUrcPdnEventCallback_t pdnEventCallback,
                                                      void                        * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRegisterUrcPdnEventCallback(cellularHandle, pdnEventCallback, pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RegisterUrcGenericCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RegisterUrcGenericCallback (CellularHandle_t             cellularHandle,
                                                     CellularUrcGenericCallback_t genericCallback,
                                                     void                       * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRegisterUrcGenericCallback(cellularHandle, genericCallback, pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RegisterUrcSignalStrengthChangedCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RegisterUrcSignalStrengthChangedCallback(CellularHandle_t cellularHandle,
                                                                  CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback,
                                                                  void * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRegisterUrcSignalStrengthChangedCallback(cellularHandle, signalStrengthChangedCallback, pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RegisterModemEventCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RegisterModemEventCallback (CellularHandle_t             cellularHandle,
                                                     CellularModemEventCallback_t modemEventCallback,
                                                     void                       * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRegisterModemEventCallback(cellularHandle, modemEventCallback, pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_ATCommandRaw wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_ATCommandRaw (CellularHandle_t                            cellularHandle,
                                       const char                                * pATCommandPrefix,
                                       const char                                * pATCommandPayload,
                                       CellularATCommandType_t                     atCommandType,
                                       CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                       void                                      * pData,
                                       uint16_t                                    dataLen)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonATCommandRaw(cellularHandle,
                                       pATCommandPrefix,
                                       pATCommandPayload,
                                       atCommandType,
                                       responseReceivedCallback,
                                       pData,
                                       dataLen);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_CreateSocket wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_CreateSocket (CellularHandle_t         cellularHandle,
                                       uint8_t                  pdnContextId,
                                       CellularSocketDomain_t   socketDomain,
                                       CellularSocketType_t     socketType,
                                       CellularSocketProtocol_t socketProtocol,
                                       CellularSocketHandle_t * pSocketHandle)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonCreateSocket(cellularHandle,
                                       pdnContextId,
                                       socketDomain,
                                       socketType,
                                       socketProtocol,
                                       pSocketHandle);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SocketSetSockOpt wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_SocketSetSockOpt (CellularHandle_t            cellularHandle,
                                           CellularSocketHandle_t      socketHandle,
                                           CellularSocketOptionLevel_t optionLevel,
                                           CellularSocketOption_t      option,
                                           const uint8_t             * pOptionValue,
                                           uint32_t                    optionValueLength)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonSocketSetSockOpt(cellularHandle,
                                           socketHandle,
                                           optionLevel,
                                           option,
                                           pOptionValue,
                                           optionValueLength);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SocketRegisterDataReadyCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_SocketRegisterDataReadyCallback (CellularHandle_t                  cellularHandle,
                                                          CellularSocketHandle_t            socketHandle,
                                                          CellularSocketDataReadyCallback_t dataReadyCallback,
                                                          void                            * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonSocketRegisterDataReadyCallback(cellularHandle,
                                                          socketHandle,
                                                          dataReadyCallback,
                                                          pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SocketRegisterSocketOpenCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_SocketRegisterSocketOpenCallback (CellularHandle_t             cellularHandle,
                                                           CellularSocketHandle_t       socketHandle,
                                                           CellularSocketOpenCallback_t socketOpenCallback,
                                                           void                       * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonSocketRegisterSocketOpenCallback(cellularHandle,
                                                           socketHandle,
                                                           socketOpenCallback,
                                                           pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SocketRegisterClosedCallback wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_SocketRegisterClosedCallback (CellularHandle_t               cellularHandle,
                                                       CellularSocketHandle_t         socketHandle,
                                                       CellularSocketClosedCallback_t closedCallback,
                                                       void                         * pCallbackContext)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonSocketRegisterClosedCallback(cellularHandle, socketHandle, closedCallback, pCallbackContext);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RfOn wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RfOn (CellularHandle_t cellularHandle)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRfOn(cellularHandle);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_RfOff wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_RfOff (CellularHandle_t cellularHandle)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonRfOff(cellularHandle);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_GetIPAddress wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetIPAddress (CellularHandle_t cellularHandle,
                                       uint8_t          contextId,
                                       char           * pBuffer,
                                       uint32_t         bufferLength)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetIPAddress(cellularHandle, contextId, pBuffer, bufferLength);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_GetModemInfo wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetModemInfo (CellularHandle_t cellularHandle, CellularModemInfo_t * pModemInfo)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetModemInfo(cellularHandle, pModemInfo);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_CommonGetRegisteredNetwork wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetRegisteredNetwork (CellularHandle_t cellularHandle, CellularPlmnInfo_t * pNetworkInfo)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetRegisteredNetwork(cellularHandle, pNetworkInfo);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_GetNetworkTime wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetNetworkTime (CellularHandle_t cellularHandle, CellularTime_t * pNetworkTime)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetNetworkTime(cellularHandle, pNetworkTime);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_GetPsmSettings wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetPsmSettings (CellularHandle_t cellularHandle, CellularPsmSettings_t * pPsmSettings)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetPsmSettings(cellularHandle, pPsmSettings);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_GetEidrxSettings wrapper, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_GetEidrxSettings (CellularHandle_t              cellularHandle,
                                           CellularEidrxSettingsList_t * pEidrxSettingsList)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonGetEidrxSettings(cellularHandle, pEidrxSettingsList);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SetEidrxSettings, implemented by Cellular Common
 **********************************************************************************************************************/
CellularError_t Cellular_SetEidrxSettings(CellularHandle_t cellularHandle,
                                          const CellularEidrxSettings_t * pEidrxSettings)
{
    CellularError_t ret;
    BaseType_t      semaphore_ret = 0;

    semaphore_ret = aws_cellular_rts_deactive((CellularContext_t *)cellularHandle);

    ret = Cellular_CommonSetEidrxSettings(cellularHandle, pEidrxSettings);

    aws_cellular_rts_active((CellularContext_t *)cellularHandle, semaphore_ret);

    return ret;
}

/*******************************************************************************************************************//**
 * Cellular_SetRatPriority wrapper, currently not supported since RYZ014A only supports CAT-M1
 **********************************************************************************************************************/
CellularError_t Cellular_SetRatPriority (CellularHandle_t      cellularHandle,
                                         const CellularRat_t * pRatPriorities,
                                         uint8_t               ratPrioritiesLength)
{
    (void)(cellularHandle);
    (void)(pRatPriorities);
    (void)(ratPrioritiesLength);

    return CELLULAR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Cellular_GetRatPriority wrapper, currently not supported since RYZ014A only supports CAT-M1
 **********************************************************************************************************************/
CellularError_t Cellular_GetRatPriority (CellularHandle_t cellularHandle,
                                         CellularRat_t  * pRatPriorities,
                                         uint8_t          ratPrioritiesLength,
                                         uint8_t        * pOutputRatPrioritiesLength)
{
    (void)(cellularHandle);
    (void)(pRatPriorities);
    (void)(ratPrioritiesLength);
    (void)(pOutputRatPrioritiesLength);

    return CELLULAR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Cellular_SetDns wrapper, currently not supported since RYZ014A uses dynamic DNS addresses.
 **********************************************************************************************************************/
CellularError_t Cellular_SetDns (CellularHandle_t cellularHandle, uint8_t contextId, const char * pDnsServerAddress)
{
    (void)(cellularHandle);
    (void)(contextId);
    (void)(pDnsServerAddress);

    return CELLULAR_UNSUPPORTED;
}
