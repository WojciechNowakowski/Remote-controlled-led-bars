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
 * rc5_ir_remote.c - C source file of library for RC5 IR Remote
 *
 * Created: 2020-12-01 23:51:46
 *  Author: Wojciech Nowakowski
 */ 

/*============================================================================*\
  Preprocessor #include directives
\*============================================================================*/
#include "rc5_ir_remote.h"

/*============================================================================*\
  Local preprocessor constant values
\*============================================================================*/
/*============================================================================*\
  Local type declarations (enum, struct, union, typedef)
\*============================================================================*/
/*============================================================================*\
  Exported object definitions
\*============================================================================*/
/*============================================================================*\
  Local object definitions
\*============================================================================*/
static volatile uint8_t ir_bod_table[66];
static volatile uint8_t ir_bod_counter = 0;

/*============================================================================*\
  Local function prototypes
\*============================================================================*/
/*============================================================================*\
  Local inline function definitions and function-like macros
\*============================================================================*/
/*============================================================================*\
  Function definitions
\*============================================================================*/
void remote_init_pilot(void)
{
	/* Pin as Input, no pull-up */
	DDRD  &= ~(1 << PD2);
	PORTD &= ~(1 << PD2);
	
	/* Signal change on input INT0 generates interrupt */
	MCUCR = (1 << ISC00);
	GIMSK |= (1 << INT0);
	
	/* Timer/Counter0 Overflow Interrupt Enable */
	TIMSK |= (1 << TOIE0);
	
	/* Start timer with 256 prescaler - 32 us for 1 tick */
	TCCR0B = (1 << CS02);
}

void Rc5Ir_StoreBodTime(uint8_t time)
{
	ir_bod_counter++;
	ir_bod_table[ir_bod_counter] = time;
}

uint8_t Rc5Ir_CheckSignalValidity(void)
{
	uint8_t signal_valid = 0;
	
	/* TODO: Check sum of the registered time */
	
	if(ir_bod_counter > 13)
	{
		ir_bod_counter = 0;
		signal_valid = 1;
	}
	
	return signal_valid;
}

uint8_t IrRc5ExtractSignalCode(void)
{
	uint8_t tab2[27];
	uint8_t return_code;
	uint8_t i = 0, bod_count = 0;
	uint8_t signal_level = 1;

	while (i < 27)
	{
		if((ir_bod_table[signal_level] > 23) && (ir_bod_table[signal_level] < 36))		// 23 = 736 us, 36 = 1.152 ms
		{
			tab2[i] = signal_level & 1;
			i++;
		}
		else if((ir_bod_table[signal_level] > 51) && (ir_bod_table[signal_level] < 64)) // 51 = 1.632ms 64 = 2.048ms
		{
			tab2[i] = signal_level & 1;
			i++;
			tab2[i] = signal_level & 1;
			i++;
		}
		else
		{
			tab2[i] = 0;
			i++;
		}

		signal_level++;
	}

	return_code = (tab2[2] << 6);
	return_code |= (tab2[16] << 5);
	return_code |= (tab2[18] << 4);
	return_code |= (tab2[20] << 3);
	return_code |= (tab2[22] << 2);
	return_code |= (tab2[24] << 1);
	return_code |= (tab2[26] << 0);

	return return_code;
}
