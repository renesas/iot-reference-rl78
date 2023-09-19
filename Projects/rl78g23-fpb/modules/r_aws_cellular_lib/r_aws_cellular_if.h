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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_aws_cellular_if.h
 * Description  : Configures the driver.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cellular_platform.h"
#include "cellular_common.h"
#include "cellular_common_internal.h"
#include "cellular_api.h"
#include "cellular_common_api.h"
#include "cellular_common_portable.h"

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "r_sci_rl_if.h"
#else
#include "r_sci_rx_if.h"
#include "r_irq_rx_if.h"
#include "r_irq_rx_config.h"
#endif

#include "r_aws_cellular_config.h"
#include "cellular_ryz.h"


#ifndef AWS_CELLULAR_IF_H
#define AWS_CELLULAR_IF_H

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/
#define AWS_CELLULAR_RESTART_LIMIT          (20U)

#define AWS_CELLULAR_MAX_UE_SVN_LENGTH      (15U)
#define AWS_CELLULAR_MAX_LR_SVN_LENGTH      (20U)

#define AWS_CELLULAR_RTS_DELAYTIME          (600U)       /* Delay time after RTS (millisecond) */

#define AWS_CELLULAR_SEMAPHORE_BLOCK_TIME   (10000U)

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
/* Reset port pin macros.  */
#define AWS_CELLULAR_SET_PDR(x, y)            (AWS_CELLULAR_SET_PDR_PREPROC(x, y))
#define AWS_CELLULAR_SET_PDR_PREPROC(x, y)    ((PM ## x ## _bit.no ## y))
#define AWS_CELLULAR_SET_PODR(x, y)           (AWS_CELLULAR_SET_PODR_PREPROC(x, y))
#define AWS_CELLULAR_SET_PODR_PREPROC(x, y)   ((P ## x ## _bit.no ## y))
#else
/* PDR port macros. */
#define AWS_CELLULAR_SET_PDR(x, y)          (AWS_CELLULAR_SET_PDR_PREPROC(x, y))
/* Set the port direction. */
#define AWS_CELLULAR_SET_PDR_PREPROC(x, y)  ((PORT ## x .PDR.BIT.B ## y))

/* PODR port macros. */
#define AWS_CELLULAR_SET_PODR(x, y)         (AWS_CELLULAR_SET_PODR_PREPROC(x, y))
/* Set the port I/O. */
#define AWS_CELLULAR_SET_PODR_PREPROC(x, y)   ((PORT ## x .PODR.BIT.B ## y))
#endif

#if AWS_CELLULAR_CFG_CTS_SW_CTRL == 1
/* PIDR port macros. */
#define AWS_CELLULAR_GET_PIDR(x, y)         (AWS_CELLULAR_GET_PIDR_PREPROC(x, y))
/* Get the port I/O. */
#define AWS_CELLULAR_GET_PIDR_PREPROC(x, y) ((PORT ## x .PIDR.BIT.B ## y))
/* PMR port macros. */
#define AWS_CELLULAR_SET_PMR(x, y)          (AWS_CELLULAR_SET_PMR_PREPROC(x, y))
/* SET the port I/O. */
#define AWS_CELLULAR_SET_PMR_PREPROC(x, y)  ((PORT ## x .PMR.BIT.B ## y))
/* PFS port macros. */
#define AWS_CELLULAR_SET_PFS(x, y)          (AWS_CELLULAR_SET_PFS_PREPROC(x, y))
/* SET the PFS value. */
#define AWS_CELLULAR_SET_PFS_PREPROC(x, y)  ((MPC.P ## x ## y ##PFS.BYTE))
#endif

/* Convert a macro value to a string */
#define AWS_CELLULAR_STRING_MACRO(str)          #str
/* Call CELLULAR_STRING_MACRO */
#define AWS_CELLULAR_STRING_CONVERT(arg)        (AWS_CELLULAR_STRING_MACRO(arg))

/**********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/
typedef enum
{
    AWS_CELLULAR_FIRM_UPGRADE_NONBLOCKING = 1,  // Perform firmware upgrade in non-blocking mode
    AWS_CELLULAR_FIRM_UPGRADE_CANCEL      = 2,  // Cancel firmware upgrade
} e_aws_cellular_firmupgrade_t;

typedef enum
{
    AWS_CELLULAR_NVM_TYPE_CERTIFICATE = 0,  // Certificate
    AWS_CELLULAR_NVM_TYPE_PRIVATEKEY        // PrivateKey
} e_aws_cellular_nvm_type_t;

typedef enum
{
    AWS_CELLULAR_NO_CERT_VALIDATE         = 0,  // Certificate not validated
    AWS_CELLULAR_VALIDATE_CERT_EXPDATE    = 1,  // Validate certificate chain, validity period
    AWS_CELLULAR_VALIDATE_CERT_EXPDATE_CN = 5,  // Validate certificate chain, validity period, common name
} e_aws_cellular_validate_level_t;

typedef enum
{
    AWS_CELLULAR_DISABLE_AUTO_CONNECT = 0,  // Disable automatic connection to AP
    AWS_CELLULAR_ENABLE_AUTO_CONNECT,       // Enable automatic connection to AP (next start-up or restart)
} e_aws_cellular_auto_connect_t;

typedef enum
{
    AWS_CELLULAR_MODULE_OPERATING_LEVEL0 = 0,    // Default
    AWS_CELLULAR_MODULE_OPERATING_LEVEL1,        // Full functional use possible
    AWS_CELLULAR_MODULE_OPERATING_LEVEL2,        // Transmission function disabled
    AWS_CELLULAR_MODULE_OPERATING_LEVEL3,        // Reception function disabled
    AWS_CELLULAR_MODULE_OPERATING_LEVEL4,        // In-flight mode
    AWS_CELLULAR_MODULE_OPERATING_LEVEL5,        // Reserved
} e_aws_cellular_module_status_t;

typedef enum
{
    AWS_CELLULAR_MODULE_FLG_INIT = 0,   // Initial value
    AWS_CELLULAR_MODULE_START_FLG,      // Received +SYSSTART URC
    AWS_CELLULAR_MODULE_SHUTDOWN_FLG,   // Received +SHUTDONW URC
} e_aws_cellular_module_flg_t;

typedef struct aws_cellular_svn
{
    uint8_t ue_svn[AWS_CELLULAR_MAX_UE_SVN_LENGTH+1];   // UE Software Version Number
    uint8_t lr_svn[AWS_CELLULAR_MAX_LR_SVN_LENGTH+1];   // LR Software Version Number
} st_aws_cellular_svn_t;

typedef struct aws_cellular_timeout
{
    volatile uint8_t    over_flg;
    volatile uint8_t    timeout_flg;
    uint32_t            start_time;
    uint32_t            end_time;
} st_aws_cellular_timeout_ctrl_t;

typedef struct aws_cellular_ctrl
{
    e_aws_cellular_module_flg_t module_flg;         // Module state
    uint8_t                     psm_mode;           // PSM setting state
    void *                      rts_semaphore;      // Semaphore handle
    void *                      ring_event;         // Event Group Handles
    void *                      ring_taskhandle;    // Task handle
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
    irq_handle_t                ring_irqhandle;     // IRQ control block handle
#endif
} st_aws_cellular_ctrl_t;

extern CellularCommInterface_t g_cellular_comm_interface;
extern st_aws_cellular_ctrl_t  g_aws_cellular_ctrl;

/*****************************************************************************
 * Public Functions
 *****************************************************************************/
/*************************************************************************************************
 * Function Name  @fn            R_AWS_CELLULAR_Open
 * Description    @details       Establish a communication state with the module.
 * Arguments      @param[in/out] cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully established a communication state with the module
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t R_AWS_CELLULAR_Open (CellularHandle_t * cellularHandle);

/*************************************************************************************************
 * Function Name  @fn            R_AWS_CELLULAR_Close
 * Description    @details       Terminate communication with the module.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully disconnected communication with module.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t R_AWS_CELLULAR_Close (CellularHandle_t cellularHandle);

/*************************************************************************************************
 * Function Name  @fn            Cellular_GetPhoneNum
 * Description    @details       Get Phone Number.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[out]    p_phonenum -
 *                                  Pointer to store the retrieved Phone number.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully obtained phone number.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t Cellular_GetPhoneNum (CellularHandle_t cellularHandle,
                                      const uint8_t  * p_phonenum);

/*************************************************************************************************
 * Function Name  @fn            Cellular_SetOperator
 * Description    @details       Set up the operator.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_operator_name -
 *                                  Operator to be set.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully set operator.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t Cellular_SetOperator (CellularHandle_t      cellularHandle,
                                      const uint8_t * const p_operator_name);

/*************************************************************************************************
 * Function Name  @fn            Cellular_SetBand
 * Description    @details       Set the band settings.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_band -
 *                                  Band to be set.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully set band.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t Cellular_SetBand (CellularHandle_t      cellularHandle,
                                  const uint8_t * const p_band);

/*************************************************************************************************
 * Function Name  @fn            Cellular_FirmUpgrade
 * Description    @details       Perform firmware upgrade.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_url -
 *                                  URL of the firmware.
 *                @param[in]     command -
 *                                  Firmware upgrade command.
 *                @param[in]     spid -
 *                                  Security Profile ID.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully updated the firmware.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t Cellular_FirmUpgrade (CellularHandle_t                   cellularHandle,
                                      const uint8_t * const              p_url,
                                      const e_aws_cellular_firmupgrade_t command,
                                      const uint8_t                      spid);

/*************************************************************************************************
 * Function Name  @fn            Cellular_GetUpgradeState
 * Description    @details       Get farm update status.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[out]    p_state -
 *                                  Pointer to structure to store state.
 *                @param[in]     size -
 *                                  Size of p_state.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully obtained farm update status.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 ************************************************************************************************/
CellularError_t Cellular_GetUpgradeState (CellularHandle_t   cellularHandle,
                                          const uint8_t    * p_state,
                                          const uint32_t     size);

/******************************************************************************************************
 * Function Name  @fn            Cellular_WriteCertificate
 * Description    @details       Write certificate or private key to non-volatile memory of the module.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     data_type -
 *                                  Information to write.
 *                @param[in]     index -
 *                                  Destination index.
 *                @param[in]     p_data -
 *                                  Pointer to the data to write.
 *                @param[in]     size -
 *                                  Size of the data to be written.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully write certificate.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 *****************************************************************************************************/
CellularError_t Cellular_WriteCertificate (CellularHandle_t                 cellularHandle,
                                            const e_aws_cellular_nvm_type_t data_type,
                                            const uint8_t                   index,
                                            const uint8_t * const           p_data,
                                            const uint32_t                  size);

/******************************************************************************************************
 * Function Name  @fn            Cellular_EraseCertificate
 * Description    @details       Erase certificate or private key to non-volatile memory of the module.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     data_type -
 *                                  Information to be deleted.
 *                @param[in]     index -
 *                                  Destination index.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully erase certificate.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 *****************************************************************************************************/
CellularError_t Cellular_EraseCertificate (CellularHandle_t                 cellularHandle,
                                            const e_aws_cellular_nvm_type_t data_type,
                                            const uint8_t                   index);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_GetCertificate
 * Description    @details       Obtain a certificate or private key stored in the module's non-volatile memory.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     data_type -
 *                                  Information to be obtained.
 *                @param[in]     index -
 *                                  Destination index.
 *                @param[out]    p_buff -
 *                                  Pointer to data writing destination.
 *                @param[in]     buff_length -
 *                                  Size of p_buff.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully obtained certificate
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_GetCertificate (CellularHandle_t                cellularHandle,
                                         const e_aws_cellular_nvm_type_t data_type,
                                         const uint8_t                   index,
                                         const uint8_t                 * p_buff,
                                         const uint32_t                  buff_length);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_ConfigSSLProfile
 * Description    @details       Configurate security profile.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     security_profile_id -
 *                                  Security Profile identifier
 *                @param[in]     cert_valid_level -
 *                                  Server certificate validation
 *                @param[in]     ca_certificate_id -
 *                                  Certificate ID of a trusted certification authority
 *                                    by written to non-volatile memory.
 *                @param[in]     client_certificate_id -
 *                                  Client certificate ID by written to non-volatile memory.
 *                @param[in]     client_privatekey_id -
 *                                  Privatekey ID by written to non-volatile memory.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully configurate.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_ConfigSSLProfile (CellularHandle_t                       cellularHandle,
                                            const uint8_t                         security_profile_id,
                                            const e_aws_cellular_validate_level_t cert_valid_level,
                                            const uint8_t                         ca_certificate_id,
                                            const uint8_t                         client_certificate_id,
                                            const uint8_t                         client_privatekey_id);

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
/***************************************************************************************************************
 * Function Name  @fn            Cellular_ConfigTlslSocket
 * Description    @details       TLS socket configuration.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     socket_id -
 *                                  Socket ID.
 *                @param[in]     enable -
 *                                  enable TLS.
 *                @param[in]     security_profile_id -
 *                                  Security profile ID.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully configurate.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_ConfigTlslSocket (CellularHandle_t cellularHandle,
                                           const uint8_t    socket_id,
                                           const bool       enable,
                                           const uint8_t    security_profile_id);
#endif

/***************************************************************************************************************
 * Function Name  @fn            Cellular_PingRequest
 * Description    @details       Perform Ping.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_hostname -
 *                                  Pointer to destination host name.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully Ping.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_PingRequest (CellularHandle_t      cellularHandle,
                                      const uint8_t * const p_hostname);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_StartUpLink
 * Description    @details       Starts an uplink continuous wave service.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     enable -
 *                                  1 to start; 0 to stop.
 *                @param[in]     earfcn -
 *                                  An E-UTRA Absolute Radio Frequency Channel Number.
 *                @param[in]     level -
 *                                  RF output power level of the continuous wave signal, in hundredths of dBm.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully launch of uplink continuous wave service.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_StartUpLink (CellularHandle_t cellularHandle,
                                      const uint8_t    enable,
                                      const uint16_t   earfcn,
                                      const uint32_t   level);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_StartDownLink
 * Description    @details       Starts a downlink continuous wave service.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     earfcn -
 *                                  An E-UTRA Absolute Radio Frequency Channel Number.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully launch of downlink continuous wave service.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_StartDownLink (CellularHandle_t cellularHandle,
                                        const uint16_t   earfcn);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_AutoAPConnectConfig
 * Description    @details       Enable/disable automatic connection to APs.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     type -
 *                                  Enable/disable.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully configured.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_AutoAPConnectConfig (CellularHandle_t                    cellularHandle,
                                              const e_aws_cellular_auto_connect_t type);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_UnlockSIM
 * Description    @details       Unlock the SIM.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_pass -
 *                                  Pointer to password.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully unlocked SIM.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_UnlockSIM (CellularHandle_t      cellularHandle,
                                    const uint8_t * const p_pass);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_FactoryReset
 * Description    @details       Restore factory conditions.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully factory reset.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_FactoryReset (CellularHandle_t cellularHandle);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_SetBaudrate
 * Description    @details       Set communication baud rate with module.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     baudrate -
 *                                  Set baud rate.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully set baud rate.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_SetBaudrate (CellularHandle_t cellularHandle,
                                      const uint32_t   baudrate);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_GetSVN
 * Description    @details       Get SVN.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[out]    p_svn -
 *                                  Pointer to store the retrieved SVN.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully obtained SVN.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_GetSVN (CellularHandle_t              cellularHandle,
                                 const st_aws_cellular_svn_t * p_svn);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_SetEidrxSettingsEXT
 * Description    @details       Set eDRX.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     p_eidrxsettings -
 *                                  Pointer to the configuration structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully set up eDRX.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_SetEidrxSettingsEXT (CellularHandle_t                cellularHandle,
                                              const CellularEidrxSettings_t * p_eidrxsettings);

/***************************************************************************************************************
 * Function Name  @fn            Cellular_GetSocketDataSize
 * Description    @details       Get socket information.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     socketHandle -
 *                                  Socket handle.
 *                @param[out]    p_size -
 *                                  Pointer to destination of acquisition information.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Socket information is successfully obtained.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t Cellular_GetSocketDataSize (CellularHandle_t        cellularHandle,
                                             CellularSocketHandle_t socketHandle,
                                             const uint16_t       * p_size);

#if AWS_CELLULAR_CFG_URC_CHARGET_ENABLED == 1
void AWS_CELLULAR_CFG_URC_CHARGET_FUNCTION (const char * pRawData,
                                            void       * pCallbackContext);
#endif

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_serial_reopen
 * Description    @details       Start serial communication again after setting the baud rate.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 *                @param[in]     baudrate -
 *                                  Set baud rate.
 * Return Value   @retval        SCI_SUCCESS -
 *                                  Successfully initiated serial communication.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
sci_err_t aws_cellular_serial_reopen (CellularHandle_t cellularHandle, const uint32_t baudrate);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_temp_close
 * Description    @details       Terminate serial communication.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        SCI_SUCCESS -
 *                                  Successfully terminated serial communication.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
sci_err_t aws_cellular_temp_close (CellularHandle_t cellularHandle);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_hardreset
 * Description    @details       Perform hard reset of the module.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully hard reset.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t aws_cellular_hardreset (CellularHandle_t cellularHandle);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_closesocket
 * Description    @details       Close the socket.
 * Arguments      @param[in]     cellularHandle -
 *                                  Pointer to managed structure.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully closed the socket.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t aws_cellular_closesocket (CellularHandle_t cellularHandle);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_modemevent
 * Description    @details       Module Event Management.
 * Arguments      @param[in]     modemEvent -
 *                                  Event.
 *                @param[in]     pCallbackContext -
 *                                  Context pointer to store event content.
 **************************************************************************************************************/
void aws_cellular_modemevent (CellularModemEvent_t   modemEvent,
                              void                 * pCallbackContext);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_timeout_init
 * Description    @details       Initialize timeout settings.
 * Arguments      @param[in]     timeout_ctrl -
 *                                  Timeout management structure.
 *                @param[in]     timeout -
 *                                  Set timeout period.
 **************************************************************************************************************/
void aws_cellular_timeout_init (st_aws_cellular_timeout_ctrl_t * timeout_ctrl, uint32_t timeout);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_check_timeout
 * Description    @details       Check timeout.
 * Arguments      @param[in]     timeout_ctrl -
 *                                  Timeout management structure.
 * Return Value   @retval        false -
 *                                  Not timed out.
 *                               true -
 *                                  Time out.
 **************************************************************************************************************/
bool aws_cellular_check_timeout (st_aws_cellular_timeout_ctrl_t * const timeout_ctrl);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_ctrl
 * Description    @details       RTS pin control.
 * Arguments      @param[in]     lowhigh -
 *                                  Output signal.
 **************************************************************************************************************/
void aws_cellular_rts_ctrl (const uint8_t lowhigh);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_callback
 * Description    @details       IRQ interrupt callback.
 * Arguments      @param[in]     p_Args -
 *                                  Callback Content.
 **************************************************************************************************************/
void aws_cellular_irq_callback (void * const p_Args);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_open
 * Description    @details       Enable irq interrupt.
 * Arguments      @param[in]     p_aws_ctrl -
 *                                  Pointer to management structure for cellular module.
 * Return Value   @retval        true -
 *                                  Success.
 *                               false -
 *                                  Failure.
 **************************************************************************************************************/
bool aws_cellular_irq_open (st_aws_cellular_ctrl_t * const p_aws_ctrl);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_irq_close
 * Description    @details       Disable irq interrupt.
 * Arguments      @param[in]     p_aws_ctrl -
 *                                  Pointer to management structure for cellular module.
 **************************************************************************************************************/
void aws_cellular_irq_close (st_aws_cellular_ctrl_t * const p_aws_ctrl);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_psm_config
 * Description    @details       Perform PSM config.
 * Arguments      @param[in]     p_context -
 *                                  Pointer to managed structure.
 *                @param[in]     mode -
 *                                  Enable(1)/Disable(0) PSM.
 * Return Value   @retval        CELLULAR_SUCCESS -
 *                                  Successfully configured PSM config.
 *                               Others -
 *                                  Error code indicating the cause of the error.
 **************************************************************************************************************/
CellularError_t aws_cellular_psm_config (CellularContext_t * p_context, const uint8_t mode);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_psm_conf_fail
 * Description    @details       Disable PSM config settings.
 * Arguments      @param[in]     p_context -
 *                                  Pointer to managed structure.
 *                @param[in]     phase -
 *                                  PSM Config Progress.
 **************************************************************************************************************/
void aws_cellular_psm_conf_fail (CellularContext_t * p_context, const uint8_t phase);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_active
 * Description    @details       Set the RTS pin to High output.
 * Arguments      @param[in]     p_context -
 *                                  Pointer to managed structure.
 *                @param[in]     semaphore_ret -
 *                                  Semaphore Acquisition Status.
 **************************************************************************************************************/
void aws_cellular_rts_active (CellularContext_t * p_context, BaseType_t semaphore_ret);

/***************************************************************************************************************
 * Function Name  @fn            aws_cellular_rts_deactive
 * Description    @details       Set the RTS pin to Low output.
 * Arguments      @param[in]     p_context -
 *                                  Pointer to managed structure.
 * Return Value   @retval        pdTRUE(1) -
 *                                  Succeeded in obtaining semaphore and set RTS to Low output.
 *                               pdFALSE(0) -
 *                                  Processing was not executed.
 **************************************************************************************************************/
BaseType_t aws_cellular_rts_deactive (CellularContext_t * p_context);

#endif /* AWS_CELLULAR_IF_H */
