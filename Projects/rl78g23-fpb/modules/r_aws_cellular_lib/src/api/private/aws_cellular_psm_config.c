/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : aws_cellular_psm_config.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_aws_cellular_if.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/
#define RM_PSM_PHASE_1    (0x01U << 0)
#define RM_PSM_PHASE_2    (0x01U << 1)
#define RM_PSM_PHASE_3    (0x01U << 2)
#define RM_PSM_PHASE_4    (0x01U << 3)
#define RM_PSM_PHASE_5    (0x01U << 4)
#define RM_PSM_PHASE_6    (0x01U << 5)

/**********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/
static void aws_cellular_irq_callback (void * const p_Args);
static bool aws_cellular_irq_open (st_aws_cellular_ctrl_t * const p_aws_ctrl);
static void aws_cellular_irq_close (st_aws_cellular_ctrl_t * const p_aws_ctrl);
static void aws_cellular_psm_conf_fail (CellularContext_t * p_context, const uint8_t phase);
static void aws_cellular_timeout_init (st_aws_cellular_timeout_ctrl_t * timeout_ctrl, uint32_t timeout);
static bool aws_cellular_check_timeout (st_aws_cellular_timeout_ctrl_t * const timeout_ctrl);
static void ring_thread (void * const p_pvParameters);

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_psm_config
 ************************************************************************************************/
CellularError_t aws_cellular_psm_config(CellularContext_t * p_context, const uint8_t mode)
{
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;   //cast

    CellularError_t     cellularStatus = CELLULAR_UNKNOWN;
    CellularPktStatus_t pktStatus      = CELLULAR_PKT_STATUS_OK;

    bool       status     = false;
    uint8_t    cmdBuf[32] = {'\0'};
    uint8_t    phase      = 0;
    BaseType_t rtos_ret   = pdFALSE;

    CellularAtReq_t atReqpsmconfig =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    if (1 == mode)
    {
        (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                        "AT+SQNRICFG=2,3,%d", AWS_CELLULAR_CFG_RING_LINE_ACTIVE_TIME);

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);

        if (CELLULAR_PKT_STATUS_OK != pktStatus)
        {
            LogError(("Cellular_SetPsmSettings: couldn't resolve ring line config"));
        }
        else
        {
            phase |= RM_PSM_PHASE_1;
        }

        if (CELLULAR_PKT_STATUS_OK == pktStatus)
        {
            (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                            "AT+SQNIPSCFG=1,%d", AWS_CELLULAR_CFG_PSM_PREPARATION_TIME);

            pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);

            if (CELLULAR_PKT_STATUS_OK != pktStatus)
            {
                LogError(("Cellular_SetPsmSettings: couldn't resolve ring line config"));
            }
            else
            {
                phase |= RM_PSM_PHASE_2;
            }
        }

        if (CELLULAR_PKT_STATUS_OK == pktStatus)
        {
            (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                            "AT+SQNPSCFG=%d", AWS_CELLULAR_CFG_PSM_WAKEUP_LATENCY);

            pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);

            if (CELLULAR_PKT_STATUS_OK != pktStatus)
            {
                LogError(("Cellular_SetPsmSettings: couldn't resolve ring line config"));
            }
        }

        if (CELLULAR_PKT_STATUS_OK == pktStatus)
        {
            p_aws_ctrl->ring_event = PlatformEventGroup_Create();
            if (NULL != p_aws_ctrl->ring_event)
            {
                phase |= RM_PSM_PHASE_3;
            }
            else
            {
                cellularStatus = CELLULAR_NO_MEMORY;
            }
        }
        else
        {
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }

        if (NULL != p_aws_ctrl->ring_event)
        {
            p_aws_ctrl->rts_semaphore = xSemaphoreCreateMutex();
            if (NULL != p_aws_ctrl->rts_semaphore)
            {
                phase |= RM_PSM_PHASE_4;
            }
            else
            {
                cellularStatus = CELLULAR_NO_MEMORY;
            }
        }

        if (NULL != p_aws_ctrl->rts_semaphore)
        {
            rtos_ret = xTaskCreate((TaskFunction_t)ring_thread,                     //cast
                                    "aws_cellular_ring_thread",
                                    (configSTACK_DEPTH_TYPE)512,                    //cast
                                    (void *)p_context,                              //cast
                                    6,
                                    (TaskHandle_t *)p_aws_ctrl->ring_taskhandle);   //cast
            if (pdFAIL != rtos_ret)
            {
                phase |= RM_PSM_PHASE_5;
            }
            else
            {
                cellularStatus = CELLULAR_NO_MEMORY;
            }
        }

        if (pdFAIL != rtos_ret)
        {
            status = aws_cellular_irq_open(p_aws_ctrl);
            if (false != status)
            {
                phase         |= RM_PSM_PHASE_6;
                cellularStatus = CELLULAR_SUCCESS;
            }
            else
            {
                cellularStatus = CELLULAR_UNKNOWN;
            }
        }

        if (true != status)
        {
            aws_cellular_psm_conf_fail(p_context, phase);
        }
    }
    else
    {
        (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                        "AT+SQNRICFG=0,3,%d", AWS_CELLULAR_CFG_RING_LINE_ACTIVE_TIME);

        pktStatus = _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);

        if (CELLULAR_PKT_STATUS_OK == pktStatus)
        {
            (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                            "AT+SQNIPSCFG=0,%d", AWS_CELLULAR_CFG_PSM_PREPARATION_TIME);

            _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);
        }

        aws_cellular_irq_close(p_aws_ctrl);

        if (NULL != p_aws_ctrl->ring_taskhandle)
        {
            vTaskSuspend((TaskHandle_t)p_aws_ctrl->ring_taskhandle);    //cast
            vTaskDelete((TaskHandle_t)p_aws_ctrl->ring_taskhandle);     //cast
            p_aws_ctrl->ring_taskhandle = NULL;
        }

        if (NULL != p_aws_ctrl->rts_semaphore)
        {
            vSemaphoreDelete(p_aws_ctrl->rts_semaphore);
            p_aws_ctrl->rts_semaphore = NULL;
        }

        if (NULL != p_aws_ctrl->ring_event)
        {
            PlatformEventGroup_Delete(p_aws_ctrl->ring_event);
            p_aws_ctrl->ring_event = NULL;
        }

        cellularStatus = CELLULAR_SUCCESS;

        p_aws_ctrl->psm_mode = 0;

#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
        aws_cellular_rts_hw_flow_enable();
#else
        aws_cellular_rts_ctrl(0);
#endif
#ifdef AWS_CELLULAR_RTS_DELAY
        Platform_Delay(AWS_CELLULAR_RTS_DELAYTIME);
#endif
    }

    return cellularStatus;
}
/**********************************************************************************************************************
 * End of function aws_cellular_psm_config
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_psm_conf_fail
 ************************************************************************************************/
static void aws_cellular_psm_conf_fail(CellularContext_t * p_context, const uint8_t phase)
{
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;   //cast

    uint8_t    cmdBuf[32] = {'\0'};

    CellularAtReq_t atReqpsmconfig =
    {
        (char *)cmdBuf, //cast
        CELLULAR_AT_NO_RESULT,
        NULL,
        NULL,
        NULL,
        0,
    };

    if ((phase & RM_PSM_PHASE_6) == RM_PSM_PHASE_6)
    {
        aws_cellular_irq_close(p_aws_ctrl);
    }

    if ((phase & RM_PSM_PHASE_5) == RM_PSM_PHASE_5)
    {
        vTaskSuspend((TaskHandle_t)p_aws_ctrl->ring_taskhandle);    //cast
        vTaskDelete((TaskHandle_t)p_aws_ctrl->ring_taskhandle);     //cast
        p_aws_ctrl->ring_taskhandle = NULL;
    }

    if ((phase & RM_PSM_PHASE_4) == RM_PSM_PHASE_4)
    {
        vSemaphoreDelete(p_aws_ctrl->rts_semaphore);
        p_aws_ctrl->rts_semaphore = NULL;
    }

    if ((phase & RM_PSM_PHASE_3) == RM_PSM_PHASE_3)
    {
        PlatformEventGroup_Delete(p_aws_ctrl->ring_event);
        p_aws_ctrl->ring_event = NULL;
    }

    if ((phase & RM_PSM_PHASE_2) == RM_PSM_PHASE_2)
    {
        (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                        "AT+SQNIPSCFG=0,%d", AWS_CELLULAR_CFG_PSM_PREPARATION_TIME);

        _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);
    }

    if ((phase & RM_PSM_PHASE_1) == RM_PSM_PHASE_1)
    {
        (void) snprintf((char *)cmdBuf, sizeof(cmdBuf),   //cast
                        "AT+SQNRICFG=0,3,%d", AWS_CELLULAR_CFG_RING_LINE_ACTIVE_TIME);

        _Cellular_AtcmdRequestWithCallback(p_context, atReqpsmconfig);
    }

    return;
}
/**********************************************************************************************************************
 * End of function aws_cellular_psm_conf_fail
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            ring_thread
 ************************************************************************************************/
static void ring_thread(void * const p_pvParameters)
{
    CellularContext_t      * p_context  = (CellularContext_t *)p_pvParameters;                  //cast
    st_aws_cellular_ctrl_t * p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;   //cast

    st_aws_cellular_timeout_ctrl_t timeout_ctrl;

    bool status = false;

    /* WAIT_LOOP */
    while (1)
    {
        if (NULL != p_aws_ctrl->ring_event)
        {
            (PlatformEventGroup_EventBits)PlatformEventGroup_WaitBits((PlatformEventGroupHandle_t )p_aws_ctrl->ring_event,
                                                                        1,
                                                                        pdTRUE,
                                                                        pdTRUE,
                                                                        portMAX_DELAY);

            aws_cellular_timeout_init(&timeout_ctrl, AWS_CELLULAR_CFG_RING_LINE_ACTIVE_TIME);
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
            aws_cellular_rts_hw_flow_enable();
#else
            aws_cellular_rts_ctrl(0);
#endif

    /* WAIT_LOOP */
            while (1)
            {
                status = aws_cellular_check_timeout(&timeout_ctrl);
                if (true == status)
                {
                    break;
                }
            }

            xSemaphoreTake((SemaphoreHandle_t)p_aws_ctrl->rts_semaphore, portMAX_DELAY);
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
            aws_cellular_rts_hw_flow_disable();
#endif
            aws_cellular_rts_ctrl(1);
            xSemaphoreGive((SemaphoreHandle_t)p_aws_ctrl->rts_semaphore);
        }
        else
        {
            Platform_Delay(1);  //cast
        }
    }
}
/**********************************************************************************************************************
 * End of function ring_thread
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_callback
 ************************************************************************************************/
static void aws_cellular_irq_callback(void * const p_Args)
{
    (void)p_Args;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult                  = pdFALSE;

    xResult = PlatformEventGroup_SetBitsFromISR(g_aws_cellular_ctrl.ring_event,
                                                1,
                                                &xHigherPriorityTaskWoken);

    if (pdTRUE == xResult)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/**********************************************************************************************************************
 * End of function aws_cellular_irq_callback
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_open
 ************************************************************************************************/
static bool aws_cellular_irq_open(st_aws_cellular_ctrl_t * const p_aws_ctrl)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return true;
#else
    irq_err_t irq_ret = IRQ_SUCCESS;
    bool      status  = true;

    irq_ret = R_IRQ_Open((irq_number_t)AWS_CELLULAR_CFG_IRQ_NUM,    //cast
                            IRQ_TRIG_FALLING,
                            IRQ_PRI_1,
                            &p_aws_ctrl->ring_irqhandle,
                            aws_cellular_irq_callback);

    if (IRQ_SUCCESS != irq_ret)
    {
        status = false;
    }
    else
    {
        R_ICU_PinSet();
    }

    return status;
#endif
}
/**********************************************************************************************************************
 * End of function aws_cellular_irq_open
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_close
 ************************************************************************************************/
static void aws_cellular_irq_close(st_aws_cellular_ctrl_t * const p_aws_ctrl)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return;
#else
    R_IRQ_Close(p_aws_ctrl->ring_irqhandle);
    return;
#endif /* #if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__) */
}
/**********************************************************************************************************************
 * End of function aws_cellular_irq_close
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_timeout_init
 ************************************************************************************************/
static void aws_cellular_timeout_init(st_aws_cellular_timeout_ctrl_t * timeout_ctrl, uint32_t timeout)
{
    timeout_ctrl->over_flg    = 0;
    timeout_ctrl->timeout_flg = 0;
    timeout_ctrl->start_time  = pdMS_TO_TICKS(xTaskGetTickCount());
    timeout_ctrl->end_time    = timeout_ctrl->start_time + timeout;

    if (timeout_ctrl->end_time < timeout_ctrl->start_time)
    {
        timeout_ctrl->over_flg = 1;
    }

    return;
}
/**********************************************************************************************************************
 * End of function aws_cellular_timeout_init
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_check_timeout
 ************************************************************************************************/
static bool aws_cellular_check_timeout(st_aws_cellular_timeout_ctrl_t * const timeout_ctrl)
{
    uint32_t this_time = pdMS_TO_TICKS(xTaskGetTickCount());
    bool     status    = false;

    if (1 == timeout_ctrl->over_flg)
    {
        if ((this_time >= timeout_ctrl->end_time) ||
                (this_time < timeout_ctrl->start_time))
        {
            status = true;
        }
    }
    else
    {
        if (((this_time > timeout_ctrl->start_time) &&
                (this_time >= timeout_ctrl->end_time)) ||
                (this_time < timeout_ctrl->start_time))
        {
            status = true;
        }
    }

    Platform_Delay(1);  //cast

    return status;
}
/**********************************************************************************************************************
 * End of function aws_cellular_check_timeout
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_ctrl
 ************************************************************************************************/
void aws_cellular_rts_ctrl(const uint8_t lowhigh)
{
    AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = lowhigh;
}
/**********************************************************************************************************************
 * End of function aws_cellular_rts_ctrl
 *********************************************************************************************************************/

#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
/*************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_hw_flow_enable
 ************************************************************************************************/
void aws_cellular_rts_hw_flow_enable(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    AWS_CELLULAR_SET_PODR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 0;
    AWS_CELLULAR_SET_PDR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN)  = 0;
    AWS_CELLULAR_SET_PFS(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN)  = AWS_CELLULAR_CFG_PFS_SET_VALUE;
    AWS_CELLULAR_SET_PMR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN)  = 1;
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}
/**********************************************************************************************************************
 * End of function aws_cellular_rts_hw_flow_enable
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_hw_flow_disable
 ************************************************************************************************/
void aws_cellular_rts_hw_flow_disable(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    AWS_CELLULAR_SET_PFS(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 0x00U;
    AWS_CELLULAR_SET_PMR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 0;
    AWS_CELLULAR_SET_PDR(AWS_CELLULAR_CFG_RTS_PORT, AWS_CELLULAR_CFG_RTS_PIN) = 1;
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}
/**********************************************************************************************************************
 * End of function aws_cellular_rts_hw_flow_disable
 *********************************************************************************************************************/
#endif  /* AWS_CELLULAR_CFG_CTS_SW_CTRL == 1 */

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_active
 ************************************************************************************************/
void aws_cellular_rts_active(CellularContext_t * p_context, BaseType_t semaphore_ret)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return;
#else
    st_aws_cellular_ctrl_t * p_aws_ctrl     = NULL;
    CellularError_t          cellularStatus = CELLULAR_SUCCESS;

    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS == cellularStatus)
    {
        p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;    //cast

        if ((1 == p_aws_ctrl->psm_mode) && (pdTRUE == semaphore_ret))
        {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
            aws_cellular_rts_hw_flow_disable();
#endif
            aws_cellular_rts_ctrl(1);
            xSemaphoreGive((SemaphoreHandle_t)p_aws_ctrl->rts_semaphore);
        }
    }

    return;
#endif
}
/**********************************************************************************************************************
 * End of function aws_cellular_rts_active
 *********************************************************************************************************************/

/*************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_deactive
 ************************************************************************************************/
BaseType_t aws_cellular_rts_deactive(CellularContext_t * p_context)
{
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    return pdTRUE;
#else
    BaseType_t               semaphore_ret  = pdFALSE;
    st_aws_cellular_ctrl_t * p_aws_ctrl     = NULL;
    CellularError_t          cellularStatus = CELLULAR_SUCCESS;

    cellularStatus = _Cellular_CheckLibraryStatus(p_context);

    if (CELLULAR_SUCCESS == cellularStatus)
    {
        p_aws_ctrl = (st_aws_cellular_ctrl_t *)p_context->pModueContext;   //cast

        if (1 == p_aws_ctrl->psm_mode)
        {
            semaphore_ret = xSemaphoreTake((SemaphoreHandle_t)p_aws_ctrl->rts_semaphore,
                                            pdMS_TO_TICKS(AWS_CELLULAR_SEMAPHORE_BLOCK_TIME));
            if (pdTRUE == semaphore_ret)
            {
#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
                aws_cellular_rts_hw_flow_enable();
#else
                aws_cellular_rts_ctrl(0);
#endif
#ifdef AWS_CELLULAR_RTS_DELAY
                Platform_Delay(AWS_CELLULAR_RTS_DELAYTIME);
#endif
            }
        }
    }

    return semaphore_ret;
#endif
}
/**********************************************************************************************************************
 * End of function aws_cellular_rts_deactive
 *********************************************************************************************************************/
