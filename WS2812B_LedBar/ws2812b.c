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
 * ws2812b.c - C source file of a WS2812B led driver
 *
 * Created: 2020-11-08 18:50:11
 *  Author: Wojciech Nowakowski
 */ 
/*============================================================================*\
  Preprocessor #include directives
\*============================================================================*/
#include "ws2812b.h"

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
/*============================================================================*\
  Local function prototypes
\*============================================================================*/
/*============================================================================*\
  Local inline function definitions and function-like macros
\*============================================================================*/
/*============================================================================*\
  Function definitions
\*============================================================================*/
void LedBarInit(void)
{
	/* Set pin as output and drive it Low */
	WS2812B_DDR |= (1 << WS2812B_PIN);
	WS2812B_PORT &= ~(1 << WS2812B_PIN);
}

static void LedBarSendLogicZero(void)
{
	/* Set pin high for ca 350 us */
	SetWs2812bPinHigh();
	asm volatile ("nop"::);

	/* Set pin low for ca 900 us */
	SetWs2812bPinLow();
	
	/* No 'nops' here because other instructions takes 
	 * enough time and only Pin high time really matters 
	 */
}

static void LedBarSendLogicOne(void)
{
	/* Set pin high for ca 900 us */
	SetWs2812bPinHigh();
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);

	/* Set pin low for ca 350 us */
	SetWs2812bPinLow();
	
	/* No 'nops' here because other instructions takes 
	 * enough time and only Pin high time really matters 
	 */
}

void LedBarClear(void)
{
	for (uint16_t i = 0; i < WS2812B_BIT_SIZE; ++i)
	{
		LedBarSendLogicZero();
	}

	/* Reset the signal line, so next operations would
	 * affect the leds in bar from the beginning, not further */
	SetWs2812bPinLow();
	_delay_us(WS2812B_RESET_DELAY);
}

void LedBarSendColor(uint8_t red, uint8_t green, uint8_t blue)
{
	(green & 0x80) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x40) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x20) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x10) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x08) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x04) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x02) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(green & 0x01) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x80) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x40) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x20) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x10) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x08) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x04) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x02) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(red & 0x01) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x80) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x40) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x20) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x10) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x08) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x04) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x02) ? LedBarSendLogicOne() : LedBarSendLogicZero();
	(blue & 0x01) ? LedBarSendLogicOne() : LedBarSendLogicZero();
}

void LedBarFillWithColor(uint8_t red, uint8_t green, uint8_t blue)
{
	for (uint16_t i = 0; i < WS2812B_LEDS_COUNT; ++i)
	{
		LedBarSendColor(red, green, blue);
	}

	/* Release the leds */
	SetWs2812bPinLow();
	_delay_us(WS2812B_RESET_DELAY);
}
