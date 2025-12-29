/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
* File Name    : code_signer_public_key.h
* Description  : Define public key information for code signer.
***********************************************************************************************************************/

#ifndef CODE_SIGNER_PUBLIC_KEY_H_
#define CODE_SIGNER_PUBLIC_KEY_H_

/*
 * PEM-encoded code signer public key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"\
 * "...base64 data..."\
 * "-----END CERTIFICATE-----"
 */
#define CODE_SIGNER_PUBLIC_KEY_PEM \
"-----BEGIN PUBLIC KEY-----"\
"ABEiM0RVZneImaq7zN3u/wARIjNEVWZ3iJmqu8zd7v8AESIzRFVmd4iZqrvM3e7/"\
"ABEiM0RVZneImaq7zN3u/wARIjNEVWZ3iJmqu8zd7v8="\
"-----END PUBLIC KEY-----"

#endif /* CODE_SIGNER_PUBLIC_KEY_H_ */
