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

//{definicje
#define PCIE0	5
#define PCIE1	3
#define PCIE2	4
#define start	200
#define tryb_pilot_odtwarzacza	1
#define tryb_wizualizacja				2
#define tryb_pilot_do_PC				3
#define	PD6_kod 3
#define	PB0_kod 4
#define minus_kod ' '
//}

volatile unsigned char odb_x;					// odebrana liczba X
volatile unsigned char g_usart_rx_flag = 0;	// flaga informuj¹ca main o odebraniu liczby
volatile unsigned char pilot_flaga, n, tab[66], code_type, czas;
volatile unsigned char wysylanie, tryb = 3, stop2 = 1;
volatile unsigned char usart_bufor_ind;
volatile unsigned char code;//, czas;
volatile unsigned char pop_PINB = 0xFF, pop_PIND = 0b01110000;

void usart_inicjuj(void)  // BAUD = 9600
{
	#define BAUD 9600        //tutaj podaj ¿¹dan¹ prêdkoœæ transmisji
	#include <util/setbaud.h> //linkowanie tego pliku musi byæ
	//po zdefiniowaniu BAUD
	
	//ustaw obliczone przez makro wartoœci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |= (1 << U2X);
	#else
	UCSRA &= ~(1 << U2X);
	#endif
	
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);  //bitów danych: 8
	//bity stopu:  1
	//parzystoœæ:  brak
	//w³¹cz nadajnik i odbiornik oraz przerwania odbiornika
	//przerwania nadajnika w³¹czamy w funkcji wyslij_wynik()
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
}

void wyslij_wynik(void)
{
	//Zaczekaj, a¿ bufor nadawania bêdzie pusty
	while (!(UCSRA & (1<<UDRE)));
	//nastêpny znak do wysy³ki to znak nr 1
	usart_bufor_ind = 0;
	//w³¹cz przerwania pustego bufora UDR, co rozpocznie transmisjê
	//aktualnej zawartoœci bufora
	UCSRB |= (1<<UDRIE);
}

ISR(USART_UDRE_vect)
{
	if(tab[usart_bufor_ind]!= 0)
	{
		UDR = tab[usart_bufor_ind++];
	}else
	{
		UCSRB &= ~(1<<UDRIE); //wy³¹cz przerwania pustego bufora nadawania
	}
}

ISR(USART_RX_vect)  //przerwanie generowane po odebraniu bajtu
{
	odb_x = UDR;   //zapamiêtaj odebran¹ liczbê
	g_usart_rx_flag = 1; //ustaw flagê odbioru liczby dla main()
}

ISR(INT0_vect)	// pilot RC5
{
	czas = TCNT0;
	TCNT0  = 0x00;
	
	switch(tryb)
	{
		case tryb_pilot_do_PC:
		case tryb_pilot_odtwarzacza:
			if(stop2)
			{
				stop2 = 0;
			}else
			{
				n++;
				tab[n] = czas;
			}
			MCUCR ^= (1<<ISC00);
		break;
		case tryb_wizualizacja:
			czas = TCNT1;
			TCNT1 = 0;
			
			if(stop2)
			{
				tab[0] = start;
				n = 1;
				stop2 = 0;
			}else
			{
				n++;
				tab[n] = czas;
			}
		break;
	}
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

ISR(TIMER1_COMPA_vect)
{
	stop2 = 1;
	if(n > 2)
	{
		wysylanie = 1;
		tab[1] = n - 1;
		tab[n+1] = 0;
		n = 0;
	}
}

ISR(PCINT_B_vect)
{
	unsigned char i;
	for (i = 0; i < 8 ; i++)
	{
		if ((PINB & (1 << i)) != (pop_PINB & (1 << i)))	// wykryj zmianê i-ego bita
		{
			while (!(UCSRA & (1<<UDRE)));
			if (pop_PINB & (1 << i))	// wykryj czy to zmiana z 1 na 0 czy odwrotnie
			{
				UDR = '0' + PB0_kod + i;
			}
			else
			{
				UDR = minus_kod + PB0_kod + i;
			}
		}
	}
	pop_PINB = PINB;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 10;
}

ISR(PCINT_D_vect)
{
	unsigned char i;
	for (i = 3; i < 7 ; i++)
	{
		if ((PIND & (1 << i)) != (pop_PIND & (1 << i)))	// wykryj zmianê i-ego bita
		{
			while (!(UCSRA & (1<<UDRE)));
			if (pop_PIND & (1 << i))	// wykryj czy to zmiana z 1 na 0 czy odwrotnie
			{
				UDR = '0' + i - PD6_kod;
			}
			else
			{
				UDR = minus_kod + i - PD6_kod;
			}
		}
	}
	pop_PIND = PIND;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 10;
}

void remote_init_pilot(void)
{
	DDRD  &= ~(1<<PD2);
	PORTD &= ~(1<<PD2);
	MCUCR = (1<<ISC01) | (1<<ISC00); // narastaj¹ce zbocze
	GIMSK |= (1<<INT0);
	TCNT0 = 0;
	TCCR0B = (1<<CS02); // prescaler 256
	TIMSK = (1 << TOIE0);
	TCCR1B = 0;		// zatrzymaj timer 1
}

void remote_init_wizualizacja(void)
{
	DDRD  &= ~(1<<PD2);
	PORTD &= ~(1<<PD2);
	MCUCR = (1<<ISC00); // ka¿de zbocze
	GIMSK |= (1<<INT0);
	TCNT1 = 0;
	OCR1A = 1000;					// 32ms
	TCCR1B = (1<<CS12) | (1<<WGM12); // CTC prescaler 256 - 32us na zliczenie
	TIMSK = (1 << OCIE1A);
	TCCR0B = 0;		// zatrzymaj timer 0
}

int main(void)
{
	uint8_t tab2[27];
	uint8_t led_luminance = 0x80;
	uint8_t led_bar_color[3] = {0x10, 0x10, 0x10};
	
	PORTD = 0b01110000;	// podci¹gnij wejœcia do zasilania
	PORTB = 0xFF;
	GIMSK |= (1 << PCIE0) | (1 << PCIE2);
	PCMSK = 0xFF;
	PCMSK2 = 0b01110000;
	LedBarInit();
	LedBarClear();
	remote_init_pilot();
	usart_inicjuj();
	pop_PINB = PINB;
	pop_PIND = PIND;
	sei();

	for(;;)
	{
		if(wysylanie)
		{
			wysylanie = 0;
			wyslij_wynik();
		}
		if(g_usart_rx_flag)
		{
			g_usart_rx_flag = 0;
			switch(odb_x)
			{
				case 11:
					tryb = tryb_pilot_odtwarzacza;
					remote_init_pilot();
				break;
				case 22:
					tryb = tryb_wizualizacja;
					remote_init_wizualizacja();
				break;
				case 33:
					tryb = tryb_pilot_do_PC;
					remote_init_pilot();
				break;
			}
		}
		if(pilot_flaga)
		{
			unsigned char i = 0, j = 1;
			pilot_flaga = 0;
			while (i < 27)
			{
				if((tab[j]>23)&&(tab[j]<36))		// 26 = 0.8ms, 33 = 1.056ms
				{
					tab2[i] = j & 1;
					i++;
				}else
				{
					if((tab[j]>51)&&(tab[j]<64)) // 54 = 1,728ms 61 = 1.952ms
					{
						tab2[i] = j & 1;
						i++;
						tab2[i] = j & 1;
						i++;
					}
				}
				j++;
			}
			if(1)
			{
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

				tab[0] = code;
				if(tab[0] == 0)
					tab[0] = 2;
				tab[1] = 10;
				tab[2] = 0;
				wyslij_wynik();
			}
		}
	}
}