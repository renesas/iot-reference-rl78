/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_wrap_flash.c
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
#include "r_fwup_if.h"
#include "r_fwup_wrap_com.h"
#include "r_fwup_wrap_flash.h"

/**** Start user code ****/
#if defined(__RX)
#include "r_flash_rx_if.h"
#else
#include "r_rfd_common_api.h"
#include "r_rfd_data_flash_api.h"
#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
#include "r_rfd_common_get_api.h"
#include "r_rfd_extra_area_control_api.h"
#include "r_rfd_extra_area_api.h"
#include "r_rfd_common_control_api.h"
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK) */
#endif /* defined(__RX) */
/**** End user code   ****/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/
/* Sector Erase busy timeout 200*1ms = 0.2s  */
#define FLASH_SE_BUSY_WAIT      (200UL)
/* Chip Erase busy timeout 600,000*1ms = 600s */
#define FLASH_CE_BUSY_WAIT      (600000UL)
/* Page Program time out 3*1ms = 3ms */
#define FLASH_PP_BUSY_WAIT      (3UL)

#define FLASH_PAGE_SIZE         (256)
#define FLASH_TEST_BUF_SIZE     (FLASH_PAGE_SIZE * 2)
#define TIMER_CH_FLG            (0)
#define TIMER_CH_COUNT          (1)
#define TIMER_CH_MAX_COUNT      (2)

#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
#define SAMPLE_VALUE_U16_MASK1_FLSEC_BTFLG                    (0x0100u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_CFDF_SEQUENCER  (0x10u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_EXTRA_SEQUENCER (0x20u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_ERASE           (0x01u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_WRITE           (0x02u)
#define SAMPLE_VALUE_U08_MASK1_FSQ_STATUS_ERR_BLANKCHECK      (0x08u)
#define SAMPLE_VALUE_U08_MASK0_8BIT                           (0x00u)
#define SAMPLE_VALUE_U08_MASK1_8BIT                           (0xFFu)
#endif

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/**** Start user code ****/
volatile uint32_t   g_timer_cnt[TIMER_CH_MAX_COUNT+1];
uint32_t            g_cbuf1[FLASH_TEST_BUF_SIZE/sizeof(uint32_t)];
uint32_t            g_cbuf2[FLASH_TEST_BUF_SIZE/sizeof(uint32_t)];
uint32_t            g_addr;
uint8_t             g_config[4];
uint8_t             g_id[4];
uint8_t             g_stat;
uint8_t             g_scur;
/**** End user code   ****/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/**** Start user code ****/
#if (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT)
static e_nor_flash_status_t ext_flash_busy_check (e_nor_flash_check_busy_t mode, uint32_t loop_cnt);
#endif
/**** End user code   ****/

/*
 * Internal flash
 */
/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_flash_open
 * Description  : wrapper function for initializing Flash module.
 * Arguments    : None
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash open error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_flash_open(void)
{
    /**** Start user code ****/
#if defined(__RX)
    if (FLASH_SUCCESS != R_FLASH_Open())
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#else
    if (0 != rfd_wrap_flash_open())
    {
        return (FWUP_ERR_FLASH);
    }

    /* Data flash access enable */
    if (0 < FWUP_CFG_DF_NUM_BLKS)
    {
        rfd_wrap_df_access_enable();
    }
    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_flash_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_flash_close
 * Description  : wrapper function for closing Flash module.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void r_fwup_wrap_flash_close(void)
{
    /**** Start user code ****/
#if defined(__RX)
    R_FLASH_Close();
#else
    /* Data flash access enable */
    if (0 < FWUP_CFG_DF_NUM_BLKS)
    {
        rfd_wrap_df_access_disable();
    }
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_flash_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_flash_erase
 * Description  : wrapper function for erasing Flash.
 * Arguments    : addr           : erasure destination address
 *                num_blocks     : number of blocks to erase
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash erase error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_flash_erase(uint32_t addr, uint32_t num_blocks)
{
    /**** Start user code ****/
#if defined(__RX)
    uint32_t blk_addr;

    blk_addr = (addr + (FWUP_CFG_CF_BLK_SIZE * num_blocks) - 1) & 0xffffc000;
    r_fwup_wrap_disable_interrupt();
    flash_err_t ret = R_FLASH_Erase((flash_block_address_t )blk_addr, num_blocks);
    r_fwup_wrap_enable_interrupt();
    if (FLASH_SUCCESS != ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#else
    uint32_t ret;
    uint32_t start_block;
#if (BSP_MCU_GROUP_RL78L23)
    uint32_t erase_length;

    /* Convert Flash address to Flash block number */
    if (FWUP_CFG_DF_ADDR_L > addr)
    {
        /* Code flash */
        start_block = addr / FWUP_CFG_CF_BLK_SIZE;
        /* Erase code flash memory. */
        r_fwup_wrap_disable_interrupt();
        ret = rfd_wrap_cf_erase((uint16_t)start_block, (uint16_t)num_blocks);
        r_fwup_wrap_enable_interrupt();
    }
    else
    {
        /* Data flash */
    	erase_length = addr - FWUP_CFG_DF_ADDR_L;
        start_block  = erase_length / FWUP_CFG_DF_BLK_SIZE;
        /* Erase data flash memory. */
        r_fwup_wrap_disable_interrupt();
        ret = rfd_wrap_df_erase(addr, erase_length, (uint16_t)start_block, (uint16_t)num_blocks);
        r_fwup_wrap_enable_interrupt();
    }
#else
    uint8_t (FWUP_FAR_FUNC *pfunc)(uint16_t, uint16_t) = rfd_wrap_cf_erase;

    /* Convert Flash address to Flash block number */
    if (FWUP_CFG_DF_ADDR_L > addr)
    {
        /* Code flash */
        start_block = addr / FWUP_CFG_CF_BLK_SIZE;
    }
    else
    {
        /* Data flash */
        start_block = (addr - FWUP_CFG_DF_ADDR_L) / FWUP_CFG_DF_BLK_SIZE;
        pfunc = rfd_wrap_df_erase;
    }

    /* Erase flash memory. */
    r_fwup_wrap_disable_interrupt();
    ret = pfunc((uint16_t)start_block, (uint16_t)num_blocks);
    r_fwup_wrap_enable_interrupt();
#endif /* BSP_MCU_GROUP_RL78L23 */

    if (0 != ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_flash_erase
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_flash_write
 * Description  : wrapper function for writing Flash.
 * Arguments    : src_addr       : write source address
 *                dest_addr      : write destination address
 *                num_bytes      : number of bytes to write
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash write error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_flash_write(uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes)
{
    /**** Start user code ****/
#if defined(__RX)
    r_fwup_wrap_disable_interrupt();
    flash_err_t ret = R_FLASH_Write(src_addr, dest_addr, num_bytes);
    r_fwup_wrap_enable_interrupt();
    if (FLASH_SUCCESS != ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#else
    uint32_t ret;
    uint8_t (FWUP_FAR_FUNC *pfunc)(uint32_t, uint16_t, uint8_t __near *) = rfd_wrap_cf_write;

    if (FWUP_CFG_DF_ADDR_L <= dest_addr)
    {
        pfunc = rfd_wrap_df_write;
    }

    /* Write flash */
    r_fwup_wrap_disable_interrupt();
    ret = pfunc(dest_addr, (uint16_t)num_bytes, (uint8_t __near *)(uint16_t)src_addr);
    r_fwup_wrap_enable_interrupt();

    if (0 != ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_flash_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_flash_read
 * Description  : wrapper function for reading Flash.
 * Arguments    : buf_addr     : storage destination address
 *                src_addr     : read source address
 *                size         : number of bytes to read
 * Return Value : FWUP_SUCCESS : success
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_flash_read(uint32_t buf_addr, uint32_t src_addr, uint32_t size)
{
    /**** Start user code ****/

    MEMCPY((void FWUP_FAR *)buf_addr, (void FWUP_FAR *)src_addr, size);
    return (FWUP_SUCCESS);

    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_flash_read
 *********************************************************************************************************************/

#if (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK)
/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_bank_swap
 * Description  : wrapper function for bank swap.
 * Arguments    : none
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash control error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_bank_swap(void)
{
    /**** Start user code ****/
#if defined(__RX)
    flash_err_t err;

    r_fwup_wrap_disable_interrupt();
    err = R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
    r_fwup_wrap_enable_interrupt();

    if (FLASH_SUCCESS != err)
    {
        return (FWUP_ERR_FLASH);
    }

    return (FWUP_SUCCESS);
#else
	e_fwup_err_t ret_code;

    /* Set the user mode (Exit from the bank programming mode) */
    FLMWEN = 1u;
    FLMODE = FLMODE & 0xC0u;
    FLMWEN = 0u;

    if(0 == rfd_wrap_bank_swap())
    {
    	ret_code = FWUP_SUCCESS;
    }
    else
    {
    	ret_code = FWUP_ERR_FLASH;
    }

	return (ret_code);
#endif
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_bank_swap
 *********************************************************************************************************************/
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_DUAL_BANK) */

#if (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT)
/**********************************************************************************************************************
 * Function Name: ext_flash_busy_check
 * Description  : Checks flash memory status.
 * Arguments    : e_nor_flash_check_busy_t mode ;   Mode of error check
 *              :                               ;   NOR_FLASH_MODE_REG_WRITE_BUSY
 *              :                               ;   NOR_FLASH_MODE_PROG_BUSY
 *              :                               ;   NOR_FLASH_MODE_ERASE_BUSY
 *              : uint32_t loop_cnt             ;   loop count for 1ms
 * Return Value : -
 *********************************************************************************************************************/
static e_nor_flash_status_t ext_flash_busy_check(e_nor_flash_check_busy_t mode, uint32_t loop_cnt)
{
    e_nor_flash_status_t ret = NOR_FLASH_SUCCESS_BUSY;

    do
    {
        ret = R_NOR_FLASH_CheckBusy(mode);
        if (NOR_FLASH_SUCCESS_BUSY != ret)
        {
            break;
        }
        loop_cnt--;
        R_FWUP_SoftwareDelay(1, FWUP_DELAY_MILLISECS);
    }
    while (0 != loop_cnt);

    return ret;
}
/**********************************************************************************************************************
 End of function ext_flash_busy_check
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_ext_flash_open
 * Description  : wrapper function for opening external Flash.
 * Arguments    : None
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash open error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_ext_flash_open(void)
{
    /**** Start user code ****/
#if defined(__RX)
#else
    e_nor_flash_status_t ret = NOR_FLASH_SUCCESS;

    ret = R_NOR_FLASH_Open();
    if (NOR_FLASH_SUCCESS > ret)
    {
        return (FWUP_ERR_FLASH);
    }

    ret = ext_flash_busy_check(NOR_FLASH_MODE_ERASE_BUSY, FLASH_CE_BUSY_WAIT);
    if (NOR_FLASH_SUCCESS != ret)
    {
        return (FWUP_ERR_FLASH);
    }

    ret = R_NOR_FLASH_Set4byteAddressMode();
    if (NOR_FLASH_SUCCESS > ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_ext_flash_open
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_ext_flash_close
 * Description  : wrapper function for closing external Flash.
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void r_fwup_wrap_ext_flash_close(void)
{
    /**** Start user code ****/
#if defined(__RX)
#else
    e_nor_flash_status_t ret = NOR_FLASH_SUCCESS;

    ret = R_NOR_FLASH_Close();
    if (NOR_FLASH_SUCCESS > ret)
    {
        while(1);
    }
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_ext_flash_close
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_ext_flash_erase
 * Description  : wrapper function for erasing external Flash.
 * Arguments    : addr           : erasure destination address
 *              : num_sectors    : number of sectors to erase
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash erase error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_ext_flash_erase(uint32_t addr, uint32_t num_sectors)
{
    /**** Start user code ****/
#if defined(__RX)
#else
    st_nor_flash_erase_info_t flash_info_e;
    e_nor_flash_status_t      ret      = NOR_FLASH_SUCCESS;
    uint32_t sector_count;

    for (sector_count = 0; sector_count < num_sectors; sector_count++ )
    {
    	flash_info_e.addr = addr + (FWUP_CFG_EXT_BUF_AREA_BLK_SIZE * sector_count);
        flash_info_e.mode = NOR_FLASH_MODE_S_ERASE;

        ret = R_NOR_FLASH_Erase(&flash_info_e);
        if (NOR_FLASH_SUCCESS > ret)
        {
            return (FWUP_ERR_FLASH);
        }

        ret = ext_flash_busy_check(NOR_FLASH_MODE_ERASE_BUSY, FLASH_SE_BUSY_WAIT);
        if (NOR_FLASH_SUCCESS != ret)
        {
            return (FWUP_ERR_FLASH);
        }
    }

    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_ext_flash_erase
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_ext_flash_write
 * Description  : wrapper function for writing external Flash.
 * Arguments    : src_addr       : write source address
 *              : dest_addr      : write destination address
 *              : num_bytes      : number of bytes to write
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash write error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_ext_flash_write(uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes)
{
    /**** Start user code ****/
#if defined(__RX)
#else
    st_nor_flash_info_t       flash_info_w;
    e_nor_flash_status_t      ret      = NOR_FLASH_SUCCESS;

    /* Write data */
    flash_info_w.addr    = dest_addr;
    flash_info_w.cnt     = (uint16_t)num_bytes;
#if (__ICCRL78__)
    flash_info_w.p_data  = (uint8_t *)(src_addr);
#else
    flash_info_w.p_data  = (uint8_t *)((uint16_t)(src_addr & 0xffff));
#endif

    ret = R_NOR_FLASH_WriteDataPage(&flash_info_w);
    if (NOR_FLASH_SUCCESS > ret)
    {
        return (FWUP_ERR_FLASH);
    }

    ret = ext_flash_busy_check(NOR_FLASH_MODE_PROG_BUSY, FLASH_PP_BUSY_WAIT);
    if (NOR_FLASH_SUCCESS != ret)
    {
        return (FWUP_ERR_FLASH);
    }

    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_ext_flash_write
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_ext_flash_read
 * Description  : wrapper function for reading external Flash.
 * Arguments    : buf_addr       : storage destination address
 *              : src_addr       : read source address
 *              : size           : number of bytes to read
 * Return Value : FWUP_SUCCESS   : success
 *                FWUP_ERR_FLASH : flash read error
 *********************************************************************************************************************/
e_fwup_err_t r_fwup_wrap_ext_flash_read(uint32_t buf_addr, uint32_t src_addr, uint32_t size)
{
    /**** Start user code ****/
#if defined(__RX)
#else
    st_nor_flash_info_t       flash_info_r;
    e_nor_flash_status_t      ret      = NOR_FLASH_SUCCESS;

    /* Read data */
    flash_info_r.addr   = src_addr;
    flash_info_r.cnt    = (uint16_t) size;
#if (__ICCRL78__)
    flash_info_r.p_data = (uint8_t *)(buf_addr);
#else
    flash_info_r.p_data = (uint8_t *)((uint16_t)(buf_addr & 0xffff));
#endif
    ret = R_NOR_FLASH_ReadData(&flash_info_r);
    if (NOR_FLASH_SUCCESS > ret)
    {
        return (FWUP_ERR_FLASH);
    }
    return (FWUP_SUCCESS);
#endif /* defined(__RX) */
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_ext_flash_read
 *********************************************************************************************************************/
#endif /* (FWUP_CFG_UPDATE_MODE == FWUP_SINGLE_BANK_W_BUFFER_EXT) */

/*
 * static functions
 */
/**** Start user code ****/
/**** End user code   ****/

