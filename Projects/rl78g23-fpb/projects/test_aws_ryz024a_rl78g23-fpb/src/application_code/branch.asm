;;/*
;; * branch.asm
;; * Copyright (C) Renesas Electronics Corporation and/or its affiliates.
;; * All Rights Reserved.
;; *
;; * SPDX-License-Identifier: MIT
;; *
;; * Permission is hereby granted, free of charge, to any person obtaining a copy of
;; * this software and associated documentation files (the "Software"), to deal in
;; * the Software without restriction, including without limitation the rights to
;; * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
;; * the Software, and to permit persons to whom the Software is furnished to do so,
;; * subject to the following conditions:
;; *
;; * The above copyright notice and this permission notice shall be included in all
;; * copies or substantial portions of the Software.
;; *
;; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
;; * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
;; * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
;; * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
;; * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;; */
;-------------------------------------------------------------------------------
; Interrupt vector branch table for User application.
;-------------------------------------------------------------------------------

.extern    _start

usr_vect   .cseg at 0x00001300
           br       !!_start        ;reset
           .db4     0xffffffff
           .db4     0xffffffff      ;vect=0x04
           .db4     0xffffffff      ;vect=0x06
           .db4     0xffffffff      ;vect=0x08
           .db4     0xffffffff      ;vect=0x0A
           .db4     0xffffffff      ;vect=0x0C
           .db4     0xffffffff      ;vect=0x0E
           .db4     0xffffffff      ;vect=0x10
           .db4     0xffffffff      ;vect=0x12
           br       !!_sci2_txi2_isr;vect=0x14
           br       !!_sci2_rxi2_isr;vect=0x16
           br       !!_sci2_eri2_isr;vect=0x18
           .db4     0xffffffff      ;vect=0x1A
           .db4     0xffffffff      ;vect=0x1C
           .db4     0xffffffff      ;vect=0x1E
           .db4     0xffffffff      ;vect=0x20
           .db4     0xffffffff      ;vect=0x22
           .db4     0xffffffff      ;vect=0x24
           .db4     0xffffffff      ;vect=0x26
           .db4     0xffffffff      ;vect=0x28
           .db4     0xffffffff      ;vect=0x2A
           .db4     0xffffffff      ;vect=0x2C
           .db4     0xffffffff      ;vect=0x2E
           .db4     0xffffffff      ;vect=0x30
           .db4     0xffffffff      ;vect=0x32
           .db4     0xffffffff      ;vect=0x34
           .db4     0xffffffff      ;vect=0x36
           br       !!_vPortTickISR ;vect=0x38
           .db4     0xffffffff      ;vect=0x3A
           br       !!_sci3_txi3_isr;vect=0x3C
           br       !!_sci3_rxi3_isr;vect=0x3E
           .db4     0xffffffff      ;vect=0x40
           .db4     0xffffffff      ;vect=0x42
           .db4     0xffffffff      ;vect=0x44
           .db4     0xffffffff      ;vect=0x46
           .db4     0xffffffff      ;vect=0x48
           .db4     0xffffffff      ;vect=0x4A
           .db4     0xffffffff      ;vect=0x4C
           .db4     0xffffffff      ;vect=0x4E
           .db4     0xffffffff      ;vect=0x50
           .db4     0xffffffff      ;vect=0x52
           .db4     0xffffffff      ;vect=0x54
           .db4     0xffffffff      ;vect=0x56
           .db4     0xffffffff      ;vect=0x58
           .db4     0xffffffff      ;vect=0x5A
           .db4     0xffffffff      ;vect=0x5C
           br       !!_sci3_eri3_isr;vect=0x5E
           .db4     0xffffffff      ;vect=0x60
           .db4     0xffffffff      ;vect=0x62
           .db4     0xffffffff      ;vect=0x64
           .db4     0xffffffff      ;vect=0x66
           .db4     0xffffffff      ;vect=0x68
           .db4     0xffffffff      ;vect=0x6A
           .db4     0xffffffff      ;vect=0x6C
           .db4     0xffffffff      ;vect=0x6E
           .db4     0xffffffff      ;vect=0x70
           .db4     0xffffffff      ;vect=0x72
           .db4     0xffffffff      ;vect=0x74
           .db4     0xffffffff      ;vect=0x76
           .db4     0xffffffff      ;vect=0x78
           .db4     0xffffffff      ;vect=0x7A
           .db4     0xffffffff      ;vect=0x7C
           br       !!_vPortYield   ;vect=0x7E
