/*
 * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
 * Modifications Copyright (C) 2025 Renesas Electronics Corporation or its affiliates.
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License. See the LICENSE accompanying this file
 * for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include "FreeRTOS.h"
#include "task.h"

#include "core_mqtt.h"
#include "core_mqtt_agent.h"
#include "mqtt_agent_task.h"

/**
 * @fn mqttWrapper_setCoreMqttContext
 *
 * @brief Sets the core MQTT context used by the MQTT wrapper.
 *
 * This function assigns the provided MQTT context to the internal MQTT wrapper module.
 *
 * @param[in] mqttContext Pointer to the MQTTContext_t structure to be used by the wrapper.
 */
void mqttWrapper_setCoreMqttContext (MQTTContext_t * mqttContext);

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
void mqttWrapper_setThingName (const char __far * thingName,
                                size_t thingNameLength);
#else
void mqttWrapper_setThingName(char * thingName,
                                size_t thingNameLength);
#endif

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
void mqttWrapper_getThingName (char *   thingNameBuffer,
                                size_t * thingNameLength);

/**
 * @fn mqttWrapper_isConnected
 *
 * @brief Checks whether the MQTT client is currently connected to the broker.
 *
 * This function returns the connection status of the MQTT client.
 *
 * @return true if the MQTT client is connected; false otherwise.
 */
bool mqttWrapper_isConnected (void);

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
bool mqttWrapper_publish (char *  topic,
                        size_t    topicLength,
                        uint8_t * message,
                        size_t    messageLength);

/******************************************************************************
 * Function Name: mqttWrapper_subscribe
 * Description  : SUBSCRIBE to a topic
 * Arguments    : topic       : The topic filter to be subscribed
 *              : topicLength : Length of the topic filter
 * Return Value : bool        : true if successful, false otherwise.
 *****************************************************************************/
bool mqttWrapper_subscribe (char * topic,
                            size_t topicLength);

/******************************************************************************
 * Function Name: mqttWrapper_unsubscribe
 * Description  : UNSUBSCRIBE from a topic filter
 * Arguments    : topic       : The topic filter to be unsubscribed
 *              : topicLength : Length of the topic filter
 * Return Value : bool        : true if successful, false otherwise.
 *****************************************************************************/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
bool mqttWrapper_unsubscribe (char __far * topic,
                            size_t topicLength);
#else
bool mqttWrapper_unsubscribe(char * topic,
                            size_t topicLength);
#endif

#endif /* ifndef MQTT_WRAPPER_H */
