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

#ifndef DAVEGA_SIMPLE_SCREEN_H
#define DAVEGA_SIMPLE_SCREEN_H

#include "davega_screen.h"

#define SS_YELLOW_SPEED_KPH 25
#define SS_RED_SPEED_KPH 35

uint16_t primary_item_color(t_screen_item screen_item, t_davega_data* data, t_davega_screen_config* config);
uint8_t primary_item_value(t_screen_item screen_item, t_davega_data* data, t_davega_screen_config* config);

#endif //DAVEGA_SIMPLE_SCREEN_H
