/*
 * cert_profile_helper.c
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
#include "cert_profile_helper.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
extern CellularHandle_t CellularHandle;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: prvWriteCertificateToModule
 * Description  : Sets the host address to the certificate profile that matches the specified Id.
 * Arguments    : ca_data          CA certificate data
 *                ca_size          CA certificate size
 *                clientcert_data  Client certificate data
 *                clientcert_size  Client certificate size
 *                prvkey_data      Client private key data
 *                prvkey_size      Client private key size
 * Return Value : none
 *********************************************************************************************************************/
void prvWriteCertificateToModule(const uint8_t *ca_data, uint32_t ca_size,
                                 const uint8_t *clientcert_data, uint32_t clientcert_size,
                                 const uint8_t *prvkey_data, uint32_t prvkey_size)
{
    /* CA Root */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                              ROOTCA_PEM2_NVM_IDX, (const uint8_t *)ca_data, ca_size);

    /* Certificate */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                              CLIENT_CERT_NVM_IDX, (const uint8_t *)clientcert_data, clientcert_size);

    /* Private key */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_PRIVATEKEY,
                              CLIENT_PRIVATEKEY_NVM_IDX, (const uint8_t *)prvkey_data, prvkey_size);
}
/**********************************************************************************************************************
 * End of Function prvWriteCertificateToModule
 *********************************************************************************************************************/
