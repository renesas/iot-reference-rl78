/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
* File Name     : r_fwup_config.h
* Description   : Configures the Firmware update module.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.03.2023 2.00    First Release
*         : 20.11.2023 2.01    Added support RL78/G24 
*                              Fixed log messages.
*                              Add parameter checking.
*                              Added arguments to R_FWUP_WriteImageProgram API.
*         : 18.04.2025 2.03    V203 Release.
*         : 27.08.2025 2.04    V204 Release.
***********************************************************************************************************************/
#include "platform.h"

#ifndef R_FWUP_CONFIG_H
#define R_FWUP_CONFIG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Select the update mode.
    0 = Dual bank
    1 = Single bank with buffer. (default)
    2 = Single bank without buffer.
    3 = Single bank with ext-buffer.
*/
#define FWUP_CFG_UPDATE_MODE                        (1)

/* Select the function mode.
    0 = use for Boot loader
    1 = use for User program
*/
#define FWUP_CFG_FUNCTION_MODE                      (0)

/* Area configuration */
#if (FWUP_CFG_UPDATE_MODE != 2)
#define FWUP_CFG_MAIN_AREA_ADDR_L                   (0x01000U)      /* Main area   start address   */
#define FWUP_CFG_BUF_AREA_ADDR_L                    (0x59000U)      /* Buffer area start address   */
#define FWUP_CFG_AREA_SIZE                          (0x58000U)      /* Install area size           */
#else
#define FWUP_CFG_MAIN_AREA_ADDR_L                   (0x01000U)      /* Main area   start address   */
#define FWUP_CFG_BUF_AREA_ADDR_L                    (0x01000U)      /* Buffer area start address   */
#define FWUP_CFG_AREA_SIZE                          (0xB0000U)      /* Install area size           */
#endif

/* Internal flash */
#define FWUP_CFG_CF_BLK_SIZE                        (2048U)         /* Code flash block size       */
#define FWUP_CFG_CF_W_UNIT_SIZE                     (128U)          /* Code flash write unit size  */

/* External flash */
#define FWUP_CFG_EXT_BUF_AREA_ADDR_L                (0x00000U)      /* External Buffer area Start address */
#define FWUP_CFG_EXT_BUF_AREA_BLK_SIZE              (4096U)         /* Block(Sector) size  */

/* Data flash */
#define FWUP_CFG_DF_ADDR_L                          (0xF1000U)      /* DF Start address    */
#define FWUP_CFG_DF_BLK_SIZE                        (256U)          /* DF Block size       */
#define FWUP_CFG_DF_NUM_BLKS                        (32U)           /* DF number of blocks */
/* FWUP v1 compatible */
#define FWUP_CFG_FWUPV1_COMPATIBLE                  (0)                /* 0:Disable, 1:Enable */

/* Select the algorithm of signature verification.
    0 = ECDSA. (default)
    1 = SHA256
*/
#define FWUP_CFG_SIGNATURE_VERIFICATION             (0)

/* Disable Printf Output Setting.
    Disables the character output by printf to the terminal software.
    0 = Enable
    1 = Disable (default)
 */
#define FWUP_CFG_PRINTF_DISABLE                     (0)

#endif /* R_FWUP_CONFIG_H */
