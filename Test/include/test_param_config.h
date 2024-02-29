/*
 * FreeRTOS FreeRTOS LTS Qualification Tests preview
 * Copyright (C) 2022 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file test_param_config.h
 * @brief This setup the test parameters for LTS qualification test.
 */

#ifndef TEST_PARAM_CONFIG_H
#define TEST_PARAM_CONFIG_H

/**
 * @brief Flag to enable or disable provisioning mode for the demo.
 * Enabling the flags starts a CLI task, so that user can perform provisioning of the device through
 * a serial terminal. Provisioning involves running commands to fetch or set the PKI and configuration
 * information for the device to connect to broker and perform OTA updates. Disabling the flag results
 * in disabling the CLI task and execution of the demo tasks in normal device operation mode.
 */
#define appmainPROVISIONING_MODE                  ( 0 )

/**
 * @brief Configuration that indicates if the device should generate a key pair.
 *
 * @note When FORCE_GENERATE_NEW_KEY_PAIR is set to 1, the device should generate
 * a new on-device key pair and output public key. When set to 0, the device
 * should keep existing key pair.
 *
 * #define FORCE_GENERATE_NEW_KEY_PAIR   0
 */

/**
 * @brief Endpoint of the MQTT broker to connect to in mqtt test.
 *
 * #define MQTT_SERVER_ENDPOINT   "PLACE_HOLDER"
 */
#define MQTT_SERVER_ENDPOINT   "PLACE_HOLDER"

/**
 * @brief Port of the MQTT broker to connect to in mqtt test.
 *
 * #define MQTT_SERVER_PORT       (8883)
 */
#define MQTT_SERVER_PORT        ( 8883 )

/**
 * @brief Endpoint of the echo server to connect to in transport interface test.
 *
 * #define ECHO_SERVER_ENDPOINT   "PLACE_HOLDER"
 */
#define ECHO_SERVER_ENDPOINT    "PLACE_HOLDER"

/**
 * @brief Port of the echo server to connect to in transport interface test.
 *
 * #define ECHO_SERVER_PORT       (9000)
 */
#define ECHO_SERVER_PORT        ( 9000 )

/**
 * @brief Root certificate of the echo server.
 *
 * @note This certificate should be PEM-encoded.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 *
 * #define ECHO_SERVER_ROOT_CA "PLACE_HOLDER"
 */
#define ECHO_SERVER_ROOT_CA "PLACE_HOLDER"

/**
 * @brief Client certificate to connect to echo server.
 *
 * @note This certificate should be PEM-encoded.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 *
 * #define TRANSPORT_CLIENT_CERTIFICATE NULL
 */
#define TRANSPORT_CLIENT_CERTIFICATE NULL

/**
 * @brief Client private key to connect to echo server.
 *
 * @note This is should only be used for testing purpose.
 *
 * For qualification, the key should be generated on-device.
 *
 * #define TRANSPORT_CLIENT_PRIVATE_KEY  NULL
 */
/* 41167f58-633c-499f-a17f-4631918c6422:secp256r1.privatekey */
#define TRANSPORT_CLIENT_PRIVATE_KEY \
"-----BEGIN EC PRIVATE KEY-----\n"\
"MHcCAQEEIEwCUqRHaPE3ZA8Bvh4TnEutWdE7EupG7ILUvTRZIloSoAoGCCqGSM49\n"\
"AwEHoUQDQgAEedv40AoIGSF0wzc5D/n9v4dHh9071+Io02XDFmpUrxeig3FxHOiG\n"\
"NKHL57u5RA45NS/uUL20QEFNhbtpsOcksw==\n"\
"-----END EC PRIVATE KEY-----\n"

/**
 * @brief Client certificate to connect to MQTT server.
 *
 * @note This certificate should be PEM-encoded.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 *
 * #define MQTT_CLIENT_CERTIFICATE NULL
 */
/* 0863f9634be45be0fd834b0473163711cbf550013478f9dc485d5c9a06e637eb-certificate.pem.crt */
#define MQTT_CLIENT_CERTIFICATE  \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVAMcHxI2qYJPnErEl5KfuRvn0sbh0MA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzEyMjEwNjI1\n"\
"MTZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC3MnS9G4LThFOsKrxb\n"\
"EHVkZbrrqoSi9xTkt/BYu+yOuTEyMh+XkXji8u13GTANOLgu8znRTM2FHz4KwstM\n"\
"nTzXJrLyQMfig975DHdMkiwblMgaCknwhcnEhZBKKnCH9CLf3DSjTC4Jm8xISajo\n"\
"xoF/GaPthugcw7Nof1jQfJ5MlrnMKjpQUGfTCaGUqdDo5dKmGNrs3YB7uxemqZBh\n"\
"vACSYn7g+Yko+rz6oYOObeZ27gBbCbAeKjgs066vwuWtUlXkXLeORX1YOa09RGzR\n"\
"Q6W0x0+QtKRjwuQHl1cZdqPPX+jviqF9vZQn+M2ZPhciUOEBtXjRigBV+pn6JM6M\n"\
"fz99AgMBAAGjYDBeMB8GA1UdIwQYMBaAFPEZuaCxxuqOO+kqNrk5Lj8Av+3NMB0G\n"\
"A1UdDgQWBBR29iImF3Vjz0JtKLcxOIPD8/ohUjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAgZWxsBuKe4ESp5WKvTtfCgPV\n"\
"/55AJ2FX9Qjb3mr1z055Anbf/icutiq99G+hfis9ijrTipCZUxe4zMl5St6Z1W0z\n"\
"2ABQJ4Y3Py7VL2r9mrmgwHKL8Syxv+8z0zpQpjaMKgosgMbK99QskfILX3fhzHUb\n"\
"TECl71cCovYqWNiOfGi3fZv3aAvaDmTGf7zU4mmiZsscOqJyYt4kZWZ8bFfEmBB0\n"\
"OiR5cxF4RCBwsNPFLI0UqWvCnS9taTy9J6dFIw7jtNqrXmYHUSLx3dZihFX7tVF8\n"\
"krAfosWyyF1xqlJuVsgqCdRDUbGxZII6Z3hmawubrCKhDeuKhbDjwubwz0Et7g==\n"\
"-----END CERTIFICATE-----\n"

/**
 * @brief Client private key to connect to MQTT server.
 *
 * @note This is should only be used for testing purpose.
 *
 * For qualification, the key should be generated on-device.
 *
 * #define MQTT_CLIENT_PRIVATE_KEY  NULL
 */
/* 0863f9634be45be0fd834b0473163711cbf550013478f9dc485d5c9a06e637eb-private.pem.key */
#define MQTT_CLIENT_PRIVATE_KEY  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAtzJ0vRuC04RTrCq8WxB1ZGW666qEovcU5LfwWLvsjrkxMjIf\n"\
"l5F44vLtdxkwDTi4LvM50UzNhR8+CsLLTJ081yay8kDH4oPe+Qx3TJIsG5TIGgpJ\n"\
"8IXJxIWQSipwh/Qi39w0o0wuCZvMSEmo6MaBfxmj7YboHMOzaH9Y0HyeTJa5zCo6\n"\
"UFBn0wmhlKnQ6OXSphja7N2Ae7sXpqmQYbwAkmJ+4PmJKPq8+qGDjm3mdu4AWwmw\n"\
"Hio4LNOur8LlrVJV5Fy3jkV9WDmtPURs0UOltMdPkLSkY8LkB5dXGXajz1/o74qh\n"\
"fb2UJ/jNmT4XIlDhAbV40YoAVfqZ+iTOjH8/fQIDAQABAoIBAE1BHNOmp1v1YKeo\n"\
"+fdZEHg/Ij1Bkl963Le7DcMNtKtXU3S8iHANa7IhCZ0A5S+eLKW38jjeL4kk7K/z\n"\
"B3KOmFqOukXIpcJTJBNYe3Kx7aHP8zZwAlBIyxZx0LBGB2B9xrsqjJQUixGIPvR5\n"\
"A57uHyvvLx76liWTLbQyh8yb0NGwwoW8CAkVOnwpsQ5cbXCd4BEr22hrdQWkCZxE\n"\
"7KDAOg0AtJgZYgU5UOtpB4a3/pAddvv4w9kCZipbqIW+kwhYO1X1Iz3rpiDWL6M2\n"\
"O8gthy6AlX9Z00U8pZQMBa/SwGjvno1/zE2ToFfm3EZts3kcBpQwyoM86zARJour\n"\
"K5PhpsECgYEA8PuDxXX/3n+2yPISSrxmBW7P8nw/riwu9Fut8TqBJQ+4jTMHAgzV\n"\
"mi+CzOZuIvYVQ5LfRIYzeheF+Be+Vx+2J1p+MuW+2zT7/WxPnc4gMuQmCvs/Lk58\n"\
"tlOFUPVxXJNrcrosPd3vrJ1lR1vX8OFRNvgBOHOj00qjxReM7tfaf+0CgYEAwp0R\n"\
"qvwAYQARDwuQehN1ZJvtaZmPWUVbjSgARcvviHTrfg1M4v4jpwsEKEAEiA/dwcxh\n"\
"9mthBh+WSh76XqL+6yFemrZ8dlHGbAa89mA/KO71gU/yW2e+LoMZ1cohAmUXZaUt\n"\
"r7mhS3vMy6dW7Awcpp4tKdRsfBXLPhrGkOogK9ECgYEA5gKbBmkXfO6Xr3rmHLFw\n"\
"CzV2g/M18O+5LYEoKDx4McCsnREcWkeXxaAdj6dNbk9Fgob4VfZlwd4b/+3hvlTF\n"\
"TxCfrcL+BUqwTnYIFA4/S3PaH9cpufAuNvTHJbvvpLYM86dhPPfNoQtmNNMj7IWT\n"\
"OA1FVPWDY9Q1NpTA7uPWaQ0CgYBL4JGS6M39A9mi1A9wiUWsBF98XLLXS4N2dbIl\n"\
"hG3P9kYaz3S6y/t6lYD5cHxT0FvVw6ouOY+Ko6YlIA7d9w8M/eW0zJGaUCfc1oO2\n"\
"S7BUdVajJFiJ6tjrqZHZGQXluSR2LQCzEt0gYRRoEsaBIffJvsI9HOVvO9ASVr40\n"\
"ohQJAQKBgGDp/hqvxez610mYQkBP39ut9zegyoEe+xwCcJBKOrIwmqIm6CXXUYk6\n"\
"0ZjceVlqEiWHWBzDgOOH5YBvIiVwEjnZjxtmT+CpNbHxotbyVNY5BhpggOtTc2KG\n"\
"EpfxudEMOf6KUvLmDOeTFOpvHbeWZ6TEAD6kGcXA6mntCwzm8Ze5\n"\
"-----END RSA PRIVATE KEY-----\n"

/**
 * @brief The IoT Thing name for the device for OTA test and MQTT test.
 *
 * #define IOT_THING_NAME  "PLACE_HOLDER"
 */
#define IOT_THING_NAME "PLACE_HOLDER"

/**
 * @brief The MQTT client identifier used in MQTT test.  Each client identifier
 * must be unique; so edit as required to ensure that no two clients connecting to
 * the same broker use the same client identifier.
 *
 * #define MQTT_TEST_CLIENT_IDENTIFIER              "insert here."
 */
#define MQTT_TEST_CLIENT_IDENTIFIER             IOT_THING_NAME

/**
 * @brief Network buffer size specified in bytes. Must be large enough to hold the maximum
 * anticipated MQTT payload.
 *
 * #define MQTT_TEST_NETWORK_BUFFER_SIZE            ( 5000 )
 */
#define MQTT_TEST_NETWORK_BUFFER_SIZE           ( 5000 )

/**
 * @brief Client private key to connect to echo server.
 *
 * @note This is should only be used for testing purpose.
 *
 * For qualification, the key should be generated on-device.
 *
 * #define TRANSPORT_CLIENT_PRIVATE_KEY  NULL
 */
#define PKCS11_TEST_RSA_KEY_SUPPORT                      ( 0 )
#define PKCS11_TEST_EC_KEY_SUPPORT                       ( 0 )
#define PKCS11_TEST_IMPORT_PRIVATE_KEY_SUPPORT           ( 0 )
#define PKCS11_TEST_GENERATE_KEYPAIR_SUPPORT             ( 0 )
#define PKCS11_TEST_PREPROVISIONED_SUPPORT               ( 0 )

#define PKCS11_TEST_JITP_CODEVERIFY_ROOT_CERT_SUPPORTED   (0)

#define OTA_RSA_SHA1                                     1
#define OTA_RSA_SHA256                                   2
#define OTA_ECDSA_SHA256                                 3

/**
 * @brief Certificate type for OTA PAL test.
 * Valid options are: OTA_RSA_SHA1, OTA_RSA_SHA256, OTA_ECDSA_SHA256.
 *
 * #define OTA_PAL_TEST_CERT_TYPE OTA_ECDSA_SHA256
 */
#define OTA_PAL_TEST_CERT_TYPE                           OTA_ECDSA_SHA256

/**
 * @brief Path to cert for OTA test PAL. Used to verify signature.
 * If applicable, the device must be pre-provisioned with this certificate. Please see
 * test/common/ota/test_files for the set of certificates.
 */
#define OTA_PAL_CERTIFICATE_FILE                         "ecdsa-sha256-signer.crt.pem"

/**
 * @brief Some devices have a hard-coded name for the firmware image to boot.
 */
#define OTA_PAL_FIRMWARE_FILE                            "dummy.bin"

/**
 * @brief Some boards OTA PAL layers will use the file names passed into it for the
 * image and the certificates because their non-volatile memory is abstracted by a
 * file system. Set this to 1 if that is the case for your device.
 */
#define OTA_PAL_USE_FILE_SYSTEM                          0



/**
 * @brief 1 if using PKCS #11 to access the code sign certificate from NVM.
 */
#define OTA_PAL_READ_CERTIFICATE_FROM_NVM_WITH_PKCS11    0

#define OTA_APP_VERSION_MAJOR                            0

#define OTA_APP_VERSION_MINOR                            9

#define OTA_APP_VERSION_BUILD                            1

#define OUTGOING_PUBLISH_RECORD_COUNT                    ( 10 )
#define INCOMING_PUBLISH_RECORD_COUNT                    ( 10 )
#endif /* TEST_PARAM_CONFIG_H */
