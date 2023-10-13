/*
#include <ota_demo_config_.h>
 * FreeRTOS OTA PAL for Renesas RX65N-RSK V2.0.0
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

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS include. */
#include "FreeRTOS.h"
#include "task.h"

/* OTA library includes. */
#include "ota.h"
#include "ota_pal.h"

/* OTA PAL test config file include. */
#include "r_fwup_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

/* The static functions below (otaPAL_CheckFileSignature and otaPAL_ReadAndAssumeCertificate)
 * are optionally implemented. If these functions are implemented then please set the following
 * macros in aws_test_ota_config.h to 1:
 *   otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 *   otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 *
 * This function should be implemented if signature verification is not offloaded to non-volatile
 * memory io functions.
 *
 * This function is expected to be called from otaPal_CloseFile().
 *
 * @param[in] pFileContext OTA file context information.
 *
 * @return The OtaPalStatus_t error code combined with the MCU specific error code. See
 *         ota_platform_interface.h for OTA major error codes and your specific PAL implementation
 *         for the sub error code.
 *
 * Major error codes returned are:
 *   OtaPalSuccess: if the signature verification passes.
 *   OtaPalSignatureCheckFailed: if the signature verification fails.
 *   OtaPalBadSignerCert: if the signature verification certificate cannot be read.
 */
static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const pFileContext );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 *
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 *
 * This function is called from otaPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 *
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 *
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );
#endif

static OtaImageState_t s_OtaImageState;

/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const pFileContext )
{
    static uint8_t hdl;
    OtaPalMainStatus_t eResult;

    LogDebug( ( "otaPal_CreateFileForRx is called." ) );

    s_OtaImageState = OtaImageStateUnknown;

    if (pFileContext->pFilePath == NULL)
    {
        eResult = OtaPalNullFileContext;
    }
    else
    {
        pFileContext->pFile = &hdl;

        if (FWUP_SUCCESS != R_FWUP_Open())
        {
            eResult = OtaPalRxFileCreateFailed;
        }
        else
        {
            eResult = OtaPalSuccess;
        }
    }

    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const pFileContext )
{
    LogDebug( ( "otaPal_Abort is called." ) );
    OtaPalMainStatus_t eResult;

    pFileContext->pFile = NULL;
    eResult = OtaPalSuccess;

    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t otaPal_WriteBlock( OtaFileContext_t * const pFileContext,
                           uint32_t ulOffset,
                           uint8_t * const pData,
                           uint32_t ulBlockSize )
{
    LogDebug( ( "otaPal_WriteBlock is called." ) );

    if (FWUP_ERR_FLASH == R_FWUP_WriteImageProgram(FWUP_AREA_BUFFER, pData, ulOffset+512, ulBlockSize))
    {
        return 0;
    }
    return ulBlockSize;
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const pFileContext )
{
    OtaPalMainStatus_t eResult = OtaPalSignatureCheckFailed;

    LogDebug( ( "otaPal_CloseFile is called." ) );

    eResult = OTA_PAL_MAIN_ERR( otaPal_CheckFileSignature( pFileContext ) );
    if (OtaPalSuccess == eResult)
    {
        s_OtaImageState = OtaImageStateTesting;
    }
    else
    {
        s_OtaImageState = OtaImageStateRejected;
    }
    R_FWUP_Close();
    pFileContext->pFile = NULL;
    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const pFileContext )
{
    OtaPalMainStatus_t eResult;
    uint8_t sig[64];
    uint16_t idx;

    /*
     * pFileContext->pSignature->data includes ASN1 tags(^^^) so need to remove it.
     * 30 46 02 21 00 32byte(R) 02 21 00 32byte(S)
     * ^^^^^^^^^^^^^^           ^^^^^^^^
     */
    LogDebug( ( "otaPal_CheckFileSignature is called.  signature size = %d", pFileContext->pSignature->size ) );

    /* SIG(R) */
    idx = 3;
    if (0x21 == pFileContext->pSignature->data[idx++])
    {
        idx++; /* Skip 0x00 */
    }
    memcpy(sig, &pFileContext->pSignature->data[idx], 32);

    /* SIG(S) */
    idx += 32;
    idx++;
    if (0x21 == pFileContext->pSignature->data[idx++])
    {
        idx++; /* Skip 0x00 */
    }
    memcpy(sig+32, &pFileContext->pSignature->data[idx], 32);

    /* Write signature to header part. */
    R_FWUP_WriteImageHeader(FWUP_AREA_BUFFER, (uint8_t __far *)OTA_JsonFileSignatureKey, sig, 64);
    if (FWUP_SUCCESS == R_FWUP_VerifyImage(FWUP_AREA_BUFFER))
    {
        eResult = OtaPalSuccess;
    }
    else
    {
        eResult = OtaPalSignatureCheckFailed;
    }
    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    uint8_t * pucSignerCert = NULL;
    return pucSignerCert;
}
#endif
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_ResetDevice( OtaFileContext_t * const pFileContext )
{
    ( void ) pFileContext;

    LogDebug( ( "otaPal_ResetDevice is called." ) );

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    R_FWUP_SoftwareReset();
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const pFileContext )
{
    LogDebug( ( "otaPal_ActivateNewImage is called." ) );

    /* reset for self testing */
    R_FWUP_ActivateImage();
    otaPal_ResetDevice( pFileContext ); /* no return from this function */
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const pFileContext,
                                             OtaImageState_t eState )
{
    LogDebug( ( "otaPal_SetPlatformImageState is called.  OtaImageState_t = %d", eState) );

    s_OtaImageState = eState;
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}
/*-----------------------------------------------------------*/

OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const pFileContext )
{
    OtaPalImageState_t ePalState;

    switch (s_OtaImageState)
    {
        case OtaImageStateTesting:
            ePalState = OtaPalImageStatePendingCommit;
            break;
        case OtaImageStateAccepted:
            ePalState = OtaPalImageStateValid;
            break;
        case OtaPalImageStateUnknown:
            ePalState = OtaPalImageStateValid;
            break;
        default:
            ePalState = OtaPalImageStateInvalid;
            break;
    }
    LogDebug( ( "otaPal_GetPlatformImageState is called.  OtaPalImageState_t = %d",ePalState ) );
    return ePalState;
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
