PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_wifi_da16xxx
=======

Document Number
---------------
R01AN7321EU (English version)

Version
-------
v1.30

Overview
--------------------------------------------------------------------------------
The r_wifi_da16xxx module is a wifi driver.

This driver is tested and works with the following toolchain:
  Renesas Electronics C Compiler for RL78 Family V1.13.00
All API calls and their supporting interface definitions are located in r_wifi_da16xxx_if.h.

Features
--------
* UART baud rate set
* DNS query
* Ping IP
* DHCP Client
* SNTP Client
* AP Scan
* AP Connect (open, WEP, WPA, WPA2)
* AP Disconnect
* TCP Client Sockets (up to 2 sockets)
* TLS Client Sockets (up to 2 sockets)
* On-chip MQTT (w/TLS) Support
* On-chip HTTP client
* Supports FreeRTOS-based user application
* Supports Bare metal-based user application

Supported MCUs
--------------
* RL78/G23 Group

Boards Tested On
----------------
* RL78/G23 128p FPB

Required Packages
-----------------
* r_bsp
* r_sci_rl (check demo project)
* r_byteq (check demo project)

How to add to your project
--------------------------
Please refer to the Adding Software Integration System Modules to Projects.
"r01an5522ej0162-rl78-bsp.pdf"(Only English version)
"r01an5522jj0162-rl78-bsp.pdf"(Only Japanese version)

Toolchain(s) Used
-----------------
* Renesas Electronics C Compiler for RL78 Family V1.13.00

File Structure
--------------
r_wifi_da16xxx
|   readme.txt
|   r_wifi_da16xxx_if.h
|
+---doc
|    \---en
|           r01an7321eu{VERSION_NUMBER}-rl78-da16xxx-wifi.pdf
|
\---src
        r_wifi_da16xxx_atcmd.c
        r_wifi_da16xxx_common.c
        r_wifi_da16xxx_http.c
        r_wifi_da16xxx_mqtt.c
        r_wifi_da16xxx_os_wrap.c
        r_wifi_da16xxx_os_wrap.h
        r_wifi_da16xxx_private.h
        r_wifi_da16xxx_tcp.c
        r_wifi_da16xxx_tcp_tls.c

r_config
    r_wifi_da16xxx_config.h
