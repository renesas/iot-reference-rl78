/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Modifications Copyright (C) 2023 Renesas Electronics Corporation. or its affiliates.
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/**
 * @file wifi_setup.c
 * @brief Setup wifi connectivity for board with Wi-Fi DA16XXX module.
 */

/* FreeRTOS include. */
#include <FreeRTOS.h>
#include "task.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* The config header is always included first. */
#include "iot_wifi.h"

/* Demo Specific configs. */
#include "aws_clientcredential.h"
#include "demo_config.h"

/* Write certificate */
#include "cert_profile_helper.h"

bool setupWifi (void);
static bool _wifiConnectAccessPoint( void );

bool setupWifi(void)
{
    bool ret = true;

    if( WIFI_On() != eWiFiSuccess )
    {
        return false;
    }

    ret = _wifiConnectAccessPoint();

    return ret;
}

static bool _wifiConnectAccessPoint( void )
{
    bool status = true;
    WIFINetworkParams_t xConnectParams;
    size_t xSSIDLength, xPasswordLength;
    const char * pcSSID = clientcredentialWIFI_SSID;
    const char * pcPassword = clientcredentialWIFI_PASSWORD;
    WIFISecurity_t xSecurity = clientcredentialWIFI_SECURITY;

    if( NULL != pcSSID )
    {
        xSSIDLength = sizeof(clientcredentialWIFI_SSID) - 1; // excluding NULL terminator

        if( ( 0 < xSSIDLength ) && ( wificonfigMAX_SSID_LEN > xSSIDLength ) )
        {
            xConnectParams.ucSSIDLength = xSSIDLength;
        }
        else
        {
            status = false;
        }
    }
    else
    {
        status = false;
    }

    xConnectParams.xSecurity = xSecurity;
    switch( xSecurity )
    {
        case eWiFiSecurityWPA:
        case eWiFiSecurityWPA2:
            if( NULL != pcPassword )
            {
                xPasswordLength = sizeof(clientcredentialWIFI_PASSWORD) - 1; // excluding NULL terminator

                if( ( 0 < xPasswordLength ) && ( wificonfigMAX_PASSPHRASE_LEN > xPasswordLength ) )
                {
                    xConnectParams.xPassword.xWPA.ucLength = xPasswordLength;
                }
                else
                {
                    status = false;
                }
            }
            else
            {
                status = false;
            }
            break;
        case eWiFiSecurityOpen:
            /* Nothing to do. */
            break;
        case eWiFiSecurityWPA3:
        case eWiFiSecurityWPA2_ent:
        case eWiFiSecurityWEP:
        default:
            configPRINT_STRING( "The configured WiFi security option is not supported." );
            status = false;
            break;
    }

    if( status == true )
    {
        if( eWiFiSuccess != WIFI_ConnectAP( &( xConnectParams ), pcSSID, pcPassword ) )
        {
            status = false;
        }
    }

    return status;
}

