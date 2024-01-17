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
    #define LIBRARY_LOG_LEVEL    LOG_INFO
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

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
extern uint8_t CellularDisableSni;
#endif

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

#if 0   /* Renesas RX */
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
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    CellularSocketHandle_t cellularSocketHandle = NULL;
    CellularError_t cellularError = CELLULAR_SUCCESS;
    BaseType_t retRecvLength = 0;
    uint32_t recvLength = 0;
    EventBits_t waitEventBits = 0;
    uint16_t soketdatasize = 0;

    cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

    /* Wait for +SQNSRING */
    waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                         SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_CLOSE_CALLBACK_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         cellularSocketHandle->recvTimeoutMs );

    if( ( waitEventBits & SOCKET_DATA_RECEIVED_CALLBACK_BIT ) != 0U )
    {
        cellularError = Cellular_SocketRecv( CellularHandle, cellularSocketHandle, buf, len, &recvLength );
        if ( cellularError == CELLULAR_SUCCESS )
        {
            retRecvLength = ( BaseType_t ) recvLength;

            ( void ) Cellular_GetSocketDataSize( CellularHandle, cellularSocketHandle, &soketdatasize );
            if (soketdatasize == 0)
            {
                ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                               SOCKET_DATA_RECEIVED_CALLBACK_BIT );
            }
        }
    }
    else if( ( waitEventBits & SOCKET_CLOSE_CALLBACK_BIT ) != 0U )
    {
        IotLogError( "prvNetworkRecv Socket closed!");
        retRecvLength = TCP_SOCKETS_ERRNO_ECLOSED;
    }
    else
    {
        /* No data */
        retRecvLength = 0;
    }

    IotLogDebug( "prvNetworkRecv expect %d read %d", len, recvLength );
    return retRecvLength;
#else
    CellularSocketHandle_t cellularSocketHandle = NULL;
    BaseType_t retRecvLength = 0;
    uint32_t recvLength = 0;
    TickType_t recvTimeout = 0;
    TickType_t recvStartTime = 0;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    EventBits_t waitEventBits = 0;
    uint16_t soketdatasize = 0;

    cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

    if( pCellularSocketContext->cellularSocketHandle->recvTimeoutMs >= portMAX_DELAY )
    {
        recvTimeout = portMAX_DELAY;
    }
    else
    {
        recvTimeout = pCellularSocketContext->cellularSocketHandle->recvTimeoutMs;
    }

    recvStartTime = xTaskGetTickCount();

    ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                   SOCKET_DATA_RECEIVED_CALLBACK_BIT );

    socketStatus = Cellular_GetSocketDataSize(CellularHandle, cellularSocketHandle, &soketdatasize);

    if ( ( socketStatus == CELLULAR_SUCCESS ) && ( 0 != soketdatasize ) )
    {
        socketStatus = Cellular_SocketRecv( CellularHandle, cellularSocketHandle, buf, len, &recvLength );
    }

    if( ( socketStatus == CELLULAR_SUCCESS ) && ( recvLength == 0U ) )
    {
        /* Calculate remain recvTimeout. */
        if( recvTimeout != portMAX_DELAY )
        {
            if( ( recvStartTime + recvTimeout ) > xTaskGetTickCount() )
            {
                recvTimeout = recvTimeout - ( xTaskGetTickCount() - recvStartTime );
            }
            else
            {
                recvTimeout = 0;
            }
        }

        waitEventBits = xEventGroupWaitBits( pCellularSocketContext->socketEventGroupHandle,
                                             SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_CLOSE_CALLBACK_BIT,
                                             pdTRUE,
                                             pdFALSE,
                                             recvTimeout );

        if( ( waitEventBits & SOCKET_CLOSE_CALLBACK_BIT ) != 0U )
        {
            socketStatus = CELLULAR_SOCKET_CLOSED;
        }
        else if( ( waitEventBits & SOCKET_DATA_RECEIVED_CALLBACK_BIT ) != 0U )
        {
            socketStatus = Cellular_SocketRecv( CellularHandle, cellularSocketHandle, buf, len, &recvLength );
        }
        else
        {
            IotLogInfo( "prvNetworkRecv timeout" );
            socketStatus = CELLULAR_SUCCESS;
            recvLength = 0;
        }
    }

    if( socketStatus == CELLULAR_SUCCESS )
    {
        retRecvLength = ( BaseType_t ) recvLength;
    }
    else
    {
        IotLogError( "prvNetworkRecv failed %d", socketStatus );
        retRecvLength = TCP_SOCKETS_ERRNO_ERROR;
    }

    IotLogDebug( "prvNetworkRecv expect %d read %d", len, recvLength );
    return retRecvLength;
#endif
}

/*-----------------------------------------------------------*/
static void prvCellularSocketOpenCallback( CellularUrcEvent_t urcEvent,
                                           CellularSocketHandle_t socketHandle,
                                           void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
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
    else
    {
        IotLogError( "Spurious socket open callback." );
    }
}
/*-----------------------------------------------------------*/

static void prvCellularSocketDataReadyCallback( CellularSocketHandle_t socketHandle,
                                                void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
        IotLogDebug( "Data ready on Socket %p", pCellularSocketContext );
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_DATA_RECEIVED_CALLBACK_BIT );
    }
    else
    {
        IotLogError( "spurious data callback" );
    }
}

/*-----------------------------------------------------------*/

static void prvCellularSocketClosedCallback( CellularSocketHandle_t socketHandle,
                                             void * pCallbackContext )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) pCallbackContext;

    ( void ) socketHandle;

    if( pCellularSocketContext != NULL )
    {
        IotLogInfo( "Socket Close on Socket %p", pCellularSocketContext );
        pCellularSocketContext->ulFlags = pCellularSocketContext->ulFlags & ( ~CELLULAR_SOCKET_CONNECT_FLAG );
        ( void ) xEventGroupSetBits( pCellularSocketContext->socketEventGroupHandle,
                                     SOCKET_CLOSE_CALLBACK_BIT );
    }
    else
    {
        IotLogError( "spurious socket close callback" );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetupSocketRecvTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t receiveTimeout )
{
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    BaseType_t retSetSockOpt = TCP_SOCKETS_ERRNO_NONE;
    uint32_t receiveTimeoutMs = 0;
    CellularSocketHandle_t cellularSocketHandle = NULL;

    if( pCellularSocketContext == NULL )
    {
        retSetSockOpt = TCP_SOCKETS_ERRNO_EINVAL;
    }
    else
    {
        cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

        if( receiveTimeout >= UINT32_MAX_MS_TICKS )
        {
            /* Check if the ticks cause overflow. */
            receiveTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else if( receiveTimeout >= portMAX_DELAY )
        {
            IotLogWarn( "Receievetimeout %d longer than portMAX_DELAY, %d ms is used instead",
                        receiveTimeout, UINT32_MAX_DELAY_MS );
            receiveTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else
        {
            receiveTimeoutMs = TICKS_TO_MS( receiveTimeout );
        }

        socketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                  cellularSocketHandle,
                                                  CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                  CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,
                                                  ( const uint8_t * ) &receiveTimeoutMs,
                                                  sizeof( uint32_t ) );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            retSetSockOpt = TCP_SOCKETS_ERRNO_EINVAL;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetupSocketSendTimeout( cellularSocketWrapper_t * pCellularSocketContext,
                                             TickType_t sendTimeout )
{
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    BaseType_t retSetSockOpt = TCP_SOCKETS_ERRNO_NONE;
    uint32_t sendTimeoutMs = 0;
    CellularSocketHandle_t cellularSocketHandle = NULL;

    if( pCellularSocketContext == NULL )
    {
        retSetSockOpt = TCP_SOCKETS_ERRNO_EINVAL;
    }
    else
    {
        cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

        if( sendTimeout >= UINT32_MAX_MS_TICKS )
        {
            /* Check if the ticks cause overflow. */
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else if( sendTimeout >= portMAX_DELAY )
        {
            IotLogWarn( "Sendtimeout %d longer than portMAX_DELAY, %d ms is used instead",
                        sendTimeout, UINT32_MAX_DELAY_MS );
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else
        {
            sendTimeoutMs = TICKS_TO_MS( sendTimeout );
        }

        socketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                  cellularSocketHandle,
                                                  CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                  CELLULAR_SOCKET_OPTION_SEND_TIMEOUT,
                                                  ( const uint8_t * ) &sendTimeoutMs,
                                                  sizeof( uint32_t ) );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            retSetSockOpt = TCP_SOCKETS_ERRNO_EINVAL;
        }
    }

    return retSetSockOpt;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCellularSocketRegisterCallback( CellularSocketHandle_t cellularSocketHandle,
                                                     cellularSocketWrapper_t * pCellularSocketContext )
{
    BaseType_t retRegCallback = TCP_SOCKETS_ERRNO_NONE;
    CellularError_t socketStatus = CELLULAR_SUCCESS;

    if( cellularSocketHandle == NULL )
    {
        retRegCallback = TCP_SOCKETS_ERRNO_EINVAL;
    }

    if( retRegCallback == TCP_SOCKETS_ERRNO_NONE )
    {
        socketStatus = Cellular_SocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle,
                                                                 prvCellularSocketDataReadyCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterDataReadyCallback. Socket status %d.", socketStatus );
            retRegCallback = TCP_SOCKETS_ERRNO_ERROR;
        }
    }

    if( retRegCallback == TCP_SOCKETS_ERRNO_NONE )
    {
        socketStatus = Cellular_SocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle,
                                                                  prvCellularSocketOpenCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterSocketOpenCallbac. Socket status %d.", socketStatus );
            retRegCallback = TCP_SOCKETS_ERRNO_ERROR;
        }
    }

    if( retRegCallback == TCP_SOCKETS_ERRNO_NONE )
    {
        socketStatus = Cellular_SocketRegisterClosedCallback( CellularHandle, cellularSocketHandle,
                                                              prvCellularSocketClosedCallback, ( void * ) pCellularSocketContext );

        if( socketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to SocketRegisterClosedCallback. Socket status %d.", socketStatus );
            retRegCallback = TCP_SOCKETS_ERRNO_ERROR;
        }
    }

    return retRegCallback;
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

/*-----------------------------------------------------------*/

BaseType_t TCP_Sockets_Connect( Socket_t * pTcpSocket,
                                const char * pHostName,
                                uint16_t port,
                                uint32_t receiveTimeoutMs,
                                uint32_t sendTimeoutMs )
{
    CellularSocketHandle_t cellularSocketHandle = NULL;
    cellularSocketWrapper_t * pCellularSocketContext = NULL;
    CellularError_t cellularSocketStatus = CELLULAR_INVALID_HANDLE;

    CellularSocketAddress_t serverAddress;
    EventBits_t waitEventBits = 0;
    BaseType_t retConnect = TCP_SOCKETS_ERRNO_NONE;
    const uint32_t defaultReceiveTimeoutMs = CELLULAR_SOCKET_RECV_TIMEOUT_MS;
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    e_aws_cellular_validate_level_t certValidLevel = AWS_CELLULAR_NO_CERT_VALIDATE;
#endif

    /* Create a new TCP socket. */
    cellularSocketStatus = Cellular_CreateSocket( CellularHandle,
                                                  CellularSocketPdnContextId,
                                                  CELLULAR_SOCKET_DOMAIN_AF_INET,
                                                  CELLULAR_SOCKET_TYPE_STREAM,
                                                  CELLULAR_SOCKET_PROTOCOL_TCP,
                                                  &cellularSocketHandle );

    if( cellularSocketStatus != CELLULAR_SUCCESS )
    {
        IotLogError( "Failed to create cellular sockets. %d", cellularSocketStatus );
        retConnect = TCP_SOCKETS_ERRNO_ERROR;
    }

    /* Allocate socket context. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        pCellularSocketContext = pvPortMalloc( sizeof( cellularSocketWrapper_t ) );

        if( pCellularSocketContext == NULL )
        {
            IotLogError( "Failed to allocate new socket context." );
            ( void ) Cellular_SocketClose( CellularHandle, cellularSocketHandle );
            retConnect = TCP_SOCKETS_ERRNO_ENOMEM;
        }
        else
        {
            /* Initialize all the members to sane values. */
            IotLogDebug( "Created CELLULAR Socket %p.", pCellularSocketContext );
            ( void ) memset( pCellularSocketContext, 0, sizeof( cellularSocketWrapper_t ) );
            pCellularSocketContext->cellularSocketHandle = cellularSocketHandle;
            pCellularSocketContext->ulFlags |= CELLULAR_SOCKET_OPEN_FLAG;
            pCellularSocketContext->socketEventGroupHandle = NULL;
        }
    }

    /* Allocate event group for callback function. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        pCellularSocketContext->socketEventGroupHandle = xEventGroupCreate();

        if( pCellularSocketContext->socketEventGroupHandle == NULL )
        {
            IotLogError( "Failed create cellular socket eventGroupHandle %p.", pCellularSocketContext );
            retConnect = TCP_SOCKETS_ERRNO_ENOMEM;
        }
    }

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
    /* Setup TLS socket */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        /* certValidLevel */
        if ( CellularDisableSni == pdTRUE )
        {
            certValidLevel = AWS_CELLULAR_NO_CERT_VALIDATE;
        }
        else
        {
            certValidLevel = AWS_CELLULAR_VALIDATE_CERT_EXPDATE_CN;
        }
        LogDebug( ( "socketId:%u certValidLevel %u\r\n", cellularSocketHandle->socketId + 1U, certValidLevel ) );

        /* AT+SQNSPCFG */
        cellularSocketStatus = Cellular_ConfigSSLProfile( CellularHandle,
                                                          cellularSocketHandle->socketId + 1U,
                                                          certValidLevel,
                                                          ROOTCA_PEM2_NVM_IDX,
                                                          CLIENT_CERT_NVM_IDX,
                                                          CLIENT_PRIVATEKEY_NVM_IDX );
        if( CELLULAR_SUCCESS != cellularSocketStatus )
        {
            IotLogError( "Failed Cellular_ConfigSSLProfile %u.", cellularSocketStatus );
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
    }

    /* Activate SSL/TLS on the socket. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        /* AT+SQNSSCFG */
        cellularSocketStatus = Cellular_ConfigTlslSocket( CellularHandle,
                                                          cellularSocketHandle->socketId + 1U,
                                                          true,
                                                          cellularSocketHandle->socketId + 1U );
        if( CELLULAR_SUCCESS != cellularSocketStatus )
        {
            IotLogError( "Failed Cellular_ConfigTlslSocket(true) %u.", cellularSocketStatus );
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
        else
        {
            retConnect = TCP_SOCKETS_ERRNO_NONE;
        }
    }
#endif

#if 0   /* Renesas RX */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        cellularSocketStatus = Sockets_GetHostByName(pHostName, (const uint8_t *)serverAddress.ipAddress.ipAddress);
        if (cellularSocketStatus == CELLULAR_SUCCESS )
        {
            serverAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
            serverAddress.port = port;

            LogDebug( ( "Ip address %s port %d\r\n", serverAddress.ipAddress.ipAddress, serverAddress.port ) );
            retConnect = prvCellularSocketRegisterCallback( cellularSocketHandle, pCellularSocketContext );
        }
        else
        {
            IotLogError( "Failed Get Host By Name." );
        }
    }
#else
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        serverAddress.ipAddress.ipAddressType = CELLULAR_IP_ADDRESS_V4;
        strncpy( serverAddress.ipAddress.ipAddress, pHostName, CELLULAR_IP_ADDRESS_MAX_SIZE );
        serverAddress.port = port;

        IotLogDebug( "Ip address %s port %d\r\n", serverAddress.ipAddress.ipAddress, serverAddress.port );
        retConnect = prvCellularSocketRegisterCallback( cellularSocketHandle, pCellularSocketContext );
    }
#endif
    /* Setup cellular socket recv AT command default timeout. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        cellularSocketStatus = Cellular_SocketSetSockOpt( CellularHandle,
                                                          cellularSocketHandle,
                                                          CELLULAR_SOCKET_OPTION_LEVEL_TRANSPORT,
                                                          CELLULAR_SOCKET_OPTION_RECV_TIMEOUT,
                                                          ( const uint8_t * ) &defaultReceiveTimeoutMs,
                                                           sizeof( uint32_t ) );

        if( cellularSocketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to setup cellular AT command receive timeout %d.", cellularSocketStatus );
            retConnect = TCP_SOCKETS_ERRNO_ERROR;
        }
    }

    /* Setup cellular socket send/recv timeout. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        retConnect = prvSetupSocketSendTimeout( pCellularSocketContext, pdMS_TO_TICKS( sendTimeoutMs ) );
    }

    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        retConnect = prvSetupSocketRecvTimeout( pCellularSocketContext, pdMS_TO_TICKS( receiveTimeoutMs ) );
    }

    /* Cellular socket connect. */
    if( retConnect == TCP_SOCKETS_ERRNO_NONE )
    {
        ( void ) xEventGroupClearBits( pCellularSocketContext->socketEventGroupHandle,
                                       SOCKET_DATA_RECEIVED_CALLBACK_BIT | SOCKET_OPEN_FAILED_CALLBACK_BIT );
        cellularSocketStatus = Cellular_SocketConnect( CellularHandle, cellularSocketHandle, CELLULAR_SOCKET_ACCESS_MODE, &serverAddress );

        if( cellularSocketStatus != CELLULAR_SUCCESS )
        {
            IotLogError( "Failed to establish new connection. Socket status %d.", cellularSocketStatus );
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
            IotLogError( "Socket connect timeout." );
            retConnect = TCP_SOCKETS_ERRNO_ENOTCONN;
        }
    }

    /* Cleanup the socket if any error. */
    if( retConnect != TCP_SOCKETS_ERRNO_NONE )
    {
        if( cellularSocketHandle != NULL )
        {
            ( void ) Cellular_SocketClose( CellularHandle, cellularSocketHandle );
            ( void ) Cellular_SocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_SocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_SocketRegisterClosedCallback( CellularHandle, cellularSocketHandle, NULL, NULL );

            if( pCellularSocketContext != NULL )
            {
                pCellularSocketContext->cellularSocketHandle = NULL;
            }
        }

        if( ( pCellularSocketContext != NULL ) && ( pCellularSocketContext->socketEventGroupHandle != NULL ) )
        {
            vEventGroupDelete( pCellularSocketContext->socketEventGroupHandle );
            pCellularSocketContext->socketEventGroupHandle = NULL;
        }

        if( pCellularSocketContext != NULL )
        {
            vPortFree( pCellularSocketContext );
            pCellularSocketContext = NULL;
        }
    }

    *pTcpSocket = pCellularSocketContext;

    return retConnect;
}

/*-----------------------------------------------------------*/

void TCP_Sockets_Disconnect( Socket_t xSocket )
{
    int32_t retClose = TCP_SOCKETS_ERRNO_NONE;
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    CellularSocketHandle_t cellularSocketHandle = NULL;

    /* xSocket need to be check against SOCKET_INVALID_SOCKET. */
    /* coverity[misra_c_2012_rule_11_4_violation] */
    if( ( pCellularSocketContext == NULL ) || ( xSocket == SOCKETS_INVALID_SOCKET ) )
    {
        IotLogError( "Invalid xSocket %p", pCellularSocketContext );
        retClose = TCP_SOCKETS_ERRNO_EINVAL;
    }
    else
    {
        cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;
    }

    if( retClose == TCP_SOCKETS_ERRNO_NONE )
    {
        if( cellularSocketHandle != NULL )
        {
            /* Close sockets. */
            if( Cellular_SocketClose( CellularHandle, cellularSocketHandle ) != CELLULAR_SUCCESS )
            {
                IotLogWarn( "Failed to destroy connection." );
                retClose = TCP_SOCKETS_ERRNO_ERROR;
            }

            ( void ) Cellular_SocketRegisterDataReadyCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_SocketRegisterSocketOpenCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            ( void ) Cellular_SocketRegisterClosedCallback( CellularHandle, cellularSocketHandle, NULL, NULL );
            pCellularSocketContext->cellularSocketHandle = NULL;
        }

        if( pCellularSocketContext->socketEventGroupHandle != NULL )
        {
            vEventGroupDelete( pCellularSocketContext->socketEventGroupHandle );
            pCellularSocketContext->socketEventGroupHandle = NULL;
        }

        vPortFree( pCellularSocketContext );
    }

    (void) retClose;
    IotLogDebug( "Sockets close exit with code %d", retClose );
}

/*-----------------------------------------------------------*/

int32_t TCP_Sockets_Recv( Socket_t xSocket,
                          void * pvBuffer,
                          size_t xBufferLength )
{
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    uint8_t * buf = ( uint8_t * ) pvBuffer;
    BaseType_t retRecvLength = 0;

    if( pCellularSocketContext == NULL )
    {
        IotLogError( "Cellular prvNetworkRecv Invalid xSocket %p", pCellularSocketContext );
        retRecvLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_EINVAL;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) ||
             ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U ) )
    {
        IotLogError( "Cellular prvNetworkRecv Invalid xSocket flag %p %u",
                     pCellularSocketContext, pCellularSocketContext->ulFlags );
        retRecvLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_ENOTCONN;
    }
    else
    {
        retRecvLength = prvNetworkRecvCellular( pCellularSocketContext, buf, xBufferLength );
    }

    return retRecvLength;
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
    uint8_t __far * buf = ( uint8_t __far * ) pvBuffer;
    CellularSocketHandle_t cellularSocketHandle = NULL;
    BaseType_t retSendLength = 0;
    uint32_t sentLength = 0;
    CellularError_t socketStatus = CELLULAR_SUCCESS;
    cellularSocketWrapper_t * pCellularSocketContext = ( cellularSocketWrapper_t * ) xSocket;
    uint32_t bytesToSend = xDataLength;
    uint64_t entryTimeMs = getTimeMs();
    uint64_t elapsedTimeMs = 0;
    uint32_t sendTimeoutMs = 0;

    if( pCellularSocketContext == NULL )
    {
        IotLogError( "Cellular Sockets_Send Invalid xSocket %p", pCellularSocketContext );
        retSendLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_ERROR;
    }
    else if( ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_OPEN_FLAG ) == 0U ) ||
             ( ( pCellularSocketContext->ulFlags & CELLULAR_SOCKET_CONNECT_FLAG ) == 0U ) )
    {
        IotLogError( "Cellular Sockets_Send Invalid xSocket flag %p 0x%08x",
                     pCellularSocketContext, pCellularSocketContext->ulFlags );
        retSendLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_ERROR;
    }
    else
    {
        cellularSocketHandle = pCellularSocketContext->cellularSocketHandle;

        /* Convert ticks to ms delay. */
        if( pCellularSocketContext->cellularSocketHandle->sendTimeoutMs >= UINT32_MAX_MS_TICKS )
        {
            /* Check if the ticks cause overflow. */
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else if( pCellularSocketContext->cellularSocketHandle->sendTimeoutMs >= portMAX_DELAY )
        {
            IotLogWarn( "Sendtimeout %d longer than portMAX_DELAY, %d ms is used instead",
                    pCellularSocketContext->cellularSocketHandle->sendTimeoutMs, UINT32_MAX_DELAY_MS );
            sendTimeoutMs = UINT32_MAX_DELAY_MS;
        }
        else
        {
            sendTimeoutMs = TICKS_TO_MS( pCellularSocketContext->cellularSocketHandle->sendTimeoutMs );
        }

        /* Loop sending data until data is sent completely or timeout. */
        while( bytesToSend > 0U )
        {
            socketStatus = Cellular_SocketSend( CellularHandle,
                                                cellularSocketHandle,
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
                    if( ( socketStatus == CELLULAR_SOCKET_CLOSED ) ||
                        ( socketStatus == CELLULAR_TIMEOUT ) )
                    {
                        /* Socket already closed. No data is sent. */
                        retSendLength = 0;
                    }
                    else
                    {
                        retSendLength = ( BaseType_t ) TCP_SOCKETS_ERRNO_ERROR;
                    }
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
                    IotLogError( "TCP_Sockets_Send() error : socketStatus %d, retSendLength:%d",
                                 socketStatus, retSendLength );
#endif
                }

                break;
            }
        }

        IotLogDebug( "Sockets_Send expect %d write %d", xDataLength, sentLength );
    }

    return retSendLength;
}

/*-----------------------------------------------------------*/
#if 0   /* Renesas RX */
static CellularError_t Sockets_GetHostByName( const char * pcHostName , const uint8_t * addr)
{
    CellularError_t ret;

    ret = Cellular_GetHostByName(CellularHandle, RM_CELLULAR_RYZ_DEFAULT_BEARER_CONTEXT_ID,
                                    pcHostName,
                                    (char *)addr);

    return ret;
}
#endif
/*-----------------------------------------------------------*/

