/* utils.c - TinyCrypt platform-dependent run-time operations */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <tinycrypt/utils.h>
#include <tinycrypt/constants.h>

#include <string.h>

#define MASK_TWENTY_SEVEN 0x1b

TC_UINT32 _copy(uint8_t *to, TC_UINT32 to_len,
        const uint8_t *from, TC_UINT32 from_len)
{
    if (from_len <= to_len) {
#if defined(__CCRL__)
        (void)_COM_memcpy_ff(to, from, from_len);
#else
        (void)memcpy(to, from, from_len);
#endif
        return from_len;
    } else {
        return TC_CRYPTO_FAIL;
    }
}

void _set(void *to, uint8_t val, TC_UINT32 len)
{
#if defined(__CCRL__)
    (void)_COM_memset_f(to, val, len);
#else
    (void)memset(to, val, len);
#endif
}

/*
 * Doubles the value of a byte for values up to 127.
 */
uint8_t _double_byte(uint8_t a)
{
    return ((a<<1) ^ ((a>>7) * MASK_TWENTY_SEVEN));
}

TC_INT32 _compare(const uint8_t *a, const uint8_t *b, TC_SIZE_T size)
{
    const uint8_t *tempa = a;
    const uint8_t *tempb = b;
    uint8_t result = 0;

    for (TC_UINT32 i= 0; i < size; i++) {
        result |= tempa[i] ^ tempb[i];
    }
    return result;
}
