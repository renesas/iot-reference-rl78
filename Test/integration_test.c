/*
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Modifications Copyright (C) 2024 Renesas Electronics Corporation. or its affiliates.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 */

/**
 * @brief File implements the required stub code and task to run and pass the integration
 * tests.
 */


#include "test_execution_config.h"
#include "qualification_test.h"
#include "ota_pal_test.h"
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "trng_helper.h"
#include "cert_profile_helper.h"
#else
#include "core_pkcs11_config.h"
#endif
#include "test_param_config.h"
#include "qualification_test.h"
#include "transport_interface_test.h"
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "transport_plaintext.h"
#else
#include "transport_mbedtls_pkcs11.h"
#endif
#include "demo_config.h"
#include "mqtt_test.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ota_config.h"

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#if !defined(__DA16XXX_DEMO__)
extern uint8_t CellularDisableSni;
#endif
#endif

struct NetworkContext
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    PlaintextTransportParams_t * pParams;
#else
    TlsTransportParams_t * pParams;
#endif
};

typedef struct TaskParam
{
    StaticSemaphore_t pxjoinMutexBuffer;
    SemaphoreHandle_t pxjoinMutexHandle;
    FRTestThreadFunction_t xthreadFunc;
    void * pxParam;
    TaskHandle_t taskHandle;
} TaskParam_t;

/* Function declaration. */
uint32_t MqttTestGetTimeMs( void );
void prvTransportTestDelay( uint32_t delayMs );
/**
 * @brief Socket send and receive timeouts to use.  Specified in milliseconds.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 1000U )
#define mqttexampleMILLISECONDS_PER_SECOND           ( 1000U )
#define mqttexampleMILLISECONDS_PER_TICK             ( mqttexampleMILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
static NetworkCredentials_t xNetworkCredentials = { 0 };
#endif
static TransportInterface_t xTransport = { 0 };
static NetworkContext_t xSecondNetworkContext = { 0 };
static NetworkContext_t xNetworkContext = { 0 };
static uint32_t ulGlobalEntryTimeMs = 0;


static NetworkConnectStatus_t prvTransportNetworkConnect( void * pvNetworkContext,
                                                          TestHostInfo_t * pxHostInfo,
                                                          void * pvNetworkCredentials )
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    PlaintextTransportStatus_t xStatus = PLAINTEXT_TRANSPORT_SUCCESS;
    xStatus = Plaintext_FreeRTOS_Connect( pvNetworkContext,
                                          pxHostInfo->pHostName,
                                          pxHostInfo->port,
                                          mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                          mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS );

    configASSERT( PLAINTEXT_TRANSPORT_SUCCESS == xStatus );
#else
    TlsTransportStatus_t xStatus = TLS_TRANSPORT_SUCCESS;
    xStatus = TLS_FreeRTOS_Connect( pvNetworkContext,
                                    pxHostInfo->pHostName,
                                    pxHostInfo->port,
                                    pvNetworkCredentials,
                                    mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                    mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS );

    configASSERT( TLS_TRANSPORT_SUCCESS == xStatus );
#endif
    return NETWORK_CONNECT_SUCCESS;
}

static void prvTransportNetworkDisconnect( void * pNetworkContext )
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    Plaintext_FreeRTOS_Disconnect( pNetworkContext );
#else
    TLS_FreeRTOS_Disconnect( pNetworkContext );
#endif
}

static void ThreadWrapper( void * pParam )
{
    TaskParam_t * pTaskParam = pParam;

    if( ( pTaskParam != NULL ) && ( pTaskParam->xthreadFunc != NULL ) && ( pTaskParam->pxjoinMutexHandle != NULL ) )
    {
        pTaskParam->xthreadFunc( pTaskParam->pxParam );

        /* Give the mutex. */
        xSemaphoreGive( pTaskParam->pxjoinMutexHandle );
    }

    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/


int FRTest_GenerateRandInt(void)
{
    uint32_t random_number = 0;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    xTrngGenerateRandomNumber( ( uint8_t * ) &random_number, sizeof( uint32_t ) );
#else
    get_random_number( ( uint8_t * ) &random_number, sizeof( uint32_t ) );
#endif
    return random_number;
}

/*-----------------------------------------------------------*/

FRTestThreadHandle_t FRTest_ThreadCreate( FRTestThreadFunction_t threadFunc,
                                          void * pParam )
{
    TaskParam_t * pTaskParam = NULL;
    FRTestThreadHandle_t threadHandle = NULL;
    BaseType_t xReturned;

    pTaskParam = pvPortMalloc( sizeof( TaskParam_t ) );
    configASSERT( pTaskParam != NULL );

    pTaskParam->pxjoinMutexHandle = xSemaphoreCreateBinaryStatic( &pTaskParam->pxjoinMutexBuffer );
    configASSERT( pTaskParam->pxjoinMutexHandle != NULL );

    pTaskParam->xthreadFunc = threadFunc;
    pTaskParam->pxParam = pParam;

    xReturned = xTaskCreate( ThreadWrapper,    /* Task code. */
                             "ThreadWrapper",  /* All tasks have same name. */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
                             1200,             /* Task stack size. */
#else
                             8192,             /* Task stack size. */
#endif
                             pTaskParam,       /* Where the task writes its result. */
                             tskIDLE_PRIORITY, /* Task priority. */
                             &pTaskParam->taskHandle );
    configASSERT( xReturned == pdPASS );

    threadHandle = pTaskParam;

    return threadHandle;
}

/*-----------------------------------------------------------*/

int FRTest_ThreadTimedJoin( FRTestThreadHandle_t threadHandle,
                            uint32_t timeoutMs )
{
    TaskParam_t * pTaskParam = threadHandle;
    BaseType_t xReturned;
    int retValue = 0;

    /* Check the parameters. */
    configASSERT( pTaskParam != NULL );
    configASSERT( pTaskParam->pxjoinMutexHandle != NULL );

    /* Wait for the thread. */
    xReturned = xSemaphoreTake( pTaskParam->pxjoinMutexHandle, pdMS_TO_TICKS( timeoutMs ) );

    if( xReturned != pdTRUE )
    {
        LogWarn( ( "Waiting thread exist failed after %u %d. Task abort.", timeoutMs, xReturned ) );

        /* Return negative value to indicate error. */
        retValue = -1;

        /* There may be used after free. Assert here to indicate error. */
        configASSERT( 0 );
    }

    FRTest_MemoryFree( pTaskParam );

    return retValue;
}

/*-----------------------------------------------------------*/

void FRTest_TimeDelay( uint32_t delayMs )
{
    vTaskDelay( pdMS_TO_TICKS( delayMs ) );
}

/*-----------------------------------------------------------*/

void * FRTest_MemoryAlloc( size_t size )
{
    return pvPortMalloc( size );
}

/*-----------------------------------------------------------*/

void FRTest_MemoryFree( void * ptr )
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    vPortFree( ptr );
#else
    return vPortFree( ptr );
#endif
}
uint32_t FRTest_GetTimeMs(void)
{
    return MqttTestGetTimeMs();
}
/*-----------------------------------------------------------*/

uint32_t MqttTestGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * mqttexampleMILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}
/*-----------------------------------------------------------*/
#if ( MQTT_TEST_ENABLED == 1 )
void SetupMqttTestParam( MqttTestParam_t * pTestParam )
{
    configASSERT( pTestParam != NULL );
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    PlaintextTransportParams_t *xPlaintextTransportParams0 = ( PlaintextTransportParams_t * ) pvPortMalloc( sizeof( PlaintextTransportParams_t ) );
    PlaintextTransportParams_t *xPlaintextTransportParams1 = ( PlaintextTransportParams_t * ) pvPortMalloc( sizeof( PlaintextTransportParams_t ) );
    xNetworkContext.pParams = xPlaintextTransportParams0;
    xSecondNetworkContext.pParams = xPlaintextTransportParams1;
#else
    TlsTransportParams_t *xTlsTransportParams0 = ( TlsTransportParams_t * ) pvPortMalloc( sizeof( TlsTransportParams_t ) );
    TlsTransportParams_t *xTlsTransportParams1 = ( TlsTransportParams_t * ) pvPortMalloc( sizeof( TlsTransportParams_t ) );
    xNetworkContext.pParams = xTlsTransportParams0;
    xSecondNetworkContext.pParams = xTlsTransportParams1;
#endif
    /* Initialization of timestamp for MQTT. */
    ulGlobalEntryTimeMs = MqttTestGetTimeMs();

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    /* Setup the transport interface. */
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;
    xTransport.writev = NULL;
    prvWriteCertificateToModule((const uint8_t *)CFG_ROOT_CA_PEM1,
                                strlen((const char *)CFG_ROOT_CA_PEM1),
                                (const uint8_t *)MQTT_CLIENT_CERTIFICATE,
                                strlen((const char *)MQTT_CLIENT_CERTIFICATE),
                                (const uint8_t *)MQTT_CLIENT_PRIVATE_KEY,
                                strlen((const char *)MQTT_CLIENT_PRIVATE_KEY));
#if !defined(__DA16XXX_DEMO__)
    CellularDisableSni = pdFALSE;
#endif
#else
    /* Setup the transport interface. */
    xTransport.send = TLS_FreeRTOS_send;
    xTransport.recv = TLS_FreeRTOS_recv;
    xTransport.writev = NULL;

    xNetworkCredentials.pRootCa = ( unsigned char * ) democonfigROOT_CA_PEM;
    xNetworkCredentials.rootCaSize = sizeof( democonfigROOT_CA_PEM );
    xNetworkCredentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    xNetworkCredentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    xNetworkCredentials.disableSni = pdFALSE;
	xNetworkCredentials.pAlpnProtos = NULL;
#endif

    pTestParam->pTransport = &xTransport;
    pTestParam->pNetworkContext = &xNetworkContext;
    pTestParam->pSecondNetworkContext = &xSecondNetworkContext;
    pTestParam->pNetworkConnect = prvTransportNetworkConnect;
    pTestParam->pNetworkDisconnect = prvTransportNetworkDisconnect;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    pTestParam->pNetworkCredentials = &xNetworkCredentials;
#endif
    pTestParam->pGetTimeMs = MqttTestGetTimeMs;
}
#endif /* TRANSPORT_INTERFACE_TEST_ENABLED == 1 */

#if ( TRANSPORT_INTERFACE_TEST_ENABLED == 1 )
void SetupTransportTestParam( TransportTestParam_t * pTestParam )
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    PlaintextTransportParams_t *xPlaintextTransportParams0 = ( PlaintextTransportParams_t * ) pvPortMalloc( sizeof( PlaintextTransportParams_t ) );
    PlaintextTransportParams_t *xPlaintextTransportParams1 = ( PlaintextTransportParams_t * ) pvPortMalloc( sizeof( PlaintextTransportParams_t ) );
    xNetworkContext.pParams = xPlaintextTransportParams0;
    xSecondNetworkContext.pParams = xPlaintextTransportParams1;
#else
    TlsTransportParams_t *xTlsTransportParams0 = ( TlsTransportParams_t * ) pvPortMalloc( sizeof( TlsTransportParams_t ) );
    TlsTransportParams_t *xTlsTransportParams1 = ( TlsTransportParams_t * ) pvPortMalloc( sizeof( TlsTransportParams_t ) );
    xNetworkContext.pParams = xTlsTransportParams0;
    xSecondNetworkContext.pParams = xTlsTransportParams1;
#endif

    configASSERT( pTestParam != NULL );

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    /* Setup the transport interface. */
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;
    xTransport.writev = NULL;
    prvWriteCertificateToModule((const uint8_t *)ECHO_SERVER_ROOT_CA,
                                strlen((const char *)ECHO_SERVER_ROOT_CA),
                                (const uint8_t *)TRANSPORT_CLIENT_CERTIFICATE,
                                strlen((const char *)TRANSPORT_CLIENT_CERTIFICATE),
                                (const uint8_t *)TRANSPORT_CLIENT_PRIVATE_KEY,
                                strlen((const char *)TRANSPORT_CLIENT_PRIVATE_KEY));
#if !defined(__DA16XXX_DEMO__)
    CellularDisableSni = pdTRUE;
#endif
#else
    /* Setup the transport interface. */
    xTransport.send = TLS_FreeRTOS_send;
    xTransport.recv = TLS_FreeRTOS_recv;
    xTransport.writev = NULL;

    xNetworkCredentials.pRootCa = ( unsigned char * ) ECHO_SERVER_ROOT_CA;
    xNetworkCredentials.rootCaSize = sizeof( ECHO_SERVER_ROOT_CA );
    xNetworkCredentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    xNetworkCredentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    xNetworkCredentials.disableSni = pdTRUE;
//    xNetworkCredentials.pAlpnProtos = NULL;
#endif

    pTestParam->pTransport = &xTransport;
    pTestParam->pNetworkContext = &xNetworkContext;
    pTestParam->pSecondNetworkContext = &xSecondNetworkContext;
    pTestParam->pNetworkConnect = prvTransportNetworkConnect;
    pTestParam->pNetworkDisconnect = prvTransportNetworkDisconnect;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
    pTestParam->pNetworkCredentials = &xNetworkCredentials;
#endif

}
#endif

#if ( OTA_PAL_TEST_ENABLED == 1 )
void SetupOtaPalTestParam( OtaPalTestParam_t * pTestParam )
{
    pTestParam->pageSize = ( 1UL << otaconfigLOG2_FILE_BLOCK_SIZE );
}
#endif /* if ( OTA_PAL_TEST_ENABLED == 1 ) */
void prvQualificationTestTask( void * pvParameters )
{
    RunQualificationTest();
    vTaskDelete( NULL );
}
