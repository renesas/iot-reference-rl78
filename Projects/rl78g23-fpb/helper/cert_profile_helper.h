/*
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name    : cert_profile_helper.h
* Description  : Header file containing function definitions and declarations
*                for provisioning certificates to the DA16600
***********************************************************************************************************************/

#ifndef CERT_PROFILE_HELPER_H_
#define CERT_PROFILE_HELPER_H_

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if defined(__DA16XXX_DEMO__)
#include <string.h>
#include "r_wifi_da16xxx_if.h"
#else
#include "r_aws_cellular_if.h"
#endif

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
/**********************************************************************************************************************
 * function name: prvWriteCertificateToModule
 *********************************************************************************************************************/
void prvWriteCertificateToModule (const uint8_t * ca_data, uint32_t ca_size,
                                const uint8_t * clientcert_data, uint32_t clientcert_size,
                                const uint8_t * prvkey_data, uint32_t prvkey_size);

#endif /* CERT_PROFILE_HELPER_H_ */
