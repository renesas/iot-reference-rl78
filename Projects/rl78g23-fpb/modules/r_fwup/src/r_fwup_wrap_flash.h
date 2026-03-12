/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_wrap_flash.h
 * Description  : Functions for the Firmware update module.
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
/**** Start user code ****/

#if defined(__RX)
#else
#include "rfd_wrapper.h"
#if (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT)
#include "r_nor_flash_rl78_if.h"
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT) */
#endif /* defined(__RX) */

/**** End user code   ****/


/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef R_FWUP_WRAP_FLASH_H_
#define R_FWUP_WRAP_FLASH_H_

/* Flash functions */
#if (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT)
#define FWUP_FUNC_BUFFER_AREA_ERASE               r_fwup_wrap_ext_flash_erase
#define FWUP_FUNC_BUFFER_AREA_WRITE               r_fwup_wrap_ext_flash_write
#define FWUP_FUNC_BUFFER_AREA_READ                r_fwup_wrap_ext_flash_read
#else
#define FWUP_FUNC_BUFFER_AREA_ERASE               r_fwup_wrap_flash_erase
#define FWUP_FUNC_BUFFER_AREA_WRITE               r_fwup_wrap_flash_write
#define FWUP_FUNC_BUFFER_AREA_READ                r_fwup_wrap_flash_read
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT) */

/**** Start user code ****/
/**** End user code   ****/


/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_flash_open  (void);
void         r_fwup_wrap_flash_close (void);
e_fwup_err_t r_fwup_wrap_flash_erase (uint32_t addr, uint32_t num_blocks);
e_fwup_err_t r_fwup_wrap_flash_write (uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes);
e_fwup_err_t r_fwup_wrap_flash_read  (uint32_t buf_addr, uint32_t src_addr, uint32_t size);
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
e_fwup_err_t r_fwup_wrap_bank_swap   (void);
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK) */

#if (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT)
e_fwup_err_t r_fwup_wrap_ext_flash_open  (void);
void         r_fwup_wrap_ext_flash_close (void);
e_fwup_err_t r_fwup_wrap_ext_flash_erase (uint32_t offsetadd, uint32_t num_sectors);
e_fwup_err_t r_fwup_wrap_ext_flash_write (uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes);
e_fwup_err_t r_fwup_wrap_ext_flash_read  (uint32_t buf_addr, uint32_t src_addr, uint32_t size);
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT) */

#endif /* R_FWUP_WRAP_FLASH_H_ */
