/*
 * Copyright (c) 2020 Wojciech Nowakowski Tronix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * rc5_ir_remote.h - C header file of library for RC5 IR Remote
 *
 * Created: 2020-12-01 23:53:26
 *  Author: Wojciech Nowakowski
 */ 


#ifndef RC5_IR_REMOTE_H_
#define RC5_IR_REMOTE_H_

/*============================================================================*\
  Preprocessor #include directives for header files
\*============================================================================*/
#include <avr/io.h>

/*============================================================================*\
  Exported constant values
\*============================================================================*/

/*============================================================================*\
  Exported type declarations (enum, struct, union, typedef)
\*============================================================================*/

/*============================================================================*\
  Exported object declarations
\*============================================================================*/

/*============================================================================*\
  Exported inline function definitions and function-like macros
\*============================================================================*/

/*============================================================================*\
  Exported function prototypes
\*============================================================================*/
void remote_init_pilot(void);
void Rc5Ir_StoreBodTime(uint8_t time);
uint8_t Rc5Ir_CheckSignalValidity(void);
uint8_t IrRc5ExtractSignalCode(void);

#endif /* RC5_IR_REMOTE_H_ */