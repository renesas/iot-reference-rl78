/*
 * boot_loader.c
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
#include "r_fwup_if.h"
#include "putchar_rl78.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define UART_RTS                    (P6_bit.no1)
#define BL_USER_SW                  (P13_bit.no7)
#define FLASH_BUF_SIZE              (128 * 5)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
/* Flash Buffer */
typedef struct st_bl_buf
{
    uint8_t  buf[FLASH_BUF_SIZE + 8];
    uint32_t cnt;
    uint32_t total;
} st_flash_buf_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
extern uint16_t g_using_uservector;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
S_C_CH_FAR MSG_SEND_VIA_UART[] = "send image(*.rsu) via UART.\r\n";
S_C_CH_FAR MSG_SW_RESET[] = "software reset...\r\n";
S_C_CH_FAR MSG_ERROR[] = "error occurred. please reset your board.\r\n";
S_C_CH_FAR MSG_JUMP_TO_USER_PROG[] = "execute new image ...\r\n";
S_C_CH_FAR MSG_UPDATER[] = "\r\n==== RL78G23 : Image updater [%s] ====\r\n";
S_C_CH_FAR MSG_START_BOOTLOADER[] = "\r\n==== RL78G23 : BootLoader [%s] ====\r\n";
S_C_CH_FAR MSG_UPDATE_MODE_STR[][32] = {"dual bank", "with buffer", "without buffer", "with ext-buffer"};

#if (FWUP_CFG_UPDATE_MODE != FWUP_DUAL_BANK) && (FWUP_CFG_UPDATE_MODE != FWUP_SINGLE_BANK_WO_BUFFER)
S_C_CH_FAR MSG_TEMPCOPY_OK[] = "copy to main area ... OK\r\n";
S_C_CH_FAR MSG_TEMPCOPY_NG[] = "copy to main area ... NG\r\n";
#endif /* (FWUP_CFG_UPDATE_MODE == 0) */

static void open_boot_loader  (void);
static void close_boot_loader (void);
static void sample_buf_init   (void);
static e_fwup_err_t sample_write_image (e_fwup_area_t bank);
static st_flash_buf_t s_flash_buf;
static uint8_t s_err_flg = 0;

/**********************************************************************************************************************
* Function Name: sample_buf_init
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
static void sample_buf_init(void)
{
    memset(&s_flash_buf, 0, sizeof(st_flash_buf_t));
}
/**********************************************************************************************************************
 End of function sample_buf_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: sample_buffering
* Description  : buffering image data function.
* Arguments    : rx_data
* Return Value : none
**********************************************************************************************************************/
void sample_buffering(uint8_t rx_data)
{
    static uint32_t  file_size;

    /* Error occurred? */
    if (0 != s_err_flg)
    {
        return;
    }

    /* Buffering firmware */
    s_flash_buf.buf[s_flash_buf.cnt++] = rx_data;
    s_flash_buf.total++;

    /* Buffer full? */
    if (FLASH_BUF_SIZE == s_flash_buf.cnt)
    {
        file_size = R_FWUP_GetImageSize();
        UART_RTS = 1;
    }
    else
    {
        /* end of file? */
        if ((0 < file_size) && (file_size <= s_flash_buf.total))
        {
            UART_RTS = 1;
        }
    }
}
/**********************************************************************************************************************
 End of function sample_buffering
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: sample_write_image
* Description  :
* Arguments    : bank
* Return Value : FWUP_SUCCESS   : updated user program
*                FWUP_PROGRESS  : updating user program
*                FWUP_ERR_FLASH : flash write error
**********************************************************************************************************************/
static e_fwup_err_t sample_write_image(e_fwup_area_t area)
{
    e_fwup_err_t ret_val = FWUP_ERR_FAILURE;
    uint32_t write_size;

    while(1)
    {
        if (1 == UART_RTS)
        {
            /* Wait for RTS=ON */
            R_FWUP_SoftwareDelay(500, FWUP_DELAY_MICROSECS);

            /* update firmware */
            write_size = (FLASH_BUF_SIZE < s_flash_buf.cnt) ? FLASH_BUF_SIZE : s_flash_buf.cnt;
            ret_val = R_FWUP_WriteImage(area, &s_flash_buf.buf[0], write_size);

            /* there are received data during RTS=ON */
            if (FLASH_BUF_SIZE < s_flash_buf.cnt)
            {
                s_flash_buf.cnt -= FLASH_BUF_SIZE;
                MEMCPY(&s_flash_buf.buf[0], &s_flash_buf.buf[FLASH_BUF_SIZE], s_flash_buf.cnt);
            }
            else
            {
                s_flash_buf.cnt = 0;
            }

            /* updated or error? */
            if (FWUP_PROGRESS != ret_val)
            {
                break;
            }
            UART_RTS = 0;
        }
    }

    sample_buf_init();

    /* Verify */
    if (FWUP_SUCCESS == ret_val)
    {
        if (0 != R_FWUP_VerifyImage(area))
        {
            ret_val = FWUP_ERR_VERIFY;
        }
        else
        {
            ret_val = FWUP_SUCCESS;
        }
    }
    return ret_val;
}
/**********************************************************************************************************************
 End of function sample_write_image
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: open_boot_loader
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
static void open_boot_loader(void)
{
    BSP_NOP();

    BSP_ENABLE_INTERRUPT();
    UART_START_FUNC();
    R_FWUP_SoftwareDelay(50, FWUP_DELAY_MILLISECS);

    g_using_uservector = 0;
    UART_RTS = 0;
    s_err_flg = 0;
    sample_buf_init();
    R_FWUP_Open();
}
/**********************************************************************************************************************
 End of function open_boot_loader
 *********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: close_boot_loader
* Description  :
* Arguments    :
* Return Value :
**********************************************************************************************************************/
static void close_boot_loader(void)
{
    UART_STOP_FUNC();
    R_FWUP_Close();
    R_FWUP_SoftwareDelay(50, FWUP_DELAY_MILLISECS);
}
/**********************************************************************************************************************
 End of function close_boot_loader
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: main
 *********************************************************************************************************************/
void main(void)
{
    open_boot_loader();

#if (FWUP_CFG_UPDATE_MODE != FWUP_SINGLE_BANK_WO_BUFFER)
    /* use Main and Buffer */
    if (0 == BL_USER_SW)
    {
        printf(MSG_UPDATER, MSG_UPDATE_MODE_STR[FWUP_CFG_UPDATE_MODE]);
        printf(MSG_SEND_VIA_UART);
        if (FWUP_SUCCESS != sample_write_image(FWUP_AREA_BUFFER))
        {
            goto END;
        }
        goto SW_RESET;
    }

    printf(MSG_START_BOOTLOADER, MSG_UPDATE_MODE_STR[FWUP_CFG_UPDATE_MODE]);
    if (true != R_FWUP_IsExistImage(FWUP_AREA_MAIN))
    {
        /* Install initial firmware */
        printf(MSG_SEND_VIA_UART);
        if (FWUP_SUCCESS != sample_write_image(FWUP_AREA_MAIN))
        {
            goto END;
        }
        goto SW_RESET;
    }

    if (true != R_FWUP_IsExistImage(FWUP_AREA_BUFFER))
    {
        /* Verify Main area */
        if (FWUP_SUCCESS != R_FWUP_VerifyImage(FWUP_AREA_MAIN))
        {
            R_FWUP_EraseArea(FWUP_AREA_MAIN);
            goto END;
        }

        /* Jump to user program */
        printf(MSG_JUMP_TO_USER_PROG);
        close_boot_loader();
        g_using_uservector = 1;
        R_FWUP_ExecImage();
    }

    /* Verify BUFFER_AREA */
    if (FWUP_SUCCESS != R_FWUP_VerifyImage(FWUP_AREA_BUFFER))
    {
        R_FWUP_EraseArea(FWUP_AREA_BUFFER);
    }
    else
    {
        if (FWUP_SUCCESS != R_FWUP_ActivateImage())
        {
            printf(MSG_TEMPCOPY_NG);
        }
        else
        {
            printf(MSG_TEMPCOPY_OK);
        }
    }
#else
    /* use only Main area */
    if (0 == BL_USER_SW)
    {
        printf(MSG_UPDATER, MSG_UPDATE_MODE_STR[FWUP_CFG_UPDATE_MODE]);
        printf(MSG_SEND_VIA_UART);
        if (FWUP_SUCCESS != sample_write_image(FWUP_AREA_MAIN))
        {
            goto END;
        }
        goto SW_RESET;
    }

    printf(MSG_START_BOOTLOADER, MSG_UPDATE_MODE_STR[FWUP_CFG_UPDATE_MODE]);

    if (true != R_FWUP_IsExistImage(FWUP_AREA_MAIN))
    {
        /* Install initial firmware */
        printf(MSG_SEND_VIA_UART);
        if (FWUP_SUCCESS != sample_write_image(FWUP_AREA_MAIN))
        {
            goto END;
        }
        goto SW_RESET;
    }

    /* Verify Main area */
    if (FWUP_SUCCESS != R_FWUP_VerifyImage(FWUP_AREA_MAIN))
    {
        R_FWUP_EraseArea(FWUP_AREA_MAIN);
        goto END;
    }
    R_FWUP_ActivateImage();

    /* Jump to user program */
    printf(MSG_JUMP_TO_USER_PROG);
    close_boot_loader();
    g_using_uservector = 1;
    R_FWUP_ExecImage();
#endif /* (FWUP_CFG_UPDATE_MODE != FWUP_SINGLE_BANK_WO_BUFFER) */
SW_RESET:
    printf(MSG_SW_RESET);
    close_boot_loader();
    R_FWUP_SoftwareReset();
END:
    printf(MSG_ERROR);
    s_err_flg = 1;
    UART_RTS = 0;
    while (1);
}/* End of function main */
