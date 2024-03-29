/**********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : code_signer_public_key.h
* Description  : Define public key information for code signer.
**********************************************************************************************************************/
#ifndef CODE_SIGNER_PUBLIC_KEY_H_
#define CODE_SIGNER_PUBLIC_KEY_H_

#include "test_execution_config.h"

/*
 * PEM-encoded code signer public key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#if (OTA_PAL_TEST_ENABLED == 1)
/* for Full_OTA_PAL Test */
#define CODE_SIGNER_PUBLIC_KEY_PEM \
"-----BEGIN PUBLIC KEY-----"\
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEKwbEshKaHHhtxmojJidJ/GX+1S/e"\
"5uwH1jMq32opLTX6vfckKXG4/WvB9IqZB99O6BQ4wWj2qfwWrV9YXVytSA=="\
"-----END PUBLIC KEY-----"
#else
/* 41167f58-633c-499f-a17f-4631918c6422 */
#define CODE_SIGNER_PUBLIC_KEY_PEM \
"-----BEGIN PUBLIC KEY-----"\
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEedv40AoIGSF0wzc5D/n9v4dHh907"\
"1+Io02XDFmpUrxeig3FxHOiGNKHL57u5RA45NS/uUL20QEFNhbtpsOcksw=="\
"-----END PUBLIC KEY-----"
#endif

#endif /* CODE_SIGNER_PUBLIC_KEY_H_ */
