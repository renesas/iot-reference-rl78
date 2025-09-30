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
#include "aws_clientcredential_keys.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
#if !defined(__DA16XXX_DEMO__)
extern CellularHandle_t CellularHandle;
#endif

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
#if !defined(__DA16XXX_DEMO__)
    /* CA Root */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                            ROOTCA_PEM2_NVM_IDX, (const uint8_t *)ca_data, ca_size);

    /* Certificate */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                            CLIENT_CERT_NVM_IDX, (const uint8_t *)clientcert_data, clientcert_size);

    /* Private key */
    Cellular_WriteCertificate(CellularHandle, AWS_CELLULAR_NVM_TYPE_PRIVATEKEY,
                            CLIENT_PRIVATEKEY_NVM_IDX, (const uint8_t *)prvkey_data, prvkey_size);
#else
    /* Check existed Certificates */
    wifi_tls_cert_info_t cert_infor = {0};
    if (WIFI_SUCCESS == R_WIFI_DA16XXX_GetServerCertificate(&cert_infor))
    {
        if ((NULL != cert_infor.cert_ca) || (NULL != cert_infor.cert_name))
        {
            /* Use when deleting all Certificates */
            /* CA */
            R_WIFI_DA16XXX_DeleteCertificate(WIFI_TLS_TYPE_CA_CERT, &cert_infor);

            /* Certificate & Private key */
            R_WIFI_DA16XXX_DeleteCertificate(WIFI_TLS_TYPE_CLIENT_CERT, &cert_infor);
        }

        /* CA Root */
        R_WIFI_DA16XXX_WriteCertificate((const uint8_t __far *)WIFI_CFG_TLS_CERT_CA_NAME,
                                        WIFI_TLS_TYPE_CA_CERT,
                                        (const uint8_t __far *)ca_data,
                                        ca_size);

        /* Certificate */
        R_WIFI_DA16XXX_WriteCertificate((const uint8_t __far *)WIFI_CFG_TLS_CERT_CLIENT_NAME,
                                        WIFI_TLS_TYPE_CLIENT_CERT,
                                        (const uint8_t __far *)clientcert_data,
                                        clientcert_size);

        /* Private key */
        R_WIFI_DA16XXX_WriteCertificate((const uint8_t __far *)WIFI_CFG_TLS_CERT_PRIVATE_NAME,
                                        WIFI_TLS_TYPE_CLIENT_PRIVATE_KEY,
                                        (const uint8_t __far *)prvkey_data,
                                        prvkey_size);
    }
#endif /* !defined__DA16XXX_DEMO__ */
}
/**********************************************************************************************************************
 * End of Function prvWriteCertificateToModule
 *********************************************************************************************************************/
