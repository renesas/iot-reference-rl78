/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Modifications Copyright (C) 2023 Renesas Electronics Corporation. or its affiliates.
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
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/**
 * @file cellular_setup.c
 * @brief Setup cellular connectivity for board with cellular module.
 */

/* FreeRTOS include. */
#include <FreeRTOS.h>
#include "task.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* The config header is always included first. */
#include "r_aws_cellular_if.h"

/* Demo Specific configs. */
#include "demo_config.h"

/* Write certificate */
#include "cert_profile_helper.h"

/*-----------------------------------------------------------*/

#define CELLULAR_SIM_CARD_WAIT_INTERVAL_MS       ( 1000UL )
#define CELLULAR_MAX_SIM_RETRY                   ( 5U )

#define CELLULAR_PDN_CONNECT_TIMEOUT             ( 100000UL )
#define CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS    ( 1000UL )

#define CELLULAR_PDN_CONTEXT_NUM                 ( CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U )

/*-----------------------------------------------------------*/

/* Secure socket needs application to provide the cellular handle and pdn context id. */
/* User of secure sockets cellular should provide this variable. */
CellularHandle_t CellularHandle = NULL;

/* User of secure sockets cellular should provide this variable. */
uint8_t CellularSocketPdnContextId = RM_CELLULAR_RYZ_DEFAULT_BEARER_CONTEXT_ID;

/*-----------------------------------------------------------*/

bool setupCellular (void);

bool setupCellular(void)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    bool cellularRet = false;
#endif
    CellularError_t cellularStatus;
    CellularSimCardStatus_t simStatus;
    CellularServiceStatus_t serviceStatus;
    uint8_t tries;

    CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4,
                                      (CellularPdnAuthType_t)AWS_CELLULAR_CFG_AUTH_TYPE,
                                      AWS_CELLULAR_CFG_AP_NAME,
                                      AWS_CELLULAR_CFG_AP_USERID,
                                      AWS_CELLULAR_CFG_AP_PASSWORD };

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    uint32_t timeoutCountLimit = ( CELLULAR_PDN_CONNECT_TIMEOUT / CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) + 1U;
    uint32_t timeoutCount = 0;
#else
    CellularPdnStatus_t PdnStatusBuffers[ CELLULAR_PDN_CONTEXT_NUM ] = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    uint32_t timeoutCountLimit = ( CELLULAR_PDN_CONNECT_TIMEOUT / CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 0;
    bool pdnStatus = false;
    uint32_t i = 0U;

    CellularPsmSettings_t psmSettings = { 0 };
#endif

    /* Initialize Cellular Common Interface. */
    cellularStatus = R_AWS_CELLULAR_Open(&CellularHandle);
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_Init failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

    /* RF off. */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    cellularStatus = Cellular_CommonRfOff(CellularHandle);
#else
    cellularStatus = Cellular_RfOff(CellularHandle);
#endif
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_RfOff failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    /* Write certificate to RYZ. */
    prvWriteCertificateToModule(CellularHandle);
#endif

    /* wait until SIM is ready */
    for (tries = 0; tries < CELLULAR_MAX_SIM_RETRY; tries++ )
    {
        cellularStatus = Cellular_GetSimCardStatus(CellularHandle, &simStatus);
        if ((CELLULAR_SUCCESS ==  cellularStatus) &&
            ((CELLULAR_SIM_CARD_INSERTED == simStatus.simCardState) && (CELLULAR_SIM_CARD_READY == simStatus.simCardLockState)))
        {
            LogInfo( ( ">>>  Cellular SIM okay  <<<\r\n" ) );
            break;
        }
        else
        {
            LogError( ( ">>>  Cellular SIM card state %d, Lock State %d <<<\r\n",
                        simStatus.simCardState,
                        simStatus.simCardLockState ) );
        }
        vTaskDelay(pdMS_TO_TICKS( CELLULAR_SIM_CARD_WAIT_INTERVAL_MS));
    }

    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular SIM failure  <<<\r\n" ) );
        return (false);
    }

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    /* Turn off PSM because this is demo to showcase MQTT instead of PSM mode. */
    psmSettings.mode = 0;
    cellularStatus =  Cellular_CommonSetPsmSettings( CellularHandle, &psmSettings );
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_SetPsmSettings failure  <<<\r\n" ) );
        return (false);
    }
#endif

    /* Setup the PDN config. */
    cellularStatus = Cellular_SetPdnConfig(CellularHandle, CellularSocketPdnContextId, &pdnConfig);
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_SetPdnConfig failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

    /* RF On */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    cellularStatus = Cellular_CommonRfOn(CellularHandle);
#else
    cellularStatus = Cellular_RfOn(CellularHandle);
#endif
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_RfOn failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

    /* Get service status. */
    while (timeoutCount < timeoutCountLimit)
    {
        cellularStatus = Cellular_GetServiceStatus(CellularHandle, &serviceStatus);
        if ((CELLULAR_SUCCESS == cellularStatus) &&
           ((REGISTRATION_STATUS_REGISTERED_HOME == serviceStatus.psRegistrationStatus) ||
            (REGISTRATION_STATUS_ROAMING_REGISTERED == serviceStatus.psRegistrationStatus)) )
        {
            LogInfo( ( ">>>  Cellular module registered  <<<\r\n" ) );
            break;
        }

        LogInfo( ( ">>>  Cellular_GetServiceStatus %d, PS registration status %d  <<<\r\n",
                   cellularStatus, serviceStatus.psRegistrationStatus ) );

        timeoutCount++;

        if (timeoutCount >= timeoutCountLimit)
        {
            /* Return timeout to indicate network is not registered within CELLULAR_PDN_CONNECT_TIMEOUT. */
            LogError( ( ">>>  Cellular module can't be registered within %d <<<\r\n",
                        CELLULAR_PDN_CONNECT_TIMEOUT) );
            return (false);
        }
        vTaskDelay(pdMS_TO_TICKS(CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS));
    }

    cellularStatus = Cellular_ActivatePdn( CellularHandle, CellularSocketPdnContextId );
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_ActivatePdn failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return (true);
#else
    cellularStatus = Cellular_GetIPAddress(CellularHandle, CellularSocketPdnContextId, localIP, sizeof(localIP));
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_GetIPAddress failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

    cellularStatus = Cellular_GetPdnStatus(CellularHandle, PdnStatusBuffers, CELLULAR_PDN_CONTEXT_NUM, &NumStatus);
    if (CELLULAR_SUCCESS != cellularStatus)
    {
        LogError( ( ">>>  Cellular_GetPdnStatus failure %d  <<<\r\n", cellularStatus ) );
        return (false);
    }

    for (i = 0U; i < NumStatus; i++ )
    {
        if ((PdnStatusBuffers[ i ].contextId == CellularSocketPdnContextId) && (PdnStatusBuffers[ i ].state == 1))
        {
            pdnStatus = true;
            break;
        }
    }

    if (false == pdnStatus)
    {
        LogError( ( ">>>  Cellular PDN is not activated <<<\r\n" ) );
    }

    if ((CELLULAR_SUCCESS == cellularStatus) && (true == pdnStatus))
    {
        LogInfo( ( ">>>  Cellular module registered, IP address %s  <<<\r\n", localIP ) );
        cellularRet = true;
    }
    else
    {
        cellularRet = false;
    }

    return (cellularRet);
#endif
}

/*-----------------------------------------------------------*/

#if AWS_CELLULAR_CFG_URC_CHARGET_ENABLED == 1
void AWS_CELLULAR_CFG_URC_CHARGET_FUNCTION( const char * pRawData,
                                            void       * pCallbackContext )
{
    printf("urc = %s\n", pRawData);

    return;
}
#endif

