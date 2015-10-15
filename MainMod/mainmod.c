/** EEPROM
0: id
**/

#define F_CPU 16000000UL
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__ // http://lists.gnu.org/archive/html/avr-gcc-list/2012-05/msg00030.html
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include <stdbool.h>
#include "comms.h"
#include "util.h"

#define LED1R PF6
#define LED1G PF5
#define LED1B PF7
#define LED2R PF1
#define LED2G PF0
#define LED2B PF4


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
		uint8_t id = eeprom_read_byte((uint8_t*)0);
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
		//set id
		par1 = atoi(command + 2);
		eeprom_update_byte((uint8_t*)0, par1);
	}
	else if (strpref(command, "?"))
	{
		//get info: id
		par1 = eeprom_read_byte((uint8_t*)0);
		sprintf(response, "id:%d", par1);
		reply_func(response);
	}
	else
	{
		reply_raw_func(command);
	}
}

ISR(TIMER0_COMPA_vect)
{
	bit_flip(PORTF, BIT(LED1G)); // visualize heartbeat
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
	bit_set(DDRF, 0b11110011);


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

	sei(); // enable interrupts

	// LED test
	bit_set(PORTF, BIT(LED1R));
	bit_set(PORTF, BIT(LED2B));

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
