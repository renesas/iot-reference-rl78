/*
 * FreeRTOS V202112.00
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#pragma section const const_middleware
#endif

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "event_groups.h"

/* Sockets wrapper includes. */
#include "tcp_sockets_wrapper.h"

/* FreeRTOS Cellular Library api includes. */
#include "r_aws_cellular_if.h"

/* Configure logs for the functions in this file. */
#include "logging_levels.h"
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "CELLULAR_SOCKETS"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_NONE
#endif
#include "logging_stack.h"

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#include "cert_profile_helper.h"
#endif

/*-----------------------------------------------------------*/

/* Cellular socket wrapper needs application provide the cellular handle and pdn context id. */
/* User of cellular socket wrapper should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern CellularHandle_t CellularHandle;

/* User of cellular socket wrapper should provide this variable. */
/* coverity[misra_c_2012_rule_8_6_violation] */
extern uint8_t CellularSocketPdnContextId;

/*-----------------------------------------------------------*/

/* Windows simulator implementation. */
#if defined( _WIN32 ) || defined( _WIN64 )
    #define strtok_r                         strtok_s
#endif

#define CELLULAR_SOCKET_OPEN_FLAG              ( 1UL << 0 )
#define CELLULAR_SOCKET_CONNECT_FLAG           ( 1UL << 1 )

#define SOCKET_DATA_RECEIVED_CALLBACK_BIT      ( 0x00000001U )
#define SOCKET_OPEN_CALLBACK_BIT               ( 0x00000002U )
#define SOCKET_OPEN_FAILED_CALLBACK_BIT        ( 0x00000004U )
#define SOCKET_CLOSE_CALLBACK_BIT              ( 0x00000008U )

/* Ticks MS conversion macros. */
#define TICKS_TO_MS( xTicks )    ( ( ( xTicks ) * 1000U ) / ( ( uint32_t ) configTICK_RATE_HZ ) )
#define UINT32_MAX_DELAY_MS                    ( 0xFFFFFFFFUL )
#define UINT32_MAX_MS_TICKS                    ( UINT32_MAX_DELAY_MS / ( TICKS_TO_MS( 1U ) ) )

/* Cellular socket access mode. */
#define CELLULAR_SOCKET_ACCESS_MODE            CELLULAR_ACCESSMODE_BUFFER

/* Cellular socket open timeout. */
#define CELLULAR_SOCKET_OPEN_TIMEOUT_TICKS     ( portMAX_DELAY )
#define CELLULAR_SOCKET_CLOSE_TIMEOUT_TICKS    ( pdMS_TO_TICKS( 10000U ) )

/* Cellular socket AT command timeout. */
#define CELLULAR_SOCKET_RECV_TIMEOUT_MS        ( 1000UL )

/* Time conversion constants. */
#define _MILLISECONDS_PER_SECOND               ( 1000 )                                          /**< @brief Milliseconds per second. */
#define _MILLISECONDS_PER_TICK                 ( _MILLISECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Milliseconds per FreeRTOS tick. */

/* Logging macros definition. */
#define IotLogError( ... )                     LogError( ( __VA_ARGS__ ) )
#define IotLogWarn( ... )                      LogWarn( ( __VA_ARGS__ ) )
#define IotLogInfo( ... )                      LogInfo( ( __VA_ARGS__ ) )
#define IotLogDebug( ... )                     LogDebug( ( __VA_ARGS__ ) )

/*-----------------------------------------------------------*/

typedef struct xSOCKET
{
    CellularSocketHandle_t cellularSocketHandle;
    uint32_t ulFlags;
    EventGroupHandle_t socketEventGroupHandle;
} cellularSocketWrapper_t;

/*-----------------------------------------------------------*/

/**
 * @brief Get the count of milliseconds since vTaskStartScheduler was called.
 *
 * @return The count of milliseconds since vTaskStartScheduler was called.
 */
static uint64_t getTimeMs( void );

/**
 * @brief Receive data from cellular socket.
 *
 * @param[in] pCellularSocketContext Cellular socket wrapper context for socket operations.
 * @param[out] buf The data buffer for receiving data.
 * @param[in] len The length of the data buffer
 *
 * @note This function receives data. It returns when non-zero bytes of data is received,
 * when an error occurs, or when timeout occurs. Receive timeout unit is TickType_t.
 * Any timeout value bigger than portMAX_DELAY will be regarded as portMAX_DELAY.
 * In this case, this function waits portMAX_DELAY until non-zero bytes of data is received
 * or until an error occurs.
 *
 * @return Positive value indicate the number of bytes received. Otherwise, error code defined
 * in sockets_wrapper.h is returned.
 */
static BaseType_t prvNetworkRecvCellular( const cellularSocketWrapper_t * pCellularSocketContext,
                                          uint8_t * buf,
                                          size_t len );

/**
 * @brief Callback used to inform about the status of socket open.
 *
 * @param[in] urcEvent URC Event that happened.
 * @param[in] socketHandle Socket handle for which data is ready.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterSocketOpenCallback function.
 */
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext );

/**
 * @brief Callback used to inform that data is ready for reading on a socket.
 *
 * @param[in] socketHandle Socket handle for which data is ready.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterDataReadyCallback function.
 */
static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext );


/**
 * @brief Callback used to inform that remote end closed the connection for a
 * connected socket.
 *
 * @param[in] socketHandle Socket handle for which remote end closed the
 * connection.
 * @param[in] pCallbackContext pCallbackContext parameter in
 * Cellular_SocketRegisterClosedCallback function.
 */
static void prvCellularSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                             void * pCallbackContext );

/**
 * @brief Setup socket receive timeout.
 *
 * @param[in] pCellularSocketContext Cellular socket wrapper context for socket operations.
 * @param[out] receiveTimeout Socket receive timeout in TickType_t.
 *
 * @return On success, TCP_SOCKETS_ERRNO_NONE is returned. If an error occurred, error code defined
 * in sockets_wrapper.h is returned.
 */
static BaseType_t prvSetupSocketRecvTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t receiveTimeout );

/**
 * @brief Setup socket send timeout.
 *
 * @param[in] pCellularSocketContext Cellular socket wrapper context for socket operations.
 * @param[out] sendTimeout Socket send timeout in TickType_t.
 *
 * @note Send timeout unit is TickType_t. The underlying cellular API uses miliseconds for timeout.
 * Any send timeout greater than UINT32_MAX_MS_TICKS( UINT32_MAX_DELAY_MS/MS_PER_TICKS ) or
 * portMAX_DELAY is regarded as UINT32_MAX_DELAY_MS for cellular API.
 *
 * @return On success, TCP_SOCKETS_ERRNO_NONE is returned. If an error occurred, error code defined
 * in sockets_wrapper.h is returned.
 */
static BaseType_t prvSetupSocketSendTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t sendTimeout );

/**
 * @brief Setup cellular socket callback function.
 *
 * @param[in] cellularSocketHandle Cellular socket handle for cellular socket operations.
 * @param[in] pCellularSocketContext Cellular socket wrapper context for socket operations.
 *
 * @return On success, TCP_SOCKETS_ERRNO_NONE is returned. If an error occurred, error code defined
 * in sockets_wrapper.h is returned.
 */
static BaseType_t prvCellularSocketRegisterCallback( CellularSocketHandle_t cellularSocketHandle,
                                                     cellularSocketWrapper_t * pCellularSocketContext );

/**
 * @brief Calculate elapsed time from current time and input parameters.
 *
 * @param[in] entryTimeMs The entry time to be compared with current time.
 * @param[in] timeoutValueMs Timeout value for the comparison between entry time and current time.
 * @param[out] pElapsedTimeMs The elapsed time if timeout condition is true.
 *
 * @return True if the difference between entry time and current time is bigger or
 * equal to timeoutValueMs. Otherwise, return false.
 */
static bool _calculateElapsedTime( uint64_t entryTimeMs,
                                   uint32_t timeoutValueMs,
                                   uint64_t * pElapsedTimeMs );

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
static BaseType_t prvSetupSocketTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                         CellularSocketOption_t option,
                                         TickType_t timeout );
#else
static CellularError_t Sockets_GetHostByName( const char * pcHostName , const uint8_t * addr);
#endif

/*-----------------------------------------------------------*/

static uint64_t getTimeMs( void )
{
    TimeOut_t xCurrentTime = { 0 };

    /* This must be unsigned because the behavior of signed integer overflow is undefined. */
    uint64_t ullTickCount = 0ULL;

    /* Get the current tick count and overflow count. vTaskSetTimeOutState()
     * is used to get these values because they are both static in tasks.c. */
    vTaskSetTimeOutState( &xCurrentTime );

    /* Adjust the tick count for the number of times a TickType_t has overflowed. */
    ullTickCount = ( uint64_t ) ( xCurrentTime.xOverflowCount ) << ( sizeof( TickType_t ) * 8 );

    /* Add the current tick count. */
    ullTickCount += xCurrentTime.xTimeOnEntering;

    /* Return the ticks converted to milliseconds. */
    return ullTickCount * _MILLISECONDS_PER_TICK;
}

/*-----------------------------------------------------------*/

static BaseType_t prvNetworkRecvCellular( const cellularSocketWrapper_t * pCellularSocketContext,
                                          uint8_t * buf,
                                          size_t len )
{
    CellularSocketHandle_t cellularSocketHandle;
    BaseType_t retRecvLength;
    uint32_t recvLength = 0;
    EventBits_t waitEventBits;
    uint16_t soketdatasize = 0;

    cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

    /* Wait for +SQNSRING */
    waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                         SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_CLOSE_CALLBACK_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         10 );

    if( ( waitEventBits & SOCKET_DATA_RECEIVED_CALLBACK_BIT ) != 0U )
    {
        ( void ) Cellular_SocketRecv( CellularHandle, cellularSocketHandle, buf, len, &recvLength );
        ( void ) Cellular_GetSocketDataSize( CellularHandle, cellularSocketHandle, &soketdatasize );
        if (soketdatasize == 0)
        {
            ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                           SOCKET_DATA_RECEIVED_CALLBACK_BIT );
        }
        retRecvLength = ( BaseType_t ) recvLength;
    }
    else if( ( waitEventBits & SOCKET_CLOSE_CALLBACK_BIT ) != 0U )
    {
        IotLogDebug( "prvNetworkRecv failed %d", socketStatus );
        retRecvLength = TCP_SOCKETS_ERRNO_ERROR;
    }
    else
    {
        /* No data */
        retRecvLength = 0;
    }

    IotLogDebug( "prvNetworkRecv expect %d read %d", len, recvLength );
    return retRecvLength;
}

/*-----------------------------------------------------------*/
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    IotLogDebug( "Socket open callback on Socket %p %d %d.",
                 pCellularSocketContext, socketHandle, urcEvent );

    if( urcEvent == CELLULAR_URC_SOCKET_OPENED )
    {
        pCellularSocketContext->ulFlags = pCellularSocketContext->ulFlags | CELLULAR_SOCKET_CONNECT_FLAG;
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_OPEN_CALLBACK_BIT );
    }
    else
    {
        /* Socket open failed. */
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_OPEN_FAILED_CALLBACK_BIT );
    }
}
/*-----------------------------------------------------------*/

static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    IotLogDebug( "Data ready on Socket %p", pCellularSocketContext );
    ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                 SOCKET_DATA_RECEIVED_CALLBACK_BIT );
}

/*-----------------------------------------------------------*/

static void prvCellularSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                             void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    IotLogDebug( "Socket Close on Socket %p", pCellularSocketContext );
    pCellularSocketContext->ulFlags = pCellularSocketContext->ulFlags & ( ~CELLULAR_SOCKET_CONNECT_FLAG );
    ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                 SOCKET_CLOSE_CALLBACK_BIT );
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetupSocketRecvTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t receiveTimeout )
{
    return prvSetupSocketTimeout( pCellularSocketContext,
                                  CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,
                                  receiveTimeout );
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetupSocketSendTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t sendTimeout )
{
    return prvSetupSocketTimeout( pCellularSocketContext,
                                  CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,
                                  sendTimeout );
}

/*-----------------------------------------------------------*/

static BaseType_t prvCellularSocketRegisterCallback( CellularSocketHandle_t cellularSocketHandle,
                                                     cellularSocketWrapper_t * pCellularSocketContext )
{
    if( Cellular_CommonSocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle,
            prvCellularSocketDataReadyCallback, ( void * ) pCellularSocketContext) != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Failed to SocketRegisterDataReadyCallback. Socket status %d.", socketStatus );
        return TCP_SOCKETS_ERRNO_ERROR;
    }

    if( Cellular_CommonSocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle,
            prvCellularSocketOpenCallback, ( void * ) pCellularSocketContext ) != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Failed to SocketRegisterSocketOpenCallbac. Socket status %d.", socketStatus );
        return TCP_SOCKETS_ERRNO_ERROR;
    }

    if( Cellular_CommonSocketRegisterClosedCallback( CellularHandle, cellularSocketHandle,
            prvCellularSocketClosedCallback, ( void * ) pCellularSocketContext ) != CELLULAR_SUCCESS )
    {
        IotLogDebug( "Failed to SocketRegisterClosedCallback. Socket status %d.", socketStatus );
        return TCP_SOCKETS_ERRNO_ERROR;
    }

    return TCP_SOCKETS_ERRNO_NONE;
}

/*-----------------------------------------------------------*/

static bool _calculateElapsedTime( uint64_t entryTimeMs,
                                   uint32_t timeoutValueMs,
                                   uint64_t * pElapsedTimeMs )
{
    uint64_t currentTimeMs = getTimeMs();
    bool isExpired = false;

    /* timeoutValueMs with UINT32_MAX_DELAY_MS means wait for ever, same behavior as freertos_plus_tcp. */
    if( timeoutValueMs == UINT32_MAX_DELAY_MS )
    {
        isExpired = false;
    }

    /* timeoutValueMs = 0 means none blocking mode. */
    else if( timeoutValueMs == 0U )
    {
        isExpired = true;
    }
    else
    {
        *pElapsedTimeMs = currentTimeMs - entryTimeMs;

        if( ( currentTimeMs - entryTimeMs ) >= timeoutValueMs )
        {
            isExpired = true;
        }
        else
        {
            isExpired = false;
        }
    }

    return isExpired;
}

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
/*-----------------------------------------------------------*/

static BaseType_t prvSetupSocketTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                         CellularSocketOption_t option,
                                         TickType_t timeout )
{
    uint32_t TimeoutMs;

    TimeoutMs = TICKS_TO_MS( timeout );
    if( Cellular_CommonSocketSetSockOpt( CellularHandle,
                                         pCellularSocketContext->cellularSocketHandle,
                                         CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                         option,
                                         ( const uint8_t * ) &TimeoutMs,
                                         sizeof( uint32_t ) ) != CELLULAR_SUCCESS )
    {
        return TCP_SOCKETS_ERRNO_EINVAL;
    }

    return TCP_SOCKETS_ERRNO_NONE;
}

/*-----------------------------------------------------------*/

#else
static CellularError_t Sockets_GetHostByName( const char * pcHostName , const uint8_t * addr)
{
    return Cellular_GetHostByName( CellularHandle,
                                   RM_CELLULAR_RYZ_DEFAULT_BEARER_CONTEXT_ID,
                                   pcHostName,
                                   (char *)addr );
}
#endif

/*-----------------------------------------------------------*/

BaseType_t TCP_Sockets_Connect( Socket_t * pTcpSocket,
                                const char * pHostName,
                                uint16_t port,
                                uint32_t receiveTimeoutMs,
                                uint32_t sendTimeoutMs )
{
    CellularSocketHandle_t cellularSocketHandle;
    cellularSocketWrapper_t * pCellularSocketContext = NULL;
    CellularError_t cellularSocketStatus;

    CellularSocketAddress_t serverAddress;
    EventBits_t waitEventBits = 0;
    BaseType_t retConnect = TCP_SOCKETS_ERRNO_NONE;
    const uint32_t defaultReceiveTimeoutMs = CELLULAR_SOCKET_RECV_TIMEOUT_MS;

    /* Create a new TCP socket. */
    cellularSocketStatus = Cellular_CommonCreateSocket( CellularHandle,
                                                        CellularSocketPdnContextId,
                                                        CELLULAR_SOCKET_DOMAIN_AF_INET,
                                                        CELLULAR_SOCKET_TYPE_STREAM,
                                                        CELLULAR_SOCKET_PROTOCOL_TCP,
                                                        &cellularSocketHandle );

    if( CELLULAR_SUCCESS != cellularSocketStatus )
    {
        IotLogDebug( "Failed to create cellular sockets. %d", cellularStatus );
        return TCP_SOCKETS_ERRNO_ERROR;
    }

    /* Allocate socket context. */
    pCellularSocketContext = pvPortMalloc( sizeof( cellularSocketWrapper_t ) );
    if( pCellularSocketContext == NULL )
    {
        IotLogDebug( "Failed to allocate new socket context." );
        ( void ) Cellular_SocketClose( CellularHandle, cellularSocketHandle );
        return TCP_SOCKETS_ERRNO_ENOMEM;
    }

    /* Initialize all the members to sane values. */
    IotLogDebug( "Created CELLULAR Socket %p.", pCellularSocketContext );
    ( void ) memset( pCellularSocketContext, 0, sizeof( cellularSocketWrapper_t ) );
    pCellularSocketContext->cellularSocketHandle = cellularSocketHandle;
    pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_OPEN_FLAG;

    /* Allocate event group for callback function. */
    pCellularSocketContext->socketEventGroupHandle = xEventGroupCreate();
    if( pCellularSocketContext->socketEventGroupHandle == NULL )
    {
        IotLogDebug( "Failed create cellular socket eventGroupHandle %p.", pCellularSocketContext );
        retConnect = TCP_SOCKETS_ERRNO_ENOMEM;
    }

    /* Setup TLS socket */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        /* AT+SQNSPCFG */
        cellularSocketStatus = Cellular_ConfigSSLProfile( CellularHandle,
                                                          cellularSocketHandle->socketId + 1U,
                                                          AWS_CELLULAR_VALIDATE_CERT_EXPDATE_CN,
                                                          ROOTCA_PEM2_NVM_IDX,
                                                          CLIENT_CERT_NVM_IDX,
                                                          CLIENT_PRIVATEKEY_NVM_IDX );
        if( CELLULAR_SUCCESS != cellularSocketStatus )
        {
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
    }

    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        /* AT+SQNSSCFG */
        cellularSocketStatus = Cellular_ConfigTlslSocket( CellularHandle,
                                                          cellularSocketHandle->socketId + 1U,
                                                          true,
                                                          cellularSocketHandle->socketId + 1U );
        if( CELLULAR_SUCCESS != cellularSocketStatus )
        {
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
    }

    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        strncpy(serverAddress.ipAddress.ipAddress, pHostName, sizeof(serverAddress.ipAddress.ipAddress));
        serverAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
        serverAddress.port = port;
        retConnect = prvCellularSocketRegisterCallback( cellularSocketHandle, pCellularSocketContext );
    }

    /* Setup cellular socket receive AT command default timeout. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        cellularSocketStatus = Cellular_CommonSocketSetSockOpt( CellularHandle,
                                                                cellularSocketHandle,
                                                                CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                                CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,
                                                                ( const uint8_t * ) &defaultReceiveTimeoutMs,
                                                                sizeof( uint32_t ) );
        if( cellularSocketStatus != CELLULAR_SUCCESS )
        {
            IotLogDebug( "Failed to setup cellular AT command receive timeout %d.", cellularStatus );
            retConnect = TCP_SOCKETS_ERRNO_ERROR;
        }

        /* Setup cellular socket send/recv timeout. */
        ( void ) prvSetupSocketSendTimeout( pCellularSocketContext, pdMS_TO_TICKS( sendTimeoutMs ) );
        ( void ) prvSetupSocketRecvTimeout( pCellularSocketContext, pdMS_TO_TICKS( receiveTimeoutMs ) );
    }

    /* Cellular socket connect. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                       SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT );
        cellularSocketStatus = Cellular_SocketConnect( CellularHandle, cellularSocketHandle, CELLULAR_SOCKET_ACCESS_MODE, &serverAddress );

        if( cellularSocketStatus != CELLULAR_SUCCESS )
        {
            IotLogDebug( "Failed to establish new connection. Socket status %d.", cellularStatus );
            retConnect = TCP_SOCKETS_ERRNO_ERROR;
        }
    }

    /* Wait the socket connection. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                             SOCKET_OPEN_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT,
                                             pdTRUE,
                                             pdFALSE,
                                             CELLULAR_SOCKET_OPEN_TIMEOUT_TICKS );

        if( waitEventBits != SOCKET_OPEN_CALLBACK_BIT )
        {
            IotLogDebug( "Socket connect timeout." );
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
    }

    /* Cleanup the socket if any error. */
    if( retConnect != TCP_SOCKETS_ERRNO_NONE )
    {
        if( cellularSocketHandle != NULL )
        {
            ( void ) Cellular_SocketClose( CellularHandle, cellularSocketHandle );
            ( void ) Cellular_CommonSocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_CommonSocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_CommonSocketRegisterClosedCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            pCellularSocketContext->cellularSocketHandle = NULL;
        }

        if( pCellularSocketContext->socketEventGroupHandle != NULL )
        {
            vEventGroupDelete( pCellularSocketContext->socketEventGroupHandle );
            pCellularSocketContext->socketEventGroupHandle = NULL;
        }

        vPortFree( pCellularSocketContext );
    }

    *pTcpSocket = pCellularSocketContext;

    return retConnect;
}

/*-----------------------------------------------------------*/

void TCP_Sockets_Disconnect( Socket_t xSocket )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    CellularSocketHandle_t cellularSocketHandle;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

    if( cellularSocketHandle != NULL )
    {
        /* Close sockets. */
        ( void ) Cellular_SocketClose( CellularHandle, cellularSocketHandle );
        ( void ) Cellular_CommonSocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
        ( void ) Cellular_CommonSocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
        ( void ) Cellular_CommonSocketRegisterClosedCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
        pCellularSocketContext->cellularSocketHandle = NULL;
    }

    if( pCellularSocketContext->socketEventGroupHandle != NULL )
    {
        vEventGroupDelete( pCellularSocketContext->socketEventGroupHandle );
        pCellularSocketContext->socketEventGroupHandle = NULL;
    }

    vPortFree( pCellularSocketContext );
}

/*-----------------------------------------------------------*/

int32_t TCP_Sockets_Recv( Socket_t xSocket,
                          void * pvBuffer,
                          size_t xBufferLength )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    uint8_t * buf = ( uint8_t * ) pvBuffer;

    return prvNetworkRecvCellular( pCellularSocketContext, buf, xBufferLength );
}

/*-----------------------------------------------------------*/

/* This function sends the data until timeout or data is completely sent to server.
 * Send timeout unit is TickType_t. Any timeout value greater than UINT32_MAX_MS_TICKS
 * or portMAX_DELAY will be regarded as MAX delay. In this case, this function
 * will not return until all bytes of data are sent successfully or until an error occurs. */
int32_t TCP_Sockets_Send( Socket_t xSocket,
                          const void * pvBuffer,
                          size_t xDataLength )
{
    uint8_t * buf = ( uint8_t * ) pvBuffer;
    BaseType_t retSendLength = 0;
    uint32_t sentLength = 0;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    uint32_t bytesToSend = xDataLength;
    uint64_t entryTimeMs = getTimeMs();
    uint64_t elapsedTimeMs = 0;
    uint32_t sendTimeoutMs;

    if( ( ( pCellularSocketContext->ulFlags & (CELLULAR_SOCKET_OPEN_FLAG | CELLULAR_SOCKET_CONNECT_FLAG) )
            != (CELLULAR_SOCKET_OPEN_FLAG | CELLULAR_SOCKET_CONNECT_FLAG) ) )
    {
        IotLogDebug( "Cellular Sockets_Send Invalid xSocket flag %p 0x%08x",
                     pCellularSocketContext, pCellularSocketContext->ulFlags );
        return ( BaseType_t ) TCP_SOCKETS_ERRNO_ERROR;
    }

    /* Convert ticks to ms delay. */
    sendTimeoutMs = TICKS_TO_MS( pCellularSocketContext->cellularSocketHandle->sendTimeoutMs );

    /* Loop sending data until data is sent completely or timeout. */
    while( bytesToSend > 0U )
    {
        socketStatus = Cellular_SocketSend( CellularHandle,
                                            pCellularSocketContext->cellularSocketHandle,
                                            &buf[ retSendLength ],
                                            bytesToSend,
                                            &sentLength );

        if( socketStatus == CELLULAR_SUCCESS )
        {
            retSendLength = retSendLength + ( BaseType_t ) sentLength;
            bytesToSend = bytesToSend - sentLength;
        }

        /* Check socket status or timeout break. */
        if( ( socketStatus != CELLULAR_SUCCESS ) ||
            ( _calculateElapsedTime( entryTimeMs, sendTimeoutMs, &elapsedTimeMs ) ) )
        {
            if( socketStatus != CELLULAR_SUCCESS )
            {
                if( socketStatus == CELLULAR_SOCKET_CLOSED )
                {
                    /* Socket already closed. No data is sent. */
                    retSendLength = 0;
                }
                else
                {
                    retSendLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_ERROR;
                }
            }
            break;
        }
    }

    IotLogDebug( "Sockets_Send expect %d write %d", xDataLength, sentLength );

    return retSendLength;
}

/*-----------------------------------------------------------*/
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#pragma section
#endif

