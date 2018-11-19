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

#include "davega_text_screen.h"
#include "davega_screen.h"
#include "vesc_comm.h"
#include "tft_util.h"
#include <TFT_22_ILI9225.h>

#define KM_PER_MILE 0.621371

#define LEN(X) (sizeof(X) / sizeof(X[0]))

DavegaTextScreen::DavegaTextScreen(TFT_22_ILI9225* tft, t_davega_screen_config* config) {
    _tft = tft;
    _config = config;

    if (_config->imperial_units)
        _distance_speed_multiplier = KM_PER_MILE;
    else
        _distance_speed_multiplier = 1.0;
}

void DavegaTextScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);
}

void DavegaTextScreen::set_fw_version(char* fw_version) {
    String s = String("FW version: ") + String(fw_version);
    _write_line(&s, 0);
}

void DavegaTextScreen::set_volts(float volts) {
    if (_config->per_cell_voltage)
        _draw_volts(volts / _config->battery_cells, 2);
    else
        _draw_volts(volts, 1);
}

void DavegaTextScreen::_draw_volts(float volts, uint8_t decimals) {
    String s = String("voltage: ") + String(volts) + String(" V");
    _write_line(&s, 1);
}

void DavegaTextScreen::set_mah(int32_t mah) {
    _mah = mah;
    _draw_mah(_mah, COLOR_WHITE);
}

void DavegaTextScreen::set_mah_reset_progress(float progress){
    float brightness = 255.0 * (1.0 - progress);
    uint16_t color = _tft->setColor(brightness, brightness, brightness);
    _draw_mah(_mah, color);
}

void DavegaTextScreen::_draw_mah(int32_t mah, uint16_t color) {
    String s = String("capacity: ") + String(mah) + String(" mAh");
    _write_line(&s, 2, color);
}

void DavegaTextScreen::set_trip_distance(uint32_t meters) {
    _trip_distance = meters;
    _draw_trip_distance(_trip_distance, COLOR_WHITE);
}

void DavegaTextScreen::set_trip_reset_progress(float progress) {
    float brightness = 255.0 * (1.0 - progress);
    uint16_t color = _tft->setColor(brightness, brightness, brightness);
    _draw_trip_distance(_trip_distance, COLOR_WHITE);
}

void DavegaTextScreen::_draw_trip_distance(uint32_t meters, uint16_t color) {
    String s = String("trip: ") + String(meters) + String(" m");
    _write_line(&s, 4, color);
}

void DavegaTextScreen::set_total_distance(uint32_t meters) {
    String s = String("total: ") + String(meters) + String(" m");
    _write_line(&s, 5);
}

void DavegaTextScreen::set_speed(uint8_t kph) {
    String s = String("speed: ") + String(kph) + String(" km/h");
    _write_line(&s, 6);
}

void DavegaTextScreen::update_battery_indicator(float battery_percent, bool redraw = false) {
    String s = String("capacity: ") + String(battery_percent * 100) + String("%");
    _write_line(&s, 3);
}

void DavegaTextScreen::update_speed_indicator(float speed_percent, bool redraw = false) {}

void DavegaTextScreen::indicate_read_success(uint32_t duration_ms) {
    delay(duration_ms);
}

void DavegaTextScreen::indicate_read_failure(uint32_t duration_ms) {
    delay(duration_ms);
}

void DavegaTextScreen::set_warning(char* warning) {
    String s = String("warning: ") + String(warning);
    _write_line(&s, 7);
}

void DavegaTextScreen::_write_line(String *text, int lineno, uint16_t color = COLOR_WHITE) {
    const int max_line_length = 23;
    char line_buffer[max_line_length + 1];
    String s = String("") + *text;
    while (s.length() < max_line_length)
        s.concat(String(" "));
    s.toCharArray(line_buffer, sizeof(line_buffer));

    int y = lineno * 12 + 5;
    _tft->setFont(Terminal6x8);
    _tft->drawText(5, y, line_buffer, color);
}
