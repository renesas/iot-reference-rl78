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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : branch_table.c
 * Version      : 0.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description

 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "platform.h"
#include "branch_table.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#if defined(__CCRL__)
#define BL_NEAR_FUNC                __near
#define BL_ISR_NEAR_FUNC            BL_NEAR_FUNC
#elif defined(__ICCRL78__)
#define BL_NEAR_FUNC                __near_func
#define BL_ISR_NEAR_FUNC            __interrupt BL_NEAR_FUNC
#endif

#define FWUP_ISR_FUNC_NAME(x)  br_##x##_interrupt

#define FWUP_BRANCH_FUNC(x)  \
do {\
    if(0 == g_using_uservector)\
    {\
        BL_ISR_##x();\
        return;\
    }\
     jump_branch_interrupts(0x##x);\
} while (0)

#define FWUP_ISR_FUNC(x)  BL_ISR_NEAR_FUNC static void  FWUP_ISR_FUNC_NAME(x)(void) {FWUP_BRANCH_FUNC(x);}

#if defined(__CCRL__)
#define FWUP_ISR_VECT(x)  FWUP_ISR_FUNC_NAME(x)(vect=0x##x##)
#pragma interrupt FWUP_ISR_VECT(04) /* INTWDTI */
#pragma interrupt FWUP_ISR_VECT(06) /* INTLVI */
#pragma interrupt FWUP_ISR_VECT(08) /* INTP0 */
#pragma interrupt FWUP_ISR_VECT(0A) /* INTP1 */
#pragma interrupt FWUP_ISR_VECT(0C) /* INTP2 */
#pragma interrupt FWUP_ISR_VECT(0E) /* INTP3 */
#pragma interrupt FWUP_ISR_VECT(10) /* INTP4 */
#pragma interrupt FWUP_ISR_VECT(12) /* INTP5 */
#pragma interrupt FWUP_ISR_VECT(14) /* INTCSI20/INTIIC20/INTST2 */
#pragma interrupt FWUP_ISR_VECT(16) /* INTCSI21/INTIIC21/INTSR2 */
#pragma interrupt FWUP_ISR_VECT(18) /* INTSRE2/INTTM11H */
#pragma interrupt FWUP_ISR_VECT(1A) /* INTELCL */
#pragma interrupt FWUP_ISR_VECT(1C) /* INTSMSE */
#pragma interrupt FWUP_ISR_VECT(1E) /* INTCSI00/INTIIC00/INTST0 */
#pragma interrupt FWUP_ISR_VECT(20) /* INTTM00 */
#pragma interrupt FWUP_ISR_VECT(22) /* INTSRE0/INTTM01H */
#pragma interrupt FWUP_ISR_VECT(24) /* INTCSI10/INTIIC10/INTST1 */
#pragma interrupt FWUP_ISR_VECT(26) /* INTCSI11/INTIIC11/INTSR1 */
#pragma interrupt FWUP_ISR_VECT(28) /* INTSRE1/INTTM03H */
#pragma interrupt FWUP_ISR_VECT(2A) /* INTIICA0 */
#pragma interrupt FWUP_ISR_VECT(2C) /* INTCSI01/INTIIC01/INTSR0 */
#pragma interrupt FWUP_ISR_VECT(2E) /* INTTM01 */
#pragma interrupt FWUP_ISR_VECT(30) /* INTTM02 */
#pragma interrupt FWUP_ISR_VECT(32) /* INTTM03 */
#pragma interrupt FWUP_ISR_VECT(34) /* INTAD */
#pragma interrupt FWUP_ISR_VECT(36) /* INTRTC */
#pragma interrupt FWUP_ISR_VECT(38) /* INTITL */
#pragma interrupt FWUP_ISR_VECT(3A) /* INTKR */
#pragma interrupt FWUP_ISR_VECT(3C) /* INTCSI30/INTIIC30/INTST3 */
#pragma interrupt FWUP_ISR_VECT(3E) /* INTCSI31/INTIIC31/INTSR3 */
#pragma interrupt FWUP_ISR_VECT(40) /* INTTM13 */
#pragma interrupt FWUP_ISR_VECT(42) /* INTTM04 */
#pragma interrupt FWUP_ISR_VECT(44) /* INTTM05 */
#pragma interrupt FWUP_ISR_VECT(46) /* INTTM06 */
#pragma interrupt FWUP_ISR_VECT(48) /* INTTM07 */
#pragma interrupt FWUP_ISR_VECT(4A) /* INTP6 */
#pragma interrupt FWUP_ISR_VECT(4C) /* INTP7 */
#pragma interrupt FWUP_ISR_VECT(4E) /* INTP8 */
#pragma interrupt FWUP_ISR_VECT(50) /* INTP9 */
#pragma interrupt FWUP_ISR_VECT(52) /* INTFL */
#pragma interrupt FWUP_ISR_VECT(54) /* INTCMP0/INTP10 */
#pragma interrupt FWUP_ISR_VECT(56) /* INTCMP1/INTP11 */
#pragma interrupt FWUP_ISR_VECT(58) /* INTTM10/INTURE0 */
#pragma interrupt FWUP_ISR_VECT(5A) /* INTTM11/INTURE1 */
#pragma interrupt FWUP_ISR_VECT(5C) /* INTTM12 */
#pragma interrupt FWUP_ISR_VECT(5E) /* INTSRE3/INTTM13H */
#pragma interrupt FWUP_ISR_VECT(60) /* INTCTSUWR */
#pragma interrupt FWUP_ISR_VECT(62) /* INTIICA1 */
#pragma interrupt FWUP_ISR_VECT(64) /* INTCTSURD */
#pragma interrupt FWUP_ISR_VECT(66) /* INTCTSUFN */
#pragma interrupt FWUP_ISR_VECT(68) /* INTREMC */
#pragma interrupt FWUP_ISR_VECT(6A) /* INTUT0 */
#pragma interrupt FWUP_ISR_VECT(6C) /* INTUR0 */
#pragma interrupt FWUP_ISR_VECT(6E) /* INTUT1 */
#pragma interrupt FWUP_ISR_VECT(70) /* INTUR1 */
#pragma interrupt FWUP_ISR_VECT(72) /* INTTM14 */
#pragma interrupt FWUP_ISR_VECT(74) /* INTTM15 */
#pragma interrupt FWUP_ISR_VECT(76) /* INTTM16 */
#pragma interrupt FWUP_ISR_VECT(78) /* INTTM17 */
#pragma interrupt_brk FWUP_ISR_FUNC_NAME(7E)
#endif /* End of defined__CCRL__ */

#if (__CCRL__)
#pragma section data BL_DATA
uint16_t g_using_uservector = 0;
#pragma section
#elif (__ICCRL78__)
#pragma location =  "BL_DATA"
uint16_t g_using_uservector = 0;
#endif /* (__ICCRL78__) */
/**********************************************************************************************************************
 * function name: dummy_func
 *********************************************************************************************************************/
BL_NEAR_FUNC static void dummy_func(void)
{
    ;
}
/**********************************************************************************************************************
 * End of function dummy_func
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * function name : jump_branch_interrupts
 *********************************************************************************************************************/
BL_NEAR_FUNC static void jump_branch_interrupts(uint32_t vectaddr)
{
    uint32_t addr;
    addr = (uint32_t)BRANCH_TABLE_START + (vectaddr * 2);
    ((void (*)(void))addr)();
}
/**********************************************************************************************************************
 * End of function jump_branch_interrupts
 *********************************************************************************************************************/

/*
 * vectors
 */
#if defined(__ICCRL78__)
#pragma vector = 0x04
#endif
FWUP_ISR_FUNC(04);

#if defined(__ICCRL78__)
#pragma vector = 0x06
#endif
FWUP_ISR_FUNC(06);

#if defined(__ICCRL78__)
#pragma vector = 0x08
#endif
FWUP_ISR_FUNC(08);

#if defined(__ICCRL78__)
#pragma vector = 0x0A
#endif
FWUP_ISR_FUNC(0A);

#if defined(__ICCRL78__)
#pragma vector = 0x0C
#endif
FWUP_ISR_FUNC(0C);

#if defined(__ICCRL78__)
#pragma vector = 0x0E
#endif
FWUP_ISR_FUNC(0E);

#if defined(__ICCRL78__)
#pragma vector = 0x10
#endif
FWUP_ISR_FUNC(10);

#if defined(__ICCRL78__)
#pragma vector = 0x12
#endif
FWUP_ISR_FUNC(12);

#if defined(__ICCRL78__)
#pragma vector = 0x14
#endif
FWUP_ISR_FUNC(14)

#if defined(__ICCRL78__)
#pragma vector = 0x16
#endif
FWUP_ISR_FUNC(16)

#if defined(__ICCRL78__)
#pragma vector = 0x18
#endif
FWUP_ISR_FUNC(18);

#if defined(__ICCRL78__)
#pragma vector = 0x1A
#endif
FWUP_ISR_FUNC(1A);

#if defined(__ICCRL78__)
#pragma vector = 0x1C
#endif
FWUP_ISR_FUNC(1C);

#if defined(__ICCRL78__)
#pragma vector = 0x1E
#endif
FWUP_ISR_FUNC(1E);

#if defined(__ICCRL78__)
#pragma vector = 0x20
#endif
FWUP_ISR_FUNC(20);

#if defined(__ICCRL78__)
#pragma vector = 0x22
#endif
FWUP_ISR_FUNC(22);

#if defined(__ICCRL78__)
#pragma vector = 0x24
#endif
FWUP_ISR_FUNC(24);

#if defined(__ICCRL78__)
#pragma vector = 0x26
#endif
FWUP_ISR_FUNC(26);

#if defined(__ICCRL78__)
#pragma vector = 0x28
#endif
FWUP_ISR_FUNC(28);

#if defined(__ICCRL78__)
#pragma vector = 0x2A
#endif
FWUP_ISR_FUNC(2A);

#if defined(__ICCRL78__)
#pragma vector = 0x2C
#endif
FWUP_ISR_FUNC(2C);

#if defined(__ICCRL78__)
#pragma vector = 0x2E
#endif
FWUP_ISR_FUNC(2E);

#if defined(__ICCRL78__)
#pragma vector = 0x30
#endif
FWUP_ISR_FUNC(30);

#if defined(__ICCRL78__)
#pragma vector = 0x32
#endif
FWUP_ISR_FUNC(32);

#if defined(__ICCRL78__)
#pragma vector = 0x34
#endif
FWUP_ISR_FUNC(34);

#if defined(__ICCRL78__)
#pragma vector = 0x36
#endif
FWUP_ISR_FUNC(36);

#if defined(__ICCRL78__)
#pragma vector = 0x38
#endif
FWUP_ISR_FUNC(38);

#if defined(__ICCRL78__)
#pragma vector = 0x3A
#endif
FWUP_ISR_FUNC(3A);

#if defined(__ICCRL78__)
#pragma vector = 0x3C
#endif
FWUP_ISR_FUNC(3C);

#if defined(__ICCRL78__)
#pragma vector = 0x3E
#endif
FWUP_ISR_FUNC(3E);

#if defined(__ICCRL78__)
#pragma vector = 0x40
#endif
FWUP_ISR_FUNC(40);

#if defined(__ICCRL78__)
#pragma vector = 0x42
#endif
FWUP_ISR_FUNC(42);

#if defined(__ICCRL78__)
#pragma vector = 0x44
#endif
FWUP_ISR_FUNC(44);

#if defined(__ICCRL78__)
#pragma vector = 0x46
#endif
FWUP_ISR_FUNC(46);

#if defined(__ICCRL78__)
#pragma vector = 0x48
#endif
FWUP_ISR_FUNC(48);

#if defined(__ICCRL78__)
#pragma vector = 0x4A
#endif
FWUP_ISR_FUNC(4A);

#if defined(__ICCRL78__)
#pragma vector = 0x4C
#endif
FWUP_ISR_FUNC(4C);

#if defined(__ICCRL78__)
#pragma vector = 0x4E
#endif
FWUP_ISR_FUNC(4E);

#if defined(__ICCRL78__)
#pragma vector = 0x50
#endif
FWUP_ISR_FUNC(50);

#if defined(__ICCRL78__)
#pragma vector = 0x52
#endif
FWUP_ISR_FUNC(52);

#if defined(__ICCRL78__)
#pragma vector = 0x54
#endif
FWUP_ISR_FUNC(54);

#if defined(__ICCRL78__)
#pragma vector = 0x56
#endif
FWUP_ISR_FUNC(56);

#if defined(__ICCRL78__)
#pragma vector = 0x58
#endif
FWUP_ISR_FUNC(58);

#if defined(__ICCRL78__)
#pragma vector = 0x5A
#endif
FWUP_ISR_FUNC(5A);

#if defined(__ICCRL78__)
#pragma vector = 0x5C
#endif
FWUP_ISR_FUNC(5C);

#if defined(__ICCRL78__)
#pragma vector = 0x5E
#endif
FWUP_ISR_FUNC(5E);

#if defined(__ICCRL78__)
#pragma vector = 0x60
#endif
FWUP_ISR_FUNC(60);

#if defined(__ICCRL78__)
#pragma vector = 0x62
#endif
FWUP_ISR_FUNC(62);

#if defined(__ICCRL78__)
#pragma vector = 0x64
#endif
FWUP_ISR_FUNC(64);

#if defined(__ICCRL78__)
#pragma vector = 0x66
#endif
FWUP_ISR_FUNC(66);

#if defined(__ICCRL78__)
#pragma vector = 0x68
#endif
FWUP_ISR_FUNC(68);

#if defined(__ICCRL78__)
#pragma vector = 0x6A
#endif
FWUP_ISR_FUNC(6A);

#if defined(__ICCRL78__)
#pragma vector = 0x6C
#endif
FWUP_ISR_FUNC(6C);

#if defined(__ICCRL78__)
#pragma vector = 0x6E
#endif
FWUP_ISR_FUNC(6E);

#if defined(__ICCRL78__)
#pragma vector = 0x70
#endif
FWUP_ISR_FUNC(70);

#if defined(__ICCRL78__)
#pragma vector = 0x72
#endif
FWUP_ISR_FUNC(72);

#if defined(__ICCRL78__)
#pragma vector = 0x74
#endif
FWUP_ISR_FUNC(74);

#if defined(__ICCRL78__)
#pragma vector = 0x76
#endif
FWUP_ISR_FUNC(76);

#if defined(__ICCRL78__)
#pragma vector = 0x78
#endif
FWUP_ISR_FUNC(78);

#if defined(__ICCRL78__)
#pragma vector = 0x7E
#endif
FWUP_ISR_FUNC(7E);
