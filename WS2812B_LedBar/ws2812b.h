/*
 * ws2812b.h
 *
 * Created: 2020-11-08 18:52:48
 *  Author: Wojciech
 */ 


#ifndef WS2812B_H_
#define WS2812B_H_

#include <avr/io.h>
#include <util/delay.h>

/* Configuration */
#define WS2812B_DDR	 DDRD
#define WS2812B_PORT PORTD
#define WS2812B_PIN	 PD3
#define WS2812B_LEDS_COUNT 60
#define WS2812B_BITS_PER_LED 24
#define WS2812B_BIT_SIZE (WS2812B_LEDS_COUNT * WS2812B_BITS_PER_LED)
#define WS2812B_RESET_DELAY 60

/* Macros */
#define SetWs2812bPinHigh() WS2812B_PORT |= (1 << WS2812B_PIN)
#define SetWs2812bPinLow()  WS2812B_PORT &= ~(1 << WS2812B_PIN)

/* Function prototypes */
void LedBarInit(void);
void LedBarClear(void);
void LedBarSendColor(uint8_t red, uint8_t green, uint8_t blue);
void LedBarFillWithColor(uint8_t red, uint8_t green, uint8_t blue);

#endif /* WS2812B_H_ */