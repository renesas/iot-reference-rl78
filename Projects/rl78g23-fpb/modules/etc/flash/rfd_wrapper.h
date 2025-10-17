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
 * File Name    : rfd_wrapper.h
 * Version      :
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"
#include "r_fwup_config.h"
#include "r_rfd.h"
#include "r_rfd_types.h"

#ifndef RFD_WRAPPER_HEADER_FILE
#define RFD_WRAPPER_HEADER_FILE

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_CFDF_SEQUENCER  (0x10u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_EXTRA_SEQUENCER (0x20u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_ERASE           (0x01u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_WRITE           (0x02u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_BLANKCHECK      (0x08u)
#define SAMPLE_VALUE_U08_MASK0_8BIT                           (0x00u)
#define SAMPLE_VALUE_U08_MASK1_8BIT                           (0xFFu)
#endif /* FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
typedef enum e_sample_ret
{
    SAMPLE_ENUM_RET_STS_OK                 = 0x00u,
    SAMPLE_ENUM_RET_ERR_PARAMETER          = 0x10u,
    SAMPLE_ENUM_RET_ERR_CONFIGURATION      = 0x11u,
    SAMPLE_ENUM_RET_ERR_MODE_MISMATCHED    = 0x12u,
    SAMPLE_ENUM_RET_ERR_CHECK_WRITE_DATA   = 0x13u,
    SAMPLE_ENUM_RET_ERR_CFDF_SEQUENCER     = 0x20u,
    SAMPLE_ENUM_RET_ERR_EXTRA_SEQUENCER    = 0x21u,
    SAMPLE_ENUM_RET_ERR_ACT_ERASE          = 0x22u,
    SAMPLE_ENUM_RET_ERR_ACT_WRITE          = 0x23u,
    SAMPLE_ENUM_RET_ERR_ACT_BLANKCHECK     = 0x24u,
    SAMPLE_ENUM_RET_ERR_CMD_ERASE          = 0x30u,
    SAMPLE_ENUM_RET_ERR_CMD_WRITE          = 0x31u,
    SAMPLE_ENUM_RET_ERR_CMD_BLANKCHECK     = 0x32u,
    SAMPLE_ENUM_RET_ERR_CMD_SET_EXTRA_AREA = 0x33u,
} e_sample_ret_t;
#endif /* FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK */

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void rfd_wrap_code_copy(void);
uint8_t rfd_wrap_flash_open (void);
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_erase (uint16_t start_block_number, uint16_t erase_blocks);
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_write (uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data);
R_RFD_FAR_FUNC void    rfd_wrap_df_access_enable (void);
R_RFD_FAR_FUNC void    rfd_wrap_df_access_disable (void);
#if BSP_MCU_GROUP_RL78L23
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_erase (uint32_t addr, uint32_t length, uint16_t start_block_number, uint16_t erase_blocks);
#else
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_erase (uint16_t start_block_number, uint16_t erase_blocks);
#endif
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_write (uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data);
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
R_RFD_FAR_FUNC uint8_t rfd_wrap_bank_swap(void);
R_RFD_FAR_FUNC e_sample_ret_t rfd_wrap_CheckExtraSeqEnd(void);
#endif /* FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK */
#endif /* RFD_WRAPPER_HEADER_FILE */
