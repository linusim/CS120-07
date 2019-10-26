/*	Author: lim001
 *  Partner(s) Name: Festo Bwogi
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char tmpB = 0x00;
unsigned char tempC; // temp variable to store C
unsigned char count = 0x00; // variable storing number of victories
unsigned char message[8] = {89, 111, 117, 32, 119, 111, 110, 33}; // Output for victory royal ('You won!')

enum states{Start, LED1, LED2, LED3, WAIT, RESTART, INC, DEC, WIN} state;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick()
{
	unsigned char button = (~PINA) & 0x01;
	
	switch(state)		// transitions
	{
		case Start:
		{
			PORTB = 0x00;
			state = LED1;
			break;
		}
		
		case LED1:
		if(button == 0x01)
		{
			count--;
			state = WAIT;
			break;
		}
		else
		{
			state = LED2;
			break;
		}
		
		case LED2:
		if(button == 0x01)
		{
			count++;
			if(count == 0x09){
				state = WIN;
			}
			else{
				state = INC;
			}
			break;
		}
		else
		{
			state = LED3;
			break;
		}
		
		case LED3:
		if(button == 0x01)
		{
			count--;
			state = WAIT;
			break;
		}
		else
		{
			state = LED1;
			break;
		}
		
		case WAIT:
		if(button == 0x01)
		{
			state = WAIT;
			break;
		}
		else
		{
			state = RESTART;
			break;
		}
		
		case RESTART:
		if(button == 0x00)
		{
			state = LED1;
			break;
		}
		else
		{
			state = RESTART;
			break;
		}
		
		case INC:
		state = RESTART;
		break;
		
		case DEC:
		state = RESTART;
		break;
		
		case WIN:
		state = Start; // restarts game, resetting score to 5
		break;
		
		default:
		break;
	}
	
	switch(state)		// actions
	{
		case Start:
		tempC = 5;
		break;
		
		case LED1:
		tmpB = 0x01;
		break;
		
		case LED2:
		tmpB = 0x02;
		break;
		
		case LED3:
		tmpB = 0x04;
		break;
		
		case WAIT:
		break;
		
		case RESTART:
		break;
		
		case INC:
		tempC = tempC + 1;
		break;
		
		case DEC:
		tempC = tempC - 1;
		break;
		
		case WIN:
		LCD_WriteData(message);
		break;
		
		default:
		break;
	}
}

int main()
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00; // Configure C's 8 pins as outputs. Initialize to 0s. // LED data lines
	DDRD = 0xFF; PORTD = 0x00; // Configure D's 8 pins as outputs. Initialize to 0s. // LED control lines
	
	TimerSet(300);
	TimerOn();
	
	LCD_init();
	LCD_ClearScreen();
	
	tempC = 0x00;
	
	state = Start;
	
	while(1) {
		LCD_Cursor(1);
		LCD_WriteData(tempC + '0');
		Tick();
		PORTB = tmpB;
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}
