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
#define MQTT_SERVER_ENDPOINT   "a3lklnx40j1phd-ats.iot.ap-northeast-1.amazonaws.com"

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
#define ECHO_SERVER_ENDPOINT    "192.168.1.1"

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
/**
 * @brief Port of the MQTT broker to connect to in mqtt test.
 *
 * #define MQTT_SERVER_PORT       (8883)
 */
#define TRANSPORT_CLIENT_PRIVATE_KEY  NULL

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
#ifdef RL78_DBG
#define MQTT_CLIENT_CERTIFICATE \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUFeYR3JSsJbTOS7huEq++YBGgwtowDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDMzMTA0MDE0\n"\
"MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANxW+zu40FuJYB5jxNwC\n"\
"J1E1lAmke5Fnlh7mxJPWGzX98cMXYHQuTc11BNTJU032yHdBbe8H3JChupEcnD3R\n"\
"I/HEy+2VWOLjpIHRcQCh9zcppD4L1cV5KkYvlcpaP+6obvGlEVe87ZmLrj7nSYEi\n"\
"64C2X3/fdeDzCbWjP4AVgJaNryCYioiOG4uo7CpR3QhCnDRKt8jXXItdirhuRw6/\n"\
"v9oI9G9AAkziK18AbTOChKW9MZx4NBdNcAIv9K2w/Pr9/ESw/664lVWfK38IJUdc\n"\
"2x3CcyQ91r7k7ppKXJNgEya/6HlKByYdn91IeyNQdcZ7rWlF1OF3kTxFXrGhAid7\n"\
"XR8CAwEAAaNgMF4wHwYDVR0jBBgwFoAU9Im/0dcvs3UhepaPnYy7DvEnvGcwHQYD\n"\
"VR0OBBYEFALS+2jatb5tYzjLybrI5yTgcG86MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBfQx3v7fq57niIHSWwXnSSV4te\n"\
"uUn5Ts8imQWrBEyZpi8Hf9LiLr8fKwWqW8G8Ipa9pzBAdH4NTBLKidQaVcSKp2pb\n"\
"Oeyp3h2Z/WsvsA3Jc4lP+Pno8gMsArFvPao8a10/gIobDfg0ytqn9uYQLoOxfdAa\n"\
"VP7uEmiimvqpvuZ+UmVrD5E1kEktelEPhkGilwahMROuaZTwvLiYo+EjKIlklMPL\n"\
"ZzOIZ0OdfnngY1eCT5SD/+MXfsxOYd3OqdMUqr74o+LZgz4TZ6hijN+0ujwG37W6\n"\
"7qHumsC6fsEapoptgcfEpdERl4c9hJR45jHamDVhxZjitQD4klLA0gqTlBNL\n"\
"-----END CERTIFICATE-----\n"
#else
#define MQTT_CLIENT_CERTIFICATE "PLACE_HOLDER"
#endif

	/**
 * @brief Client private key to connect to MQTT server.
	 *
	 * @note This is should only be used for testing purpose.
	 *
	 * For qualification, the key should be generated on-device.
	 *
	 * #define MQTT_CLIENT_PRIVATE_KEY  NULL
	 */
#ifdef RL78_DBG
#define MQTT_CLIENT_PRIVATE_KEY  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEA3Fb7O7jQW4lgHmPE3AInUTWUCaR7kWeWHubEk9YbNf3xwxdg\n"\
"dC5NzXUE1MlTTfbId0Ft7wfckKG6kRycPdEj8cTL7ZVY4uOkgdFxAKH3NymkPgvV\n"\
"xXkqRi+Vylo/7qhu8aURV7ztmYuuPudJgSLrgLZff9914PMJtaM/gBWAlo2vIJiK\n"\
"iI4bi6jsKlHdCEKcNEq3yNdci12KuG5HDr+/2gj0b0ACTOIrXwBtM4KEpb0xnHg0\n"\
"F01wAi/0rbD8+v38RLD/rriVVZ8rfwglR1zbHcJzJD3WvuTumkpck2ATJr/oeUoH\n"\
"Jh2f3Uh7I1B1xnutaUXU4XeRPEVesaECJ3tdHwIDAQABAoIBAE9Vrj3xrGvZCdfH\n"\
"WoFQ8TEnVncmVyuinYXg11TXmZKWLe9twlfyuCbLBINNPgZrXgC6eUfAj+UvpkpL\n"\
"f+IuJIAPuNGBnbCMAJaRf9uXdbjR8IY6oqzolJUS2x8x4aUf1lqBcb4ik3Zeuc2C\n"\
"X4/0r9D3WWlkX0AwogVZkyqeE1+hVrzphE4mwZb7z2anz2LB1Q5yCmagf8labHxh\n"\
"tyYSv+vUHyfCrMVeqrL0yiovsWCPzoSn1nZFj1rxMIAsI/9hDbt7gXxMAM+EhfTy\n"\
"0J1c4w229kw4Rv0Kn9Pu8L3lHlNPaV+OiQwLsAx1r2f9DpMHaKhXFfDWiIe90ihr\n"\
"H5F1S9ECgYEA/zoIhITbU3p7NsgKYWvphpZXCR18MqxL5vRMK9I2uVXJpnG5tDEN\n"\
"DHetx1OizXaDcUAw8WWGdvvHpjtwZPnM9wviFqo3KARZc58DVXYjMDYVnOPu4n4E\n"\
"JW6XgAhZ7Ud+TOwQVoeCHkwka/BK/3nY9VS7l+RkeBuwxPMgO3VeCRkCgYEA3QHj\n"\
"VxdmMQhMw7tbUy6LZ0MoK68h8TsvIOuhtGRMHsHpXwY3ZVBbmiaF2mejRUTYZ2fk\n"\
"wz26HC4/0xoZO3rqNXX7dLWYwDf4H0q6/X0sg6iAkAvhbs8s9PjlZh5zS7KEvmB0\n"\
"BH8YLg/WIjXC1VLJfwdno2Q97OTBlLfygTEaBvcCgYAtY522K4Zxzxtwf5zQ/Ib4\n"\
"R25NEbBPtrJ87T9Kn8TZqvcEob7726lNaU8/XpFaZtQzZM4PcBW8vUQhPPrdCku4\n"\
"N6XKkBTlCfj6hfHq1eqmHUg0sZh/MRf9rDBHNbUAbLF6bpTtPoYWa0Yi6AX+byX1\n"\
"Y09FUPnJHmuOawp1aIcDqQKBgGFKHXwPTusyzigz6AX8LV3t0Fc6p2R8O0jwsaSp\n"\
"qmC32DX+lJBDCbAYWUBF8zfhEKqkr63LFIynR210xbrLZyLVpzY/YtjrSozr5tkE\n"\
"K35hxQakVD7lcNL+Zm3H5dVi5tCvwSLhmBFe41L3+6bQ5ObIqbThwejw4s7HEzgQ\n"\
"F3cNAoGBAJrUm62sAhzP31jR2llO6g/dCMPlk3dWm04suUDvl/IEhUL8mXykpU6M\n"\
"fY9Dal2cBdQBr52idT7h828kILHI7riORQQ6bqk19Q4vwb0V2eMeQTt5B4XzhsW/\n"\
"s/OlVUiygf0RgeoMVx/3GzZPfmTrB0cQ8XZ7mxCd2dgY9UXQ/oja\n"\
"-----END RSA PRIVATE KEY-----\n"
#else
#define MQTT_CLIENT_PRIVATE_KEY "PLACE_HOLDER"
#endif

/**
 * @brief The IoT Thing name for the device for OTA test and MQTT test.
 *
 * #define IOT_THING_NAME  "PLACE_HOLDER"
 */
#ifdef RL78_DBG
#define IOT_THING_NAME                                   "rx-ota-firm-things-rx65n-rsk"
#else
#define IOT_THING_NAME "PLACE_HOLDER"
#endif

/**
 * @brief The MQTT client identifier used in MQTT test.  Each client identifier
 * must be unique; so edit as required to ensure that no two clients connecting to
 * the same broker use the same client identifier.
 *
 * #define MQTT_TEST_CLIENT_IDENTIFIER				"insert here."
 */
#define MQTT_TEST_CLIENT_IDENTIFIER	IOT_THING_NAME

/**
 * @brief Network buffer size specified in bytes. Must be large enough to hold the maximum
 * anticipated MQTT payload.
 *
 * #define MQTT_TEST_NETWORK_BUFFER_SIZE			( 5000 )
 */
#define MQTT_TEST_NETWORK_BUFFER_SIZE    ( 5000 )

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
