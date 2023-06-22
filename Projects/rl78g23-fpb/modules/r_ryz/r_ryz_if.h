/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_ryz_if.h
 * Version      : 1.0
 * Description  : API functions definition for SX ULPGN of RX65N.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : DD.MM.2023 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"
#include "r_ryz_config.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef R_RYZ_CFG_IF_H
#define R_RYZ_CFG_IF_H

/* Driver Version Number. */
#define WIFI_SX_ULPGN_CFG_VERSION_MAJOR           (1)
#define WIFI_SX_ULPGN_CFG_VERSION_MINOR           (13)

/* Configuration */
#define RYZ_PRTCL_TCP                             (0)       // Socket type = TCP
#define RYZ_PRTCL_UDP                             (1)       // Socket type = UDP
#define RYZ_PRTCL_TLS                             (2)       // Socket type = TLS
#define RYZ_IP_VER4                               (4)       // IP version = IPv4

#define RYZ_C_CH_FAR                               const char __far *
#define RYZ_C_U8_FAR                               const uint8_t __far *

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/*  WiFi API error code */
typedef enum
{
    WIFI_SUCCESS            = 0,      // success
    WIFI_ERR_PARAMETER      = -1,     // invalid parameter
    WIFI_ERR_ALREADY_OPEN   = -2,     // already WIFI module opened
    WIFI_ERR_NOT_OPEN       = -3,     // WIFI module is not opened
    WIFI_ERR_SERIAL_OPEN    = -4,     // serial open failed
    WIFI_ERR_MODULE_COM     = -5,     // cannot communicate WiFi module
    WIFI_ERR_NOT_CONNECT    = -6,     // not connect to access point
    WIFI_ERR_SOCKET_NUM     = -7,     // no available sockets
    WIFI_ERR_SOCKET_CREATE  = -8,     // create socket failed
    WIFI_ERR_CHANGE_SOCKET  = -9,     // cannot change socket
    WIFI_ERR_SOCKET_CONNECT = -10,    // cannot connect socket
    WIFI_ERR_BYTEQ_OPEN     = -11,    // cannot assigned BYTEQ
    WIFI_ERR_SOCKET_TIMEOUT = -12,    // socket timeout
    WIFI_ERR_TAKE_MUTEX     = -13     // cannot take mutex
} cellular_err_t;

/* Security type */
typedef enum
{
    WIFI_SECURITY_OPEN = 0,           // Open
    WIFI_SECURITY_WEP,                // WEP
    WIFI_SECURITY_WPA,                // WPA
    WIFI_SECURITY_WPA2,               // WPA2
    WIFI_SECURITY_UNDEFINED           // Undefined
} wifi_security_t;

/* Query current socket status */
typedef enum
{
    SOCKET_STATUS_CLOSED = 0,   // "CLOSED"
    SOCKET_STATUS_SOCKET,       // "SOCKET"
    SOCKET_STATUS_BOUND,        // "BOUND"
    SOCKET_STATUS_LISTEN,       // "LISTEN"
    SOCKET_STATUS_CONNECTED,    // "CONNECTED"
    SOCKET_STATUS_BROKEN,       // "BROKEN"
    SOCKET_STATUS_MAX           // Stopper
} sx_ulpgn_socket_status_t;

/* Error event for user callback */
typedef enum
{
    WIFI_EVENT_WIFI_REBOOT = 0,       // reboot WIFI
    WIFI_EVENT_WIFI_DISCONNECT,       // disconnected WIFI
    WIFI_EVENT_SERIAL_OVF_ERR,        // serial : overflow error
    WIFI_EVENT_SERIAL_FLM_ERR,        // serial : flaming error
    WIFI_EVENT_SERIAL_RXQ_OVF_ERR,    // serial : receiving queue overflow
    WIFI_EVENT_RCV_TASK_RXB_OVF_ERR,  // receiving task : receive buffer overflow
    WIFI_EVENT_SOCKET_CLOSED,         // socket is closed
    WIFI_EVENT_SOCKET_RXQ_OVF_ERR     // socket : receiving queue overflow
} wifi_err_event_enum_t;

typedef enum
{
    PRTCL_TYPE_TCP = 0,       /* Disconnected WIFI module */
    PRTCL_TYPE_UDP,           /* Connected WIFI module    */
    PRTCL_TYPE_SSL,           /* Connected access point   */
} e_ryz_prtcl_type_t;

typedef enum
{
    SSL_DEACTIVATE = 0,       /* Deactivate secure socket */
    SSL_ACTIVATE              /* Activate SSL/TLS on the socket */
} e_ryz_ssl_enable_t;

typedef enum
{
    WRITE_CERTIFICATE = 0,    /*  */
    WRITE_PRIVATEKEY          /*  */
} e_ryz_write_type_t;

typedef struct
{
    wifi_err_event_enum_t event;      // Error event
    uint8_t socket_number;            // Socket number
} wifi_err_event_t;

/* Certificate information */
typedef struct {
    uint8_t  num_of_files;            // certificate number
    struct {
        uint8_t  file_name[20];       // certificate file name
    } cert[10];
} wifi_certificate_infomation_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_RYZ_Open
 * Description  : Open Cellular Module.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_SERIAL_OPEN
 *                WIFI_ERR_SOCKET_BYTEQ
 *                WIFI_ERR_ALREADY_OPEN
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_Open (void);

/**********************************************************************************************************************
 * Function Name: R_RYZ_Close
 * Description  : Close Cellular Module.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *********************************************************************************************************************/
cellular_err_t R_RYZ_Close (void);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectAP
 * Description  : Connect to Access Point.
 * Arguments    : ap_name
 *                user_id
 *                passwd
 *                auth_type
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectAP (RYZ_C_CH_FAR ap_name, RYZ_C_CH_FAR user_id, RYZ_C_CH_FAR passwd, uint8_t auth_type);

/**********************************************************************************************************************
 * Function Name: R_RYZ_DisconnectAP
 * Description  : Disconnect from Access Point.
 * Arguments    : none.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *********************************************************************************************************************/
cellular_err_t R_RYZ_DisconnectAP (void);

/**********************************************************************************************************************
 * Function Name: R_RYZ_IsConnected
 * Description  : Check connected access point.
 * Arguments    : none.
 * Return Value : 0  - connected
 *                -1 - not connected
 *********************************************************************************************************************/
int32_t R_RYZ_IsConnected (void);

/**********************************************************************************************************************
 * Function Name: R_RYZ_CreateSocket
 * Description  : Create socket.
 * Arguments    : prtcl_type
 *                ip_version
 * Return Value : Positive number - created socket number
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_CREATE
 *********************************************************************************************************************/
int32_t R_RYZ_CreateSocket (e_ryz_prtcl_type_t prtcl_type, uint32_t ip_version);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectSocket
 * Description  : Open client mode TCP/UDP socket.
 * Arguments    : sock_id
 *                ip_address
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectSocket (uint8_t sock_id, e_ryz_prtcl_type_t tx_prtcl,
        uint16_t remote_port, uint32_t ipaddr, uint16_t local_port, uint8_t conn_mode);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConnectSocketByHostName
 * Description  : Open client mode TCP/UDP socket.
 * Arguments    : sock_id
 *                host_name
 *                port
 *                destination
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConnectSocketByHostName (uint8_t sock_id, e_ryz_prtcl_type_t tx_prtcl,
        uint16_t remote_port, RYZ_C_CH_FAR host_name, uint16_t local_port, uint8_t conn_mode);

/**********************************************************************************************************************
 * Function Name: R_RYZ_SendSocket
 * Description  : Send data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of sent data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_RYZ_SendSocket (uint8_t socket_number, uint8_t * data, uint32_t length, uint32_t timeout_ms);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ReceiveSocket
 * Description  : Receive data on connecting socket.
 * Arguments    : socket_number
 *                data
 *                length
 *                timeout_ms
 * Return Value : Positive number - number of received data
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
int32_t R_RYZ_ReceiveSocket (uint8_t socket_number, uint8_t * data, uint32_t length, uint32_t timeout_ms);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ShutdownSocket
 * Description  : Shutdown connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ShutdownSocket (uint8_t socket_number);

/**********************************************************************************************************************
 * Function Name: R_RYZ_CloseSocket
 * Description  : Disconnect connecting socket.
 * Arguments    : socket_number
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_CHANGE_SOCKET
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_CloseSocket (uint8_t socket_number);

/**********************************************************************************************************************
 * Function Name: R_RYZ_DnsQuery
 * Description  : Execute DNS query.
 * Arguments    : domain_name
 *                ip_address
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_NOT_CONNECT
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_DnsQuery (RYZ_C_CH_FAR domain_name, uint32_t * ip_address);

/**********************************************************************************************************************
 * Function Name: R_RYZ_GetVersion
 * Description  : Get FIT module version.
 * Arguments    : none.
 * Return Value : FIT module version
 *********************************************************************************************************************/
uint32_t R_RYZ_GetVersion (void);

/**********************************************************************************************************************
 * Function Name: R_RYZ_GetSocketStatus
 * Description  : Get socket status.
 * Arguments    : socket_number
 * Return Value : -1    - not exist
 *                other - socket table pointer
 *********************************************************************************************************************/
int32_t R_RYZ_GetSocketStatus (uint8_t socket_number);

/**********************************************************************************************************************
 * Function Name: R_RYZ_ConfigTlslSocket
 * Description  : Request TLS socket communication.
 * Arguments    : sock_id    Socket connection identifier.
 *                enable     Security enable status.
 *                spid       Security profile identifier.
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_SOCKET_NUM
 *                WIFI_ERR_NOT_CONNECT
 *********************************************************************************************************************/
cellular_err_t R_RYZ_ConfigTlslSocket (uint8_t sock_id, e_ryz_ssl_enable_t enable, uint8_t spid);

/**********************************************************************************************************************
 * Function Name: R_RYZ_WriteCertificate
 * Description  : Write server certificate to module.
 * Arguments    : type
 *                index
 *                p_data
 *                len
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *                WIFI_ERR_NOT_OPEN
 *                WIFI_ERR_TAKE_MUTEX
 *                WIFI_ERR_MODULE_COM
 *********************************************************************************************************************/
cellular_err_t R_RYZ_WriteCertificate (e_ryz_write_type_t type, uint32_t index, RYZ_C_U8_FAR p_data, uint32_t len);

/**********************************************************************************************************************
 * Function Name: R_RYZ_SetTlsProfile
 * Description  : Associate server information to certificate.
 * Arguments    : spid                   Security Profile identifier, integer in range [0-6].
 *                ca_cert_id             Trusted Certificate Authority certificate ID, integer in range [0-19].
 *                client_cert_id         Client certificate ID, integer in range [0-19].
 *                client_privateKey_id   Client private key ID, integer in range [0-19].
 * Return Value : WIFI_SUCCESS
 *                WIFI_ERR_PARAMETER
 *********************************************************************************************************************/
cellular_err_t R_RYZ_SetTlsProfile (uint8_t spid, uint8_t ca_cert_id, uint8_t cli_cert_id, uint8_t cli_privateKey_id);
#endif /* R_RYZ_CFG_IF_H */
