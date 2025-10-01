/*
FreeRTOS
Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
Modifications Copyright (C) 2024 Renesas Electronics Corporation. or its affiliates.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_start.h"

/* Logging includes. */
#include "iot_logging_task.h"
#include "r_smc_entry.h"
#include "rl78_serial_term_uart.h"

/* Demo includes */
#include "aws_clientcredential.h"
#include "demo_config.h"
#include "mqtt_agent_task.h"

/* Write Certificate */
#include "cert_profile_helper.h"

#if (ENABLE_AFR_IDT == 0)
#if (ENABLE_OTA_UPDATE_DEMO == 1)
#define START_DEMO_FUNC   vStartOtaDemo
#else
#define START_DEMO_FUNC   vStartSimplePubSubDemo
#endif
#endif

extern void START_DEMO_FUNC (void);
extern bool setupWifi (void);
#if (ENABLE_AFR_IDT == 1)
extern void prvQualificationTestTask( void * pvParameters );
extern void vSubscribePublishTestTask( void * pvParameters );
#endif

/**
 * @brief Flag which enables OTA update task in background along with other demo tasks.
 * OTA update task polls regularly for firmware update jobs or acts on a new firmware update
 * available notification from OTA service.
 */
#define appmainINCLUDE_OTA_UPDATE_TASK            (1)

/**
 * @brief Stack size and priority for OTA Update task.
 */
#define appmainMQTT_OTA_UPDATE_TASK_STACK_SIZE    ( 1600 )
#define appmainMQTT_OTA_UPDATE_TASK_PRIORITY      ( tskIDLE_PRIORITY )

#define IDLE_TASK_STACK_SIZE                      (configMINIMAL_STACK_SIZE * 2)

/**
 * @brief Stack size and priority for MQTT agent task.
 * Stack size is capped to an adequate value based on requirements from MbedTLS stack
 * for establishing a TLS connection. Task priority of MQTT agent is set to a priority
 * higher than other MQTT application tasks, so that the agent can drain the queue
 * as work is being produced.
 */
#if (ENABLE_OTA_UPDATE_DEMO == 1)
#define appmainMQTT_AGENT_TASK_STACK_SIZE         (1200)
#define appmainMQTT_AGENT_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )
#else
#define appmainMQTT_AGENT_TASK_STACK_SIZE         (1200)
#define appmainMQTT_AGENT_TASK_PRIORITY           (tskIDLE_PRIORITY + 2)
#endif

#if (ENABLE_AFR_IDT == 1)
#define appmainTEST_TASK_STACK_SIZE               (2000)
#define appmainTEST_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#endif

/* Logging Task Defines. */
#define mainLOGGING_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 2 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH          (15)
#define LED_PORT                                  (P5_bit.no0)

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook (void);

/**
 * @brief Initializes the board.
 */
void              prvMiscInitialization (void);
static BaseType_t xPlatformNetworkUp (void);

#if (ENABLE_AFR_IDT == 1)
int RunDeviceAdvisorDemo( void )
{
    BaseType_t xResult = pdFAIL;

    xResult = xMQTTAgentInit();
    xSetMQTTAgentState( MQTT_AGENT_STATE_INITIALIZED );
    vStartMQTTAgent (appmainMQTT_AGENT_TASK_STACK_SIZE, appmainMQTT_AGENT_TASK_PRIORITY);

    if( xResult == pdPASS )
    {
        xResult = xTaskCreate(vSubscribePublishTestTask,
                            "TEST",
                            appmainTEST_TASK_STACK_SIZE,
                            NULL,
                            appmainTEST_TASK_PRIORITY,
                            NULL);

    }
    return ( xResult == pdPASS ) ? 0 : -1;
}

#if (OTA_E2E_TEST_ENABLED == 1)
int RunOtaE2eDemo( void )
{
    xMQTTAgentInit();
    xSetMQTTAgentState( MQTT_AGENT_STATE_INITIALIZED );
    vStartMQTTAgent (appmainMQTT_AGENT_TASK_STACK_SIZE, appmainMQTT_AGENT_TASK_PRIORITY);

    vStartOtaDemo();
    return 0;
}
#endif
#endif /* ENABLE_AFR_IDT == 1 */

/*-----------------------------------------------------------*/

/**
 * @fn main
 * @brief The application entry point from a power on reset is PowerON_Reset_PC()
 * in resetprg.c.
 */
void main(void)
{
    Processing_Before_Start_Kernel();
    vTaskStartScheduler();

    while (1)
    {
        ;
    }
}/* End of function main()*/

/*-----------------------------------------------------------*/

/**
 * @fn main_task
 *
 * @brief This function is the entry point for a FreeRTOS task
 *
 * @param[in] pvParameters
 */
void main_task(void * pvParameters)
{
#if (ENABLE_AFR_IDT == 1)
    if (pdTRUE == xPlatformNetworkUp())
    {
#if ( OTA_E2E_TEST_ENABLED == 1)

        RunOtaE2eDemo();
#else
        xTaskCreate(prvQualificationTestTask,
                    "TEST",
                    appmainTEST_TASK_STACK_SIZE,
                    NULL,
                    appmainTEST_TASK_PRIORITY,
                    NULL );
#endif
    }

    while( 1 )
    {
        vTaskSuspend( NULL );
    }
#else
    if (pdTRUE == xPlatformNetworkUp())
    {
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
        /* Write certificate to DA16600. */
        prvWriteCertificateToModule((const uint8_t *)CFG_ROOT_CA_PEM1,
                                    /* Cast to proper datatype to avoid warning */
                                    strlen((const char *)CFG_ROOT_CA_PEM1),
                                    (const uint8_t *)keyCLIENT_CERTIFICATE_PEM,
                                    /* Cast to proper datatype to avoid warning */
                                    strlen((const char *)keyCLIENT_CERTIFICATE_PEM),
                                    (const uint8_t *)keyCLIENT_PRIVATE_KEY_PEM,
                                    /* Cast to proper datatype to avoid warning */
                                    strlen((const char *)keyCLIENT_PRIVATE_KEY_PEM));
#endif

        configPRINT_STRING(("---------STARTING DEMO---------\r\n"));

        xMQTTAgentInit();
        xSetMQTTAgentState(MQTT_AGENT_STATE_INITIALIZED);
        vStartMQTTAgent(appmainMQTT_AGENT_TASK_STACK_SIZE, appmainMQTT_AGENT_TASK_PRIORITY);

        START_DEMO_FUNC();
    }

    while (1)
    {
        LED_PORT ^= 1;
        vTaskDelay(5000);
    }
#endif /* ENABLE_AFR_IDT == 1 */
}/* End of function main_task()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvMiscInitialization
 *
 * @brief This function initializes UART and creates logging task for logging purposes
 */
void prvMiscInitialization(void)
{
    /* Initialize UART for serial terminal. */
    uart_config();
    configPRINT_STRING(("Hello World.\r\n"));

    /* Start logging task. */
    xLoggingTaskInitialize(mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY + 2,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH);

}/* End of function prvMiscInitialization()*/

/*-----------------------------------------------------------*/

/**
 * @fn vApplicationDaemonTaskStartupHook
 *
 * @brief This function is called when the Timer Svc Task is created
 */
void vApplicationDaemonTaskStartupHook(void)
{
    prvMiscInitialization();
}/* End of function vApplicationDaemonTaskStartupHook()*/

/*-----------------------------------------------------------*/

/**
 * @fn vApplicationGetIdleTaskMemory
 *
 * @brief This is to provide the memory that is used by the RTOS Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task.
 *
 * @param[out] ppxIdleTaskTCBBuffer     Pointer to the buffer that will hold the Idle task's TCB
 * @param[out] ppxIdleTaskStackBuffer   Pointer to the buffer that will be used as the Idle task's stack
 * @param[out] puxIdleTaskStackSize     Pointer to a variable that will hold the size of the Idle task's stack
 */
void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE * puxIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t  uxIdleTaskStack[IDLE_TASK_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *puxIdleTaskStackSize = IDLE_TASK_STACK_SIZE;
}/* End of function vApplicationGetIdleTaskMemory()*/

/*-----------------------------------------------------------*/

/**
 * @fn vApplicationGetTimerTaskMemory
 *
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 *
 * @param[out] ppxTimerTaskTCBBuffer     Pointer to the buffer that will hold the Idle task's TCB
 * @param[out] ppxTimerTaskStackBuffer   Pointer to the buffer that will be used as the Idle task's stack
 * @param[out] puxTimerTaskStackSize     Pointer to a variable that will hold the size of the Idle task's stack
 */
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer,
                                    StackType_t ** ppxTimerTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE * puxTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t  uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *puxTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}/* End of function vApplicationGetTimerTaskMemory()*/

/*-----------------------------------------------------------*/

#ifndef iotconfigUSE_PORT_SPECIFIC_HOOKS

/**
 * @fn vApplicationMallocFailedHook
 *
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook(void)
{
    configPRINT_STRING(("ERROR: Malloc failed to allocate memory\r\n"));
    taskDISABLE_INTERRUPTS();

    /* Loop forever */
    for (;;)
    {
        /* Do Nothing */
    }
}/* End of function vApplicationMallocFailedHook()*/

/*-----------------------------------------------------------*/

/**
 * @fn vApplicationStackOverflowHook
 *
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
    void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                        char * pcTaskName)
    {
        configPRINT_STRING(("ERROR: stack overflow\r\n"));
        portDISABLE_INTERRUPTS();

        /* Unused Parameters */
        (void) xTask;
        (void) pcTaskName;

        /* Loop forever */
        for (;;)
        {
            /* Do Nothing */
        }
    }/* End of function vApplicationStackOverflowHook()*/
#endif /* iotconfigUSE_PORT_SPECIFIC_HOOKS */
/*-----------------------------------------------------------*/

/**
 * @fn xPlatformNetworkUp
 *
 * @brief Initialize Wi-Fi module
 *
 * @return Wi-Fi module initialization status. Return true if success
 */
static BaseType_t xPlatformNetworkUp(void)
{
    return ((BaseType_t)setupWifi());
}/* End of function xPlatformNetworkUp()*/
