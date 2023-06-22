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
#if defined(R_RFD_GET_PSW_IE_STATE)
#include "r_rfd_common_control_api.h"
#endif
#include "r_rfd_code_flash_api.h"
#include "r_rfd_data_flash_api.h"

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
} e_seq_err_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
R_RFD_FAR_FUNC static uint8_t get_sequencer_status (void);
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
    return ret_value;
}
#define  R_RFD_END_SECTION_RFD_DF
#include "r_rfd_memmap.h"
/**********************************************************************************************************************
 End of function rfd_wrap_df_write
 *********************************************************************************************************************/

