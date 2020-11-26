/*
 * main.c
 *
 * Created: 2014-05-21 14:36:46
 *  Author: Wojciech
 */ 


#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include "ws2812b.h"

volatile uint8_t ir_flag = 0;
volatile uint8_t ir_bod_counter = 0;
volatile uint8_t ir_bod_table[66] = {0};
volatile uint8_t czas = 0;
volatile uint8_t timer_overflow = 1;

ISR(INT0_vect)	// pilot RC5
{
	czas = TCNT0;
	TCNT0 = 0x00;

	if(timer_overflow)
	{
		timer_overflow = 0;
	}else
	{
		ir_bod_counter++;
		ir_bod_table[ir_bod_counter] = czas;
	}

	MCUCR ^= (1 << ISC00);
}

ISR(TIMER0_OVF_vect)
{
	timer_overflow = 1;

	if(ir_bod_counter > 13)
	{
		ir_bod_counter = 0;
		ir_flag = 1;
	}
}

void remote_init_pilot(void)
{
	/* Pin as Input, no pull-up */
	DDRD  &= ~(1 << PD2);
	PORTD &= ~(1 << PD2);
	MCUCR = (1 << ISC01) | (1 << ISC00); // narastaj¹ce zbocze
	GIMSK |= (1 << INT0);
	
	/* Timer/Counter0 Overflow Interrupt Enable */
	TIMSK |= (1 << TOIE0);
	
	/* Start timer with 256 prescaler - 32 us for 1 tick */
	TCCR0B = (1 << CS02);
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

void LedBarBindColorsFromCodeAndLuminance(uint8_t led_color_code, uint8_t led_luminance, uint8_t led_bar_color[3])
{
	if (led_color_code == 0x00)
	{
		led_bar_color[0] = led_luminance;
		led_bar_color[1] = 0x40;
		led_bar_color[2] = 0x00;
	}
	if (led_color_code == 0x01)
	{
		led_bar_color[0] = led_luminance;
		led_bar_color[1] = 0x00;
		led_bar_color[2] = 0x00;
	}
	if (led_color_code == 0x02)
	{
		led_bar_color[0] = 0x00;
		led_bar_color[1] = led_luminance;
		led_bar_color[2] = 0x00;
	}
	if (led_color_code == 0x03)
	{
		led_bar_color[0] = led_luminance;
		led_bar_color[1] = led_luminance;
		led_bar_color[2] = 0x00;
	}
	if (led_color_code == 0x04)
	{
		led_bar_color[0] = 0x00;
		led_bar_color[1] = 0x00;
		led_bar_color[2] = led_luminance;
	}
	if (led_color_code == 0x05)
	{
		led_bar_color[0] = 0x00;
		led_bar_color[1] = led_luminance;
		led_bar_color[2] = led_luminance;
	}
	if (led_color_code == 0x06)
	{
		led_bar_color[0] = led_luminance;
		led_bar_color[1] = 0x00;
		led_bar_color[2] = led_luminance;
	}
	if (led_color_code == 0x07)
	{
		led_bar_color[0] = led_luminance;
		led_bar_color[1] = led_luminance;
		led_bar_color[2] = led_luminance;
	}
	if (led_color_code == 0x08)
	{
		led_bar_color[0] = 0x00;
		led_bar_color[1] = 0x00;
		led_bar_color[2] = 0x00;
	}
}

int main(void)
{
	uint8_t led_bar_color[3] = {0x80, 0x40, 0x00};
	uint8_t led_luminance = 0x80;
	uint8_t led_color_code = 0;
	uint8_t remote_code = 0;
	
	LedBarInit();
	LedBarClear();
	
	/* Show Polish flag */
	LedBarSendColor(0xAA, 0xAA, 0xAA);
	LedBarSendColor(0xAA, 0xAA, 0xAA);
	LedBarSendColor(0xAA, 0xAA, 0xAA);
	LedBarSendColor(0xAA, 0xAA, 0xAA);
	LedBarSendColor(0xAA, 0xAA, 0xAA);
	LedBarSendColor(0xAA, 0x00, 0x00);
	LedBarSendColor(0xAA, 0x00, 0x00);
	LedBarSendColor(0xAA, 0x00, 0x00);
	LedBarSendColor(0xAA, 0x00, 0x00);
	LedBarSendColor(0xAA, 0x00, 0x00);
	remote_init_pilot();

	/* Unlock interrupts */
	sei();
	
	for (;;)
	{
		_delay_us(WS2812B_RESET_DELAY);
		
		if (ir_flag)
		{
			ir_flag = 0;
			remote_code = IrRc5ExtractSignalCode();
			
			switch (remote_code)
			{
				case 0x50:	led_luminance += 0x10;	break;
				case 0x51:	led_luminance -= 0x10;	break;
				case 0x2B:	led_color_code = 0x01;	break;
				case 0x2C:	led_color_code = 0x02;	break;
				case 0x2D:	led_color_code = 0x03;	break;
				case 0x2E:	led_color_code = 0x04;	break;
				case 0x7C:	led_color_code = 0x05;	break;
				case 0x6B:	led_color_code = 0x06;	break;
				case 0x69:	led_color_code = 0x07;	break;
				case 0x6C:	led_color_code = 0x08;	break;
				default:
					(remote_code & 0x40) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x20) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x10) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x08) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x04) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x02) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					(remote_code & 0x01) ? LedBarSendColor(0xFF, 0xFF, 0xFF) : LedBarSendColor(0, 0, 0);
					break;
			}
			
			LedBarBindColorsFromCodeAndLuminance(led_color_code, led_luminance, led_bar_color);
			LedBarFillWithColor(led_bar_color[0], led_bar_color[1], led_bar_color[2]);
		}
	}
}