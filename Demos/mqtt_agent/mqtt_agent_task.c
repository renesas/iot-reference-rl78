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

/*
 * This demo creates multiple tasks, all of which use the MQTT agent API to
 * communicate with an MQTT broker through the same MQTT connection.
 *
 * This file contains the initial task created after the TCP/IP stack connects
 * to the network.  The task:
 *
 * 1) Connects to the MQTT broker.
 * 2) Creates the other demo tasks, in accordance with the #defines set in
 *    demo_config.h.  For example, if demo_config.h contains the following
 *    settings:
 *
 *    #define democonfigCREATE_LARGE_MESSAGE_SUB_PUB_TASK     1
 *    #define democonfigNUM_SIMPLE_SUB_PUB_TASKS_TO_CREATE 3
 *
 *    then the initial task will create the task implemented in
 *    large_message_sub_pub_demo.c and three instances of the task
 *    implemented in simple_sub_pub_demo.c.  See the comments at the top
 *    of those files for more information.
 *
 * 3) After creating the demo tasks the initial task could create the MQTT
 *    agent task.  However, as it has no other operations to perform, rather
 *    than create the MQTT agent as a separate task the initial task just calls
 *    the agent's implementing function - effectively turning itself into the
 *    MQTT agent.
 */

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#pragma section const const_demos
#endif

/* Standard includes. */
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/* Demo Specific configs. */
#include "demo_config.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Transport interface implementation include header for TLS. */
#include "aws_clientcredential.h"
#include "iot_default_root_certificates.h"

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "trng_helper.h"
#else
#include "core_pkcs11_config.h"
#endif

/* MQTT library includes. */
#include "core_mqtt.h"

/* MQTT agent include. */
#include "core_mqtt_agent.h"

/* MQTT Agent ports. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

/* Exponential backoff retry include. */
#include "backoff_algorithm.h"

/* Transport interface header file. */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "transport_plaintext.h"
#else
#include "transport_mbedtls_pkcs11.h"
#endif

/* Includes MQTT Agent Task management APIs. */
#include "mqtt_agent_task.h"
#include "backoff_algorithm.h"

/* For IDT test. */
#if (ENABLE_AFR_IDT == 1)
#include "test_execution_config.h"
#endif

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#include "store.h"
#endif

#ifndef democonfigMQTT_BROKER_ENDPOINT
    #define democonfigMQTT_BROKER_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

/**
 * @brief The root CA certificate belonging to the broker.
 */
#ifndef democonfigROOT_CA_PEM
    #define democonfigROOT_CA_PEM    tlsATS1_ROOT_CERTIFICATE_PEM
#endif

#ifndef democonfigCLIENT_IDENTIFIER

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 */
    #define democonfigCLIENT_IDENTIFIER    clientcredentialIOT_THING_NAME
#endif

#ifndef democonfigMQTT_BROKER_PORT

/**
 * @brief The port to use for the demo.
 */
    #define democonfigMQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

/**
 * @brief The maximum number of times to run the subscribe publish loop in this
 * demo.
 */
#ifndef democonfigMQTT_MAX_DEMO_COUNT
    #define democonfigMQTT_MAX_DEMO_COUNT    (3)
#endif

/**
 * @brief Dimensions the buffer used to serialize and deserialize MQTT packets.
 * @note Specified in bytes.  Must be large enough to hold the maximum
 * anticipated MQTT payload.
 */
#ifndef MQTT_AGENT_NETWORK_BUFFER_SIZE
#define MQTT_AGENT_NETWORK_BUFFER_SIZE    (5000)
#endif

/**
 * @brief Maximum number of subscriptions maintained by the MQTT agent in the subscription store.
 */
#ifndef MQTT_AGENT_MAX_SUBSCRIPTIONS
#define MQTT_AGENT_MAX_SUBSCRIPTIONS    10U
#endif

/**
 * @brief Timeout for receiving CONNACK after sending an MQTT CONNECT packet.
 * Defined in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS           ( 2000U )

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define RETRY_MAX_ATTEMPTS                           ( BACKOFF_ALGORITHM_RETRY_FOREVER )

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed operation
 *  with server.
 */
#define RETRY_MAX_BACKOFF_DELAY_MS                   ( 5000U )

/**
 * @brief The base back-off delay (in milliseconds) to use for network operation retry
 * attempts.
 */
#define RETRY_BACKOFF_BASE_MS                        ( 500U )

/**
 * @brief The maximum time interval in seconds which is allowed to elapse
 *  between two Control Packets.
 *
 *  It is the responsibility of the Client to ensure that the interval between
 *  Control Packets being sent does not exceed the this Keep Alive value. In the
 *  absence of sending any other Control Packets, the Client MUST send a
 *  PINGREQ Packet.
 *//*_RB_ Move to be the responsibility of the agent. */
#define mqttexampleKEEP_ALIVE_INTERVAL_SECONDS       ( 60U )

/**
 * @brief Socket send and receive timeouts to use.  Specified in milliseconds.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS    ( 450 )

/**
 * @brief Configuration is used to turn on or off persistent sessions with MQTT broker.
 * If the flag is set to true, MQTT broker will remember the previous session so that a re
 * subscription to the topics are not required. Also any incoming publishes to subscriptions
 * will be stored by the broker and resend to device, when it comes back online.
 *
 */
#define mqttexamplePERSISTENT_SESSION_REQUIRED       ( 0 )

/**
 * @brief Used to convert times to/from ticks and milliseconds.
 */
#define mqttexampleMILLISECONDS_PER_SECOND           ( 1000U )
#define mqttexampleMILLISECONDS_PER_TICK             ( mqttexampleMILLISECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief The MQTT agent manages the MQTT contexts.  This set the handle to the
 * context used by this demo.
 */
#define mqttexampleMQTT_CONTEXT_HANDLE               ( ( MQTTContextHandle_t ) 0 ) ''

/**
 * @brief Event Bit corresponding to an MQTT agent state.
 * The event bit is used to set the state bit in event group so that application tasks can
 * wait on a state transition.
 */
#define mqttexampleEVENT_BIT( xState )    ( ( EventBits_t ) ( 1UL << xState ) )

/**
 * @brief Mask to clear all set event bits for the MQTT agent state event group.
 * State event group is always cleared before setting the next state event bit so that only
 * state is set at anytime.
 */
#define mqttexampleEVENT_BITS_ALL    ( ( EventBits_t ) ( ( 1ULL << MQTT_AGENT_NUM_STATES ) - 1U ) )

/**
 * @brief ThingName which is used as the client identifier for MQTT connection.
 * Thing name is retrieved  at runtime from a key value store.
 */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static char __far * pcThingName = NULL;
#else
static char * pcThingName = NULL;
#endif

/**
 * @brief Broker endpoint name for the MQTT connection.
 * Broker endpoint name is retrieved at runtime from a key value store.
 */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static char __far * pcBrokerEndpoint = NULL;
#else
static char * pcBrokerEndpoint = NULL;
#endif

/**
 * @brief Root CA
 */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static char __far * pcRootCA = NULL;
#else
static char * pcRootCA = NULL;
#endif
/*-----------------------------------------------------------*/

/**
 * @brief An element in the list of topic filter subscriptions.
 */
typedef struct TopicFilterSubscription
{
    IncomingPubCallback_t pxIncomingPublishCallback;
    void *                pvIncomingPublishCallbackContext;
    uint16_t              usTopicFilterLength;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    const char __far *    pcTopicFilter;
#else
    const char * pcTopicFilter;
#endif
    BaseType_t            xManageResubscription;
} TopicFilterSubscription_t;

/*-----------------------------------------------------------*/

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static PlaintextTransportParams_t xPlaintextTransportParams;
#else
static TlsTransportParams_t xTlsTransportParams;
#endif

/**
 * @brief Initializes an MQTT context, including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMQTTInit (void);

/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param[in] xIsReconnect Boolean flag to indicate if this is a reconnection.
 * @return `MQTTSuccess` if connection succeeds, else appropriate error code
 * from MQTT_Connect.
 */
static MQTTStatus_t prvCreateMQTTConnection (bool xIsReconnect);

/**
 * @brief Connect a TCP socket to the MQTT broker.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if connection succeeds, else `pdFAIL`.
 */
static BaseType_t prvCreateTLSConnection (NetworkContext_t * pxNetworkContext);

/**
 * @brief Disconnect a TCP connection.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if disconnect succeeds, else `pdFAIL`.
 */
static BaseType_t prvDisconnectTLS (NetworkContext_t * pxNetworkContext);

/**
 * @brief Function to attempt to resubscribe to the topics already present in the
 * subscription list.
 *
 * This function will be invoked when this demo requests the broker to
 * reestablish the session and the broker cannot do so. This function will
 * enqueue commands to the MQTT Agent queue and will be processed once the
 * command loop starts.
 *
 * @return `MQTTSuccess` if adding subscribes to the command queue succeeds, else
 * appropriate error code from MQTTAgent_Subscribe.
 */
static MQTTStatus_t prvHandleResubscribe (void);

/**
 * @brief The callback invoked by MQTT agent for a response to SUBSCRIBE request.
 * Parameter indicates whether the request was successful or not. If subscribe was not successful
 * then callback removes the topic from the subscription store and displays a warning log.
 *
 *
 * @param pxCommandContext Pointer to the command context passed from caller
 * @param pxReturnInfo Return Info containing the result of the subscribe command.
 */
static void prvSubscriptionCommandCallback (MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo);

/**
 * @brief Fan out the incoming publishes to the callbacks registered by different
 * tasks. If there are no callbacks registered for the incoming publish, it will be
 * passed to the unsolicited publish handler.
 *
 * @param[in] pMqttAgentContext Agent context.
 * @param[in] packetId Packet ID of publish.
 * @param[in] pxPublishInfo Info of incoming publish.
 */
static void prvIncomingPublishCallback (MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo);

/**
 * @fn prvMatchTopicFilterSubscriptions
 *
 * @brief Checks if the topic in the given publish information
 * matches any of the existing MQTT topic filter subscriptions.
 *
 * @param[in] pxPublishInfo Pointer to the MQTT publish information containing the topic to be matched.
 *
 * @return true if a matching subscription is found; false otherwise.
 */
static bool prvMatchTopicFilterSubscriptions (MQTTPublishInfo_t * pxPublishInfo);

/**
 * @fn prvSetMQTTAgentState
 *
 * @brief Set MQTT Agent State.
 */
static void prvSetMQTTAgentState (MQTTAgentState_t xAgentState);

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs (void);

/**
 * @brief Connects a TCP socket to the MQTT broker, then creates and MQTT
 * connection to the same.
 * @param[in] xIsReconnect Boolean flag to indicate if its a reconnection.
 * @return MQTTConnected if connection was successful, MQTTNotConnected if MQTT connection
 *         failed and all retries exhausted.
 */
static MQTTConnectionStatus_t prvConnectToMQTTBroker (bool xIsReconnect);

/**
 * @brief Get the string value for a key from the KV store.
 * Memory allocated for the string should be freed by calling vPortFree.
 *
 * @return NULL if value not found, pointer to the NULL terminated string value
 *         if found.
 */
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
static char * prvKVStoreGetString( KVStoreKey_t xKey );
#endif

static void prvMQTTAgentTask (void * pvParameters);

/*-----------------------------------------------------------*/

/**
 * @brief The network context used by the MQTT library transport interface.
 * See https://www.freertos.org/network-interface.html
 */
struct NetworkContext
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    PlaintextTransportParams_t * pParams;
#else
    TlsTransportParams_t * pParams;
#endif
};

static NetworkContext_t xNetworkContext;

/**
 * @brief Global entry time into the application to use as a reference timestamp
 * in the #prvGetTimeMs function. #prvGetTimeMs will always return the difference
 * between the current time and the global entry time. This will reduce the chances
 * of overflow for the 32 bit unsigned integer used for holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

MQTTAgentContext_t xGlobalMqttAgentContext;

static uint8_t xNetworkBuffer[MQTT_AGENT_NETWORK_BUFFER_SIZE];

static MQTTAgentMessageContext_t xCommandQueue;

static TopicFilterSubscription_t xTopicFilterSubscriptions[MQTT_AGENT_MAX_SUBSCRIPTIONS];

static SemaphoreHandle_t xSubscriptionsMutex;

/**
 * @brief Holds the current state of the MQTT agent.
 */
static MQTTAgentState_t xState = MQTT_AGENT_STATE_NONE;


/**
 * @brief Event group used by other tasks to synchronize with the MQTT agent states.
 */
static EventGroupHandle_t xStateEventGrp;

/*-----------------------------------------------------------*/

/**
 * @fn prvMQTTInit
 *
 * @brief Initializes an MQTT context, including transport interface and
 * network buffer.
 *
 * @return `MQTTSuccess` if the initialization succeeds, else `MQTTBadParameter`.
 */
static MQTTStatus_t prvMQTTInit(void)
{
    TransportInterface_t        xTransport;
    MQTTStatus_t                xReturn;
    MQTTFixedBuffer_t           xFixedBuffer = { .pBuffer = xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE };
    static uint8_t              staticQueueStorageArea[MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof(MQTTAgentCommand_t *)];
    static StaticQueue_t        staticQueueStructure;
    MQTTAgentMessageInterface_t messageInterface =
    {
        .pMsgCtx        = NULL,
        .send           = Agent_MessageSend,
        .recv           = Agent_MessageReceive,
        .getCommand     = Agent_GetCommand,
        .releaseCommand = Agent_ReleaseCommand
    };

    LogDebug( ( "Creating command queue." ) );
    xCommandQueue.queue = xQueueCreateStatic(MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                            sizeof( MQTTAgentCommand_t * ),
                                            staticQueueStorageArea,
                                            &staticQueueStructure );
    configASSERT(xCommandQueue.queue);
    messageInterface.pMsgCtx = &xCommandQueue;

    /* Initialize the command pool. */
    Agent_InitializePool();

    /* Fill in Transport Interface send and receive function pointers. */
    xTransport.pNetworkContext = &xNetworkContext;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    xTransport.send = Plaintext_FreeRTOS_send;
    xTransport.recv = Plaintext_FreeRTOS_recv;
#else
    xTransport.send = TLS_FreeRTOS_send;
    xTransport.recv = TLS_FreeRTOS_recv;
#endif
    xTransport.writev = NULL;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init(&xGlobalMqttAgentContext,
                            &messageInterface,
                            &xFixedBuffer,
                            &xTransport,
                            prvGetTimeMs,
                            prvIncomingPublishCallback,
                            NULL ); /* Context to pass into the callback. Passing the pointer to subscription array. */

    return xReturn;
}

/*-----------------------------------------------------------*/

/**
 * @fn prvCreateMQTTConnection
 *
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param[in] xIsReconnect Boolean flag to indicate if this is a reconnection.
 * @return `MQTTSuccess` if connection succeeds, else appropriate error code
 * from MQTT_Connect.
 */
static MQTTStatus_t prvCreateMQTTConnection(bool xIsReconnect)
{
    MQTTStatus_t      xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool              xSessionPresent = false;

    /* Many fields are not used in this demo so start with everything at 0. */
    memset(&xConnectInfo, 0x00, sizeof(xConnectInfo));

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean session
     * will ensure that the broker does not store any data when this client
     * gets disconnected. */
#if ( mqttexamplePERSISTENT_SESSION_REQUIRED == 1 )
    {
        xConnectInfo.cleanSession = false;
    }
#else
    {
        xConnectInfo.cleanSession = true;
    }
#endif

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier      = pcThingName;
    xConnectInfo.clientIdentifierLength = (uint16_t)strlen(pcThingName); /* Cast to proper datatype to avoid warning */

    /* Set MQTT keep-alive period. It is the responsibility of the application
     * to ensure that the interval between Control Packets being sent does not
     * exceed the Keep Alive value. In the absence of sending any other Control
     * Packets, the Client MUST send a PINGREQ Packet.  This responsibility will
     * be moved inside the agent. */
    xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_INTERVAL_SECONDS;

    /* Append metrics when connecting to the AWS IoT Core broker. */
#ifdef democonfigUSE_AWS_IOT_CORE_BROKER
#ifdef democonfigCLIENT_USERNAME
    xConnectInfo.pUserName = CLIENT_USERNAME_WITH_METRICS;
    xConnectInfo.userNameLength = ( uint16_t ) strlen( CLIENT_USERNAME_WITH_METRICS );
    xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
    xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
#else
    xConnectInfo.pUserName      = AWS_IOT_METRICS_STRING;
    xConnectInfo.userNameLength = AWS_IOT_METRICS_STRING_LENGTH;

    /* Password for authentication is not used. */
    xConnectInfo.pPassword      = NULL;
    xConnectInfo.passwordLength = 0U;
#endif
#else /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */
#ifdef democonfigCLIENT_USERNAME
    xConnectInfo.pUserName = democonfigCLIENT_USERNAME;
    xConnectInfo.userNameLength = ( uint16_t ) strlen( democonfigCLIENT_USERNAME );
    xConnectInfo.pPassword = democonfigCLIENT_PASSWORD;
    xConnectInfo.passwordLength = ( uint16_t ) strlen( democonfigCLIENT_PASSWORD );
#endif /* ifdef democonfigCLIENT_USERNAME */
#endif /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */

    LogInfo(("Creating an MQTT connection to the broker."));

    /* Send MQTT CONNECT packet to broker. MQTT's Last Will and Testament feature
     * is not used in this demo, so it is passed as NULL. */
    xResult = MQTT_Connect( &( xGlobalMqttAgentContext.mqttContext ),
                            &xConnectInfo,
                            NULL,
                            mqttexampleCONNACK_RECV_TIMEOUT_MS,
                            &xSessionPresent );

    if ((MQTTSuccess == xResult) && (true == xIsReconnect))
    {
        LogInfo(("Resuming previous MQTT session with broker."));
        xResult = MQTTAgent_ResumeSession(&xGlobalMqttAgentContext, xSessionPresent);

        if ((MQTTSuccess == xResult) && (false == xSessionPresent))
        {
            /* Resubscribe to all the subscribed topics. */
            xResult = prvHandleResubscribe();
        }
    }

    return xResult;
}/* End of function prvCreateMQTTConnection()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvCreateTLSConnection
 *
 * @brief Connect a TCP socket to the MQTT broker.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if connection succeeds, else `pdFAIL`.
 */
static BaseType_t prvCreateTLSConnection(NetworkContext_t * pxNetworkContext)
{
    BaseType_t xConnected = pdFAIL;

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) /* @suppress("3.1c Compiler specific functions") */
    PlaintextTransportStatus_t xNetworkStatus = PLAINTEXT_TRANSPORT_CONNECT_FAILURE;
#else
    TlsTransportStatus_t xNetworkStatus = TLS_TRANSPORT_CONNECT_FAILURE;
    NetworkCredentials_t xNetworkCredentials = { 0 };
#endif
    BackoffAlgorithmStatus_t  xBackoffAlgStatus  = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams   = { 0 };
    uint16_t                  usNextRetryBackOff = 0U;

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__) /* @suppress("3.1c Compiler specific functions") */
#ifdef democonfigUSE_AWS_IOT_CORE_BROKER

    /* ALPN protocols must be a NULL-terminated list of strings. Therefore,
     * the first entry will contain the actual ALPN protocol string while the
     * second entry must remain NULL. */
    const char * pcAlpnProtocols[] = { NULL, NULL };

    /* The ALPN string changes depending on whether username/password authentication is used. */
#ifdef democonfigCLIENT_USERNAME
    pcAlpnProtocols[ 0 ] = AWS_IOT_CUSTOM_AUTH_ALPN;
#else
    pcAlpnProtocols[ 0 ] = AWS_IOT_MQTT_ALPN;
#endif
    xNetworkCredentials.pAlpnProtos = pcAlpnProtocols;
#endif /* ifdef democonfigUSE_AWS_IOT_CORE_BROKER */

    /* Set the credentials for establishing a TLS connection. */
    xNetworkCredentials.pRootCa = ( const unsigned char * ) pcRootCA;
    xNetworkCredentials.rootCaSize = strlen( pcRootCA ) + 1;
    xNetworkCredentials.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    xNetworkCredentials.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;


    xNetworkCredentials.disableSni = democonfigDISABLE_SNI;
#endif
    BackoffAlgorithm_InitializeParams( &xReconnectParams,
                                        RETRY_BACKOFF_BASE_MS,
                                        RETRY_MAX_BACKOFF_DELAY_MS,
                                        RETRY_MAX_ATTEMPTS );

    /* Establish a TCP connection with the MQTT broker. This example connects to
     * the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
     * democonfigMQTT_BROKER_PORT at the top of this file. */
    uint32_t ulRandomNum = 0;
    do // @suppress("3.1c Compiler specific functions")
    {
        LogInfo( ( "Creating a TLS connection to %s:%u.",
                pcBrokerEndpoint,
                democonfigMQTT_BROKER_PORT ) );
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) /* @suppress("3.1c Compiler specific functions") */
        xNetworkStatus = Plaintext_FreeRTOS_Connect( pxNetworkContext,
                                                pcBrokerEndpoint,
                                                democonfigMQTT_BROKER_PORT,
                                                mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                                mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS );

        xConnected = (PLAINTEXT_TRANSPORT_SUCCESS == xNetworkStatus) ? pdPASS : pdFAIL;
#else
    xNetworkStatus = TLS_FreeRTOS_Connect( pxNetworkContext,
                                        pcBrokerEndpoint,
                                        democonfigMQTT_BROKER_PORT,
                                        &xNetworkCredentials,
                                        mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
                                        mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS );

    xConnected = ( xNetworkStatus == TLS_TRANSPORT_SUCCESS ) ? pdPASS : pdFAIL;
#endif

        if (!xConnected)    /* @suppress("3.1c Compiler specific functions") */
        {
            /* Get back-off value (in milliseconds) for the next connection retry. */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) /* @suppress("3.1c Compiler specific functions") */
            /* Cast to proper datatype to avoid warning */
            if (xTrngGenerateRandomNumber((uint8_t *)&ulRandomNum, sizeof(ulRandomNum)) == pdPASS)
#else
            if( xPkcs11GenerateRandomNumber( ( uint8_t * ) &ulRandomNum,
                                            sizeof( ulRandomNum ) ) == pdPASS )
#endif
            {
                xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff(&xReconnectParams,
                        ulRandomNum,
                        &usNextRetryBackOff);
            }

            if (BackoffAlgorithmSuccess == xBackoffAlgStatus)
            {
                LogWarn(("Connection to the broker failed. "
                        "Retrying connection in %hu ms.",
                        usNextRetryBackOff));
                vTaskDelay(pdMS_TO_TICKS(usNextRetryBackOff));
            }
        }

        if (BackoffAlgorithmRetriesExhausted == xBackoffAlgStatus)
        {
            LogError(("Connection to the broker failed, all attempts exhausted."));
        }
    } while ((pdPASS != xConnected) && (BackoffAlgorithmSuccess == xBackoffAlgStatus));

    return xConnected;
}/* End of function prvCreateTLSConnection()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvDisconnectTLS
 *
 * @brief Disconnect a TCP connection.
 *
 * @param[in] pxNetworkContext Network context.
 *
 * @return `pdPASS` if disconnect succeeds, else `pdFAIL`.
 */
static BaseType_t prvDisconnectTLS(NetworkContext_t * pxNetworkContext)
{
    LogInfo(("Disconnecting TLS connection.\n"));
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    Plaintext_FreeRTOS_Disconnect(pxNetworkContext);
#else
    TLS_FreeRTOS_Disconnect( pxNetworkContext );
#endif
    return pdPASS;
}/* End of function prvDisconnectTLS()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvHandleResubscribe
 *
 * @brief Function to attempt to resubscribe to the topics already present in the
 * subscription list.
 *
 * This function will be invoked when this demo requests the broker to
 * reestablish the session and the broker cannot do so. This function will
 * enqueue commands to the MQTT Agent queue and will be processed once the
 * command loop starts.
 *
 * @return `MQTTSuccess` if adding subscribes to the command queue succeeds, else
 * appropriate error code from MQTTAgent_Subscribe.
 */
static MQTTStatus_t prvHandleResubscribe(void)
{
    MQTTStatus_t xResult            = MQTTBadParameter;
    uint32_t     ulIndex            = 0U;
    uint16_t     usNumSubscriptions = 0U;

    /* These variables need to stay in scope until command completes. */
    static MQTTAgentSubscribeArgs_t xSubArgs                               = { 0 };
    static MQTTSubscribeInfo_t      xSubInfo[MQTT_AGENT_MAX_SUBSCRIPTIONS] = { MQTTQoS0, NULL, 0 };
    static MQTTAgentCommandInfo_t   xCommandParams                         = { 0 };

    /* Loop through each subscription in the subscription list and add a subscribe
     * command to the command queue. */
    xSemaphoreTake(xSubscriptionsMutex, portMAX_DELAY);
    {
        for (ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++)
        {
            /* Check if there is a subscription in the subscription list. This demo
             * doesn't check for duplicate subscriptions. */
            if ((xTopicFilterSubscriptions[ulIndex].usTopicFilterLength > 0) &&
                (pdTRUE == xTopicFilterSubscriptions[ulIndex].xManageResubscription))
            {
                xSubInfo[usNumSubscriptions].pTopicFilter      = xTopicFilterSubscriptions[ulIndex].pcTopicFilter;
                xSubInfo[usNumSubscriptions].topicFilterLength = xTopicFilterSubscriptions[ulIndex].usTopicFilterLength;

                /* QoS1 is used for all the subscriptions in this demo. */
                xSubInfo[usNumSubscriptions].qos = MQTTQoS1;

                LogInfo(("Resubscribe to the topic %.*s will be attempted.",
                        xSubInfo[usNumSubscriptions].topicFilterLength,
                        xSubInfo[usNumSubscriptions].pTopicFilter));

                usNumSubscriptions++;
            }
        }
    }
    xSemaphoreGive(xSubscriptionsMutex);

    if (usNumSubscriptions > 0U)
    {
        xSubArgs.pSubscribeInfo   = xSubInfo;
        xSubArgs.numSubscriptions = usNumSubscriptions;

        /* The block time can be 0 as the command loop is not running at this point. */
        xCommandParams.blockTimeMs                 = 0U;
        xCommandParams.cmdCompleteCallback         = prvSubscriptionCommandCallback;
        xCommandParams.pCmdCompleteCallbackContext = (void *) &xSubArgs;  /* Cast to proper datatype to avoid warning */

        /* Enqueue subscribe to the command queue. These commands will be processed only
         * when command loop starts. */
        xResult = MQTTAgent_Subscribe(&xGlobalMqttAgentContext, &xSubArgs, &xCommandParams);
    }
    else
    {
        /* Mark the resubscribe as success if there is nothing to be subscribed. */
        xResult = MQTTSuccess;
    }

    if (MQTTSuccess != xResult)
    {
        LogError( ( "Failed to enqueue the MQTT subscribe command. xResult=%s.",
                    MQTT_Status_strerror( xResult ) ) );
    }

    return xResult;
}/* End of function prvHandleResubscribe()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvSubscriptionCommandCallback
 *
 * @brief The callback invoked by MQTT agent for a response to SUBSCRIBE request.
 * Parameter indicates whether the request was successful or not. If subscribe was not successful
 * then callback removes the topic from the subscription store and displays a warning log.
 *
 *
 * @param pxCommandContext Pointer to the command context passed from caller
 * @param pxReturnInfo Return Info containing the result of the subscribe command.
 */
static void prvSubscriptionCommandCallback(MQTTAgentCommandContext_t * pxCommandContext,
                                            MQTTAgentReturnInfo_t * pxReturnInfo)
{
    uint32_t                   ulIndex         = 0;
    MQTTAgentSubscribeArgs_t * pxSubscribeArgs = (MQTTAgentSubscribeArgs_t *) pxCommandContext;

    /* If the return code is success, no further action is required as all the topic filters
     * are already part of the subscription list. */
    if (MQTTSuccess != pxReturnInfo->returnCode)
    {
        /* Check through each of the suback codes and determine if there are any failures. */
        for (ulIndex = 0; ulIndex < pxSubscribeArgs->numSubscriptions; ulIndex++)
        {
            /* This demo doesn't attempt to resubscribe in the event that a SUBACK failed. */
            if (MQTTSubAckFailure == pxReturnInfo->pSubackCodes[ulIndex])
            {
                LogError( ( "Failed to resubscribe to topic %.*s.",
                            pxSubscribeArgs->pSubscribeInfo[ ulIndex ].topicFilterLength,
                            pxSubscribeArgs->pSubscribeInfo[ ulIndex ].pTopicFilter ) );
            }
        }

        /* Hit an assert as some of the tasks won't be able to proceed correctly without
         * the subscriptions. This logic will be updated with exponential backoff and retry.  */
        configASSERT(pdTRUE);
    }
}/* End of function prvSubscriptionCommandCallback()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvIncomingPublishCallback
 *
 * @brief Fan out the incoming publishes to the callbacks registered by different
 * tasks. If there are no callbacks registered for the incoming publish, it will be
 * passed to the unsolicited publish handler.
 *
 * @param[in] pMqttAgentContext Agent context.
 * @param[in] packetId Packet ID of publish.
 * @param[in] pxPublishInfo Info of incoming publish.
 */
static void prvIncomingPublishCallback(MQTTAgentContext_t * pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t * pxPublishInfo)
{
    bool xPublishHandled = false;
    char cOriginalChar;
    char * pcLocation;

    (void) packetId;

    /* Fan out the incoming publishes to the callbacks registered using
     * subscription manager. */
    xPublishHandled = prvMatchTopicFilterSubscriptions(pxPublishInfo);

    /* If there are no callbacks to handle the incoming publishes,
     * handle it as an unsolicited publish. */
    if (true != xPublishHandled)
    {
        /* Ensure the topic string is terminated for printing.  This will over-
         * write the message ID, which is restored afterwards. */
        pcLocation    = (char *) &(pxPublishInfo->pTopicName[pxPublishInfo->topicNameLength]);
        cOriginalChar = *pcLocation;
        *pcLocation   = 0x00;
        LogWarn(("WARN:  Received an unsolicited publish from topic %s", pxPublishInfo->pTopicName));
        *pcLocation = cOriginalChar;
    }
}/* End of function prvIncomingPublishCallback()*/

/*-----------------------------------------------------------*/

/**
 * @fn vStartMQTTAgent
 *
 * @brief Starts the MQTT agent task.
 * MQTT agent task calls MQTTAgent_CommandLoop(), until MQTTAgent_Terminate()
 * is called. If an error occurs in the command loop, then it will reconnect the
 * TCP and MQTT connections.
 *
 * @param[in] uxStackSize Stack size for MQTT agent task.
 * @param[in] uxPriority Priority of MQTT agent task.
 */
void vStartMQTTAgent(configSTACK_DEPTH_TYPE uxStackSize,
                    UBaseType_t uxPriority)
{
    xTaskCreate(prvMQTTAgentTask,
                "MQTT",
                uxStackSize,
                NULL,
                uxPriority,
                NULL);
}/* End of function vStartMQTTAgent()*/

/**
 * @fn prvMQTTAgentTask
 *
 * @brief Get the string value for a key from the KV store.
 * Memory allocated for the string should be freed by calling vPortFree.
 *
 * @return NULL if value not found, pointer to the NULL terminated string value
 *         if found.
 */
void prvMQTTAgentTask(void * pvParameters)
{
    BaseType_t      xStatus      = pdPASS;
    MQTTStatus_t    xMQTTStatus  = MQTTBadParameter;
    MQTTContext_t * pMqttContext = &(xGlobalMqttAgentContext.mqttContext);
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    extern KeyValueStore_t gKeyValueStore ;
#endif
    (void) pvParameters;

    (void)xWaitForMQTTAgentState(MQTT_AGENT_STATE_INITIALIZED, portMAX_DELAY);
    LogInfo(("---------Start MQTT Agent Task---------\r\n"));

    /* Initialization of timestamp for MQTT. */
    ulGlobalEntryTimeMs = prvGetTimeMs();

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    pcThingName      = clientcredentialIOT_THING_NAME;
    pcBrokerEndpoint = clientcredentialMQTT_BROKER_ENDPOINT;
    pcRootCA         = (char __far *)democonfigROOT_CA_PEM; /* Cast to proper datatype to avoid warning */
#else   /* defined__CCRL__ || defined__ICCRL78__ || defined__RL */
#if defined(__TEST__)
    pcThingName = clientcredentialIOT_THING_NAME ;
    pcBrokerEndpoint = clientcredentialMQTT_BROKER_ENDPOINT;
    pcRootCA = democonfigROOT_CA_PEM;
#else
    /* Load broker endpoint and thing name for client connection, from the key store. */
    if (gKeyValueStore.table[ KVS_CORE_THING_NAME ].valueLength > 0)
    {
        pcThingName = GetStringValue(KVS_CORE_THING_NAME, gKeyValueStore.table[ KVS_CORE_THING_NAME ].valueLength);
    }

    if (gKeyValueStore.table[ KVS_CORE_MQTT_ENDPOINT ].valueLength > 0)
    {
        pcBrokerEndpoint = GetStringValue(KVS_CORE_MQTT_ENDPOINT,
                gKeyValueStore.table[ KVS_CORE_MQTT_ENDPOINT ].valueLength);
    }

    if (gKeyValueStore.table[KVS_ROOT_CA_ID].valueLength > 0)
    {
        LogInfo( ( "Using rootCA cert from key store." ) );
        pcRootCA = GetStringValue(KVS_ROOT_CA_ID, gKeyValueStore.table[ KVS_ROOT_CA_ID ].valueLength);
    }
    else
    {
        LogInfo( ( "Using default rootCA cert." ) );
        pcRootCA = democonfigROOT_CA_PEM;
    }

#endif
#endif  /* defined__CCRL__ || defined__ICCRL78__ || defined__RL */

    /* Initialize the MQTT context with the buffer and transport interface. */
    if (pdPASS == xStatus)
    {
        xMQTTStatus = prvMQTTInit();

        if (MQTTSuccess != xMQTTStatus)
        {
            LogError(("Failed to initialize MQTT with error %d.", xMQTTStatus));
        }
    }

    if (MQTTSuccess == xMQTTStatus)
    {
        pMqttContext->connectStatus = prvConnectToMQTTBroker(false);

        while (MQTTConnected == pMqttContext->connectStatus)
        {
            /* MQTTAgent_CommandLoop() is effectively the agent implementation.  It
             * will manage the MQTT protocol until such time that an error occurs,
             * which could be a disconnect.  If an error occurs the MQTT context on
             * which the error happened is returned so there is an attempt to
             * clean up and reconnect. */
            prvSetMQTTAgentState(MQTT_AGENT_STATE_CONNECTED);

            xMQTTStatus = MQTTAgent_CommandLoop(&xGlobalMqttAgentContext);

            pMqttContext->connectStatus = MQTTNotConnected;
            prvSetMQTTAgentState(MQTT_AGENT_STATE_DISCONNECTED);

            if (MQTTSuccess == xMQTTStatus)
            {
                /*
                 * On a graceful termination, MQTT agent loop returns success.
                 * Cancel all pending MQTT agent requests.
                 * Disconnect the socket and terminate MQTT agent loop.
                 */
                LogInfo(("MQTT Agent loop terminated due to a graceful disconnect."));
#if (DEVICE_ADVISOR_TEST_ENABLED == 0)
                (void)MQTTAgent_CancelAll(&xGlobalMqttAgentContext);
                (void)prvDisconnectTLS(&xNetworkContext);
#else
                ( void ) prvDisconnectTLS( &xNetworkContext );
                pMqttContext->connectStatus = prvConnectToMQTTBroker( true );
#endif
            }
            else
            {
                LogInfo(("MQTT Agent loop terminated due to abrupt disconnect. Retrying MQTT connection.."));

                /* MQTT agent returned due to an underlying error, reconnect to the loop. */
                (void)prvDisconnectTLS(&xNetworkContext);
                pMqttContext->connectStatus = prvConnectToMQTTBroker(true);
            }
        }
    }

    prvSetMQTTAgentState(MQTT_AGENT_STATE_TERMINATED);
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
#if !defined(__TEST__)
    if( pcThingName != NULL )
    {
        vPortFree( pcThingName );
        pcThingName = NULL;
    }

    if( pcBrokerEndpoint != NULL )
    {
        vPortFree( pcBrokerEndpoint );
        pcBrokerEndpoint = NULL;
    }

    if (pcRootCA != NULL)
    {
        vPortFree( pcRootCA );
        pcRootCA = NULL;
    }
#endif
#endif

    LogInfo(("---------MQTT Agent Task Finished---------\r\n"));
    vTaskDelete(NULL);
}/* End of function prvMQTTAgentTask()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvConnectToMQTTBroker
 *
 * @brief Connects a TCP socket to the MQTT broker, then creates and MQTT
 * connection to the same.
 * @param[in] xIsReconnect Boolean flag to indicate if its a reconnection.
 * @return MQTTConnected if connection was successful, MQTTNotConnected if MQTT connection
 *         failed and all retries exhausted.
 */
static MQTTConnectionStatus_t prvConnectToMQTTBroker(bool xIsReconnect)
{
    BaseType_t                xStatus            = pdFAIL;
    MQTTStatus_t              xMQTTStatus;
    MQTTConnectionStatus_t    xConnectionStatus  = MQTTNotConnected;
    BackoffAlgorithmStatus_t  xBackoffAlgStatus  = BackoffAlgorithmSuccess;
    BackoffAlgorithmContext_t xReconnectParams   = { 0 };
    uint16_t                  usNextRetryBackOff = 0U;

/* Initialize network context. */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    xNetworkContext.pParams = &xPlaintextTransportParams;
#else
    xNetworkContext.pParams = &xTlsTransportParams;
#endif

    /* We will use a retry mechanism with an exponential backoff mechanism and
     * jitter.  That is done to prevent a fleet of IoT devices all trying to
     * reconnect at exactly the same time should they become disconnected at
     * the same time. We initialize reconnect attempts and interval here. */
    BackoffAlgorithm_InitializeParams(&xReconnectParams,
                                    RETRY_BACKOFF_BASE_MS,
                                    RETRY_MAX_BACKOFF_DELAY_MS,
                                    RETRY_MAX_ATTEMPTS);

    /* Attempt to connect to MQTT broker. If connection fails, retry after a
     * timeout. Timeout value will exponentially increase until the maximum
     * number of attempts are reached.
     */
    do
    {
        /* Create a TLS connection to broker */
        xStatus = prvCreateTLSConnection(&xNetworkContext);

        if (pdPASS == xStatus)
        {
            xMQTTStatus = prvCreateMQTTConnection(xIsReconnect);

            if (MQTTSuccess != xMQTTStatus)
            {
                LogError(("Failed to connect to MQTT broker, error = %u", xMQTTStatus));
                prvDisconnectTLS(&xNetworkContext);
                xStatus = pdFAIL;
            }
            else
            {
                LogInfo(("Successfully connected to MQTT broker."));
                xConnectionStatus = MQTTConnected;
            }
        }

        if (pdFAIL == xStatus)
        {
            /* Get back-off value (in milliseconds) for the next connection retry. */
            xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff(&xReconnectParams,
                                                                xTaskGetTickCount(),
                                                                &usNextRetryBackOff);

            if (BackoffAlgorithmSuccess == xBackoffAlgStatus)
            {
                LogWarn(("Connection to the broker failed. "
                        "Retrying connection in %hu ms.",
                        usNextRetryBackOff));
                vTaskDelay(pdMS_TO_TICKS(usNextRetryBackOff));
            }
            else if (xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted)
            {
                LogError(("Connection to the broker failed, all attempts exhausted."));
            }
            else
            {
                /* Empty Else. */
            }
        }
    } while ((MQTTNotConnected == xConnectionStatus) && (BackoffAlgorithmSuccess == xBackoffAlgStatus));

    return xConnectionStatus;
}/* End of function prvConnectToMQTTBroker()*/

/**
 * @fn prvGetTimeMs
 *
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs(void)
{
    TickType_t xTickCount = 0;
    uint32_t   ulTimeMs   = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = (uint32_t) xTickCount * mqttexampleMILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = (uint32_t) (ulTimeMs - ulGlobalEntryTimeMs);

    return ulTimeMs;
}/* End of function prvGetTimeMs()*/

/**
 * @fn prvMatchTopicFilterSubscriptions
 *
 * @brief Checks if the topic in the given publish information
 * matches any of the existing MQTT topic filter subscriptions.
 *
 * @param[in] pxPublishInfo Pointer to the MQTT publish information containing the topic to be matched.
 *
 * @return true if a matching subscription is found; false otherwise.
 */
static bool prvMatchTopicFilterSubscriptions(MQTTPublishInfo_t * pxPublishInfo)
{
    uint32_t ulIndex        = 0;
    bool     isMatched      = false;
    bool     publishHandled = false;

    xSemaphoreTake(xSubscriptionsMutex, portMAX_DELAY);
    {
        for (ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++)
        {
            if (xTopicFilterSubscriptions[ulIndex].usTopicFilterLength > 0)
            {
                MQTT_MatchTopic(pxPublishInfo->pTopicName,
                                pxPublishInfo->topicNameLength,
                                xTopicFilterSubscriptions[ ulIndex ].pcTopicFilter,
                                xTopicFilterSubscriptions[ ulIndex ].usTopicFilterLength,
                                &isMatched);

                if (true == isMatched)
                {
                    xTopicFilterSubscriptions[ ulIndex ].pxIncomingPublishCallback( xTopicFilterSubscriptions[ ulIndex ].pvIncomingPublishCallbackContext,
                                                                                    pxPublishInfo );
                    publishHandled = true;
                }
            }
        }
    }
    xSemaphoreGive(xSubscriptionsMutex);
    return publishHandled;
}/* End of function prvMatchTopicFilterSubscriptions()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvSetMQTTAgentState
 *
 * @brief Set MQTT Agent State.
 */
static void prvSetMQTTAgentState(MQTTAgentState_t xAgentState)
{
    xState = xAgentState;
    (void)xEventGroupClearBits(xStateEventGrp, mqttexampleEVENT_BITS_ALL);
    (void)xEventGroupSetBits(xStateEventGrp, mqttexampleEVENT_BIT(xAgentState));
}/* End of function prvSetMQTTAgentState()*/

/*-----------------------------------------------------------*/

/**
 * @fn xMQTTAgentInit
 *
 * @brief Initialize the MQTT Agent.
 * Create the semaphore and Event group.
 */
BaseType_t xMQTTAgentInit(void)
{
    BaseType_t xResult = pdFAIL;

    if (MQTT_AGENT_STATE_NONE == xState)
    {
        xSubscriptionsMutex = xSemaphoreCreateMutex();

        if (NULL != xSubscriptionsMutex)
        {
            xResult = pdPASS;
        }

        if (pdPASS == xResult)
        {
            xStateEventGrp = xEventGroupCreate();

            if (NULL == xStateEventGrp)
            {
                xResult = pdFAIL;
            }
        }
    }

    return xResult;
}/* End of function xMQTTAgentInit()*/

/*-----------------------------------------------------------*/

/**
 * @fn xGetMQTTAgentState
 *
 * @brief Get the current state of MQTT agent.
 * MQTT agent task can be any of the states as defined by the state enum MQTTAgentState_t. The
 * function can be used to poll for MQTT agent state from other application tasks
 *
 * @return State of the MQTT agent.
 */
MQTTAgentState_t xGetMQTTAgentState(void)
{
    return xState;
}/* End of function xGetMQTTAgentState()*/

/**
 * @fn xSetMQTTAgentState
 *
 * @brief Set the current state of MQTT agent.
 * MQTT agent task can be any of the states as defined by the state enum MQTTAgentState_t. The
 * function can be used to set the MQTT agent state from other application tasks
 *
 * @param[in] xAgentState State of the MQTT agent.
 */
void xSetMQTTAgentState(MQTTAgentState_t xAgentState)
{
    prvSetMQTTAgentState(xAgentState);
}/* End of function xSetMQTTAgentState()*/

/*-----------------------------------------------------------*/

/**
 * @fn xWaitForMQTTAgentState
 *
 * @brief Wait for MQTT agent to reach the desired state.
 * Function blocks caller task for a timeout period, until MQTT agent reaches the specified state. Function can be
 * called from multiple tasks concurrently.
 *
 * @param[in] xStateToWait The desired MQTT agent state to reach.
 * @param[in] xTicksToWait Ticks to wait for MQTT agent to reach the desired state. Provide portMAX_DELAY
 *                         if the caller has to wait indefinitely.
 * @return pdTRUE if the state is reached, pdFALSE if the timeout has reached waiting for the state.
 */
BaseType_t xWaitForMQTTAgentState(MQTTAgentState_t xState,
                                TickType_t xTicksToWait)
{
    EventBits_t xBitsSet;
    EventBits_t xBitsToWaitFor;
    BaseType_t  xResult = pdFAIL;

    if (MQTT_AGENT_STATE_NONE != xState)
    {
        xBitsToWaitFor = mqttexampleEVENT_BIT(xState);
        xBitsSet       = xEventGroupWaitBits(xStateEventGrp, xBitsToWaitFor, pdFALSE, pdFALSE, xTicksToWait);

        if ((xBitsSet & xBitsToWaitFor) != 0)
        {
            xResult = pdTRUE;
        }
    }

    return xResult;
}/* End of function xWaitForMQTTAgentState()*/

/*-----------------------------------------------------------------*/

/**
 * @fn xAddMQTTTopicFilterCallback
 *
 * @brief Add a callback for the topic filter with MQTT agent.
 * Function adds a local subscription for the given topic filter.
 * Each incoming publish received on the connection, will be
 * matched against the topic filter. The provided callback is invoked if
 * the incoming publish topic matches the topic filter.
 * An optional context parameter can be passed into this function from the caller,
 * the context is passed back transparently when
 * the publish callback is invoked. If a topic matches more than one registered topic filter,
 * all matching topic filter callbacks are invoked.
 * The function is thread safe and can be invoked from multiple application tasks once the MQTT agent is
 * initialized.
 *
 *
 * @param pcTopicFilter  Topic filter string for which the publish callback needs to be invoked.
 * @param usTopicFilterLength  Length of the topic filter string.
 * @param pxPublishCallback  Callback to be invoked whan a publish is received on topic filter.
 * @param pvCallbackContext Context to be paased in transparently to the user.
 * @param xManageResubscription  Whether its a managed subscription. Managed subscriptions are resubscribed by
 *                               MQTT agent if the connection is lost and then reconnected.
 * @return pdTRUE if the callback was added successfully. pdFALSE otherwise.
 */
BaseType_t xAddMQTTTopicFilterCallback(const char * pcTopicFilter,
                                        uint16_t usTopicFilterLength,
                                        IncomingPubCallback_t pxCallback,
                                        void * pvCallbackContext,
                                        BaseType_t xManageResubscription)
{
    BaseType_t xResult          = pdFAIL;
    uint32_t   ulIndex          = 0U;
    uint32_t   ulAvailableIndex = MQTT_AGENT_MAX_SUBSCRIPTIONS;

    xSemaphoreTake(xSubscriptionsMutex, portMAX_DELAY);
    {
        /**
         * If this is a duplicate subscription for same topic filter do nothing and return a failure.
         * Else insert at the first available index;
         */
        for (ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++)
        {
            if ((NULL                         == xTopicFilterSubscriptions[ulIndex].pcTopicFilter) &&
                (MQTT_AGENT_MAX_SUBSCRIPTIONS == ulAvailableIndex))
            {
                ulAvailableIndex = ulIndex;
            }
            else if ((xTopicFilterSubscriptions[ulIndex].usTopicFilterLength == usTopicFilterLength) &&
                        (strncmp(pcTopicFilter, // @suppress("5.2e Data member alignment for assignments ")
                                xTopicFilterSubscriptions[ulIndex].pcTopicFilter,
                                (size_t)usTopicFilterLength) == 0)) /* Cast to proper datatype to avoid warning */
            {
                /* If a subscription already exists, don't do anything. */
                if ((xTopicFilterSubscriptions[ulAvailableIndex].pxIncomingPublishCallback        == pxCallback) &&
                    (xTopicFilterSubscriptions[ulAvailableIndex].pvIncomingPublishCallbackContext == pvCallbackContext))
                {
                    ulAvailableIndex = MQTT_AGENT_MAX_SUBSCRIPTIONS;
                    xResult          = pdPASS;
                    break; // @suppress("3.7d Break statement")
                }
            }
            else
            {

            }
        }

        if (ulAvailableIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS)
        {
            xTopicFilterSubscriptions[ulAvailableIndex].pcTopicFilter                    = pcTopicFilter;
            xTopicFilterSubscriptions[ulAvailableIndex].usTopicFilterLength              = usTopicFilterLength;
            xTopicFilterSubscriptions[ulAvailableIndex].pxIncomingPublishCallback        = pxCallback;
            xTopicFilterSubscriptions[ulAvailableIndex].pvIncomingPublishCallbackContext = pvCallbackContext;
            xTopicFilterSubscriptions[ulAvailableIndex].xManageResubscription            = xManageResubscription;
            xResult                                                                      = pdPASS;
        }
    }
    xSemaphoreGive(xSubscriptionsMutex);

    return xResult;
}/* End of function xAddMQTTTopicFilterCallback()*/

/**
 * @fn vRemoveMQTTTopicFilterCallback
 *
 * @brief Remove a topic filter callback from the MQTT agent.
 * Function is thread safe and can be invoked by multiple application tasks.
 *
 * @param pcTopicFilter Topic filter string for which the callback needs to be removed.
 * @param usTopicFilterLength Length of the topic filter string.
 */
void vRemoveMQTTTopicFilterCallback(const char * pcTopicFilter,
                                    uint16_t usTopicFilterLength)
{
    uint32_t ulIndex;

    xSemaphoreTake(xSubscriptionsMutex, portMAX_DELAY);
    {
        for (ulIndex = 0U; ulIndex < MQTT_AGENT_MAX_SUBSCRIPTIONS; ulIndex++)
        {
            if (xTopicFilterSubscriptions[ulIndex].usTopicFilterLength == usTopicFilterLength)
            {
                if (strncmp(xTopicFilterSubscriptions[ulIndex].pcTopicFilter, pcTopicFilter, usTopicFilterLength) == 0)
                {
                    memset(&(xTopicFilterSubscriptions[ ulIndex ]), 0x00, sizeof(TopicFilterSubscription_t));
                }
            }
        }
    }
    xSemaphoreGive(xSubscriptionsMutex);
}/* End of function vRemoveMQTTTopicFilterCallback()*/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#pragma section
#endif
