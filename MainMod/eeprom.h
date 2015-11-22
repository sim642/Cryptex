#ifndef EEPROM_H
#define EEPROM_H

/** EEPROM
0: id
1: interrupt replies
2: automation
3: single shot
**/

#include <avr/eeprom.h>

#define EEPROM_ID ((uint8_t*)0)
#define EEPROM_INTRPL ((uint8_t*)1)
#define EEPROM_AUTOMAT ((uint8_t*)2)
#define EEPROM_SINGLE ((uint8_t*)3)

#endif
