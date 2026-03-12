/*
* Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_fwup_wrap_verify.c
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
#include "r_fwup_if.h"
#include "r_fwup_wrap_verify.h"

/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**** Start user code ****/
#define FWUP_HASH_BYTES        (TC_SHA256_DIGEST_SIZE)
#define FWUP_HASH_BITS         (FWUP_HASH_BYTES * 8)
#define FWUP_CERT_PEM           g_code_signer_public_key
#define FWUP_BITSTRING_LENGTH_OFFSET (24U)
#define FWUP_PUBLIC_KEY_DATA_OFFSET (27U)
/**** End user code   ****/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/**** Start user code ****/
/**** End user code   ****/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
/**** Start user code ****/
#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0)
static int32_t wrap_extract_pubkey (uint8_t *p_buf);
static C_CH_FAR s_keyheader[] = "-----BEGIN PUBLIC KEY-----";
static C_CH_FAR s_keyfooter[] = "-----END PUBLIC KEY-----";
static C_CH_FAR g_code_signer_public_key[] = CODE_SIGNER_PUBLIC_KEY_PEM;
S_C_CH_FAR VERIFICATION_SCHEME_ECDSA[]  = "sig-sha256-ecdsa";
S_C_CH_FAR publicKey_info[27] = {0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01,
		0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04};
#endif /* (FWUP_CFG_SIGNATURE_VERIFICATION == 0) */
S_C_CH_FAR VERIFICATION_SCHEME_SHA[]  = "hash-sha256";
/**** End user code   ****/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_get_crypt_context
 * Description  : wrapper function for get to the crypt library's context.
 * Arguments    : none
 * Return Value : library's static pointer
 *********************************************************************************************************************/
void * r_fwup_wrap_get_crypt_context(void)
{
    /* library's context. that need to be a static value. */
    /**** Start user code ****/
    static struct tc_sha256_state_struct s_ctx;
    return ((void *)&s_ctx);
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_get_crypt_context
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_sha256_init
 * Description  : wrapper function for sha256.
 * Arguments    : vp_ctx : context
 * Return Value : library processing result
 *********************************************************************************************************************/
int32_t r_fwup_wrap_sha256_init(void * vp_ctx)
{
    /**** Start user code ****/
    return (tc_sha256_init((TCSha256State_t)vp_ctx));
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_sha256_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_sha256_update
 * Description  : wrapper function for sha256.
 * Arguments    : vp_ctx   : context
 *                p_data   : message data
 *                data_len : data len
 * Return Value : library processing result
 *********************************************************************************************************************/
int32_t r_fwup_wrap_sha256_update(void * vp_ctx, C_U8_FAR *p_data, uint32_t datalen)
{
    /**** Start user code ****/
    return (tc_sha256_update((TCSha256State_t)vp_ctx, p_data, datalen));
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_sha256_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_sha256_final
 * Description  : wrapper function for sha256.
 * Arguments    : p_hash : hash value storage destination pointer
 *                vp_ctx : context
 * Return Value : library processing result
 *********************************************************************************************************************/
int32_t r_fwup_wrap_sha256_final(uint8_t *p_hash, void * vp_ctx)
{
    /**** Start user code ****/
    return (tc_sha256_final(p_hash, (TCSha256State_t)vp_ctx));
    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_sha256_final
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: r_fwup_wrap_verify_ecdsa
 * Description  : wrapper function for ECDSA verification.
 * Arguments    : p_hash     : hash data
 *                p_sig_type : signature type
 *                p_sig      : signature
 *                sig_size   : signature size
 * Return Value : 0          : verify OK
 *                other      : verify NG
 *********************************************************************************************************************/
int32_t r_fwup_wrap_verify_ecdsa(uint8_t *p_hash, uint8_t *p_sig_type, uint8_t *p_sig, uint32_t sig_size)
{
    /**** Start user code ****/
    int32_t x_result = -1;
#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0)
    uint8_t public_key[64];
#endif /* (FWUP_CFG_SIGNATURE_VERIFICATION == 0) */

    /* SHA256 */
    if (0 == STRCMP((const char FWUP_FAR *)p_sig_type, VERIFICATION_SCHEME_SHA))
    {
        return MEMCMP(p_sig, p_hash, sig_size);
    }

#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0)
    /* ECDSA */
    if (0 == STRCMP((const char FWUP_FAR *)p_sig_type, VERIFICATION_SCHEME_ECDSA))
    {
        /* Extract public key */
        if (0 == wrap_extract_pubkey(public_key))
        {
            /* ECC verify */
            if (uECC_verify(public_key, p_hash, FWUP_HASH_BITS, p_sig, uECC_secp256r1()))
            {
                x_result = 0;
            }
        }
    }
#endif /* (FWUP_CFG_SIGNATURE_VERIFICATION == 0) */

    return (x_result);

    /**** End user code   ****/
}
/**********************************************************************************************************************
 End of function r_fwup_wrap_verify_ecdsa
 *********************************************************************************************************************/

/**** Start user code ****/

#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0)
/**********************************************************************************************************************
 * Function Name: wrap_extract_pubkey
 * Description  : wrapper module for extracting public key.
 * Arguments    : p_buf : key data
 * Return Value : 0     : success
 *                other : error
 *********************************************************************************************************************/
static int32_t wrap_extract_pubkey(uint8_t *p_buf)
{
    int32_t result = -1;
    uint8_t binary[128] = {0};
    uint8_t FWUP_FAR * p_head;
    uint8_t FWUP_FAR * p_current;
    uint8_t FWUP_FAR * p_tail;
    /* Base64 decode */
    p_head = (uint8_t FWUP_FAR *)g_code_signer_public_key + STRLEN(s_keyheader);
    p_tail =  (uint8_t FWUP_FAR *)STRSTR((char FWUP_FAR *)g_code_signer_public_key, s_keyfooter);
    base64_decode(p_head, binary, p_tail - p_head);

    /* Extract Public key */
    /*
      --- Format ---
      RFC 5480 - Elliptic Curve Cryptography Subject Public Key Information
      SEQUENCE (2 elem)
          SEQUENCE (2 elem)
              OBJECT IDENTIFIER 1.2.840.10045.2.1 ecPublicKey (ANSI X9.62 public key type)
              OBJECT IDENTIFIER 1.2.840.10045.3.1.7 prime256v1 (ANSI X9.62 named elliptic curve)
          BIT STRING (520 bit) 0000010010000001011001110101110110000001011111111011011000000101000000… <- public key
      Example:
        30 : Tag=SEQUENCE
        59 : Length
        30 : Tag=SEQUENCE
        13 : Length
        06 : OBJECT IDENTIFIER (ecPubkey)
        07 : Length
        2A 86 48 CE 3D 02 01
        06 : OBJECT IDENTIFIER (prime256v1)
        08 : Length
        2A 86 48 CE 3D 03 01 07
        03 : Tag=BITSTRING
        42 : Length
        00 : Number of not using bits
        04 : 04 = Not compressed , 02 = Compressed
        -- Public Key Data (256bit) --
        81 67 5D 81 7F B6 05 03 21 7D D6 FE 2F 24 CF 79
        05 5C 81 A6 4E 72 F2 D2 0E EC 56 CC 4E 82 5D 4E
        DB E7 E1 CD 54 8C 58 49 3C 9F 08 A4 E1 54 D5 32
        B3 C1 56 F7 D4 D8 00 4D 4B EF 8B 6F 23 FB 3D AC
     */
    p_current = binary;
    if((MEMCMP(p_current, publicKey_info, sizeof(publicKey_info))) == 0)
    {
    	MEMCPY(p_buf, p_current + FWUP_PUBLIC_KEY_DATA_OFFSET, *(p_current + FWUP_BITSTRING_LENGTH_OFFSET) - 2);
        result = 0;
    }
    return (result);
}
/**********************************************************************************************************************
 End of function wrap_extract_pubkey
 *********************************************************************************************************************/
#endif  /* FWUP_CFG_SIGNATURE_VERIFICATION == 0 */

/**** End user code   ****/

