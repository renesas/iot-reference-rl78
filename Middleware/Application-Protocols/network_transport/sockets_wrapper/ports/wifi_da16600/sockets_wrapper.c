/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Modifications Copyright (C) 2024 Renesas Electronics Corporation. or its affiliates.
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
 * @file sockets_wrapper.c
 * @brief FreeRTOS Sockets connect and disconnect wrapper implementation.
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Sockets wrapper includes. */
#include "tcp_sockets_wrapper.h"

/* Wi-Fi Library api includes. */
#include "r_wifi_da16xxx_if.h"
#include "user_tcp_hook_config.h"

#define MALLOC_SOCKET_CREATION_ERROR -50
#define NO_SOCKET_CREATION_ERROR -51

/*-----------------------------------------------------------*/
typedef struct xSOCKETContext
{
    uint32_t ulSendTimeout;
    uint32_t ulRecvTimeout;
    uint8_t  socket_no;
} SSOCKETContext_t, * SSOCKETContextPtr_t;

/**
 * @brief Maximum number of sockets.
 *
 * 16 total sockets
 */
#define MAX_NUM_SSOCKETS    (WIFI_CFG_TLS_CREATABLE_SOCKETS)

/*-----------------------------------------------------------*/
#define FORCE_RESET     1
#define NO_FORCE_RESET  0
static volatile uint32_t count_module_comm = 0;

static wifi_err_t SocketErrorHook (wifi_err_t error, bool force_reset);
static wifi_err_t CloseSocket (uint8_t socket_number);

#if (0 == USER_TCP_HOOK_ENABLED)
static wifi_err_t SocketErrorHook( wifi_err_t error, bool force_reset )
{
    (void) force_reset;
    return error;
}
#else

/**
 * @fn SocketErrorHook
 *
 * @brief Called when a socket error is detected.
 *
 * @param[in] error        The socket error code
 * @param[in] force_reset  Indicates whether a forced reset should be performed
 * @return wifi_err_t      The result of the error handling process
 */
static wifi_err_t SocketErrorHook(wifi_err_t error, bool force_reset)
{
    uint32_t reconnect_tries = 0;

    if ((WIFI_ERR_MODULE_COM != error) && (WIFI_ERR_MODULE_TIMEOUT != error))
    {
        count_module_comm = 0;
        return error;
    }

    if (FORCE_RESET == force_reset)
    {
        for (reconnect_tries = 0; reconnect_tries < USER_RECONNECT_TRIES; reconnect_tries++)
        {
            if (WIFI_SUCCESS == R_WIFI_DA16XXX_HardwareReset())
            {
                break;
            }
        }
        return error;
    }
    else
    {
        if (WIFI_ERR_MODULE_COM == error)
        {
            count_module_comm++;
            if (USER_COMM_ERROR_TRIES > count_module_comm)
            {
                return WIFI_SUCCESS;
            }
        }

        if ((USER_COMM_ERROR_TRIES <= count_module_comm) || (WIFI_ERR_MODULE_COM != error))
        {
            count_module_comm = 0;

            for (reconnect_tries = 0; reconnect_tries < USER_RECONNECT_TRIES; reconnect_tries++)
            {
                if (WIFI_SUCCESS == R_WIFI_DA16XXX_HardwareReset())
                {
                    break;
                }
            }
        }

        return error;
    }
}/* End of function SocketErrorHook()*/
#endif /* 0 == USER_TCP_HOOK_ENABLED */

/**
 * @fn CloseSocket
 *
 * @brief Close the specified socket.
 *
 * @param[in] socket_number  The number of socket to be closed
 * @return wifi_err_t      The result of the socket close operation
 */
static wifi_err_t CloseSocket(uint8_t socket_number)
{
    uint8_t    count;
    wifi_err_t ret;

    for (count = 0; count < USER_CLOSE_SOCKET_TRIES; count++)
    {
        /* Close sockets. */
        ret = R_WIFI_DA16XXX_CloseTlsSocket(socket_number);
        if (WIFI_SUCCESS == ret)
        {
            break;
        }
        LogInfo(("Try to close in %d times.", count));
    }
    if (WIFI_SUCCESS != ret)
    {
        SocketErrorHook(ret, FORCE_RESET);
    }

    return ret;
}/* End of function CloseSocket()*/

/*-----------------------------------------------------------*/

/**
 * @fn TCP_Sockets_Connect
 *
 * @brief Connects to the specified server using the given socket.
 *
 * @param[out] pTcpSocket        Pointer to the structure holding socket information
 * @param[in]  pHostName         Pointer to the server hostName
 * @param[in]  port              Port number to connect to on the server
 * @param[in]  receiveTimeoutMs  Timeout (in milliseconds) for receiving data
 * @param[in]  sendTimeoutMs     Timeout (in milliseconds) for sending data
 * @return     BaseType_t        Result of the connection attempt
 */
BaseType_t TCP_Sockets_Connect(Socket_t * pTcpSocket,
                                const char * pHostName,
                                uint16_t port,
                                uint32_t receiveTimeoutMs,
                                uint32_t sendTimeoutMs)
{
    SSOCKETContextPtr_t  pxContext = NULL;
    wifi_err_t           ret;
    uint8_t              socketId     = 0;
    uint8_t              ipAddress[4] = { 0 };
    wifi_tls_cert_info_t cert_infor   = { 0 };
    BaseType_t           retConnect   = TCP_SOCKETS_ERRNO_NONE;

    /* Register TSL Socket on DA16XXX */
    if (WIFI_SUCCESS == R_WIFI_DA16XXX_GetServerCertificate(&cert_infor))
    {
        /* Cast to proper datatype to avoid warning */
        ret = R_WIFI_DA16XXX_ConfigTlsSocket(&socketId, &cert_infor, (uint8_t __far *)pHostName, 1, 8192, 8192, 1000);
        if (WIFI_SUCCESS == ret)
        {
            /* Create a new TLS socket. */
            ret = R_WIFI_DA16XXX_CreateTlsSocket(socketId, WIFI_SOCKET_TYPE_TLS, 4);
            if (WIFI_SUCCESS != ret)
            {
                LogError(("Failed to create WiFi sockets. %d\r\n", ret));
                retConnect = NO_SOCKET_CREATION_ERROR;
            }
        }
        else
        {
            LogError(("Failed to config Tls Socket on WiFi module.\r\n"));
            retConnect = (BaseType_t) ret;
        }
    }
    else
    {
        LogError(("Failed to get stored server certificate on WiFi module.\r\n"));
        retConnect = (BaseType_t) ret;
    }

    /* Allocate socket context. */
    if (TCP_SOCKETS_ERRNO_NONE == retConnect)
    {
        pxContext = pvPortMalloc(sizeof(SSOCKETContext_t));

        if (NULL == pxContext)
        {
            LogError(("Failed to allocate new socket context."));
            (void)CloseSocket(socketId);
            retConnect = TCP_SOCKETS_ERRNO_ENOMEM;
        }
        else
        {
            /* Initialize the wrapped socket. */
            LogDebug(("Created new TCP Socket %p.", pxContext));
            (void)memset(pxContext, 0, sizeof(SSOCKETContext_t));
            pxContext->socket_no     = socketId;
            pxContext->ulRecvTimeout = receiveTimeoutMs;
            pxContext->ulSendTimeout = sendTimeoutMs;
        }
    }

    /* Setup TLS socket */
    if (TCP_SOCKETS_ERRNO_NONE == retConnect)
    {
        /* Cast to proper datatype to avoid warning */
        ret = R_WIFI_DA16XXX_DnsQuery((uint8_t __far *)pHostName, ipAddress);
        if (WIFI_SUCCESS != ret)
        {
            LogError(( "Failed to connect to server: DNS resolution failed: Hostname=%s.",
                            pHostName ));
            retConnect = (BaseType_t) ret;
        }
    }

    /* Wi-Fi socket connect. */
    if (TCP_SOCKETS_ERRNO_NONE == retConnect)
    {
        ret = R_WIFI_DA16XXX_TlsConnect(pxContext->socket_no, ipAddress, port);
        if (WIFI_SUCCESS != ret)
        {
            LogError(( "Failed to connect to server: Connect failed: ReturnCode=%d,"
                        " Hostname=%u.%u.%u.%u, Port=%u.",
                        ret,
                        ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3],
                        port ));
            retConnect = (BaseType_t) ret;
        }
    }

    if (TCP_SOCKETS_ERRNO_NONE != retConnect)
    {
        (void)CloseSocket(socketId);

        if (NULL != pxContext)
        {
            vPortFree(pxContext);
            pxContext = NULL;
        }
        SocketErrorHook((wifi_err_t)retConnect, FORCE_RESET);  /* Cast to proper datatype to avoid warning */
        LogError(("Failed to create new socket."));
    }

    /* Set the socket. */
    *pTcpSocket = (Socket_t)pxContext;

    return retConnect;
}/* End of function TCP_Sockets_Connect()*/

/*-----------------------------------------------------------*/

/**
 * @fn TCP_Sockets_Disconnect
 *
 * @brief Disconnects the given socket.
 *
 * @param[in] xSocket      The structure holding the socket information to be disconnected
 */
void TCP_Sockets_Disconnect(Socket_t xSocket)
{
    int32_t             retClose    = TCP_SOCKETS_ERRNO_NONE;
    SSOCKETContextPtr_t pxContext   = (SSOCKETContextPtr_t) xSocket; /*lint !e9087 cast used for portability. */

    if ((NULL == pxContext) || (SOCKETS_INVALID_SOCKET == xSocket) || (MAX_NUM_SSOCKETS <= pxContext->socket_no))
    {
        LogError(("Invalid xSocket %p", pxContext));
        retClose = TCP_SOCKETS_ERRNO_EINVAL;
    }

    if (TCP_SOCKETS_ERRNO_NONE == retClose)
    {
        (void)CloseSocket(pxContext->socket_no);
        vPortFree(pxContext);
        pxContext = NULL;
    }

    (void) retClose;
    LogDebug(( "Sockets close exit with code %d", retClose ));
}/* End of function TCP_Sockets_Disconnect()*/

/*-----------------------------------------------------------*/

/**
 * @fn TCP_Sockets_Recv
 *
 * @brief Receives data from the given socket.
 *
 * @param[in]  xSocket       The structure holding the socket information
 * @param[out] pvBuffer      The pointer to the buffer holding the receive data
 * @param[in]  xBufferLength The length of the buffer in bytes
 *
 * @return     int32_t       The number of bytes received, or a negative value if an error occurred
 */
int32_t TCP_Sockets_Recv(Socket_t xSocket,
                        void * pvBuffer,
                        size_t xBufferLength)
{
    SSOCKETContextPtr_t pxContext    = (SSOCKETContextPtr_t) xSocket; /*lint !e9087 cast used for portability. */
    BaseType_t          receive_byte = 0;

    if (NULL == pxContext)
    {
        LogError(("Wi-Fi prvNetworkRecv Invalid xSocket %p", pxContext));
        receive_byte = (BaseType_t) TCP_SOCKETS_ERRNO_EINVAL;
    }
    else
    {
        /* Cast to proper datatype to avoid warning */
        receive_byte = R_WIFI_DA16XXX_ReceiveTlsSocket(pxContext->socket_no, (uint8_t __far *) pvBuffer,
                                                        xBufferLength, pxContext->ulRecvTimeout);
        if (0 > receive_byte)
        {
            if ((WIFI_ERR_NOT_CONNECT == receive_byte) || (WIFI_ERR_TAKE_MUTEX == receive_byte))
            {
                receive_byte      = 0;
                count_module_comm = 0;
            }
            else if (WIFI_ERR_SOCKET_NUM == receive_byte)
            {
                receive_byte      = (BaseType_t) TCP_SOCKETS_ERRNO_ERROR;
                count_module_comm = 0;
                (void)CloseSocket(pxContext->socket_no);
            }
            else
            {
                receive_byte = (BaseType_t) TCP_SOCKETS_ERRNO_ERROR;
            }
        }
        else
        {
            count_module_comm = 0;
        }
    }

    return receive_byte;
}/* End of function TCP_Sockets_Recv()*/

/*-----------------------------------------------------------*/

/**
 * @fn TCP_Sockets_Send
 *
 * @brief Sends data from the given socket.
 *
 * @param[in]  xSocket       The structure holding the socket information
 * @param[in]  pvBuffer      The pointer to the buffer containing the data to be sent
 * @param[in]  xBufferLength The length of the buffer in bytes
 *
 * @return     int32_t       The number of bytes sent, or a negative value if an error occurred
 */
int32_t TCP_Sockets_Send(Socket_t xSocket,
                        const void * pvBuffer,
                        size_t xDataLength)
{
    BaseType_t          send_byte = 0;
    SSOCKETContextPtr_t pxContext = (SSOCKETContextPtr_t) xSocket; /*lint !e9087 cast used for portability. */

    if (NULL == pxContext)
    {
        LogError(("Wi-Fi Sockets_Send Invalid xSocket %p", pxContext));
        send_byte = (BaseType_t) TCP_SOCKETS_ERRNO_ERROR;
    }
    else
    {
        /* Cast to proper datatype to avoid warning */
        send_byte = R_WIFI_DA16XXX_SendTlsSocket(pxContext->socket_no, (uint8_t __far *) pvBuffer,
                                                xDataLength, pxContext->ulSendTimeout);
        if (0 > send_byte)
        {
            if ((WIFI_ERR_NOT_CONNECT == send_byte) || (WIFI_ERR_TAKE_MUTEX == send_byte))
            {
                send_byte         = 0;
                count_module_comm = 0;
            }
            else if ((WIFI_ERR_SOCKET_NUM == send_byte) || (WIFI_ERR_MODULE_COM == send_byte))
            {
                send_byte         = (BaseType_t) TCP_SOCKETS_ERRNO_ERROR;
                count_module_comm = 0;
                (void)CloseSocket(pxContext->socket_no);
            }
            else
            {
                send_byte = (BaseType_t) TCP_SOCKETS_ERRNO_ERROR;
            }
        }
        else
        {
            count_module_comm = 0;
        }
    }

    return send_byte;
}/* End of function TCP_Sockets_Send()*/
