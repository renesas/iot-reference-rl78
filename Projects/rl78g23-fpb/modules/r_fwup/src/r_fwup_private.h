/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_private.h
 * Description  : Functions for using Firmware update.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.03.2023 2.00    First Release
 *         : 20.11.2023 2.01    Fixed log messages.
 *                              Add parameter checking.
 *                              Added arguments to R_FWUP_WriteImageProgram API.
 *         : 18.04.2025 2.03    V203 Release.
 *         : 27.08.2025 2.04    V204 Release.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "r_fwup_config.h"

#ifndef R_FWUP_PRIVATE_H
#define R_FWUP_PRIVATE_H

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#if (BSP_CFG_RTOS_USED == 1)
#include "FreeRTOS.h"
#include "iot_logging_task.h"
#define FWUP_PRINTF                 vLoggingPrintf
#else
#define FWUP_PRINTF                 printf
#endif

#if (FWUP_CFG_PRINTF_DISABLE == 1)
#if defined(__llvm__)
#define FWUP_LOG_ERR(...)
#define FWUP_LOG_INFO(...)
#define FWUP_LOG_DBG(...)
#else /* defined(__llvm__) */
#define FWUP_LOG_ERR(...)
#define FWUP_LOG_INFO(...)
#define FWUP_LOG_DBG(...)
#endif /* defined(__llvm__) */
#else /* (FWUP_CFG_PRINTF_DISABLE == 1) */
#if defined(__llvm__)
#define FWUP_LOG_ERR                dbg_printf
#define FWUP_LOG_INFO               dbg_printf
#define FWUP_LOG_DBG                dbg_printf
#else /* defined(__llvm__) */
#define FWUP_LOG_ERR                FWUP_PRINTF
#define FWUP_LOG_INFO               FWUP_PRINTF
#define FWUP_LOG_DBG                FWUP_PRINTF
#endif /* defined(__llvm__) */
#endif /* (FWUP_CFG_PRINTF_DISABLE == 1) */

#define FWUP_IMAGE_BLOCKS           (31)
#define FWUP_MAGIC_CODE_LEN         (7)
#define FWUP_SIG_TYPE_LENGTH        (32)
#define FWUP_SIG_SIZE_MAX           (64)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/* Firmware header */
typedef struct st_fw_header
{
    uint8_t  magic_code[FWUP_MAGIC_CODE_LEN];
    uint8_t  image_flag;
    uint8_t  sig_type[FWUP_SIG_TYPE_LENGTH];
    uint32_t sig_size;
    uint8_t  sig[FWUP_SIG_SIZE_MAX];
    uint32_t image_file_size;
    uint8_t  rsv[400];
} st_fw_header_t;

/* Firmware descriptor */
typedef struct st_fw_image
{
    uint32_t addr;
    uint32_t size;
} st_fw_image_t;

typedef struct st_fw_desc
{
    uint32_t n;
    st_fw_image_t fw[FWUP_IMAGE_BLOCKS];
    uint8_t  rsv[4];
} st_fw_desc_t;

/* Firmware control block */
typedef struct st_fw_ctrlblk
{
    st_fw_header_t head;
    st_fw_desc_t   desc;
} st_fw_ctrlblk_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* R_FWUP_PRIVATE_H */
