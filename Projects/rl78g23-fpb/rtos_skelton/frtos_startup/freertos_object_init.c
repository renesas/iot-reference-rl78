/*
 * freertos_object_init.c
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

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "FreeRTOS.h"
#include "freertos_start.h"
/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
void Kernel_Object_init (void);
void Object_init_manual (void);
/***********************************************************************************************************************
 * Function Name: Kernel_Object_init
 * Description  : This function initializes FreeRTOS objects.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void Kernel_Object_init (void)
{
    /************** task creation ****************************/

    /************** semaphore creation ***********************/

    /************** queue creation ***************************/

    /************** software time creation **************************/

    /************** event groups creation ********************/

    /************** stream buffer creation *************************/

    /************** message buffer creation *********************/

} /* End of function Kernel_Object_init()*/

/***********************************************************************************************************************
 * Function Name : Object_init_manual
 * Description   : This function re-initializes FreeRTOS objects and should be called at runtime.
 * Arguments     : None.
 * Return value  : None.
 **********************************************************************************************************************/
void Object_init_manual (void)
{
    /************** task creation ****************************/
} /* End of function Object_init_manual()*/
