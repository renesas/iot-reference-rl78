/*
 * base64_decode.h
 * Copyright (C) Renesas Electronics Corporation and/or its affiliates.
 * All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
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

#ifndef BASE64_DECODE_H_
#define BASE64_DECODE_H_

#include "platform.h"

#if defined(__CCRX__)
#define BASE64_FAR
#define STRCHR          ( strchr )
 #if !defined(STRSTR)
 #define STRSTR          ( strstr )
 #endif
#elif defined (__llvm__)
#define BASE64_FAR      __far
#define STRCHR          ( _COM_strchr_f )
 #if !defined(STRSTR)
 #define STRSTR          ( _COM_strstr_ff )
 #endif
#elif defined(__GNUC__)
#define BASE64_FAR
#define STRCHR          ( strchr )
 #if !defined(STRSTR)
 #define STRSTR          ( strstr )
 #endif
#elif defined(__CCRL__)
#define BASE64_FAR      __far
#define STRCHR          ( _COM_strchr_f )
 #if !defined(STRSTR)
 #define STRSTR          ( _COM_strstr_ff )
 #endif
#elif defined(__ICCRL78__)
#define BASE64_FAR      __far
#define STRCHR          ( strchr )
 #if !defined(STRSTR)
 #define STRSTR          ( strstr )
 #endif
#endif

uint32_t base64_decode(uint8_t BASE64_FAR *source, uint8_t BASE64_FAR *result, uint32_t size);

#endif /* BASE64_DECODE_H_ */
