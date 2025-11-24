/*
 * This file is a partially changed from the Newlib.
 * The Newlib is governed by a collection of licenses, which are BSD, or BSD-alike.
 * All these licenses are listed in at https://sourceware.org/newlib/COPYING.NEWLIB
 * According to this licensing model’s statement
 * ("Permission to use, copy, modify, and distribute this software for any purpose without fee is hereby granted ..."),
 * the library may also be used in proprietary software without any licensing issues.
 */

/*
 * string_rl78.h
 *
 * Definitions for reentrant string tokenization (strtok_r).
 */

/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
#ifndef POSIX_RL78_H
#define POSIX_RL78_H

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Public Functions
**********************************************************************************************************************/
/**********************************************************************************************************************
 * function name: strtok_r
 *********************************************************************************************************************/
char __far * strtok_r (register char __far * restrict s,
                        register const char __far * restrict delim,
                        char __far ** restrict lasts);

#endif /* POSIX_RL78_H */

