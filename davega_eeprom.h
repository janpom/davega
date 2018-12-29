/*
    Copyright 2018 Jan Pomikalek <jan.pomikalek@gmail.com>

    This file is part of the DAVEga firmware.

    DAVEga firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAVEga firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DAVEga firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DAVEGA_EEPROM_H
#define DAVEGA_EEPROM_H

#include "davega_data.h"
#include <Arduino.h>

bool eeprom_is_initialized(uint8_t magic_value);
void eeprom_initialize(uint8_t magic_value);

float eeprom_read_volts();
void eeprom_write_volts(float volts);

uint16_t eeprom_read_mah_spent();
void eeprom_write_mah_spent(uint16_t mah_spent);

uint32_t eeprom_read_total_distance();
void eeprom_write_total_distance(uint32_t meters);

t_davega_session_data eeprom_read_session_data();
void eeprom_write_session_data(t_davega_session_data session_data);

#endif //DAVEGA_EEPROM_H
