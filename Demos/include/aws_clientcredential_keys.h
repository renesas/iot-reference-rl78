/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*
 ****************************************************************************
 * NOTE!
 * This file is for ease of demonstration only.  Secret information should not
 * be pasted into the header file in production devices.  Do not paste
 * production secrets here!  Production devices should store secrets such as
 * private keys securely, such as within a secure element.  See our examples that
 * demonstrate how to use the PKCS #11 API for secure keys access.
 ****************************************************************************
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
/* 0863f9634be45be0fd834b0473163711cbf550013478f9dc485d5c9a06e637eb-certificate.pem.crt */
#define keyCLIENT_CERTIFICATE_PEM \
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

/*
 * @brief PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 *
 * @todo If you are using AWS IoT Just in Time Registration (JITR), set this to
 * the issuer (Certificate Authority) certificate of the client certificate above.
 *
 * @note This setting is required by JITR because the issuer is used by the AWS
 * IoT gateway for routing the device's initial request. (The device client
 * certificate must always be sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM    NULL

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 * Please note pasting a key into the header file in this manner is for
 * convenience of demonstration only and should not be done in production.
 * Never past a production private key here!.  Production devices should
 * store keys securely, such as within a secure element.  Additionally,
 * we provide the corePKCS library that further enhances security by
 * enabling keys to be used without exposing them to software.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
/* 0863f9634be45be0fd834b0473163711cbf550013478f9dc485d5c9a06e637eb-private.pem.key */
#define keyCLIENT_PRIVATE_KEY_PEM \
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

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
