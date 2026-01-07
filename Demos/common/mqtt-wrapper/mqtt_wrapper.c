/*
 * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
 * Modifications Copyright (C) 2026 Renesas Electronics Corporation or its affiliates.
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License. See the LICENSE accompanying this file
 * for the specific language governing permissions and limitations under
 * the License.
 */

#include <string.h>

#include "FreeRTOS.h"
#include "mqtt_wrapper.h"
#include "ota_demo.h"

#define MQTT_AGENT_NOTIFY_IDX    ((uint32_t)2)

static MQTTContext_t * globalCoreMqttContext = NULL;

/**
 * @brief Static handle used for MQTT agent context.
 */
extern MQTTAgentContext_t xGlobalMqttAgentContext;

#define MAX_THING_NAME_SIZE    (128U)

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static char __far globalThingName[MAX_THING_NAME_SIZE + 1];
#else
static char globalThingName[MAX_THING_NAME_SIZE + 1];
#endif

static size_t globalThingNameLength = 0U;

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct MQTTAgentCommandContext
{
    MQTTStatus_t xReturnStatus;
    TaskHandle_t xTaskToNotify;
    void *       pArgs;
};

/**
 * @fn mqttWrapper_setCoreMqttContext
 *
 * @brief Sets the core MQTT context used by the MQTT wrapper.
 *
 * This function assigns the provided MQTT context to the internal MQTT wrapper module.
 *
 * @param[in] mqttContext Pointer to the MQTTContext_t structure to be used by the wrapper.
 */
void mqttWrapper_setCoreMqttContext(MQTTContext_t * mqttContext)
{
    globalCoreMqttContext = mqttContext;
}
/******************************************************************************
 End of function mqttWrapper_setCoreMqttContext
 *****************************************************************************/

/**
 * @fn mqttWrapper_setThingName
 *
 * @brief Set the device's Thing Name used in MQTT communication.
 *
 * This function set the unique Thing Name to the device.
 *
 * @param[in] thingNameBuffer   Pointer to the buffer where the Thing Name will be stored.
 * @param[in] thingNameLength   Pointer to a variable that will receive the length of the Thing Name.
 */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
void mqttWrapper_setThingName(const char __far * thingName,
                            size_t thingNameLength)
#else
void mqttWrapper_setThingName(char * thingName,
                            size_t thingNameLength)
#endif
{
    configASSERT(thingNameLength <= MAX_THING_NAME_SIZE);
    memcpy(globalThingName, thingName, MAX_THING_NAME_SIZE);
    globalThingName[thingNameLength] = '\0';
    globalThingNameLength            = thingNameLength;
}
/******************************************************************************
 End of function mqttWrapper_setThingName
 *****************************************************************************/

/**
 * @fn mqttWrapper_getThingName
 *
 * @brief Retrieves the device's Thing Name used in MQTT communication.
 *
 * This function obtains the unique Thing Name assigned to the device.
 *
 * @param[out] thingNameBuffer   Pointer to the buffer where the Thing Name will be stored.
 * @param[out] thingNameLength   Pointer to a variable that will receive the length of the Thing Name.
 */
void mqttWrapper_getThingName(char *   thingNameBuffer,
                            size_t * thingNameLength)
{
    configASSERT(globalThingName[ 0 ] != 0);

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    /* Cast to type appropriate datatype to be compatible with parameter type */
    memcpy(thingNameBuffer, (const void __far *)globalThingName, globalThingNameLength);
#else
    memcpy(thingNameBuffer, globalThingName, globalThingNameLength);
#endif
    thingNameBuffer[globalThingNameLength] = '\0';
    *thingNameLength                       = globalThingNameLength;
}
/******************************************************************************
 End of function mqttWrapper_getThingName
 *****************************************************************************/

/**
 * @fn mqttWrapper_isConnected
 *
 * @brief Checks whether the MQTT client is currently connected to the broker.
 *
 * This function returns the connection status of the MQTT client.
 *
 * @return true if the MQTT client is connected; false otherwise.
 */
bool mqttWrapper_isConnected(void)
{
    bool isConnected = false;

    configASSERT(globalCoreMqttContext != NULL);
    isConnected = (MQTTConnected == globalCoreMqttContext->connectStatus);
    return isConnected;
}
/******************************************************************************
 End of function mqttWrapper_isConnected
 *****************************************************************************/

/**
 * @fn prvPublishCommandCallback
 *
 * @brief Callback invoked upon completion of an MQTT publish command.
 *
 * This function is called by the MQTT agent when a publish command has finished processing.
 *
 * @param[in] pCmdCallbackContext Pointer to the context associated with the original publish command.
 * @param[in] pReturnInfo         Pointer to the structure containing the result of the publish operation.
 */
static void prvPublishCommandCallback(MQTTAgentCommandContext_t * pCmdCallbackContext,
                                    MQTTAgentReturnInfo_t *     pReturnInfo)
{
    TaskHandle_t xTaskHandle = (struct tskTaskControlBlock *) pCmdCallbackContext->xTaskToNotify;


    if (NULL != xTaskHandle)
    {
        uint32_t ulNotifyValue = MQTTSuccess; /* ( pxReturnInfo->returnCode & 0xFFFFFF ); */

/* */
/*      if( pxReturnInfo->pSubackCodes ) */
/*      { */
/*          ulNotifyValue += ( pxReturnInfo->pSubackCodes[ 0 ] << 24 ); */
/*      } */

        (void) xTaskNotifyIndexed(xTaskHandle,
                                MQTT_AGENT_NOTIFY_IDX,
                                ulNotifyValue,
                                eSetValueWithOverwrite);
    }
}
/******************************************************************************
 End of function prvPublishCommandCallback
 *****************************************************************************/

/******************************************************************************
 * Function Name: prvUnsubscribeCommandCallback
 * Description  : Callback of UNSUBSCRIBE command issued by prvUnsubscribeCommandCallback
 * Arguments    : pCmdCallbackContext : The command context
 *              : pReturnInfo         : The return information
 * Return Value : None
 *****************************************************************************/
static void prvUnsubscribeCommandCallback(MQTTAgentCommandContext_t * pCmdCallbackContext,
                                        MQTTAgentReturnInfo_t *     pReturnInfo)
{
    TaskHandle_t               xTaskHandle    = (struct tskTaskControlBlock *) pCmdCallbackContext->xTaskToNotify;
    MQTTAgentSubscribeArgs_t * pSubscribeArgs = (MQTTAgentSubscribeArgs_t *) pCmdCallbackContext->pArgs;

    if (NULL != xTaskHandle)
    {
        uint32_t ulNotifyValue = MQTTSuccess; /* ( pxReturnInfo->returnCode & 0xFFFFFF ); */

        (void) xTaskNotifyIndexed(xTaskHandle,
                                MQTT_AGENT_NOTIFY_IDX,
                                ulNotifyValue,
                                eSetValueWithOverwrite);

        /* Remove the topic filter callback */
        vRemoveMQTTTopicFilterCallback(pSubscribeArgs->pSubscribeInfo->pTopicFilter,
                                        pSubscribeArgs->pSubscribeInfo->topicFilterLength);
    }
}
/******************************************************************************
 End of function prvUnsubscribeCommandCallback
 *****************************************************************************/

/**
 * @fn mqttWrapper_publish
 *
 * @brief Publishes an MQTT message to the specified topic.
 *
 * This function sends a publish command message to the MQTT Agent on the given topic.
 *
 * @param[in] topic          Pointer to the topic string to publish to.
 * @param[in] topicLength    Length of the topic string in bytes.
 * @param[in] message        Pointer to the message payload.
 * @param[in] messageLength  Length of the message payload in bytes.
 *
 * @return true if the message was successfully published; false otherwise.
 */
bool mqttWrapper_publish(char *   topic,
                        size_t    topicLength,
                        uint8_t * message,
                        size_t    messageLength)
{
    bool success = false;

    configASSERT(NULL != globalCoreMqttContext);
    configASSERT(topicLength > 0);

    success = mqttWrapper_isConnected();

    if (success)
    {
        MQTTStatus_t mqttStatus = MQTTSuccess;

        /* TODO: This should be static or should we wait? */
        static MQTTPublishInfo_t pubInfo      = { .qos = MQTTQoS0 };
        MQTTAgentContext_t *     xAgentHandle = &xGlobalMqttAgentContext;

        pubInfo.qos             = MQTTQoS0;
        pubInfo.retain          = false;
        pubInfo.dup             = false;
        pubInfo.pTopicName      = topic;
        pubInfo.topicNameLength = topicLength;
        pubInfo.pPayload        = message;
        pubInfo.payloadLength   = messageLength;

        MQTTAgentCommandContext_t xCommandContext =
        {
            .xTaskToNotify = xTaskGetCurrentTaskHandle(),
            .xReturnStatus = MQTTIllegalState,
        };

        MQTTAgentCommandInfo_t xCommandParams =
        {
            .blockTimeMs                 = 1000,
            .cmdCompleteCallback         = prvPublishCommandCallback,
            .pCmdCompleteCallbackContext = &xCommandContext,
        };

        (void)xTaskNotifyStateClearIndexed(NULL, MQTT_AGENT_NOTIFY_IDX);

        mqttStatus = MQTTAgent_Publish(xAgentHandle,
                                    &pubInfo,
                                    &xCommandParams);

        if (MQTTSuccess == mqttStatus)
        {
            uint32_t ulNotifyValue = 0;

            if (xTaskNotifyWaitIndexed(MQTT_AGENT_NOTIFY_IDX,
                                        0x0,
                                        0xFFFFFFFF,
                                        &ulNotifyValue,
                                        portMAX_DELAY))
            {
                mqttStatus = (MQTTStatus_t) (ulNotifyValue & 0x00FFFFFF);
            }
            else
            {
                mqttStatus = MQTTKeepAliveTimeout;
            }
        }

        success = (MQTTSuccess == mqttStatus);
    }

    return success;
}
/******************************************************************************
 End of function mqttWrapper_publish
 *****************************************************************************/

/**
 * @fn handleReceivedPublish
 *
 * @brief Handles an incoming MQTT PUBLISH message.
 *
 * This function is called when a new MQTT PUBLISH message is received. It processes the message
 * and performs any necessary actions based on its content and topic. The callback context can be
 * used to pass user-defined data or state information relevant to the handling logic.
 *
 * @param[in] pvIncomingPublishCallbackContext Pointer to user-defined context associated with the publish callback.
 * @param[in] pxPublishInfo Pointer to the structure containing details of the received MQTT PUBLISH message.
 */
void handleReceivedPublish(void *           pvIncomingPublishCallbackContext,
                        MQTTPublishInfo_t * pxPublishInfo)
{
    (void) pvIncomingPublishCallbackContext;
    char *    topic          = NULL;
    size_t    topicLength    = 0U;
    uint8_t * message        = NULL;
    size_t    messageLength  = 0U;
    bool      messageHandled = false;

    /* Cast to type appropriate datatype to be compatible with parameter type */
    topic         = (char *) pxPublishInfo->pTopicName;
    topicLength   = pxPublishInfo->topicNameLength;

    /* Cast to type appropriate datatype to be compatible with parameter type */
    message       = (uint8_t *) pxPublishInfo->pPayload;
    messageLength = pxPublishInfo->payloadLength;

    /* Cast to type "unsigned int" to be compatible with parameter type */
    configPRINTF(("[INFO] Incoming PUBLISH received on topic %.*s\n",
                    (unsigned int) topicLength, topic));

    messageHandled = otaDemo_handleIncomingMQTTMessage(topic, topicLength, message, messageLength);

    if (!messageHandled)
    {
        /* Cast to type appropriate datatype to be compatible with parameter type */
        configPRINTF(("[WARN] Unhandled incoming PUBLISH received on topic %.*s\n, "
                        "message: %.*s\n",
                        (unsigned int) topicLength,
                        topic,
                        (unsigned int) messageLength,
                        (char *) message));
        configPRINTF(("\n"));
    }
}
/******************************************************************************
 End of function handleReceivedPublish
 *****************************************************************************/

/******************************************************************************
 * Function Name: mqttWrapper_subscribe
 * Description  : SUBSCRIBE to a topic
 * Arguments    : topic       : The topic filter to be subscribed
 *              : topicLength : Length of the topic filter
 * Return Value : bool        : true if successful, false otherwise.
 *****************************************************************************/
bool mqttWrapper_subscribe(char * topic,
                        size_t topicLength)
{
    bool success = false;

    configASSERT(NULL != globalCoreMqttContext);

    configASSERT(topic);
    configASSERT(topicLength > 0);

    success = mqttWrapper_isConnected();

    if (success)
    {
        MQTTStatus_t mqttStatus = MQTTSuccess;

        mqttStatus = MqttAgent_SubscribeSync(topic,
                                            topicLength,
                                            MQTTQoS1,
                                            handleReceivedPublish,
                                            NULL);

        configASSERT(MQTTSuccess == mqttStatus);

        success = (MQTTSuccess == mqttStatus);
    }

    return success;
}
/******************************************************************************
 End of function mqttWrapper_subscribe
 *****************************************************************************/

/******************************************************************************
 * Function Name: mqttWrapper_unsubscribe
 * Description  : UNSUBSCRIBE from a topic filter
 * Arguments    : topic       : The topic filter to be unsubscribed
 *              : topicLength : Length of the topic filter
 * Return Value : bool        : true if successful, false otherwise.
 *****************************************************************************/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
bool mqttWrapper_unsubscribe(char __far * topic,
                            size_t topicLength)
#else
bool mqttWrapper_unsubscribe(char * topic,
                            size_t topicLength)
#endif
{

    configASSERT(topic);
    configASSERT(topicLength > 0);

    MQTTAgentSubscribeArgs_t  xSubscribeArgs  = {0};
    MQTTSubscribeInfo_t       xSubscribeInfo  = {.qos = MQTTQoS0};
    MQTTAgentCommandContext_t xCommandContext = {.xReturnStatus = MQTTSuccess};
    MQTTAgentCommandInfo_t    xCommandParams  = {0UL};

    /* Complete the subscribe information.  The topic string must persist for
     * duration of subscription! */
    xSubscribeInfo.pTopicFilter      = topic;

    /* Cast to type appropriate datatype to be compatible with parameter type */
    xSubscribeInfo.topicFilterLength = (uint16_t)topicLength;
    xSubscribeInfo.qos               = MQTTQoS0;

    xSubscribeArgs.pSubscribeInfo   = &xSubscribeInfo;
    xSubscribeArgs.numSubscriptions = 1;

    /* Complete an application defined context associated with this subscribe message.
     * This gets updated in the callback function so the variable must persist until
     * the callback executes. */
    xCommandContext.xTaskToNotify = xTaskGetCurrentTaskHandle();
    xCommandContext.xReturnStatus = MQTTIllegalState;

    /* Cast to type appropriate datatype to be compatible with parameter type */
    xCommandContext.pArgs         = (void *) &xSubscribeArgs;

    xCommandParams = (MQTTAgentCommandInfo_t)
    {
        .blockTimeMs                 = 1000,
        .cmdCompleteCallback         = prvUnsubscribeCommandCallback,
        .pCmdCompleteCallbackContext = &xCommandContext,
    };

    MQTTStatus_t xCommandStatus = MQTTAgent_Unsubscribe(&xGlobalMqttAgentContext,
                                                        &xSubscribeArgs,
                                                        &xCommandParams);

    if (MQTTSuccess == xCommandStatus)
    {
        uint32_t ulNotifyValue = 0;

        if (xTaskNotifyWaitIndexed(MQTT_AGENT_NOTIFY_IDX,
                                0x0,
                                0xFFFFFFFF,
                                &ulNotifyValue,
                                portMAX_DELAY))
        {
            xCommandStatus = (MQTTStatus_t) (ulNotifyValue & 0x00FFFFFF);
        }
        else
        {
            xCommandStatus = MQTTKeepAliveTimeout;
        }
    }

    return (MQTTSuccess == xCommandStatus);
}
/******************************************************************************
 End of function mqttWrapper_unsubscribe
 *****************************************************************************/
