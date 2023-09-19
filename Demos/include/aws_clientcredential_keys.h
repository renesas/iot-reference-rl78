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
#define keyCLIENT_CERTIFICATE_PEM \
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
#define keyCLIENT_PRIVATE_KEY_PEM \
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

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
