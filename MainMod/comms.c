#include "comms.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "util.h"

volatile unsigned char usart_buf[16];
volatile uint8_t usart_i = 0;
volatile bool usart_data_ready = false;
char reply[16];

void usart_init()
{
	UBRR1 = 51; // 19200, 8
	UCSR1B = BIT(RXEN1) | BIT(TXEN1) | BIT(RXCIE1); // enable Tx, Rx
	UCSR1C = BIT(USBS1) | BITS(0b11, UCSZ10); // 2-bit stop, 8-bit character
}

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
	while(*data)
	{
		while(!bit_get(UCSR1A, BIT(UDRE1)));
		UDR1 = *data;
		data++;
	}
	_delay_ms(5);
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
