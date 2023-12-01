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

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: prvWriteCertificateToModule
 * Description  : Sets the host address to the certificate profile that matches the specified Id.
 * Arguments    : hdl    cellular module handle.
 * Return Value : none
 *********************************************************************************************************************/
void prvWriteCertificateToModule(CellularHandle_t hdl)
{
    /* Write certificate */
    Cellular_WriteCertificate(hdl,
                              AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                              ROOTCA_PEM2_NVM_IDX,
                              (const uint8_t *)CFG_ROOT_CA_PEM2,
                              strlen((const char *)CFG_ROOT_CA_PEM2));

    /* Certificate */
    Cellular_WriteCertificate(hdl,
                              AWS_CELLULAR_NVM_TYPE_CERTIFICATE,
                              CLIENT_CERT_NVM_IDX,
                              (const uint8_t *)keyCLIENT_CERTIFICATE_PEM,
                              strlen((const char *)keyCLIENT_CERTIFICATE_PEM));

    /* Private key */
    Cellular_WriteCertificate(hdl,
                              AWS_CELLULAR_NVM_TYPE_PRIVATEKEY,
                              CLIENT_PRIVATEKEY_NVM_IDX,
                              (const uint8_t *)keyCLIENT_PRIVATE_KEY_PEM,
                              strlen((const char *)keyCLIENT_PRIVATE_KEY_PEM));
}
/**********************************************************************************************************************
 * End of Function prvWriteCertificateToModule
 *********************************************************************************************************************/
