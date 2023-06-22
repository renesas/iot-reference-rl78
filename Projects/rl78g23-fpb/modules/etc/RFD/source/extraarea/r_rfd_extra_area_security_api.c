/**********************************************************************************************************************
    Program Name    : Renesas Flash Driver (RFD RL78 Type01)
    
    File Name       : r_rfd_extra_area_security_api.c
    Program Version : V1.00
    Device(s)       : RL78/G23 microcontroller
    Description     : Extra Area Control program
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
    
    Copyright (C) 2020-2021 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_rfd_extra_area_security_api.h"

/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraEraseProtectReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the block erase protect flag.
 *  
 *  @param[in]      -
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraEraseProtectReq(void)
{
    /* Local variable definitions */
    uint16_t l_u16_flsec_value;
    
    /* Set local variables */
    l_u16_flsec_value = R_RFD_REG_U16_FLSEC;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Set the value for FLWH/L register */
    /* Keep the boot flag and set the appropriate protect flag */
    R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK0_ERASE_PROTECT_FLAG 
                         & (l_u16_flsec_value | R_RFD_VALUE_U16_MASK0_BOOT_FLAG);
    R_RFD_REG_U16_FLWH = R_RFD_VALUE_U16_MASK1_16BIT;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_SECURITY_FLAG;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraEraseProtectReq
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraWriteProtectReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the write protect flag.
 *  
 *  @param[in]      -
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraWriteProtectReq(void)
{
    /* Local variable definitions */
    uint16_t l_u16_flsec_value;
    
    /* Set local variables */
    l_u16_flsec_value = R_RFD_REG_U16_FLSEC;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Set the value for FLWH/L register */
    /* Keep the boot flag and set the appropriate protect flag */
    R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK0_WRITE_PROTECT_FLAG 
                         & (l_u16_flsec_value | R_RFD_VALUE_U16_MASK0_BOOT_FLAG);
    R_RFD_REG_U16_FLWH = R_RFD_VALUE_U16_MASK1_16BIT;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_SECURITY_FLAG;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraWriteProtectReq
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraBootAreaProtectReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the boot area protect flag.
 *  
 *  @param[in]      -
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraBootAreaProtectReq(void)
{
    /* Local variable definitions */
    uint16_t l_u16_flsec_value;
    
    /* Set local variables */
    l_u16_flsec_value = R_RFD_REG_U16_FLSEC;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Set the value for FLWH/L register */
    /* Keep the boot flag and set the appropriate protect flag */
    R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK0_BOOT_CLUSTER_PROTECT_FLAG 
                         & (l_u16_flsec_value | R_RFD_VALUE_U16_MASK0_BOOT_FLAG);
    R_RFD_REG_U16_FLWH = R_RFD_VALUE_U16_MASK1_16BIT;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_SECURITY_FLAG;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraBootAreaProtectReq
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraFSWProtectReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the flash shield window protect flag.
 *  
 *  @param[in]      -
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraFSWProtectReq(void)
{
    /* Local variable definitions */
    uint16_t l_u16_flfsws_value;
    uint16_t l_u16_flfswe_value;
    
    /* Set local variables */
    l_u16_flfsws_value = R_RFD_REG_U16_FLFSWS;
    l_u16_flfswe_value = R_RFD_REG_U16_FLFSWE;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Set the value for FLWH/L register */
    R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK0_FSW_PROTECT_FLAG 
                         & (l_u16_flfsws_value | R_RFD_VALUE_U16_MASK1_FSW_EXCEPT_BLOCK_INFO);
    R_RFD_REG_U16_FLWH = l_u16_flfswe_value 
                         | (R_RFD_VALUE_U16_MASK0_FSW_CONTROL_FLAG & R_RFD_VALUE_U16_MASK1_FSW_EXCEPT_BLOCK_INFO);
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_FSW;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraFSWProtectReq
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraSoftwareReadProtectAreaReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the software read protect area and set protect flag.
 *  
 *  @param[in]      i_u16_start_block_number : 
 *                    Start block number
 *  @param[in]      i_u16_end_block_number : 
 *                    End block number
 *  @param[in]      i_e_protect_flag : 
 *                    Software read protect security flag 
 *                    (If valid, it becomes unable to rewrite the software read protect area by this function.)
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraSoftwareReadProtectAreaReq(uint16_t i_u16_start_block_number, 
                                                             uint16_t i_u16_end_block_number, 
                                                             e_rfd_protect_t i_e_protect_flag)
{
    /* Local variable definitions */
    uint16_t l_u16_flwl_value;
    uint16_t l_u16_flwh_value;
    
    /* Set local variables */
    l_u16_flwl_value = i_u16_start_block_number | R_RFD_VALUE_U16_MASK1_SW_READ_EXCEPT_BLOCK_INFO;
    l_u16_flwh_value = i_u16_end_block_number | R_RFD_VALUE_U16_MASK1_SW_READ_EXCEPT_BLOCK_INFO;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Software read protect security flag is valid */
    if (R_RFD_ENUM_PROTECT_ON == i_e_protect_flag)
    {
        /* Set the value for FLWH/L register */
        l_u16_flwh_value = l_u16_flwh_value & R_RFD_VALUE_U16_MASK0_SW_READ_PROTECT_FLAG;
    }
    /* Software read protect security flag is invalid */
    else
    {
        /* No operation */
    }
    
    /* Set the value for FLWH/L register */
    R_RFD_REG_U16_FLWL = l_u16_flwl_value;
    R_RFD_REG_U16_FLWH = l_u16_flwh_value;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_SOFTWARE_READ;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraSoftwareReadProtectAreaReq
 *********************************************************************************************************************/
