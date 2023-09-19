/*******************************************************************************************************************//**
 * @addtogroup RX_CELLULAR_PLATFORM_AWS
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "task.h"

#include "cellular_config.h"
#include "cellular_config_defaults.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef RX_CELLULAR_PLATFORM_AWS_H
#define RX_CELLULAR_PLATFORM_AWS_H
/**
 * @brief Cellular library platform event group APIs.
 *
 * Cellular library use platform event group for process synchronization.
 *
 * The EventGroup functions in the following link can be referenced as function prototype.
 * https://www.freertos.org/event-groups-API.html
 *
 */

#define Platform_Delay(delayMs)    vTaskDelay(pdMS_TO_TICKS(delayMs))

#define PlatformEventGroupHandle_t           EventGroupHandle_t
#define PlatformEventGroup_Delete            vEventGroupDelete
#define PlatformEventGroup_ClearBits         xEventGroupClearBits
#define PlatformEventGroup_Create            xEventGroupCreate
#define PlatformEventGroup_GetBits           xEventGroupGetBits
#define PlatformEventGroup_SetBits           xEventGroupSetBits
#define PlatformEventGroup_SetBitsFromISR    xEventGroupSetBitsFromISR
#define PlatformEventGroup_WaitBits          xEventGroupWaitBits
#define PlatformEventGroup_EventBits         EventBits_t

#define PlatformTickType                     TickType_t

/* Converts a time in milliseconds to a time in ticks.  This macro can be
 * overridden by a macro of the same name defined in FreeRTOSConfig.h in case the
 * definition here is not suitable for your application. */
#ifndef pdMS_TO_TICKS
 #define pdMS_TO_TICKS(xTimeInMs)    ((TickType_t) (((TickType_t) (xTimeInMs) * (TickType_t) 1000) / \
                                                    (TickType_t) 1000U))
#endif

/**
 * @brief Cellular library platform thread API and configuration.
 *
 * Cellular library create a detached thread by this API.
 * The threadRoutine should be called with pArgument in the created thread.
 *
 * PLATFORM_THREAD_DEFAULT_STACK_SIZE and PLATFORM_THREAD_DEFAULT_PRIORITY defines
 * the platform related stack size and priority.
 */

bool Platform_CreateDetachedThread(void ( * threadRoutine)(void *),
                                   void   * pArgument,
                                   uint32_t priority,
                                   size_t   stackSize);

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define PLATFORM_THREAD_DEFAULT_STACK_SIZE    (1024U)
#else
#define PLATFORM_THREAD_DEFAULT_STACK_SIZE    (2048U)
#endif /* defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL) */
#define PLATFORM_THREAD_DEFAULT_PRIORITY      (configMAX_PRIORITIES - 1)

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform mutex APIs.
 *
 * Cellular library use platform mutex to protect resource.
 *
 * The IotMutex_ functions can be referenced as function prototype for
 * PlatfromMutex_ prefix function in the following link.
 * https://docs.aws.amazon.com/freertos/latest/lib-ref/c-sdk/platform/platform_threads_functions.html
 *
 */

// typedef long BaseType_t;
typedef struct PlatformMutex
{
    StaticSemaphore_t xMutex;          /**< FreeRTOS mutex. */
    BaseType_t        recursive;       /**< Type; used for indicating if this is reentrant or normal. */
    bool              created;
} PlatformMutex_t;

bool PlatformMutex_Create(PlatformMutex_t * pNewMutex, bool recursive);
void PlatformMutex_Destroy(PlatformMutex_t * pMutex);
void PlatformMutex_Lock(PlatformMutex_t * pMutex);
bool PlatformMutex_TryLock(PlatformMutex_t * pMutex);
void PlatformMutex_Unlock(PlatformMutex_t * pMutex);

/*-----------------------------------------------------------*/

/**
 * @brief Cellular library platform memory allocation APIs.
 *
 * Cellular library use platform memory allocation APIs to allocate memory dynamically.
 * The FreeRTOS memory management document can be referenced for these APIs.
 * https://www.freertos.org/a00111.html
 *
 */

#define Platform_Malloc    pvPortMalloc
#define Platform_Free      vPortFree

/*******************************************************************************************************************//**
 * @} (end addtogroup RX_CELLULAR_PLATFORM_AWS)
 **********************************************************************************************************************/

#endif
