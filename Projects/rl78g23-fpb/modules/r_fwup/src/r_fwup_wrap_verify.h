/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_wrap_verify.h
 * Description  : Functions for the Firmware update module.
 **********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.03.2023 2.00    First Release
 *         : 20.11.2023 2.01    Fixed log messages.
 *                              Add parameter checking.
 *                              Added arguments to R_FWUP_WriteImageProgram API.
 *         : 18.04.2025 2.03    V203 Release.
 *         : 27.08.2025 2.04    V204 Release.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_fwup_private.h"

/**** Start user code ****/
#include "base64_decode.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/sha256.h"
#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0)
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dsa.h"
#include "code_signer_public_key.h"
#endif /* (FWUP_CFG_SIGNATURE_VERIFICATION == 0) */

/**** End user code   ****/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef R_FWUP_WRAP_VERIFY_H_
#define R_FWUP_WRAP_VERIFY_H_

/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void *  r_fwup_wrap_get_crypt_context (void);
int32_t r_fwup_wrap_sha256_init   (void *vp_ctx);
int32_t r_fwup_wrap_sha256_update (void *vp_ctx, C_U8_FAR *p_data, uint32_t datalen);
int32_t r_fwup_wrap_sha256_final  (uint8_t *p_hash, void *vp_ctx);
int32_t r_fwup_wrap_verify_ecdsa  (uint8_t *p_hash, uint8_t *p_sig_type, uint8_t *p_sig, uint32_t sig_size);

#endif /* R_FWUP_WRAP_VERIFY_H_ */
