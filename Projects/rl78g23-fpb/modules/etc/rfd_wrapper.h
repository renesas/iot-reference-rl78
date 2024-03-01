/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : rfd_wrapper.h
 * Description  : Functions for the wrapping Renesas Flash Driver(RFD).
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 27.12.2023 2.00    First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"
#include "r_rfd.h"
#include "r_rfd_types.h"

#ifndef RFD_WRAPPER_HEADER_FILE
#define RFD_WRAPPER_HEADER_FILE

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void rfd_wrap_code_copy(void);
uint8_t rfd_wrap_flash_open (void);
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_erase (uint16_t start_block_number, uint16_t erase_blocks);
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_write (uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data);
R_RFD_FAR_FUNC void    rfd_wrap_df_access_enable (void);
R_RFD_FAR_FUNC void    rfd_wrap_df_access_disable (void);
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_erase (uint16_t start_block_number, uint16_t erase_blocks);
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_write (uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data);

#endif /* RFD_WRAPPER_HEADER_FILE */
