/*
    Copyright 2018 Jan Pomikalek <jan.pomikalek@gmail.com>

    This file is part of the VeGa firmware.

    VeGa firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VeGa firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VeGa firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef VEGA_DISPLAY_H
#define VEGA_DISPLAY_H

#include <TFT_22_ILI9225.h>

void display_init();
void display_set_imperial(bool enable);
void display_draw_labels();
void display_set_volts(float volts);
void display_set_mah(uint16_t mah);
void display_set_trip_distance(uint32_t meters, uint16_t color = COLOR_WHITE);
void display_set_total_distance(uint32_t meters);
void display_set_speed(uint8_t kph);
void display_update_battery_indicator(float battery_percent, bool redraw = false);
void display_update_speed_indicator(float speed_percent, bool redraw = false);
void display_indicate_read_success(uint32_t duration_ms);
void display_indicate_read_failure(uint32_t duration_ms);
uint16_t display_make_color(uint8_t red, uint8_t green, uint8_t blue);

#endif //VEGA_DISPLAY_H
