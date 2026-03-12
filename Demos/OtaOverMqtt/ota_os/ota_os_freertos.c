/*
 * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
 * Modifications Copyright (C) 2026 Renesas Electronics Corporation or its affiliates.
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License. See the LICENSE accompanying this file
 * for the specific language governing permissions and limitations under
 * the License.
 */

/**
 * @file ota_os_freertos.c
 * @brief Example implementation of the OTA OS Functional Interface for
 * FreeRTOS.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

/* OTA OS POSIX Interface Includes.*/

#include "ota_demo.h"
#include "ota_os_freertos.h"

/* OTA Event queue attributes.*/
#define MAX_MESSAGES    (20)
#define MAX_MSG_SIZE    sizeof( OtaEventMsg_t )

/* Array containing pointer to the OTA event structures used to send events to
 * the OTA task. */
static OtaEventMsg_t queueData[MAX_MESSAGES * MAX_MSG_SIZE];

/* The queue control structure.  .*/
static StaticQueue_t staticQueue;

/* The queue control handle.  .*/
static QueueHandle_t otaEventQueue = NULL;

/**
 * @fn OtaInitEvent_FreeRTOS
 *
 * @brief Initialize the OTA events.
 *
 * This function initializes the OTA events mechanism for freeRTOS platforms.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @return               OtaOsStatus_t, OtaOsSuccess if success , other error
 * code on failure.
 */
OtaOsStatus_t OtaInitEvent_FreeRTOS(void)
{
    OtaOsStatus_t otaOsStatus = OtaOsSuccess;

    /* Cast to type appropriate datatype to be compatible with parameter type */
    otaEventQueue = xQueueCreateStatic( ( UBaseType_t ) MAX_MESSAGES,
                                        ( UBaseType_t ) MAX_MSG_SIZE,
                                        ( uint8_t * ) queueData,
                                        &staticQueue );

    if (otaEventQueue == NULL)
    {
        otaOsStatus = OtaOsEventQueueCreateFailed;

        /* Cast to type appropriate datatype to be compatible with parameter type */
        LogError( ("Failed to create OTA Event Queue: "
                "xQueueCreateStatic returned error: "
                "OtaOsStatus_t=%d \n",
                ( int ) otaOsStatus ) );
    }
    else
    {
        LogInfo(("OTA Event Queue created.\n"));
    }

    return otaOsStatus;
}
/******************************************************************************
 End of function OtaInitEvent_FreeRTOS
 *****************************************************************************/

/**
 * @fn OtaSendEvent_FreeRTOS
 *
 * @brief Sends an OTA event.
 *
 * This function sends an event to OTA library event handler on FreeRTOS
 * platforms.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @param[pEventMsg]     Event to be sent to the OTA handler.
 *
 * @param[timeout]       The maximum amount of time (msec) the task should
 * block.
 *
 * @return               OtaOsStatus_t, OtaOsSuccess if success , other error
 * code on failure.
 */
OtaOsStatus_t OtaSendEvent_FreeRTOS(const void * pEventMsg)
{
    OtaOsStatus_t otaOsStatus = OtaOsSuccess;
    BaseType_t    retVal      = pdFALSE;

    /* Send the event to OTA event queue.*/
    retVal = xQueueSendToBack(otaEventQueue, pEventMsg, (TickType_t)0);

    if (pdTRUE == retVal)
    {
        LogInfo(("OTA Event Sent.\n"));
    }
    else
    {
        otaOsStatus = OtaOsEventQueueSendFailed;

        /* Cast to type appropriate datatype to be compatible with parameter type */
        LogError( ( "Failed to send event to OTA Event Queue: "
                "xQueueSendToBack returned error: "
                "OtaOsStatus_t=%d \n",
                ( int ) otaOsStatus ) );
    }

    return otaOsStatus;
}
/******************************************************************************
 End of function OtaSendEvent_FreeRTOS
 *****************************************************************************/


/**
 * @fn OtaReceiveEvent_FreeRTOS
 *
 * @brief Receive an OTA event.
 *
 * This function receives next event from the pending OTA events on FreeRTOS
 * platforms.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @param[pEventMsg]     Pointer to store message.
 *
 * @param[timeout]       The maximum amount of time the task should block.
 *
 * @return               OtaOsStatus_t, OtaOsSuccess if success , other error
 * code on failure.
 */
OtaOsStatus_t OtaReceiveEvent_FreeRTOS(void * pEventMsg)
{
    OtaOsStatus_t otaOsStatus = OtaOsSuccess;
    BaseType_t    retVal      = pdFALSE;

    retVal = xQueueReceive(otaEventQueue, (OtaEventMsg_t *)pEventMsg, pdMS_TO_TICKS(3000U));

    if (pdTRUE == retVal)
    {
        LogInfo(("OTA Event received \n"));
    }
    else
    {
        otaOsStatus = OtaOsEventQueueReceiveFailed;

        /* Cast to type appropriate datatype to be compatible with parameter type */
        LogInfo(("Pending event on OTA Event Queue"));
    }

    return otaOsStatus;
}
/******************************************************************************
 End of function OtaReceiveEvent_FreeRTOS
 *****************************************************************************/

/**
 * @fn OtaDeinitEvent_FreeRTOS
 *
 * @brief Deinitialize the OTA Events mechanism.
 *
 * This function deinitialize the OTA events mechanism and frees any resources
 * used on FreeRTOS platforms.
 *
 * @param[pEventCtx]     Pointer to the OTA event context.
 *
 * @return               OtaOsStatus_t, OtaOsSuccess if success , other error
 * code on failure.
 */
void OtaDeinitEvent_FreeRTOS(void)
{
    vQueueDelete(otaEventQueue);

    LogInfo(("OTA Event Queue Deleted. \n"));
}
/******************************************************************************
 End of function OtaDeinitEvent_FreeRTOS
 *****************************************************************************/
