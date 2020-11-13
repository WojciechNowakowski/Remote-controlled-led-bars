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

volatile unsigned char pilot_flaga, n, tab[66], czas;
volatile unsigned char stop2 = 1;
volatile unsigned char code;

ISR(INT0_vect)	// pilot RC5
{
	czas = TCNT0;
	TCNT0 = 0x00;
	
	if(stop2)
	{
		stop2 = 0;
	}else
	{
		n++;
		tab[n] = czas;
	}
	
	MCUCR ^= (1 << ISC00);
}

ISR(TIMER0_OVF_vect)
{
	stop2 = 1;

	if(n>2)
	{
		n = 0;
		code = 0;
		pilot_flaga = 1;
	}
}

void remote_init_pilot(void)
{
	DDRD  &= ~(1 << PD2);
	PORTD &= ~(1 << PD2);
	MCUCR = (1 << ISC01) | (1 << ISC00); // narastaj¹ce zbocze
	GIMSK |= (1 << INT0);
	TCNT0 = 0;
	TCCR0B = (1 << CS02); // prescaler 256 - 32us na zliczenie
	TIMSK = (1 << TOIE0);
}

int main(void)
{
	uint8_t tab2[27];
	uint8_t led_luminance = 0x80;
	uint8_t led_bar_color[3] = {0x10, 0x10, 0x10};
	
	LedBarInit();
	LedBarClear();
	remote_init_pilot();
	sei();

	for(;;)
	{
		if(pilot_flaga)
		{
			unsigned char i = 0, j = 1;
			pilot_flaga = 0;
			
			while (i < 27)
			{
				if((tab[j] > 23) && (tab[j] < 36))		// 23 = 736 us, 36 = 1.152 ms
				{
					tab2[i] = j & 1;
					i++;
				}
				else if((tab[j] > 51) && (tab[j] < 64)) // 51 = 1.632ms 64 = 2.048ms
				{
					tab2[i] = j & 1;
					i++;
					tab2[i] = j & 1;
					i++;
				}
				
				j++;
			}
			
			code = (tab2[2] << 6);
			code |= (tab2[16] << 5);
			code |= (tab2[18] << 4);
			code |= (tab2[20] << 3);
			code |= (tab2[22] << 2);
			code |= (tab2[24] << 1);
			code |= (tab2[26] << 0);

			if (code == 0x50)
			{
				led_luminance += 0x10;
			}
			if (code == 0x51)
			{
				led_luminance -= 0x10;
			}
			if (code == 0x2B)
			{
				led_bar_color[0] = led_luminance;
				led_bar_color[1] = 0x00;
				led_bar_color[2] = 0x00;
			}
			if (code == 0x2C)
			{
				led_bar_color[0] = 0x00;
				led_bar_color[1] = led_luminance;
				led_bar_color[2] = 0x00;
			}
			if (code == 0x2D)
			{
				led_bar_color[0] = led_luminance;
				led_bar_color[1] = led_luminance;
				led_bar_color[2] = 0x00;
			}
			if (code == 0x2E)
			{
				led_bar_color[0] = 0x00;
				led_bar_color[1] = 0x00;
				led_bar_color[2] = led_luminance;
			}
			if (code == 0x7C)
			{
				led_bar_color[0] = 0x00;
				led_bar_color[1] = led_luminance;
				led_bar_color[2] = led_luminance;
			}
			if (code == 0x6B)
			{
				led_bar_color[0] = led_luminance;
				led_bar_color[1] = 0x00;
				led_bar_color[2] = led_luminance;
			}
			if (code == 0x69)
			{
				led_bar_color[0] = led_luminance;
				led_bar_color[1] = led_luminance;
				led_bar_color[2] = led_luminance;
			}
			if (code == 0x6C)
			{
				led_bar_color[0] = 0x00;
				led_bar_color[1] = 0x00;
				led_bar_color[2] = 0x00;
			}

			LedBarFillWithColor(led_bar_color[0], led_bar_color[1], led_bar_color[2]);
		}
	}
}