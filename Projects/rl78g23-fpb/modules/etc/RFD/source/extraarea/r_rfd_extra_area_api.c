/**********************************************************************************************************************
    Program Name    : Renesas Flash Driver (RFD RL78 Type01)
    
    File Name       : r_rfd_extra_area_api.c
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
#include "r_rfd_extra_area_api.h"

/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraBootAreaReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the boot flag.
 *  
 *  @param[in]      i_e_boot_cluster : 
 *                    Boot cluster number
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraBootAreaReq(e_rfd_boot_cluster_t i_e_boot_cluster)
{
    /* Local variable definitions */
    uint8_t  l_u08_fsset_value;
    uint16_t l_u16_btflg_value;
    
    /* Set local variables */
    l_u08_fsset_value = R_RFD_REG_U08_FSSET;
    l_u16_btflg_value = R_RFD_REG_U16_FLSEC & R_RFD_VALUE_U16_MASK1_BOOT_FLAG;
    
    /* TMSPMD bit is invalid */
    if (0u == (l_u08_fsset_value & R_RFD_VALUE_U08_MASK1_FSSET_TMSPMD))
    {
        /* BTFLG bit is invalid */
        if (0u == l_u16_btflg_value)
        {
            /* Set boot cluster 1 immediately */
            R_RFD_REG_U08_FSSET = R_RFD_VALUE_U08_MASK1_FSSET_TMSPMD_AND_TMBTSEL | l_u08_fsset_value;
        }
        /* BTFLG bit is valid */
        else
        {
            /* Set boot cluster 0 immediately */
            R_RFD_REG_U08_FSSET = R_RFD_VALUE_U08_MASK1_FSSET_TMSPMD 
                                  | (R_RFD_VALUE_U08_MASK0_FSSET_TMSPMD_AND_TMBTSEL & l_u08_fsset_value);
        }
    }
    /* TMSPMD bit is valid */
    else
    {
        /* No operation */
    }
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Parameter is boot cluster 1 */
    if (R_RFD_ENUM_BOOT_CLUSTER_1 == i_e_boot_cluster)
    {
        /* Set the value for FLWL register */
        R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK0_BOOT_FLAG;
    }
    /* Parameter is boot cluster 0 or other */
    else
    {
        /* Set the value for FLWL register */
        R_RFD_REG_U16_FLWL = R_RFD_VALUE_U16_MASK1_16BIT;
    }
    
    /* Set the value for FLWH register */
    R_RFD_REG_U16_FLWH = R_RFD_VALUE_U16_MASK1_16BIT;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_SECURITY_FLAG;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraBootAreaReq
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function name : R_RFD_SetExtraFSWReq
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  This command starts the extra sequencer.
 *  Start the extra sequencer and write the start block number, end block number, and flash shield window mode.
 *  
 *  @param[in]      i_u16_start_block_number : 
 *                    Start block number
 *  @param[in]      i_u16_end_block_number : 
 *                    End block number (points one block past the end of range)
 *  @param[in]      i_e_fsw_mode : 
 *                    Flash shield window mode
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetExtraFSWReq(uint16_t i_u16_start_block_number, 
                                         uint16_t i_u16_end_block_number, 
                                         e_rfd_fsw_mode_t i_e_fsw_mode)
{
    /* Local variable definitions */
    uint16_t l_u16_flwl_value;
    uint16_t l_u16_flwh_value;
    
    /* Set local variables */
    l_u16_flwl_value = i_u16_start_block_number | R_RFD_VALUE_U16_MASK1_FSW_EXCEPT_BLOCK_INFO;
    l_u16_flwh_value = i_u16_end_block_number | R_RFD_VALUE_U16_MASK1_FSW_EXCEPT_BLOCK_INFO;
    
    /* Set the value for FLARS register */
    R_RFD_REG_U08_FLARS = R_RFD_VALUE_U08_FLARS_EXTRA_AREA;
    
    /* Flash shield window mode is inside shield mode */
    if (R_RFD_ENUM_FSW_MODE_INSIDE == i_e_fsw_mode)
    {
        l_u16_flwh_value = l_u16_flwh_value & R_RFD_VALUE_U16_MASK0_FSW_CONTROL_FLAG;
    }
    /* Flash shield window mode is outside shield mode */
    else
    {
        /* No operation */
    }
    
    /* Set the value for FLWH/L register */
    R_RFD_REG_U16_FLWL = l_u16_flwl_value;
    R_RFD_REG_U16_FLWH = l_u16_flwh_value;
    
    /* Set the value for FSSE register */
    R_RFD_REG_U08_FSSE = R_RFD_VALUE_U08_FSSE_FSW;
}

#define  R_RFD_END_SECTION_RFD_EX
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetExtraFSWReq
 *********************************************************************************************************************/
