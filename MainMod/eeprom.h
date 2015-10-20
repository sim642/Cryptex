#ifndef EEPROM_H
#define EEPROM_H

/** EEPROM
0: id
1: interrupt replies
**/

#include <avr/eeprom.h>

#define EEPROM_ID ((uint8_t*)0)
#define EEPROM_INTRPL ((uint8_t*)1)

#endif
