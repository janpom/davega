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

#include "davega_mini_screen.h"

void DavegaMiniScreen::reset() {
    _oled->clearDisplay();
}

void DavegaMiniScreen::update(t_davega_data *data) {
    _oled->clearDisplay();
    _oled->setTextSize(1);
    _oled->setTextColor(WHITE);
    _oled->setCursor(0,0);
    _oled->println(String(data->session->max_speed_kph) + String(" km/h [max spd]"));
    _oled->println(String(data->voltage / _config->battery_cells) + String(" V [battery]"));
    _oled->println(String(data->trip_km) + String(" km [trip]"));
    _oled->println(String(data->total_km) + String(" km [total]"));
    _oled->display();
}

void DavegaMiniScreen::heartbeat(uint32_t duration_ms, bool successful_vesc_read) {
    // TODO
}
