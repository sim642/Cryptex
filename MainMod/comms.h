#ifndef COMMS_H
#define COMMS_H

#define F_CPU 16000000UL
#include <avr/io.h>
#define __DELAY_BACKWARD_COMPATIBLE__ // http://lists.gnu.org/archive/html/avr-gcc-list/2012-05/msg00030.html
#include <util/delay.h>
#include <stdbool.h>

#include "usb_serial.h"

extern volatile unsigned char usart_buf[16];
extern volatile uint8_t usart_i;
extern volatile bool usart_data_ready;

void usart_init();
void usb_write(const char *str);
void usart_write(unsigned char* data);
uint8_t usb_recv_str(char *buf, uint8_t size);


typedef void (*reply_func_t)(const char*);

void usb_reply_raw(const char *str);
void usart_reply_raw(const char *str);
void usb_reply(const char *str);
void usart_reply(const char *str);

#endif
