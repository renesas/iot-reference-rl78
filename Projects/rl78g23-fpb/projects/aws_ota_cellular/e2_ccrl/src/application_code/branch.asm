;;/**********************************************************************************************************************
;; * DISCLAIMER
;; * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
;; * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
;; * applicable laws, including copyright laws.
;; * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
;; * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
;; * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
;; * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
;; * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
;; * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;; * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
;; * this software. By using this software, you agree to the additional terms and conditions found by accessing the
;; * following link:
;; * http://www.renesas.com/disclaimer
;; *
;; * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
;; *********************************************************************************************************************/
;;/**********************************************************************************************************************
;; * File Name    : branch.asm
;; * Version      : 0.0
;; * Description  : .
;; *********************************************************************************************************************/
;;/**********************************************************************************************************************
;; * History : DD.MM.YYYY Version  Description
;;
;; *********************************************************************************************************************/

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
           br       !!_vPortTickISR  ;vect=0x38
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
           br       !!_vPortYield;vect=0x7E
