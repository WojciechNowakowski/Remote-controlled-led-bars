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
 * ws2812b.h - C header file of a WS2812B led driver
 *
 * Created: 2020-11-08 18:52:48
 *  Author: Wojciech Nowakowski
 */ 


#ifndef WS2812B_H_
#define WS2812B_H_

/*============================================================================*\
  Preprocessor #include directives for header files
\*============================================================================*/
#include <avr/io.h>
#include <util/delay.h>

/*============================================================================*\
  Exported constant values
\*============================================================================*/
/* Output configuration */
#define WS2812B_DDR	 DDRD
#define WS2812B_PORT PORTD
#define WS2812B_PIN	 PD3

/* Led bar constants */
#define WS2812B_LEDS_COUNT 10
#define WS2812B_BITS_PER_LED 24
#define WS2812B_BIT_SIZE (WS2812B_LEDS_COUNT * WS2812B_BITS_PER_LED)
#define WS2812B_RESET_DELAY 200 // in microseconds

/*============================================================================*\
  Exported type declarations (enum, struct, union, typedef)
\*============================================================================*/
/*============================================================================*\
  Exported object declarations
\*============================================================================*/
/*============================================================================*\
  Exported inline function definitions and function-like macros
\*============================================================================*/
#define SetWs2812bPinHigh() WS2812B_PORT |= (1 << WS2812B_PIN)
#define SetWs2812bPinLow()  WS2812B_PORT &= ~(1 << WS2812B_PIN)

/*============================================================================*\
  Exported function prototypes
\*============================================================================*/
void LedBarInit(void);
void LedBarClear(void);
void LedBarSendColor(uint8_t red, uint8_t green, uint8_t blue);
void LedBarFillWithColor(uint8_t red, uint8_t green, uint8_t blue);

#endif /* WS2812B_H_ */
