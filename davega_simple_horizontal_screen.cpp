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

#include "davega_simple_horizontal_screen.h"
#include "davega_screen.h"
#include "vesc_comm.h"
#include "tft_util.h"
#include <TFT_22_ILI9225.h>

#define KM_PER_MILE 0.621371

#define LEN(X) (sizeof(X) / sizeof(X[0]))

DavegaSimpleHorizontalScreen::DavegaSimpleHorizontalScreen(TFT_22_ILI9225* tft, t_davega_screen_config* config) {
    _tft = tft;
    _config = config;

    if (_config->imperial_units)
        _distance_speed_multiplier = KM_PER_MILE;
    else
        _distance_speed_multiplier = 1.0;
}

void DavegaSimpleHorizontalScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);

    // labels
    _tft->setFont(Terminal6x8);
    _tft->drawText(165, 35, "TRIP (KM)", COLOR_WHITE);
    _tft->drawText(159, 115, "BATTERY %", COLOR_WHITE);
    _tft->drawText(122, 115, "KPH", COLOR_WHITE);

    //set_warning("OVER TEMP MOTOR");  // FIXME
}

void DavegaSimpleHorizontalScreen::set_fw_version(char* fw_version) {
    String s = String("FW version: ") + String(fw_version);
    _write_line(&s, 0);
}

void DavegaSimpleHorizontalScreen::set_volts(float volts) {
    if (_config->per_cell_voltage)
        _draw_volts(volts / _config->battery_cells, 2);
    else
        _draw_volts(volts, 1);
}

void DavegaSimpleHorizontalScreen::_draw_volts(float volts, uint8_t decimals) {
    String s = String("voltage: ") + String(volts) + String(" V");
    _write_line(&s, 1);
}

void DavegaSimpleHorizontalScreen::set_mah(int32_t mah) {
    _mah = mah;
    _draw_mah(_mah, COLOR_WHITE);
}

void DavegaSimpleHorizontalScreen::set_mah_reset_progress(float progress){
    float brightness = 255.0 * (1.0 - progress);
    uint16_t color = _tft->setColor(brightness, brightness, brightness);
    _draw_mah(_mah, color);
}

void DavegaSimpleHorizontalScreen::_draw_mah(int32_t mah, uint16_t color) {
    String s = String("capacity: ") + String(mah) + String(" mAh");
    _write_line(&s, 2, color);
}

void DavegaSimpleHorizontalScreen::set_trip_distance(uint32_t meters) {
    _trip_distance = meters;
    _draw_trip_distance(_trip_distance, COLOR_WHITE);
}

void DavegaSimpleHorizontalScreen::set_trip_reset_progress(float progress) {
    float brightness = 255.0 * (1.0 - progress);
    uint16_t color = _tft->setColor(brightness, brightness, brightness);
    _draw_trip_distance(_trip_distance, COLOR_WHITE);
}

void DavegaSimpleHorizontalScreen::_draw_trip_distance(uint32_t meters, uint16_t color) {
    tft_util_draw_number(_tft, "24.7", 155, 0, COLOR_WHITE, COLOR_BLACK, 2, 6);
}

void DavegaSimpleHorizontalScreen::set_total_distance(uint32_t meters) {
    String s = String("total: ") + String(meters) + String(" m");
    _write_line(&s, 5);
}

void DavegaSimpleHorizontalScreen::set_speed(uint8_t kph) {
    tft_util_draw_number(_tft, "62", 0, 0, COLOR_RED, COLOR_BLACK, 10, 22);
}

void DavegaSimpleHorizontalScreen::update_battery_indicator(float battery_percent, bool redraw = false) {
    char fmt[3];
    dtostrf(100 * battery_percent, 2, 0, fmt);
    tft_util_draw_number(_tft, "68", 155, 60, COLOR_WHITE, COLOR_BLACK, 4, 10);

    int width = 17;
    int space = 3;
    int cell_count = 11;
    for (int i=0; i<cell_count; i++) {
        int x = i * (width + space);
        uint8_t red = (uint8_t)(255.0 / (cell_count - 1) * i);
        uint8_t green = 255 - red;
        uint16_t color = _tft->setColor(red, green, 0);
        _tft->fillRectangle(x, 140, x + width, 175, color);
        if (i < 4)
            _tft->fillRectangle(x + 1, 140 + 1, x + width - 1, 175 - 1, COLOR_BLACK);
    }
}

void DavegaSimpleHorizontalScreen::update_speed_indicator(float speed_percent, bool redraw = false) {}

void DavegaSimpleHorizontalScreen::indicate_read_success(uint32_t duration_ms) {
    delay(duration_ms);
}

void DavegaSimpleHorizontalScreen::indicate_read_failure(uint32_t duration_ms) {
    delay(duration_ms);
}

void DavegaSimpleHorizontalScreen::set_warning(char* warning) {
    uint16_t bg_color = _tft->setColor(150, 0, 0);
    _tft->fillRectangle(0, 140, 220, 176, bg_color);
    _tft->setFont(Terminal12x16);
    _tft->setBackgroundColor(bg_color);
    _tft->drawText(5, 151, warning, COLOR_BLACK);
    _tft->setBackgroundColor(COLOR_BLACK);
}

void DavegaSimpleHorizontalScreen::_write_line(String *text, int lineno, uint16_t color = COLOR_WHITE) {
}
