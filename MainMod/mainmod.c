/** EEPROM
0: id
**/

#define F_CPU 16000000UL
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__ // http://lists.gnu.org/archive/html/avr-gcc-list/2012-05/msg00030.html
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
//#include <string.h>
#include "usb_serial.h"

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

int atoi(const char * str);

char response[16], reply[16];

//TODO implement UART
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
		while(!( UCSR1A & (1<<UDRE1)));
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
	uint8_t count=0;

	while (count < size)
	{
		data = usb_serial_getchar();
		//usb_serial_putchar(data);
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
volatile uint8_t usart_data_ready = 0;

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
		usart_data_ready = 1;
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

void parse_and_execute_command(char *buf, uint8_t usart)
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
			{
				command++;
			}
			command++;
		}
	}

	if ((command[0] == 'i') && (command[1] == 'd'))
	{
		//set id
		par1 = atoi(command+2);
		eeprom_update_byte((uint8_t*)0, par1);
	}
	else if (command[0] == '?')
	{
		//get info: id
		par1 = eeprom_read_byte((uint8_t*)0);
		sprintf(response, "id:%d", par1);
		reply_func(response);
	}
	else
	{
		//bit_flip(PORTC, BIT(LED1));
		reply_raw_func(command);
	}
}

int main(void)
{
	CLKPR = 0x80;
	CLKPR = 0x00;
	usb_init();

	//TODO Implement and test UART init
	//UART init
	DDRB &= 0b11110010;
	DDRB |= 0b00001100;
	DDRD &= ~(0b00000010);
	DDRD |= 0b00010100;
	//DDRD |= 0b00000010;
	//19200
	//8
	UBRR1 = 51;
	//enable Tx and Rx
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);

	//Wait for USB to be configured
	//while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);


	PCICR = 1; //enable pin change interrupt
	PCMSK0 = 0b00000001;

	OCR1AL = 0;
	//OCR1BL = 0;

	sei();

	uint8_t n;
	char buf[16];

	while (1)
	{
		//TODO implement UART read
		if (usb_serial_available())
		{
			n = usb_recv_str(buf, sizeof(buf));
			if (n == sizeof(buf))
			{
				parse_and_execute_command(buf, 0);
			}
		}
		else if (usart_data_ready == 1)
		{
			parse_and_execute_command(usart_buf, 1);
			usart_data_ready = 0;
		}
	}
}
