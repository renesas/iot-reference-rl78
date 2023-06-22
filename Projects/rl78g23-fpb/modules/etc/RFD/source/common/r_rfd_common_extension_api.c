/**********************************************************************************************************************
    Program Name    : Renesas Flash Driver (RFD RL78 Type01)
    
    File Name       : r_rfd_common_extension_api.c
    Program Version : V1.00
    Device(s)       : RL78/G23 microcontroller
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
    
    Copyright (C) 2020-2021 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_rfd_common_extension_api.h"

/**********************************************************************************************************************
 * Function name : R_RFD_SetBootAreaImmediately
 *********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/*********************************************************************************************************************/
/**
 *  Set the specified boot cluster in the boot area immediately.
 *  
 *  @param[in]      i_e_boot_cluster : 
 *                    Boot cluster number
 *  @return         ---
 */
/*********************************************************************************************************************/
R_RFD_FAR_FUNC void R_RFD_SetBootAreaImmediately(e_rfd_boot_cluster_t i_e_boot_cluster)
{
    /* Local variable definitions */
    
    /* Specified boot cluster is boot cluster 1 */
    if (R_RFD_ENUM_BOOT_CLUSTER_1 == i_e_boot_cluster)
    {
        /* Boot cluster 1 */
        R_RFD_REG_U08_FSSET = R_RFD_VALUE_U08_FSSET_BOOT_CLUSTER_1 | g_u08_cpu_frequency;
    }
    /* Specified boot cluster is boot cluster 0 or other */
    else
    {
        /* Boot cluster 0 */
        R_RFD_REG_U08_FSSET = R_RFD_VALUE_U08_FSSET_BOOT_CLUSTER_0 | g_u08_cpu_frequency;
    }
}

#define  R_RFD_END_SECTION_RFD_CMN
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function R_RFD_SetBootAreaImmediately
 *********************************************************************************************************************/
