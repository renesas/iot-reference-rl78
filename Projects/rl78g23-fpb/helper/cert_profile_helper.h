/*
 * cert_profile_helper.h
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
