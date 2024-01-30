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
 * File Name    : cert_profile_helper.h
 * Description  : write certificate to cellular module.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 27.12.2023 1.00     First Release
 *********************************************************************************************************************/
#ifndef CERT_PROFILE_HELPER_H_
#define CERT_PROFILE_HELPER_H_

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_aws_cellular_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define ROOTCA_PEM1_NVM_IDX             (17)
#define ROOTCA_PEM2_NVM_IDX             (18)
#define ROOTCA_PEM3_NVM_IDX             (19)
#define CLIENT_CERT_NVM_IDX             (5)
#define CLIENT_PRIVATEKEY_NVM_IDX       (0)
#define CFG_ROOT_CA_PEM1                tlsATS1_ROOT_CERTIFICATE_PEM
#define CFG_ROOT_CA_PEM3                tlsATS3_ROOT_CERTIFICATE_PEM

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void prvWriteCertificateToModule(const uint8_t *ca_data, uint32_t ca_size,
                                 const uint8_t *clientcert_data, uint32_t clientcert_size,
                                 const uint8_t *prvkey_data, uint32_t prvkey_size);

#endif /* CERT_PROFILE_HELPER_H_ */
