#define F_CPU 16000000UL
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__ // http://lists.gnu.org/archive/html/avr-gcc-list/2012-05/msg00030.html
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdbool.h>
#include "eeprom.h"
#include "comms.h"
#include "util.h"

#define LED1R PF6
#define LED1G PF5
#define LED1B PF7
#define LED2R PF1
#define LED2G PF0
#define LED2B PF4
#define LEDS 0b11110011

#define KICK PD5
#define CHARGE PD4

#define EXTPWM PC6

int atoi(const char * str);


char response[16];

void parse_and_execute_command(char *buf, bool usart)
{
	char *command;
	int16_t par1;
	command = buf;

	reply_func_t reply_func, reply_raw_func;
	if (usart == 1)
	{
		reply_func = usart_reply;
		reply_raw_func = usart_reply_raw;
	}
	else
	{
		reply_func = usb_reply;
		reply_raw_func = usb_reply_raw;
	}

	if (usart == 1)
	{
		uint8_t id = eeprom_read_byte(EEPROM_ID);
		par1 = atoi(command);
		if(par1 != id)
		{
			return;
		}
		else
		{
			while (*command != ':')
				command++;
			command++;
		}
	}

	if (strpref(command, "id"))
	{
		// set ID
		par1 = atoi(command + 2);
		eeprom_update_byte(EEPROM_ID, par1);
	}
	else if (strpref(command, "?"))
	{
		// get ID
		par1 = eeprom_read_byte(EEPROM_ID);
		sprintf(response, "id:%d", par1);
		reply_func(response);
	}
	else if (strpref(command, "ko"))
	{
		// kick override
		par1 = atoi(command + 2);
		bit_write(par1, PORTD, BIT(KICK));
	}
	else if (strpref(command, "co"))
	{
		// charge override
		par1 = atoi(command + 2);
		bit_write(par1, PORTD, BIT(CHARGE));
	}
	else if (strpref(command, "dm"))
	{
		// dribbler motor
		par1 = atoi(command + 2);
		OCR3AL = par1;
	}
	else
	{
		reply_raw_func(command);
	}
}

ISR(TIMER0_COMPA_vect)
{
	bit_flip(PORTF, BIT(LED1B)); // visualize heartbeat
	bit_flip(PORTF, BIT(LED2R));
}

int main(void)
{
	// clock prescaler
	CLKPR = BIT(CLKPCE); // enable prescaler change
	CLKPR = BITS(0b0000, CLKPS0); // divider 1

	// disable JTAG - control F port
	MCUCR = BIT(JTD);
	MCUCR = BIT(JTD);

	// LED outputs
	bit_set(DDRF, LEDS);
	bit_set(PORTF, LEDS);

	// coil outputs
	bit_set(DDRD, BIT(KICK));
	bit_set(DDRD, BIT(CHARGE));
	bit_clear(PORTD, BIT(KICK));
	bit_clear(PORTD, BIT(CHARGE));

	// initialize comms
	usb_init();
	usart_init();

	// wait for USB configuration
	// while (!usb_configured());
	_delay_ms(1000);

	// heartbeat timer (timer0)
	TCCR0A = BITS(0b10, WGM00); // CTC mode (mode 2)
	TCCR0B = BITS(0b101, CS00); // divider 1024
	TIMSK0 = BIT(OCIE0A); // enable compare A on timer0
	OCR0A = 250; // 62.5Hz
	TCNT0 = 0;

	// ext PWM (timer3)
	bit_set(DDRC, BIT(EXTPWM)); // ext PWM pin as output
	TCCR3A = BITS(0b10, COM3A0) | BITS(0b01, WGM30); // fast PWM, 8-bit (mode 5)
	TCCR3B = BITS(0b01, WGM32) | BITS(0b100, CS30); // divider 256
	OCR3AL = 0;

	sei(); // enable interrupts

	// LED test
	bit_clear(PORTF, BIT(LED2R));


	uint8_t n;
	char buf[16];

	while (1)
	{
		if (usb_serial_available())
		{
			n = usb_recv_str(buf, sizeof(buf));
			if (n == sizeof(buf))
			{
				parse_and_execute_command(buf, false);
			}
		}
		else if (usart_data_ready)
		{
			parse_and_execute_command(usart_buf, true);
			usart_data_ready = false;
		}
	}
}
