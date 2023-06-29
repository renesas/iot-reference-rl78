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
 * File Name    : branch_table.h
 * Version      : 0.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description

 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BRANCHTABLE_H_
#define BRANCHTABLE_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
#define BRANCH_TABLE_START    (0x1300)

#define BL_ISR_04()         ( dummy_func() )
#define BL_ISR_06()         ( dummy_func() )
#define BL_ISR_08()         ( dummy_func() )
#define BL_ISR_0A()         ( dummy_func() )
#define BL_ISR_0C()         ( dummy_func() )
#define BL_ISR_0E()         ( dummy_func() )
#define BL_ISR_10()         ( dummy_func() )
#define BL_ISR_12()         ( dummy_func() )
#define BL_ISR_14()         ( r_isr_txi() ) /* sci2_txi2_isr() */
#define BL_ISR_16()         ( r_isr_rxi() ) /* sci2_rxi2_isr() */
#define BL_ISR_18()         ( r_isr_eri() ) /* sci2_eri2_isr() */
#define BL_ISR_1A()         ( dummy_func() )
#define BL_ISR_1C()         ( dummy_func() )
#define BL_ISR_1E()         ( dummy_func() ) /* sci0_txi0_isr() */
#define BL_ISR_20()         ( dummy_func() )
#define BL_ISR_22()         ( dummy_func() ) /* sci0_eri0_isr() */
#define BL_ISR_24()         ( dummy_func() ) /* sci1_txi1_isr() */
#define BL_ISR_26()         ( dummy_func() ) /* sci1_rxi1_isr() */
#define BL_ISR_28()         ( dummy_func() ) /* sci1_eri1_isr() */
#define BL_ISR_2A()         ( dummy_func() )
#define BL_ISR_2C()         ( dummy_func() ) /* sci0_rxi0_isr() */
#define BL_ISR_2E()         ( dummy_func() )
#define BL_ISR_30()         ( dummy_func() )
#define BL_ISR_32()         ( dummy_func() )
#define BL_ISR_34()         ( dummy_func() )
#define BL_ISR_36()         ( dummy_func() )
#define BL_ISR_38()         ( dummy_func() )
#define BL_ISR_3A()         ( dummy_func() )
#define BL_ISR_3C()         ( dummy_func() ) /* sci3_txi3_isr() */
#define BL_ISR_3E()         ( dummy_func() ) /* sci3_rxi3_isr() */
#define BL_ISR_40()         ( dummy_func() )
#define BL_ISR_42()         ( dummy_func() )
#define BL_ISR_44()         ( dummy_func() )
#define BL_ISR_46()         ( dummy_func() )
#define BL_ISR_48()         ( dummy_func() )
#define BL_ISR_4A()         ( dummy_func() )
#define BL_ISR_4C()         ( dummy_func() )
#define BL_ISR_4E()         ( dummy_func() )
#define BL_ISR_50()         ( dummy_func() )
#define BL_ISR_52()         ( dummy_func() )
#define BL_ISR_54()         ( dummy_func() )
#define BL_ISR_56()         ( dummy_func() )
#define BL_ISR_58()         ( dummy_func() )
#define BL_ISR_5A()         ( dummy_func() )
#define BL_ISR_5C()         ( dummy_func() )
#define BL_ISR_5E()         ( dummy_func() ) /* sci3_eri3_isr() */
#define BL_ISR_60()         ( dummy_func() )
#define BL_ISR_62()         ( dummy_func() )
#define BL_ISR_64()         ( dummy_func() )
#define BL_ISR_66()         ( dummy_func() )
#define BL_ISR_68()         ( dummy_func() )
#define BL_ISR_6A()         ( dummy_func() )
#define BL_ISR_6C()         ( dummy_func() )
#define BL_ISR_6E()         ( dummy_func() )
#define BL_ISR_70()         ( dummy_func() )
#define BL_ISR_72()         ( dummy_func() )
#define BL_ISR_74()         ( dummy_func() )
#define BL_ISR_76()         ( dummy_func() )
#define BL_ISR_78()         ( dummy_func() )
#define BL_ISR_7E()         ( dummy_func() )

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void r_isr_txi (void);
extern void r_isr_rxi (void);
extern void r_isr_eri (void);

#endif /* BRANCHTABLE_H_ */
