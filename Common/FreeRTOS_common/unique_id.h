/*******************************************************************************
* Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
*
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************/
/**********************************************************************************************************************
* File Name    : unique_id.h
* Device(s)    : CK-RX65N
* Tool-Chain   : Renesas CC-RX
* Description  :
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
***********************************************************************************************************************/
#include "platform.h"

#ifndef FREERTOS_COMMON_UNIQUE_ID_H_
#define FREERTOS_COMMON_UNIQUE_ID_H_

#define UUID_NUMBER_OF_REGISTER     ( 4 )
#define UUID_MAX_SIZE               ( sizeof( uint32_t) * UUID_NUMBER_OF_REGISTER )

typedef struct uuid_param_t
{
    uint32_t uuid0;
    uint32_t uuid1;
    uint32_t uuid2;
    uint32_t uuid3;
} uuid_param_t;

bool get_unique_id (uuid_param_t * uuid);

#endif /* FREERTOS_COMMON_UNIQUE_ID_H_ */
