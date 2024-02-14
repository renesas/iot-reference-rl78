/*
 * rfd_wrapper.h
 * Copyright (C) Renesas Electronics Corporation and/or its affiliates.
 * All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
