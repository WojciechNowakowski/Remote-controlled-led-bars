/*
 * ws2812b.c
 *
 * Created: 2020-11-08 18:50:11
 *  Author: Wojciech
 */ 

#include "ws2812b.h"

void LedBarInit(void)
{
	WS2812B_DDR |= (1 << WS2812B_PIN);
	WS2812B_PORT &= ~(1 << WS2812B_PIN);
}

static void LedBarSendLogicZero(void)
{
	SetWs2812bPinHigh();
	asm volatile ("nop"::);
	SetWs2812bPinLow();
	/* commented because only Pin high time matters
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::); 
	*/
}

static void LedBarSendLogicOne(void)
{
	SetWs2812bPinHigh();
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	SetWs2812bPinLow();
	/* commented because only Pin high time matters
	asm volatile ("nop"::); 
	*/
}

void LedBarClear(void)
{
	for (uint16_t i = 0; i < WS2812B_BIT_SIZE; ++i)
	{
		LedBarSendLogicZero();
	}

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

	SetWs2812bPinLow();
}

void LedBarFillWithColor(uint8_t red, uint8_t green, uint8_t blue)
{
	for (uint16_t i = 0; i < WS2812B_LEDS_COUNT; ++i)
	{
		LedBarSendColor(red, green, blue);
	}
}