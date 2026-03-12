/*
 * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
 * Modifications Copyright (C) 2026 Renesas Electronics Corporation or its affiliates.
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License. See the LICENSE accompanying this file
 * for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef OTA_DEMO_H
#define OTA_DEMO_H

#include <stdint.h>
#include <stdbool.h>

#include "MQTTFileDownloader_config.h"

#define OTA_DATA_BLOCK_SIZE    mqttFileDownloader_CONFIG_BLOCK_SIZE
#define JOB_DOC_SIZE           (2048U)

/**
 * @brief Allow update to same or lower version.
 *
 * Set this to 1 to allow downgrade or same version update.This configurations parameter
 * disables version check and allows update to a same or lower version.This is provided for
 * testing purpose and it is recommended to always update to higher version and keep this
 * configuration disabled.
 */
#define otaconfigAllowDowngrade             (1U)

#if otaconfigAllowDowngrade == 0
#error "RL78/G23 does not support version rollback because it uses a single bank!\
Please set otaconfigAllowDowngrade to 1."
#endif

typedef enum OtaEvent
{
    OtaAgentEventStart = 0,           /*!< @brief Start the OTA state machine */
    OtaAgentEventWaitForJob,          /*!< @brief Event to wait for Job notification */
    OtaAgentEventRequestJobDocument,  /*!< @brief Event for requesting job document. */
    OtaAgentEventReceivedJobDocument, /*!< @brief Event when job document is received. */
    OtaAgentEventCreateFile,          /*!< @brief Event to create a file. */
    OtaAgentEventRequestFileBlock,    /*!< @brief Event to request file blocks. */
    OtaAgentEventReceivedFileBlock,   /*!< @brief Event to trigger when file block is received. */
    OtaAgentEventCloseFile,           /*!< @brief Event to trigger closing file. */
    OtaAgentEventActivateImage,       /*!< @brief Event to activate the new image. */
    OtaAgentEventVersionCheck,        /*!< @brief Event to verify the new image version. */
    OtaAgentEventSuspend,             /*!< @brief Event to suspend ota task */
    OtaAgentEventResume,              /*!< @brief Event to resume suspended task */
    OtaAgentEventUserAbort,           /*!< @brief Event triggered by user to stop agent. */
    OtaAgentEventJobCanceled,         /*!< @brief Event triggered by AWS that cancels the OTA job. */
    OtaAgentEventShutdown,            /*!< @brief Event to trigger ota shutdown */
    OtaAgentEventNotifyCanceled,      /*!< @brief Event to Unsubscribe from Job topic */
    OtaAgentEventMax                  /*!< @brief Last event specifier */
} OtaEvent_t;

/**
 * @brief OTA Agent states.
 *
 * The current state of the OTA Task (OTA Agent).
 */
typedef enum OtaState
{
    OtaAgentStateNoTransition = -1,
    OtaAgentStateInit = 0,
    OtaAgentStateReady,
    OtaAgentStateRequestingJob,
    OtaAgentStateWaitingForJob,
    OtaAgentStateCreatingFile,
    OtaAgentStateRequestingFileBlock,
    OtaAgentStateWaitingForFileBlock,
    OtaAgentStateClosingFile,
    OtaAgentStateSuspended,
    OtaAgentStateJobCanceled,
    OtaAgentStateShuttingDown,
    OtaAgentStateStopped,
    OtaAgentStateAll
} OtaState_t;

/**
 * @brief  The OTA Agent event and data structures.
 */

typedef struct OtaDataEvent
{
    uint8_t data[OTA_DATA_BLOCK_SIZE * 2]; /*!< Buffer for storing event information. */
    size_t  dataLength;                       /*!< Total space required for the event. */
    bool    bufferUsed;                         /*!< Flag set when buffer is used otherwise cleared. */
} OtaDataEvent_t;

typedef struct OtaJobEventData
{
    uint8_t jobData[JOB_DOC_SIZE];
    size_t  jobDataLength;
} OtaJobEventData_t;

/**
 * @brief Application version structure.
 *
 */
typedef struct
{
    /* MISRA Ref 19.2.1 [Unions] */
    /* More details at: https://github.com/aws/ota-for-aws-iot-embedded-sdk/blob/main/MISRA.md#rule-192 */
    /* coverity[misra_c_2012_rule_19_2_violation] */
    union
    {
#if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && ( __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN )
        struct version
        {
            uint16_t build; /*!< @brief Build of the firmware (Z in firmware version Z.Y.X). */
            uint8_t  minor;  /*!< @brief Minor version number of the firmware (Y in firmware version Z.Y.X). */

            uint8_t  major;  /*!< @brief Major version number of the firmware (X in firmware version Z.Y.X). */
        } x;                /*!< @brief Version number of the firmware. */
#elif ( defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ) || ( __big_endian__ == 1 ) || ( __BYTE_ORDER == __BIG_ENDIAN )
        struct version
        {
            uint8_t major;  /*!< @brief Major version number of the firmware (X in firmware version X.Y.Z). */
            uint8_t minor;  /*!< @brief Minor version number of the firmware (Y in firmware version X.Y.Z). */

            uint16_t build; /*!< @brief Build of the firmware (Z in firmware version X.Y.Z). */
        } x;                /*!< @brief Version number of the firmware. */
#else /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
#error "Unable to determine byte order!"
#endif /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
        uint32_t unsignedVersion32;
        int32_t  signedVersion32;
    } u; /*!< @brief Version based on configuration in big endian or little endian. */
} AppVersion32_t;

/**
 * @brief Stores information about the event message.
 *
 */
typedef struct OtaEventMsg
{
    OtaDataEvent_t *    dataEvent;   /*!< Data Event message. */
    OtaJobEventData_t * jobEvent; /*!< Job Event message. */
    OtaEvent_t          eventId;           /*!< Identifier for the event. */
} OtaEventMsg_t;

/******************************************************************************
 * Function Name: otaDemo_start
 * Description  : Starts the OTA demo
 * Return Value : None
 *****************************************************************************/
void otaDemo_start (void);

/******************************************************************************
 * Function Name: otaDemo_handleIncomingMQTTMessage
 * Description  : Handles the received MQTT message
 * Arguments    : topic
 *              : topicLength
 *              : message
 *              : messageLength
 * Return Value : true    Successful processing
 *              : false   Failure
 *****************************************************************************/
bool otaDemo_handleIncomingMQTTMessage (char * topic,
                                        size_t topicLength,
                                        uint8_t * message,
                                        size_t messageLength);

/**
 * @fn getOtaAgentState
 *
 * @brief Retrieves the current state of the OTA agent.
 *
 * This function returns the current operational state of the OTA (Over-the-Air) update agent.
 * It can be used to monitor the OTA process, check whether an update is in progress,
 * completed, or if the agent is idle.
 *
 * @return The current OTA agent state as an OtaState_t enum value.
 */
OtaState_t getOtaAgentState (void);

#endif /* ifndef OTA_DEMO_H */
