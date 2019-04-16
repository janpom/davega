/*
    Copyright 2019 Jan Pomikalek <jan.pomikalek@gmail.com>

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

#include "davega_simple_screen.h"
#include "davega_util.h"
#include <TFT_22_ILI9225.h>

uint16_t primary_item_color(t_screen_item screen_item, t_davega_data* data, t_davega_screen_config* config) {
    uint16_t color = COLOR_WHITE;
    if (screen_item == SCR_BATTERY_CURRENT || screen_item == SCR_MOTOR_CURRENT) {
        float value = screen_item == SCR_BATTERY_CURRENT ? data->battery_amps : data->motor_amps;
        if (value < 0)
            color = COLOR_RED;
        if (value >= 100)
            color = COLOR_YELLOW;
        if (value >= 200)
            color = COLOR_BLUEVIOLET;
    }
    else {
        // speed
        if (data->speed_kph > SS_RED_SPEED_KPH)
            color = COLOR_RED;
        else if (data->speed_kph > SS_YELLOW_SPEED_KPH)
            color = COLOR_YELLOW;
    }
    return color;
}

uint8_t primary_item_value(t_screen_item screen_item, t_davega_data* data, t_davega_screen_config* config) {
    float value;
    switch (screen_item) {
        case SCR_BATTERY_CURRENT:
            value = data->battery_amps;
            break;
        case SCR_MOTOR_CURRENT:
            value = data->motor_amps;
            break;
        default:
            value = convert_speed(data->speed_kph, config->imperial_units);
    }
    return abs(round(value)) % 100;
}
