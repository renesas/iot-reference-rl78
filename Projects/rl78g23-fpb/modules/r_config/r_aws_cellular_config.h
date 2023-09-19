/* Generated configuration header file - do not edit */
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_aws_cellular_config.h
 * Description  : Configures the driver.
 *********************************************************************************************************************/

#ifndef AWS_CELLULAR_CONFIG_H
#define AWS_CELLULAR_CONFIG_H

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"

/***********************************************************************************************************************
 * Configuration Options
 **********************************************************************************************************************/
#define AWS_CELLULAR_CFG_AP_NAME                "mmtmobile.jp"      /* Access point name */
#define AWS_CELLULAR_CFG_AP_USERID              "nipponsim@dhacorp" /* Login ID */
#define AWS_CELLULAR_CFG_AP_PASSWORD            "dhacorp"           /* Access point password */
#define AWS_CELLULAR_CFG_PIN_CODE               0000                /* SIM card PIN code */
#define AWS_CELLULAR_CFG_AUTH_TYPE              (2)                 /* Authentication protocol type (0=None,1=PAP,2=CHAP)*/

#define AWS_CELLULAR_CFG_NETWORK_NOTIFY_LEVEL   (2)      /* Network status notification level */
#define AWS_CELLULAR_CFG_BAUDRATE               (115200) /* SCI baud rate */
#define AWS_CELLULAR_CFG_SCI_PRIORITY           (4)      /* SCI interrupt priority */
#define AWS_CELLULAR_CFG_SCI_CH                 (3)      /* SCI channel */

#define AWS_CELLULAR_CFG_URC_CHARGET_ENABLED    (0)      /* Whether the callback function is invoked when a URC is received(0: No, 1: Yes)*/
#define AWS_CELLULAR_CFG_URC_CHARGET_FUNCTION   my_sw_urc_charget_function   /* Name of the callback function to be invoked during ringline operation.*/

#define AWS_CELLULAR_CFG_AT_COMMAND_TIMEOUT     (60000)  /* Timeout period before an Ack response is returned when AT command is executed */

#define AWS_CELLULAR_CFG_RING_LINE_ACTIVE_TIME  (1000)   /* RING line active duration (1msec, 1000~5000)*/
#define AWS_CELLULAR_CFG_PSM_PREPARATION_TIME   (100)    /* Reception guard time before the module transitions to PSM (1msec, 100~10000) */
#define AWS_CELLULAR_CFG_PSM_WAKEUP_LATENCY     (5000)   /* Maximum authorized wake-up latency (1msec, 0~10000) */

#define AWS_CELLULAR_CFG_DEBUGLOG               (0)      /* 0: OFF, 1: ERROR, 2: +WARN, 3: +INFO, 4: +DEBUG(AT command data) */
                                                         /* This macro can be used with FreeRTOS logging task. See https://www.freertos.org/logging.html */

#define AWS_CELLULAR_CFG_RESET_SIGNAL_LOGIC     (0)      /* 0: active low (in this case when user would use PMOD RYZ024A), 1: active high */
#if AWS_CELLULAR_CFG_RESET_SIGNAL_LOGIC == 1
#define AWS_CELLULAR_CFG_RESET_SIGNAL_ON        (1)
#define AWS_CELLULAR_CFG_RESET_SIGNAL_OFF       (0)
#else
#define AWS_CELLULAR_CFG_RESET_SIGNAL_ON        (0)
#define AWS_CELLULAR_CFG_RESET_SIGNAL_OFF       (1)
#endif

#define AWS_CELLULAR_CFG_CTS_SW_CTRL            (0)
#define AWS_CELLULAR_CFG_CTS_PORT               0           /* CTS port number */
#define AWS_CELLULAR_CFG_CTS_PIN                2           /* CTS Pin number */
#define AWS_CELLULAR_CFG_PFS_SET_VALUE          0x0AU

#define AWS_CELLULAR_CFG_RTS_PORT               14          /* RTS port number */
#define AWS_CELLULAR_CFG_RTS_PIN                2           /* RTS pin number */

#define AWS_CELLULAR_CFG_RESET_PORT             0           /* Reset port number */
#define AWS_CELLULAR_CFG_RESET_PIN              0           /* Reset pin number */

#define AWS_CELLULAR_CFG_IRQ_NUM                (5)

/* Parameter check enable
 0 : Disable parameter check.
 1 : Enable parameter check.
*/
#define AWS_CELLULAR_CFG_PARAM_CHECKING_ENABLE  (0)

/* Board dependent settings; please use the value for each setting listed below depending on the board you use.

Preprocessors that define board dependent settings and the corresponding values to be set are as follows:
Confirmed board number              1  2  3  4  5  6,
AWS_CELLULAR_CFG_UART_SCI_CH        0  2  6  5  6  5,
AWS_CELLULAR_CFG_RTS_PORT           2  5  0  C  0  C,
AWS_CELLULAR_CFG_RTS_PIN            2  1  2  1  2  1,
AWS_CELLULAR_CFG_RESET_PORT         D  5  F  B  5  B,
AWS_CELLULAR_CFG_RESET_PIN          0  5  5  1  5  1,
AWS_CELLULAR_CFG_IRQ_NUM            4 13  5 12  5 12,
where the confirmed board numbers listed in the first row above are as follows:
1: RX65N Cloud Kit (PMOD(CN5)),
2: RX65N Envision Kit (PMOD(CN14)),
3: RX65N RSK (2MB)(PMOD1),
4: RX671 Target Board (PMOD(CN1)), *note1.
5: Cloud Kit for RX65N, CK-RX65N v1 (PMOD1),
6: RX66N Target Board (PMOD(CN1)), *note3.
In the above preprocessor list, please use one of the values listed on the right side.
On the right side, each column corresponds to each confirmed board number.

Note1:
When you use RX671 Target Board, you need pattern cut and so on to use SCI channel 5(TXD5/RXD5/CTS5) and GPIO(PC1). 
Please refer to User's Manual: https://www.renesas.com/products/microcontrollers-microprocessors/rx-32-bit-performance-efficiency-mcus/rtk5rx6710c00000bj-target-board-rx671

Note2:
Please supply power source to USB(CN4) port on RYZ014A-PMOD.
E2 emulator supplying power to RYZ014A-PMOD may result in insufficient power source.
The E2 emulator can provide 200mA, but RYZ014A needs more current while connecting to the network.

Note3:
When you use RX66N Target Board, you need remodeling of the board to use SCI channel 5(TXD5/RXD5/CTS5) and GPIO(PC1). 
Please refer to User's Manual: https://www.renesas.com/products/microcontrollers-microprocessors/rx-32-bit-performance-efficiency-mcus/rtk5rx66n0c00000bj-target-board-rx66n

*/

#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
#define BUILD_TIME_ERROR_MESSAGE_MCU_NOT_SUPPORT    "your MCU/compiler does not support this driver."
#define BUILD_TIME_ERROR_MESSAGE_BOARD_NOT_SUPPORT  "your board does not support this driver, \
                                                    please define board name macro like BSP_BOARD_GENERIC_RX65N."
#endif

#endif /* AWS_CELLULAR_CONFIG_H */
