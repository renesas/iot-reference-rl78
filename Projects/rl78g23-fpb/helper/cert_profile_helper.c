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
 * File Name    : cert_profile_helper.c
 * Description  : write certificate to cellular module.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 27.12.2023 1.00     First Release
 *********************************************************************************************************************/

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
