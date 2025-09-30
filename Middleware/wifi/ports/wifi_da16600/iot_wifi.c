/*
 * FreeRTOS Wi-Fi V1.0.0
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

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */
#include <stdio.h>
#include <string.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "iot_wifi_config.h"

/* WiFi configuration includes. */
#include "platform.h"
#include "r_sci_rl_if.h"
#include "r_wifi_da16xxx_if.h"
#include "user_tcp_hook_config.h"

#define _NM_WIFI_CONNECTION_RETRIES              (USER_RECONNECT_TRIES)
#define _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS    (1000)

/**
 * @brief Wi-Fi initialization status.
 */
static uint32_t prvConvertSecurityFromDaAT (WIFISecurity_t xSecurity);
static uint32_t prvConvertEncryptionFromDaAT (WIFIEncryption_t xEncryption);

/**
 * @fn prvConvertSecurityFromDaAT
 *
 * @brief Converts a Wi-Fi security enum from the application layer to the port layer Wi-Fi driver format.
 *
 * @param[in] xSecurity        The Wi-Fi security type from the application layer
 * @return uint32_t            The corresponding security type used by the port layer driver
 */
static uint32_t prvConvertSecurityFromDaAT(WIFISecurity_t xSecurity)
{
    uint32_t xConvertedSecurityType = WIFI_SECURITY_UNDEFINED;

    switch (xSecurity)
    {
        case eWiFiSecurityOpen:
            xConvertedSecurityType = WIFI_SECURITY_OPEN;
            break;

        case eWiFiSecurityWEP:
            xConvertedSecurityType = WIFI_SECURITY_WEP;
            break;

        case eWiFiSecurityWPA:
            xConvertedSecurityType = WIFI_SECURITY_WPA;
            break;

        case eWiFiSecurityWPA2:
            xConvertedSecurityType = WIFI_SECURITY_WPA2;
            break;

        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = WIFI_SECURITY_UNDEFINED;
            break;
        default:
            break;
    }

    return xConvertedSecurityType;
} /* End of function prvConvertSecurityFromDaAT()*/

/*-----------------------------------------------------------*/

/**
 * @fn prvConvertEncryptionFromDaAT
 *
 * @brief Converts a Wi-Fi Encryption enum from the application layer to the port layer Wi-Fi driver format.
 *
 * @param[in] xEncryption      The Wi-Fi encryption type from the application layer
 * @return uint32_t            The corresponding encryption type used by the port layer driver
 */
static uint32_t prvConvertEncryptionFromDaAT(WIFIEncryption_t xEncryption)
{
    uint32_t xConvertedEncryptionType = WIFI_ENCRYPTION_UNDEFINED;

    switch (xEncryption)
    {
        case eWiFiEncryptionTKIP:
            xConvertedEncryptionType = WIFI_ENCRYPTION_TKIP;
            break;

        case eWiFiEncryptionAES:
            xConvertedEncryptionType = WIFI_ENCRYPTION_AES;
            break;

        case eWiFiEncryptionTKIP_AES:
            xConvertedEncryptionType = WIFI_ENCRYPTION_TKIP_AES;
            break;

        case eWiFiEncryptionUndefined:
            xConvertedEncryptionType = WIFI_ENCRYPTION_UNDEFINED;
            break;
        default:
            break;
    }

    return xConvertedEncryptionType;
} /* End of function prvConvertEncryptionFromDaAT()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_On
 *
 * @brief Turns on Wi-Fi.
 *
 * This function turns on Wi-Fi module,initializes the drivers and must be called
 * before calling any other Wi-Fi API
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully turned on, failure code otherwise.
 */
WIFIReturnCode_t WIFI_On(void)
{
    /* FIX ME. */
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    if (WIFI_SUCCESS == R_WIFI_DA16XXX_Open())
    {
        xRetVal = eWiFiSuccess;
    }

    return xRetVal;
} /* End of function WIFI_On()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_Off
 *
 * @brief Turns off Wi-Fi.
 *
 * This function turns off the Wi-Fi module. The Wi-Fi peripheral should be put in a
 * low power or off state in this routine.
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully turned off, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Off(void)
{
    /* FIX ME. */
    R_WIFI_DA16XXX_Close();

    return eWiFiSuccess;
} /* End of function WIFI_Off()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_ConnectAP
 *
 * @brief Connects to the Wi-Fi Access Point (AP) specified in the input.
 *
 * The Wi-Fi should stay connected when the same Access Point it is currently connected to
 * is specified. Otherwise, the Wi-Fi should disconnect and connect to the new Access Point
 * specified. If the new Access Point specifed has invalid parameters, then the Wi-Fi should be
 * disconnected.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return @ref eWiFiSuccess if connection is successful, failure code otherwise.
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_ConnectAP(const WIFINetworkParams_t * const pxNetworkParams,
                                const uint8_t * ssid, const uint8_t * password)
{
    wifi_err_t      ret;
    wifi_security_t convert_security;
    uint32_t        numRetries        = _NM_WIFI_CONNECTION_RETRIES;
    uint32_t        delayMilliseconds = _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS;

    /* Cast to proper datatype to avoid warning */
    convert_security = (wifi_security_t)prvConvertSecurityFromDaAT(pxNetworkParams->xSecurity);

    if ((NULL == pxNetworkParams) || (0 == pxNetworkParams->ucSSIDLength) || (pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN))
    {
        return eWiFiFailure;
    }

    if ((eWiFiSecurityOpen != pxNetworkParams->xSecurity) && (0 == pxNetworkParams->xPassword.xWPA.ucLength))
    {
        return eWiFiFailure;
    }

    if (pxNetworkParams->xPassword.xWPA.ucLength > wificonfigMAX_PASSPHRASE_LEN)
    {
        return eWiFiFailure;
    }

    if (pdTRUE == WIFI_IsConnected())
    {
        WIFI_Disconnect();
    }

    /* FIX ME. */
    do
    {
        ret = R_WIFI_DA16XXX_Connect(ssid, password, convert_security, WIFI_ENCRYPTION_AES);
        if ((WIFI_ERR_NOT_CONNECT == ret) || (WIFI_ERR_MODULE_COM == ret))
        {
            /* Start resetting due to error */
            configPRINTF(("Start resetting due to error = %d\r\n", ret));
            R_WIFI_DA16XXX_HardwareReset();
            vTaskDelay(pdMS_TO_TICKS(delayMilliseconds));
        }
        else
        {
            break;
        }
    } while (0 < numRetries--);

    if (WIFI_SUCCESS != ret)
    {
        return eWiFiFailure;
    }


    return eWiFiSuccess;
} /* End of function WIFI_ConnectAP()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_Disconnect
 *
 * @brief Disconnects from the currently connected Access Point.
 *
 * @return @ref eWiFiSuccess if disconnection was successful or if the device is already
 * disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect(void)
{
    /* FIX ME. */
    R_WIFI_DA16XXX_Disconnect();
    return eWiFiSuccess;
} /* End of function WIFI_Disconnect()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_Reset
 *
 * @brief Resets the Wi-Fi Module.
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully reset, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Reset(void)
{
    /* FIX ME. */
    WIFIReturnCode_t ret;

    WIFI_Off();
    ret = WIFI_On();
    return ret;
} /* End of function WIFI_Reset()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_Scan
 *
 * @brief Perform a Wi-Fi network Scan.
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] ucNumNetworks - Number of networks to retrieve in scan result.
 *
 * @return @ref eWiFiSuccess if the Wi-Fi network scan was successful, failure code otherwise.
 *
 * @note The input buffer will have the results of the scan.
 */
WIFIReturnCode_t WIFI_Scan(WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks)
{
    WIFIReturnCode_t result = eWiFiFailure;
    wifi_err_t       ret;

    /* Cast to proper datatype to avoid warning */
    ret = R_WIFI_DA16XXX_Scan((wifi_scan_result_t *)pxBuffer, ucNumNetworks);
    if (WIFI_SUCCESS == ret)
    {
        result = eWiFiSuccess;
    }
    return result;
} /* End of function WIFI_Scan()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_SetMode
 *
 * @brief Sets the Wi-Fi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @return @ref eWiFiSuccess if Wi-Fi mode was set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode(WIFIDeviceMode_t xDeviceMode)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_SetMode()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_GetMode
 * @brief Gets the Wi-Fi mode.
 *
 * @param[out] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @return @ref eWiFiSuccess if Wi-Fi mode was successfully retrieved, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode(WIFIDeviceMode_t * pxDeviceMode)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_GetMode()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_NetworkAdd
 *
 * @brief Add a Wi-Fi Network profile.
 *
 * Adds a Wi-fi network to the network list in Non Volatile memory.
 *
 * @param[in] pxNetworkProfile - Network profile parameters
 * @param[out] pusIndex - Network profile index in storage
 *
 * @return Index of the profile storage on success, or failure return code on failure.
 */
WIFIReturnCode_t WIFI_NetworkAdd(const WIFINetworkProfile_t * const pxNetworkProfile,
                                uint16_t * pusIndex)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_NetworkAdd()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_NetworkGet
 *
 * @brief Get a Wi-Fi network profile.
 *
 * Gets the Wi-Fi network parameters at the given index from network list in non-volatile
 * memory.
 *
 * @note The WIFINetworkProfile_t data returned must have the the SSID and Password lengths
 * specified as the length without a null terminator.
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile,
 *                      must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *
 * @return @ref eWiFiSuccess if the network profile was successfully retrieved, failure code
 * otherwise.
 *
 * @see WIFINetworkProfile_t
 */
WIFIReturnCode_t WIFI_NetworkGet(WIFINetworkProfile_t * pxNetworkProfile,
                                uint16_t usIndex)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_NetworkGet()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_NetworkDelete
 *
 * @brief Delete a Wi-Fi Network profile.
 *
 * Deletes the Wi-Fi network profile from the network profile list at given index in
 * non-volatile memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to
 *                      wificonfigMAX_NETWORK_PROFILES.
 *
 *                      If wificonfigMAX_NETWORK_PROFILES is the index, then all
 *                      network profiles will be deleted.
 *
 * @return @ref eWiFiSuccess if successful, failure code otherwise. If successful, the
 * interface IP address is copied into the IP address buffer.
 */
WIFIReturnCode_t WIFI_NetworkDelete(uint16_t usIndex)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_NetworkDelete()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_Ping
 *
 * @brief Ping an IP address in the network.
 *
 * @param[in] pucIPAddr IP Address array to ping.
 * @param[in] usCount Number of times to ping
 * @param[in] ulIntervalMS Interval in mili-seconds for ping operation
 *
 * @return @ref eWiFiSuccess if ping was successful, other failure code otherwise.
 */
WIFIReturnCode_t WIFI_Ping(uint8_t * pucIPAddr,
                            uint16_t usCount)
{
    /* FIX ME. */
    WIFIReturnCode_t result = eWiFiFailure;
    wifi_err_t       ret;

    ret = R_WIFI_DA16XXX_Ping(pucIPAddr, usCount);
    if (WIFI_SUCCESS == ret)
    {
        result = eWiFiSuccess;
    }
    return result;
} /* End of function WIFI_Ping()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_GetIPInfo
 *
 * @brief Get IP configuration (IP address, NetworkMask, Gateway and
 *        DNS server addresses).
 *
 * This is a synchronous call.
 *
 * @param[out] pxIPInfo - Current IP configuration.
 *
 * @return eWiFiSuccess if connection info was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetIPInfo(WIFIIPConfiguration_t * pxIPConfig)
{
    /* FIX ME. */
    wifi_ip_configuration_t ipconfig;

    if (NULL == pxIPConfig)
    {
        return eWiFiFailure;
    }
    if (WIFI_SUCCESS != R_WIFI_DA16XXX_GetIpAddress(&ipconfig))
    {
        return eWiFiFailure;
    }

    memset(pxIPConfig, 0x00, sizeof(WIFIIPConfiguration_t));

    pxIPConfig->xIPAddress.ulAddress[0] = (uint32_t)ipconfig.ipaddress[0];  /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xIPAddress.ulAddress[1] = (uint32_t)ipconfig.ipaddress[1];  /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xIPAddress.ulAddress[2] = (uint32_t)ipconfig.ipaddress[2];  /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xIPAddress.ulAddress[3] = (uint32_t)ipconfig.ipaddress[3];  /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xGateway.ulAddress[0]   = (uint32_t)ipconfig.gateway[0];    /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xGateway.ulAddress[1]   = (uint32_t)ipconfig.gateway[1];    /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xGateway.ulAddress[2]   = (uint32_t)ipconfig.gateway[2];    /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xGateway.ulAddress[3]   = (uint32_t)ipconfig.gateway[3];    /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xNetMask.ulAddress[0]   = (uint32_t)ipconfig.subnetmask[0]; /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xNetMask.ulAddress[1]   = (uint32_t)ipconfig.subnetmask[1]; /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xNetMask.ulAddress[2]   = (uint32_t)ipconfig.subnetmask[2]; /* Cast to (uint32_t) to avoid warning */
    pxIPConfig->xNetMask.ulAddress[3]   = (uint32_t)ipconfig.subnetmask[3]; /* Cast to (uint32_t) to avoid warning */
    return eWiFiSuccess;
} /* End of function WIFI_GetIPInfo()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_GetMAC
 *
 * @brief Retrieves the Wi-Fi interface's MAC address.
 *
 * @param[out] pucMac MAC Address buffer sized 6 bytes.
 * @return @ref eWiFiSuccess if the MAC address was successfully retrieved, failure code
 * otherwise. The returned MAC address must be 6 consecutive bytes with no delimitters.
 */
WIFIReturnCode_t WIFI_GetMAC(uint8_t * pucMac)
{
    /* FIX ME. */
    if (WIFI_SUCCESS != R_WIFI_DA16XXX_GetMacAddress(pucMac))
    {
        return eWiFiFailure;
    }
    return eWiFiSuccess;
} /* End of function WIFI_GetMAC()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_GetHostIP
 *
 * @brief Retrieves the host IP address from a host name using DNS.
 *
 * @param[in] pcHost - Host (node) name.
 * @param[in] pucIPAddr - IP Address buffer.
 *
 * @return @ref eWiFiSuccess if the host IP address was successfully retrieved, failure code
 * otherwise.
 */
WIFIReturnCode_t WIFI_GetHostIP(char * pcHost,
                                uint8_t * pucIPAddr)
{
    /* FIX ME. */
    uint8_t ipAddress[4] = { 0, 0, 0, 0 };

    /* Cast to proper datatype to avoid warning */
    if (WIFI_SUCCESS != R_WIFI_DA16XXX_DnsQuery((uint8_t*)pcHost, ipAddress))
    {
        return eWiFiFailure;
    }
    pucIPAddr[0] = ipAddress[0];
    pucIPAddr[1] = ipAddress[1];
    pucIPAddr[2] = ipAddress[2];
    pucIPAddr[3] = ipAddress[3];

    return eWiFiSuccess;
} /* End of function WIFI_GetHostIP()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_StartAP
 *
 * @brief Start SoftAP mode.
 *
 * @return @ref eWiFiSuccess if SoftAP was successfully started, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartAP(void)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_StartAP()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_StopAP
 *
 * @brief Stop SoftAP mode.
 *
 * @return @ref eWiFiSuccess if the SoftAP was successfully stopped, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StopAP(void)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_StopAP()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_ConfigureAP
 *
 * @brief Configure SoftAP.
 *
 * @param[in] pxNetworkParams - Network parameters to configure AP.
 *
 * @return @ref eWiFiSuccess if SoftAP was successfully configured, failure code otherwise.
 */
WIFIReturnCode_t WIFI_ConfigureAP(const WIFINetworkParams_t * const pxNetworkParams)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_ConfigureAP()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_SetPMMode
 *
 * @brief Set the Wi-Fi power management mode.
 *
 * @param[in] xPMModeType - Power mode type.
 *
 * @param[in] pvOptionValue - A buffer containing the value of the option to set
 *                            depends on the mode type
 *                            example - beacon interval in sec
 *
 * @return @ref eWiFiSuccess if the power mode was successfully configured, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetPMMode(WIFIPMMode_t xPMModeType,
                                const void * pvOptionValue)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_SetPMMode()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_GetPMMode
 *
 * @brief Get the Wi-Fi power management mode
 *
 * @param[out] pxPMModeType - pointer to get current power mode set.
 *
 * @param[out] pvOptionValue - optional value
 *
 * @return @ref eWiFiSuccess if the power mode was successfully retrieved, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetPMMode(WIFIPMMode_t * pxPMModeType,
                                void * pvOptionValue)
{
    /* FIX ME. */
    return eWiFiNotSupported;
} /* End of function WIFI_GetPMMode()*/

/*-----------------------------------------------------------*/

/**
 * @fn WIFI_IsConnected
 *
 * @brief Check if the Wi-Fi is connected and the AP configuration matches the query.
 *
 * param[in] pxNetworkParams - Network parameters to query, if NULL then just check the
 * Wi-Fi link status.
 */
BaseType_t WIFI_IsConnected(void)
{
    BaseType_t ret = pdFALSE;

    /* FIX ME. */
    if (0 == R_WIFI_DA16XXX_IsConnected())
    {
        ret = pdTRUE;
    }
    return ret;
} /* End of function WIFI_IsConnected()*/

/**
 * @fn WIFI_RegisterEvent
 *
 * @brief Register a Wi-Fi event Handler.
 *
 * @param[in] xEventType - Wi-Fi event type.
 * @param[in] xHandler - Wi-Fi event handler.
 *
 * @return eWiFiSuccess if registration is successful, failure code otherwise.
 */
WIFIReturnCode_t WIFI_RegisterEvent(WIFIEventType_t xEventType, WIFIEventHandler_t xHandler)
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
} /* End of function WIFI_RegisterEvent()*/

/**
 * @fn WIFI_GetCapability
 *
 * @brief Get the Wi-Fi capability.
 *
 * @param[out] pxCaps - Structure to hold the Wi-Fi capabilities.
 *
 * @return eWiFiSuccess if capabilities are retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetCapability(WIFICapabilityInfo_t * pxCaps)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetCapability()*/

/**
 * @fn WIFI_GetCountryCode
 *
 * @brief Get the currently configured country code.
 *
 * @param[out] pcCountryCode - Null-terminated string to hold the country code (see ISO-3166).
 * Must be at least 4 bytes.
 *
 * @return eWiFiSuccess if Country Code is retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetCountryCode(char * pcCountryCode)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetCountryCode()*/

/**
 * @fn WIFI_GetRSSI
 *
 * @brief Get the RSSI of the connected AP.
 *
 * This only works when the station is connected.
 *
 * @param[out] pcRSSI - RSSI of the connected AP.
 *
 * @return eWiFiSuccess if RSSI was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetRSSI(int8_t * pcRSSI)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetRSSI()*/

/**
 * @fn WIFI_GetScanResults
 *
 * @brief Get Wi-Fi scan results. It should be called only after scan is completed.
 * Scan results are sorted in decreasing rssi order.
 *
 * @param[out] pxBuffer - Handle to the READ ONLY scan results buffer.
 * @param[out] ucNumNetworks - Actual number of networks in the scan results.
 *
 * @return eWiFiSuccess if the scan results were got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetScanResults(const WIFIScanResult_t ** pxBuffer,
                                    uint16_t * ucNumNetworks)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetScanResults()*/

/**
 * @fn WIFI_GetStationList
 *
 * @brief SoftAP mode get connected station list.
 *
 * @param[out] pxStationList - Buffer for station list, supplied by the caller.
 * @param[in, out] pcStationListSize - Input size of the list, output number of connected stations.
 *
 * @return eWiFiSuccess if stations were got successfully (maybe none),
 * failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetStationList(WIFIStationInfo_t * pxStationList,
                                    uint8_t * pcStationListSize)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetStationList()*/

/**
 * @fn WIFI_GetStatistic
 *
 * @brief Get the Wi-Fi statistics.
 *
 * @param[out] pxStats - Structure to hold the WiFi statistics.
 *
 * @return eWiFiSuccess if statistics are retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetStatistic(WIFIStatisticInfo_t * pxStats)
{
    return eWiFiNotSupported;
} /* End of function WIFI_GetStatistic()*/

/**
 * @fn WIFI_SetCountryCode
 *
 * @brief Set country based configuration (including channel list, power table)
 *
 * @param[in] pcCountryCode - Country code (null-terminated string, e.g. "US", "CN". See ISO-3166).
 *
 * @return eWiFiSuccess if Country Code is set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetCountryCode(const char * pcCountryCode)
{
    return eWiFiNotSupported;
} /* End of function WIFI_SetCountryCode()*/

/**
 * @fn WIFI_StartConnectAP
 *
 * @brief Connect to the Wi-Fi Access Point (AP) specified in the input.
 *
 * This is an asynchronous call, the result will be notified by an event.
 * @see WiFiEventInfoConnected_t.
 *
 * The Wi-Fi should stay connected when the specified AP is the same as the
 * currently connected AP. Otherwise, the Wi-Fi should disconnect and connect
 * to the specified AP. If the specified AP has invalid parameters, the Wi-Fi
 * should be disconnected.
 *
 * @param[in] pxNetworkParams - Configuration of the targeted AP.
 *
 * @return eWiFiSuccess if connection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartConnectAP(const WIFINetworkParams_t * pxNetworkParams)
{
    return eWiFiNotSupported;
} /* End of function WIFI_StartConnectAP()*/

/**
 * @fn WIFI_StartDisconnect
 *
 * @brief Wi-Fi station disconnects from AP.
 *
 * This is an asynchronous call. The result will be notified by an event.
 * @see WiFiEventInfoDisconnected_t.
 *
 * @return eWiFiSuccess if disconnection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartDisconnect(void)
{
    return eWiFiNotSupported;
} /* End of function WIFI_StartDisconnect()*/

/**
 * @fn WIFI_StartScan
 *
 * @brief Start a Wi-Fi scan.
 *
 * This is an asynchronous call, the result will be notified by an event.
 * @see WiFiEventInfoScanDone_t.
 *
 * @param[in] pxScanConfig - Parameters for scan, NULL if default scan
 * (i.e. broadcast scan on all channels).
 *
 * @return eWiFiSuccess if scan was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartScan(WIFIScanConfig_t * pxScanConfig)
{
    return eWiFiNotSupported;
} /* End of function WIFI_StartScan()*/

