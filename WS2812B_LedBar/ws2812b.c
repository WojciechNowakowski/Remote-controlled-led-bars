/*
 * ws2812b.c
 *
 * Created: 2020-11-08 18:50:11
 *  Author: Wojciech
 */ 

#include "ws2812b.h"

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

	/* Release the leds */
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