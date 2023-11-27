/**********************************************************************************************************************
    Program Name    : Renesas Flash Driver (RFD RL78 Type01)
    
    File Name       : r_rfd_common_get_api.c
    Program Version : V1.20
    Device(s)       : RL78/G2x microcontroller
    Description     : Common Flash Control program
**********************************************************************************************************************/

/**********************************************************************************************************************
    DISCLAIMER
    This software is supplied by Renesas Electronics Corporation and is only intended for use with
    Renesas products. No other uses are authorized. This software is owned by Renesas Electronics
    Corporation and is protected under all applicable laws, including copyright laws.
    THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE,
    WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
    TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR
    ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR
    CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
    BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
    Renesas reserves the right, without notice, to make changes to this software and to discontinue the
    availability of this software. By using this software, you agree to the additional terms and conditions
    found by accessing the following link:
    http://www.renesas.com/disclaimer
    
    Copyright (C) 2020-2023 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_rfd_common_get_api.h"

/**********************************************************************************************************************
 * Function name : R_RFD_GetSecurityAndBootFlags
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  Get the information about security flags and boot flag.
 *  
 *  @param[in]      onp_u16_security_and_boot_flags : 
 *                    Pointer to the variable that stores security flags and boot flag information
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_GetSecurityAndBootFlags(uint16_t __near * onp_u16_security_and_boot_flags)
{
    /* Local variable definitions */
    uint16_t l_u16_flsec_value;
    
    /* Set local variables */
    l_u16_flsec_value = R_RFD_REG_U16_FLSEC;
    
    /* Write to parameter */
    * onp_u16_security_and_boot_flags = l_u16_flsec_value;
}

#define  R_RFD_END_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_GetSecurityAndBootFlags
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_GetFSW
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  Get the start block, end block, flash shield window mode, and protect flag of the flash shield window.
 *  
 *  @param[out]     onp_u16_start_block_number : 
 *                    Pointer to a variable that stores the start block number
 *  @param[out]     onp_u16_end_block_number : 
 *                    Pointer to a variable that stores the end block number
 *  @param[out]     onp_e_fsw_mode : 
 *                    Pointer to a variable that stores the flash shield window mode
 *  @param[out]     onp_e_protect_flag : 
 *                    Pointer to a variable that stores the protect flag
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_GetFSW(uint16_t __near * onp_u16_start_block_number, 
                                 uint16_t __near * onp_u16_end_block_number, 
                                 e_rfd_fsw_mode_t __near * onp_e_fsw_mode, 
                                 e_rfd_protect_t __near * onp_e_protect_flag)
{
    /* Local variable definitions */
    uint16_t l_u16_flfsws_value;
    uint16_t l_u16_flfswe_value;
    
    /* Set local variables */
    l_u16_flfsws_value = R_RFD_REG_U16_FLFSWS;
    l_u16_flfswe_value = R_RFD_REG_U16_FLFSWE;
    
    /* Write to parameter */
    * onp_u16_start_block_number = l_u16_flfsws_value & R_RFD_VALUE_U16_MASK1_FLFSW_BLOCK_NUMBER;
    * onp_u16_end_block_number   = l_u16_flfswe_value & R_RFD_VALUE_U16_MASK1_FLFSW_BLOCK_NUMBER;
    
    if (0u == (l_u16_flfswe_value & R_RFD_VALUE_U16_MASK1_FLFSWE_FSWC))
    {
        * onp_e_fsw_mode = R_RFD_ENUM_FSW_MODE_INSIDE;
    }
    else
    {
        * onp_e_fsw_mode = R_RFD_ENUM_FSW_MODE_OUTSIDE;
    }
    if (0u == (l_u16_flfsws_value & R_RFD_VALUE_U16_MASK1_FLFSWS_FSPR))
    {
        * onp_e_protect_flag = R_RFD_ENUM_PROTECT_ON;
    }
    else
    {
        * onp_e_protect_flag = R_RFD_ENUM_PROTECT_OFF;
    }
}

#define  R_RFD_END_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_GetFSW
 *********************************************************************************************************************/
