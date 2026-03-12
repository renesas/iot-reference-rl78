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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : rfd_wrapper.c
 * Version      : 0.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description

 *********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "rfd_wrapper.h"

#include "r_rfd_common_api.h"
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
#include "r_rfd_common_get_api.h"
#include "r_rfd_extra_area_control_api.h"
#include "r_rfd_extra_area_api.h"
#endif /* FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK */
#if defined(R_RFD_GET_PSW_IE_STATE)
#include "r_rfd_common_control_api.h"
#endif
#include "r_rfd_code_flash_api.h"
#include "r_rfd_data_flash_api.h"
#if BSP_MCU_GROUP_RL78L23
#include "r_rfd_code_flash_control_api.h"
#include "r_rfd_data_flash_control_api.h"
#endif /* BSP_MCU_GROUP_RL78L23 */

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define U8_FAR                            uint8_t __far
#define U8_NEAR                           uint8_t __near

#define FSASTL_ERER                       (0x01u)
#define FSASTL_WRER                       (0x02u)
#define FSASTL_BLER                       (0x08u)
#define FSASTL_SEQER                      (0x10u)
#define FSASTL_ESEQER                     (0x20u)
#define FSASTL_ERR_ALL                    (FSASTL_ERER|FSASTL_WRER|FSASTL_BLER|FSASTL_SEQER|FSASTL_ESEQER)

#define RFD_WRAP_VALUE_U16_MASK1_FLSEC_BTFLG  (0x0100u)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
typedef enum e_seq_error
{
    SEQ_STS_OK = 0,
    SEQ_ERR_CMD_ERASE,
    SEQ_ERR_CMD_WRITE,
    SEQ_ERR_CMD_BLANKCHECK,
    SEQ_ERR_OPEN,
	SEQ_ERR_CMD_EXTRA,
} e_seq_err_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
#if BSP_MCU_GROUP_RL78L23
R_RFD_FAR_FUNC static uint8_t get_cf_sequencer_status (void);
R_RFD_FAR_FUNC static uint8_t get_df_sequencer_status (void);
#else
R_RFD_FAR_FUNC static uint8_t get_sequencer_status (void);
#endif
static void code_copy (U8_FAR * p_rom, U8_FAR * p_rom_end, U8_NEAR * p_ram);
static uint32_t s_fclk;

/**********************************************************************************************************************
* Function Name: code_copy
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
static void code_copy(U8_FAR * p_rom, U8_FAR * p_rom_end, U8_NEAR * p_ram)
{
    for ( ; p_rom != p_rom_end; p_rom++, p_ram++)
    {
        *p_ram = *p_rom;
    }
}
/**********************************************************************************************************************
 End of function code_copy
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: rfd_wrap_code_copy
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
void rfd_wrap_code_copy(void)
{
#if defined(__CCRL__)
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
    /* ROM -> RAM */
    code_copy((U8_FAR*)__sectop("RFD_DATA_n"), (U8_FAR*)__secend("RFD_DATA_n"), (U8_NEAR*)__sectop("RFD_DATA_nR"));
    code_copy((U8_FAR*)__sectop("SMP_EX_f")  , (U8_FAR*)__secend("SMP_EX_f")  , (U8_NEAR*)__sectop("SMP_EX_fR"));
    code_copy((U8_FAR*)__sectop("RFD_EX_f")  , (U8_FAR*)__secend("RFD_EX_f")  , (U8_NEAR*)__sectop("RFD_EX_fR"));
#else /* (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK) */
    uint16_t i;
    U8_NEAR * p_ram;
    U8_NEAR* init_st_addr = (U8_NEAR*)__secend("RFD_CF_fR");

    /* ROM -> RAM */
    code_copy((U8_FAR*)__sectop("RFD_DATA_n"), (U8_FAR*)__secend("RFD_DATA_n"), (U8_NEAR*)__sectop("RFD_DATA_nR"));
    code_copy((U8_FAR*)__sectop("RFD_CMN_f") , (U8_FAR*)__secend("RFD_CMN_f") , (U8_NEAR*)__sectop("RFD_CMN_fR"));
    code_copy((U8_FAR*)__sectop("RFD_CF_f")  , (U8_FAR*)__secend("RFD_CF_f")  , (U8_NEAR*)__sectop("RFD_CF_fR"));

    /* Initialization of plus 10 bytes to avoid RAM parity check error */
    p_ram = init_st_addr;
    for (i = 0; i < 10; i++)
    {
        *p_ram = 0;
        p_ram++;
    }
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK) */
#endif /* defined__CCRL__ */
}
/**********************************************************************************************************************
 End of function rfd_wrap_code_copy
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: get_sequencer_status
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#if (COMPILER_IAR == COMPILER)
#define  R_RFD_START_SECTION_RFD_CF
#include "r_rfd_memmap.h"
#endif
uint8_t rfd_wrap_flash_open(void)
{
    s_fclk = (uint32_t)(R_BSP_GetFclkFreqHz() / 1000000U);
    rfd_wrap_code_copy();
    if (0 != HIOSTOP)
    {
        return SEQ_ERR_OPEN;
    }
    return SEQ_STS_OK;
}
#if (COMPILER_IAR == COMPILER)
#define  R_RFD_END_SECTION_RFD_CF
#include "r_rfd_memmap.h"
#endif
/**********************************************************************************************************************
 End of function rfd_wrap_code_copy
 *********************************************************************************************************************/

#if BSP_MCU_GROUP_RL78L23
/**********************************************************************************************************************
* Function Name: get_cf_sequencer_status
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC static uint8_t get_cf_sequencer_status(void)
{
    /* Local variable definition */
    uint8_t status_flag;

    /* Sequencer busy loop step1 : FSASTH */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckCFSeqEndStep1())
    {
        ; /* Do nothing */
    }

    /* Sequencer busy loop step2 : FSASTL */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckCFSeqEndStep2())
    {
        ; /* Do nothing */
    }

    /* Action error check */
    R_RFD_GetCFSeqErrorStatus(&status_flag);

    /* Clear sequencer registers */
    R_RFD_ClearCFSeqRegister();

    return (uint8_t)(status_flag & FSASTL_ERR_ALL);
}
#define  R_RFD_END_SECTION_RFD_CF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function get_cf_sequencer_status
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: get_df_sequencer_status
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC static uint8_t get_df_sequencer_status(void)
{
    /* Local variable definition */
    uint8_t status_flag;

    /* Sequencer busy loop step1 : FSASTH */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckDFSeqEndStep1())
    {
        ; /* Do nothing */
    }

    /* Sequencer busy loop step2 : FSASTL */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckDFSeqEndStep2())
    {
        ; /* Do nothing */
    }

    /* Action error check */
    R_RFD_GetDFSeqErrorStatus(&status_flag);

    /* Clear sequencer registers */
    R_RFD_ClearDFSeqRegister();

    return (uint8_t)(status_flag & FSASTL_ERR_ALL);
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function get_df_sequencer_status
 *********************************************************************************************************************/

#else
/**********************************************************************************************************************
* Function Name: get_sequencer_status
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC static uint8_t get_sequencer_status(void)
{
    /* Local variable definition */
    uint8_t status_flag;

    /* Sequencer busy loop step1 : FSASTH */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckCFDFSeqEndStep1())
    {
        ; /* Do nothing */
    }
    /* Sequencer busy loop step2 : FSASTL */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckCFDFSeqEndStep2())
    {
        ; /* Do nothing */
    }
    /* Action error check */
    R_RFD_GetSeqErrorStatus(&status_flag);

    /* Clear sequencer registers */
    R_RFD_ClearSeqRegister();
    return (uint8_t)(status_flag & FSASTL_ERR_ALL);
}
#define  R_RFD_END_SECTION_RFD_CF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function get_sequencer_status
 *********************************************************************************************************************/
#endif /* BSP_MCU_GROUP_RL78L23 */

/**********************************************************************************************************************
* Function Name: rfd_wrap_cf_erase
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_erase(uint16_t start_block, uint16_t erase_blocks)
{
    /* Local variable definition */
    uint8_t  ret_value = SEQ_STS_OK;
    uint16_t num;

    R_RFD_Init(s_fclk);
#if BSP_MCU_GROUP_RL78L23
    R_RFD_SetCFProgrammingMode();

    /* BLANKCHECK -> ERASE */
    for (num = start_block; num < (start_block + erase_blocks); num++)
    {
        /**** BLANKCHECK (1 block) ****/
        R_RFD_BlankCheckCodeFlashReq(num);
        if (SEQ_STS_OK == get_cf_sequencer_status())
        {
            continue;
        }

        /**** ERASE (1 block) ****/
        R_RFD_EraseCodeFlashReq(num);
        if (SEQ_STS_OK != get_cf_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_ERASE;
            break;
        }
    }

    R_RFD_SetCFNonProgrammableMode();
#else
    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_CODE_PROGRAMMING);

    /* BLANKCHECK -> ERASE */
    for (num = start_block; num < (start_block + erase_blocks); num++)
    {
        /**** BLANKCHECK (1 block) ****/
        R_RFD_BlankCheckCodeFlashReq(num);
        if (SEQ_STS_OK == get_sequencer_status())
        {
            continue;
        }

        /**** ERASE (1 block) ****/
        R_RFD_EraseCodeFlashReq(num);
        if (SEQ_STS_OK != get_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_ERASE;
            break;
        }
    }

    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_UNPROGRAMMABLE);
#endif /* BSP_MCU_GROUP_RL78L23 */

    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_CF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_cf_erase
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: rfd_wrap_cf_write
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_CF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_cf_write(uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data)
{
    uint8_t ret_value = SEQ_STS_OK;
    uint16_t cnt;

#if BSP_MCU_GROUP_RL78L23
    R_RFD_Init(s_fclk);
    R_RFD_SetCFProgrammingMode();

    /* write */
    for (cnt = 0u; cnt < data_length; cnt += 4u)
    {
        R_RFD_WriteCodeFlashReq(start_addr + cnt, &write_data[cnt]);
        if (SEQ_STS_OK != get_cf_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_WRITE;
            break;
        }
    }

    R_RFD_SetCFNonProgrammableMode();
#else
    R_RFD_Init(s_fclk);
    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_CODE_PROGRAMMING);

    /* write */
    for (cnt = 0u; cnt < data_length; cnt += 4u)
    {
        R_RFD_WriteCodeFlashReq(start_addr + cnt, &write_data[cnt]);
        if (SEQ_STS_OK != get_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_WRITE;
            break;
        }
    }

    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_UNPROGRAMMABLE);
#endif
    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_CF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_cf_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: rfd_wrap_df_access_enable
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC void rfd_wrap_df_access_enable(void)
{
    R_RFD_SetDataFlashAccessMode(R_RFD_ENUM_DF_ACCESS_ENABLE);
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_access_enable
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: rfd_wrap_df_access_enable
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC void rfd_wrap_df_access_disable(void)
{
    R_RFD_SetDataFlashAccessMode(R_RFD_ENUM_DF_ACCESS_DISABLE);
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_access_enable
 *********************************************************************************************************************/

#if BSP_MCU_GROUP_RL78L23
/**********************************************************************************************************************
* Function Name: rfd_wrap_df_erase
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_erase(uint32_t addr, uint32_t length, uint16_t start_block, uint16_t erase_blocks)
{
    /* Local variable definition */
    uint8_t  ret_value = SEQ_STS_OK;
    uint8_t  block_number;

    R_RFD_Init(s_fclk);
    R_RFD_SetDFProgrammingMode();

    block_number = (uint8_t)start_block;

    /**** BLANKCHECK ****/
    R_RFD_BlankCheckDataFlashReq(addr, (uint16_t)length);
    if (SEQ_STS_OK == get_df_sequencer_status())
    {
        /* BLANKCHECK -> ERASE */
        for (uint16_t num = block_number; num < (block_number + erase_blocks); num++)
        {
            /**** ERASE (1 block) ****/
            R_RFD_EraseDataFlashReq(num);
            if (SEQ_STS_OK != get_df_sequencer_status())
            {
                ret_value = SEQ_ERR_CMD_ERASE;
                break;
            }
        }
    }

    R_RFD_SetDFNonProgrammableMode();
    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_erase
 *********************************************************************************************************************/
#else
/**********************************************************************************************************************
* Function Name: rfd_wrap_df_erase
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_erase(uint16_t start_block, uint16_t erase_blocks)
{
    /* Local variable definition */
    uint8_t ret_value = SEQ_STS_OK;
    uint8_t block_number;

    R_RFD_Init(s_fclk);

    block_number = (uint8_t)start_block;
    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_DATA_PROGRAMMING);

    /* BLANKCHECK -> ERASE */
    for (uint16_t num = block_number; num < (block_number + erase_blocks); num++)
    {
        /**** BLANKCHECK (1 block) ****/
        R_RFD_BlankCheckDataFlashReq(num);
        if (SEQ_STS_OK == get_sequencer_status())
        {
            continue;
        }

        /**** ERASE (1 block) ****/
        R_RFD_EraseDataFlashReq(num);
        if (SEQ_STS_OK != get_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_ERASE;
            break;
        }
    }

    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_UNPROGRAMMABLE);
    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_erase
 *********************************************************************************************************************/
#endif /* BSP_MCU_GROUP_RL78L23 */

/**********************************************************************************************************************
* Function Name: rfd_wrap_df_write
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  R_RFD_START_SECTION_RFD_DF
#include "r_rfd_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_df_write(uint32_t start_addr, uint16_t data_length, uint8_t __near *write_data)
{
    /* Local variable definition */
    uint8_t  ret_value = SEQ_STS_OK;
    uint16_t cnt;

    R_RFD_Init(s_fclk);
#if BSP_MCU_GROUP_RL78L23
    R_RFD_SetDFProgrammingMode();

    /* write */
    for (cnt = 0u; cnt < data_length; cnt++)
    {
        R_RFD_WriteDataFlashReq(start_addr + cnt, &write_data[cnt]);
        if (SEQ_STS_OK != get_df_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_WRITE;
            break;
        }
    }

    R_RFD_SetDFNonProgrammableMode();
#else
    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_DATA_PROGRAMMING);

    /* write */
    for (cnt = 0u; cnt < data_length; cnt++)
    {
        R_RFD_WriteDataFlashReq(start_addr + cnt, &write_data[cnt]);
        if (SEQ_STS_OK != get_sequencer_status())
        {
            ret_value = SEQ_ERR_CMD_WRITE;
            break;
        }
    }

    R_RFD_SetFlashMemoryMode(R_RFD_ENUM_FLASH_MODE_UNPROGRAMMABLE);
#endif
    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_write
 *********************************************************************************************************************/

#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
/**********************************************************************************************************************
* Function Name: rfd_wrap_CheckExtraSeqEnd
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  SAMPLE_START_SECTION_SMP_EX
#include "sample_memmap.h"
R_RFD_FAR_FUNC e_sample_ret_t rfd_wrap_CheckExtraSeqEnd(void)
{
    /* Local variable definition */
    e_sample_ret_t l_e_sam_ret_value;
    uint8_t        l_u08_status_flag;

    /* Set local variables */
    l_e_sam_ret_value = SAMPLE_ENUM_RET_STS_OK;

    /* Sequencer busy loop step1 */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckExtraSeqEndStep1())
    {
        /* No operation */
        /* It is possible to write the program for detecting timeout here as necessity requires */
    }

    /* Sequencer busy loop step2 */
    while (R_RFD_ENUM_RET_STS_BUSY == R_RFD_CheckExtraSeqEndStep2())
    {
        /* No operation */
        /* It is possible to write the program for detecting timeout here as necessity requires */
    }

    /* Action error check */
    R_RFD_GetExtraSeqErrorStatus(&l_u08_status_flag);

    /* Check error status */
    if (SAMPLE_VALUE_U08_MASK0_8BIT != (l_u08_status_flag & SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_EXTRA_SEQUENCER))
    {
        /* Set return value */
        l_e_sam_ret_value = SAMPLE_ENUM_RET_ERR_EXTRA_SEQUENCER;
    }
    else if (SAMPLE_VALUE_U08_MASK0_8BIT != (l_u08_status_flag & SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_ERASE))
    {
        /* Set return value */
        l_e_sam_ret_value = SAMPLE_ENUM_RET_ERR_ACT_ERASE;
    }
    else if (SAMPLE_VALUE_U08_MASK0_8BIT != (l_u08_status_flag & SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_WRITE))
    {
        /* Set return value */
        l_e_sam_ret_value = SAMPLE_ENUM_RET_ERR_ACT_WRITE;
    }
    else if (SAMPLE_VALUE_U08_MASK0_8BIT != (l_u08_status_flag & SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_BLANKCHECK))
    {
        /* Set return value */
        l_e_sam_ret_value = SAMPLE_ENUM_RET_ERR_ACT_BLANKCHECK;
    }
    else
    {
        /* Set return value */
        l_e_sam_ret_value = SAMPLE_ENUM_RET_STS_OK;
    }

    /* Clear sequencer registers */
    R_RFD_ClearExtraSeqRegister();

    return (l_e_sam_ret_value);
}
#define  SAMPLE_END_SECTION_SMP_EX
#include "sample_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_CheckExtraSeqEnd
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: rfd_wrap_bank_swap
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
#define  SAMPLE_START_SECTION_SMP_EX
#include "sample_memmap.h"
R_RFD_FAR_FUNC uint8_t rfd_wrap_bank_swap(void)
{
    e_rfd_ret_t             l_e_rfd_ret_status;
    e_rfd_boot_cluster_t    l_e_boot_cluster;
    e_sample_ret_t          l_e_sam_ret_status;
    e_sample_ret_t          l_e_sam_ret_value;
    bool                    l_e_sam_error_flag;
    uint16_t                l_u16_security_and_boot_flags;

    l_e_sam_ret_value   = SAMPLE_ENUM_RET_STS_OK;
    l_e_sam_error_flag  = false;


    /* Check current boot flag condition */
    /* This command should execute in non-programmable mode */
    R_RFD_GetSecurityAndBootFlags(&l_u16_security_and_boot_flags);

    if (0x0000u != (l_u16_security_and_boot_flags & RFD_WRAP_VALUE_U16_MASK1_FLSEC_BTFLG))
    {
        /* Current boot flag is the boot cluster 0, then the boot cluster 1 will be swapped boot cluster */
        l_e_boot_cluster = R_RFD_ENUM_BOOT_CLUSTER_1;
    }
    else
    {
        /* Current boot flag is the boot cluster 1, then the boot cluster 0 will be swapped boot cluster */
        l_e_boot_cluster = R_RFD_ENUM_BOOT_CLUSTER_0;
    }

    /******************************************************************************************************************
     * Set the code flash (extra area) programming mode
     *****************************************************************************************************************/
    l_e_rfd_ret_status = R_RFD_SetExtraProgrammingMode();

    if (l_e_rfd_ret_status != R_RFD_ENUM_RET_STS_OK)
    {
        l_e_sam_error_flag = true;
        l_e_sam_ret_value  = SAMPLE_ENUM_RET_ERR_MODE_MISMATCHED;
    }
    else
    {
        /* No operation */
    }

    /******************************************************************************************************************
     * Set boot flag
     *****************************************************************************************************************/
    if (false == l_e_sam_error_flag)
    {
        R_RFD_SetExtraBootAreaReq(l_e_boot_cluster);
        l_e_sam_ret_status =  rfd_wrap_CheckExtraSeqEnd();

        if (SAMPLE_ENUM_RET_STS_OK != l_e_sam_ret_status)
        {
            l_e_sam_error_flag = true;
            l_e_sam_ret_value  = SAMPLE_ENUM_RET_ERR_CMD_SET_EXTRA_AREA;
        }
        else
        {
            /* No operation */
        }
    }
    else /* true == l_e_sam_error_flag */
    {
        /* No operation */
    }

    /******************************************************************************************************************
     * Set non-programmable mode
     *****************************************************************************************************************/
    l_e_rfd_ret_status = R_RFD_SetExtraNonProgrammableMode();

    if (R_RFD_ENUM_RET_STS_OK != l_e_rfd_ret_status)
    {
        l_e_sam_error_flag = true;
        l_e_sam_ret_value  = SAMPLE_ENUM_RET_ERR_MODE_MISMATCHED;
    }
    else
    {
        /* No operation */
    }

    return (l_e_sam_ret_value);
}
#define  SAMPLE_END_SECTION_SMP_EX
#include "sample_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_bank_swap
 *********************************************************************************************************************/
#endif /* FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK */