/*
 * FreeRTOS V202107.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_
#define AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_
#include <stdint.h>

/**
 * @brief Invalid signature for OTA PAL testing.
 */
static const uint8_t ucInvalidSignature[] =
{
    0x30, 0x44, 0x02, 0x20, 0x75, 0xde, 0xa8, 0x1f, 0xca, 0xec, 0xff, 0x16,
    0xbb, 0x38, 0x4b, 0xe3, 0x14, 0xe7, 0xfb, 0x68, 0xf5, 0x3e, 0x86, 0xa2,
    0x71, 0xba, 0x9e, 0x5e, 0x50, 0xbf, 0xb2, 0x7a, 0x9e, 0x00, 0xc6, 0x4d,
    0x02, 0x20, 0x19, 0x72, 0x42, 0x85, 0x2a, 0xac, 0xdf, 0x5a, 0x5e, 0xfa,
    0xad, 0x49, 0x17, 0x5b, 0xce, 0x5b, 0x65, 0x75, 0x08, 0x47, 0x3e, 0x55,
    0xf9, 0x0e, 0xdf, 0x9e, 0x8c, 0xdc, 0x95, 0xdf, 0x63, 0xd2
};
static const int ucInvalidSignatureLength = 70;

/**
 * @brief Valid signature matching the test block in the OTA PAL tests.
 */
#ifndef RENESAS_IDT
static const uint8_t ucValidSignature[] =
{
    0x30, 0x44,
    0x02, 0x20,
    0x66, 0x53, 0x9B, 0x15, 0x1E, 0x3D, 0xCD, 0x3E, 0x61, 0xBE, 0x32, 0x71, 0x30, 0xB8, 0x67, 0x6A,
    0xBC, 0x07, 0xD4, 0x44, 0xC4, 0x8C, 0xDC, 0xB5, 0x87, 0xE3, 0xBE, 0x02, 0x6B, 0x17, 0x35, 0xFA,
    0x02, 0x20,
    0x31, 0x9C, 0x3E, 0x34, 0x21, 0x88, 0x19, 0x22, 0xBF, 0xEC, 0x58, 0x8C, 0x70, 0x8C, 0x08, 0x09,
    0x99, 0x16, 0x91, 0xA9, 0x8B, 0x9D, 0xCE, 0x67, 0xA6, 0xA9, 0x87, 0x27, 0x00, 0xB8, 0xFC, 0x0C
};
static const int ucValidSignatureLength = 70;
#else
static const uint8_t ucValidSignature[] =
{
    0x30, 0x44, 0x02, 0x20, 0x15, 0x6a, 0x68, 0x98, 0xf0, 0x4e, 0x1e, 0x12,
    0x4c, 0xc4, 0xf1, 0x05, 0x22, 0x36, 0xfd, 0xb4, 0xe5, 0x5d, 0x83, 0x08,
    0x2a, 0xf3, 0xa6, 0x7d, 0x32, 0x6b, 0xff, 0x85, 0x27, 0x14, 0x9b, 0xbf,
    0x02, 0x20, 0x26, 0x7d, 0x5f, 0x4d, 0x12, 0xab, 0xec, 0x17, 0xd8, 0x45,
    0xc6, 0x3d, 0x8e, 0xd8, 0x8d, 0x3f, 0x28, 0x26, 0xfd, 0xce, 0x32, 0x34,
    0x17, 0x05, 0x47, 0xb2, 0xf6, 0x84, 0xd5, 0x68, 0x3e, 0x36
};
static const int ucValidSignatureLength = 70;
#endif

#ifndef RENESAS_IDT
#define OTA_PAL_CODE_SIGNING_CERTIFICATE                             \
"-----BEGIN PUBLIC KEY-----\n"                                       \
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEKwbEshKaHHhtxmojJidJ/GX+1S/e\n" \
"5uwH1jMq32opLTX6vfckKXG4/WvB9IqZB99O6BQ4wWj2qfwWrV9YXVytSA==\n"     \
"-----END PUBLIC KEY-----\n"
#else
#define OTA_PAL_CODE_SIGNING_CERTIFICATE                             \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIBXDCCAQOgAwIBAgIJAPMhJT8l0C6AMAoGCCqGSM49BAMCMCExHzAdBgNVBAMM\n" \
    "FnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwNDQyWhcNMTkwNjI3\n" \
    "MjAwNDQyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMFkwEwYH\n" \
    "KoZIzj0CAQYIKoZIzj0DAQcDQgAEyza/tGLVbVxhL41iYtC8D6tGEvAHu498gNtq\n" \
    "DtPsKaoR3t5xQx+6zdWiCi32fgFT2vkeVAmX3pf/Gl8nIP48ZqMkMCIwCwYDVR0P\n" \
    "BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0cAMEQCIDkf\n" \
    "83Oq8sOXhSyJCWAN63gc4vp9//RFCXh/hUXPYcTWAiBgmQ5JV2MZH01Upi2lMflN\n" \
    "YLbC+lYscwcSlB2tECUbJA==\n"                                         \
    "-----END CERTIFICATE-----\n"
#endif

#endif /* ifndef AWS_TEST_PAL_ECDSA_SHA256_SIGNATURE_H_ */
