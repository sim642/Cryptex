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
//#include <string.h>
#include "usb_serial.h"

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

#define LED1R PF6
#define LED1G PF5
#define LED1B PF7
#define LED2R PF1
#define LED2G PF0
#define LED2B PF4

int atoi(const char * str);

bool strpref(const char *str, const char *pre)
{
	return strncmp(str, pre, strlen(pre)) == 0;
}


char response[16], reply[16];

void usb_write(const char *str)
{
	while (*str)
	{
		usb_serial_putchar(*str);
		str++;
	}
}

void usart_write(unsigned char* data)
{
	PORTB |= 0b00001100;
	PORTD |= 0b00010000;
	while(*data)
	{
		while(!(UCSR1A & (1 << UDRE1)));
		UDR1 = *data;
		data++;
	}
	_delay_ms(5);
	PORTB &= 0b11110011;
	PORTD &= 0b11101111;
}

uint8_t usb_recv_str(char *buf, uint8_t size)
{
	char data;
	uint8_t count = 0;

	while (count < size)
	{
		data = usb_serial_getchar();
		if (data == '\r' || data == '\n')
		{
			*buf = '\0';
			return size;
		}
		if (data >= ' ' && data <= '~')
		{
			*buf++ = data;
			count++;
		}
	}
	return count;
}

volatile unsigned char usart_buf[16];
volatile uint8_t usart_i = 0;
volatile bool usart_data_ready = false;

ISR(USART1_RX_vect)
{
	unsigned char ch = UDR1;

	if(ch >= ' ' && ch <= '~')
	{
		usart_buf[usart_i] = ch;
		usart_i++;
	}

	if(ch == '\n' || ch == '\r')
	{
		usart_buf[usart_i] = '\0';
		usart_i = 0;
		usart_data_ready = true;
		return;
	}
}

typedef void (*reply_func_t)(const char*);

void usb_reply_raw(const char *str)
{
	sprintf(reply, "%s\n", str);
	usb_write(reply);
}

void usart_reply_raw(const char *str)
{
	uint8_t id = eeprom_read_byte((uint8_t*)0);
	sprintf(reply, "%d:%s\n", id, str);
	usart_write(reply);
}

void usb_reply(const char *str)
{
	sprintf(reply, "<%s>\n", str);
	usb_write(reply);
}

void usart_reply(const char *str)
{
	uint8_t id = eeprom_read_byte((uint8_t*)0);
	sprintf(reply, "<%d:%s>\n", id, str);
	usart_write(reply);
}

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

int main(void)
{
	// remove CLKDIV8
	CLKPR = 0x80;
	CLKPR = 0x00;

	// disable JTAG - control F port
	MCUCR = BIT(JTD);
	MCUCR = BIT(JTD);

	// LED outputs
	bit_set(DDRF, 0b11110011);

	// initialize USB
	usb_init();

	// initialize RS485
	UBRR1 = 51; // 19200, 8
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // enable Tx, Rx
	UCSR1C = (1 << USBS1) | (3 << UCSZ10); // 2-bit stop, 8-bit character

	// wait for USB configuration
	// while (!usb_configured());
	_delay_ms(1000);

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
			usart_data_ready = 0;
		}
	}
}
