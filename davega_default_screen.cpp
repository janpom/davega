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

#include "davega_default_screen.h"
#include "davega_screen.h"
#include "davega_util.h"
#include "vesc_comm.h"
#include "tft_util.h"
#include <TFT_22_ILI9225.h>

#define BATTERY_INDICATOR_CELL_WIDTH 14
#define BATTERY_INDICATOR_CELL_HEIGHT 15

#define SPEED_INDICATOR_CELL_WIDTH 10
#define SPEED_INDICATOR_CELL_HEIGHT 11

#define LEN(X) (sizeof(X) / sizeof(X[0]))

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

const Point PROGMEM BATTERY_INDICATOR_CELLS[] = {
    // left column
    {0, 204},  {0, 187},  {0, 170},  {0, 153},  {0, 136},
    {0, 119},  {0, 102},  {0, 85},  {0, 68},  {0, 51},
    {0, 34},  {0, 17},
    // top row
    {0, 0},  {16, 0},  {32, 0},  {48, 0},  {64, 0},
    {80, 0},  {96, 0},  {112, 0},  {128, 0},  {144, 0},
    {160, 0},
    // right column
    {160, 17},  {160, 34},  {160, 51},  {160, 68},  {160, 85},
    {160, 102},  {160, 119},  {160, 136},  {160, 153},  {160, 170},
    {160, 187},  {160, 204},
};

const Point PROGMEM SPEED_INDICATOR_CELLS[] = {
    // bottom-left row
    {57, 151},  {41, 151}, {25, 151},
    // left column
    {25, 134}, {25, 117}, {25, 100}, {25, 84},
    // top row
    {25, 66}, {41, 66}, {57, 66}, {73, 66}, {90, 66},
    {106, 66}, {122, 66}, {138, 66},
    // right column
    {138, 83}, {138, 100}, {138, 117}, {138, 134},
    // bottom-right row
    {138, 151}, {122, 151}, {106, 151},
};

void DavegaDefaultScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);
    _draw_labels();

    // draw FW version
    _tft->setFont(Terminal6x8);
    _tft->drawText(40, 140, _config->fw_version, COLOR_WHITE);

    _just_reset = true;
}

void DavegaDefaultScreen::update(t_davega_data* data) {
    char fmt[10];

    if (data->vesc_fault_code != _last_fault_code)
        reset();

    // draw voltage
    if (_config->per_cell_voltage)
        dtostrf(data->voltage / _config->battery_cells, 4, 2, fmt);
    else
        dtostrf(data->voltage, 4, 1, fmt);
    tft_util_draw_number(_tft, fmt, 24, 25, COLOR_WHITE, COLOR_BLACK, 2, 4);

    // draw mah
    dtostrf(data->mah, 5, 0, fmt);
    tft_util_draw_number(_tft, fmt, 84, 25, progress_to_color(data->mah_reset_progress, _tft), COLOR_BLACK, 2, 4);

    // draw trip distance
    dtostrf(convert_distance(data->trip_km, _config->imperial_units), 5, 2, fmt);
    tft_util_draw_number(_tft, fmt, 24, 185, progress_to_color(data->session_reset_progress, _tft), COLOR_BLACK, 2, 4);

    // draw total distance
    dtostrf(convert_distance(data->total_km, _config->imperial_units), 4, 0, fmt);
    tft_util_draw_number(_tft, fmt, 98, 185, COLOR_WHITE, COLOR_BLACK, 2, 4);

    // draw speed
    uint8_t speed = round(convert_speed(data->speed_kph, _config->imperial_units));
    if (speed >= 10)
        tft_util_draw_digit(_tft, speed / 10, 60, 91, COLOR_WHITE, COLOR_BLACK, 7);
    else
        _tft->fillRectangle(60, 91, 82, 127, COLOR_BLACK);
    tft_util_draw_digit(_tft, speed % 10, 89, 91, COLOR_WHITE, COLOR_BLACK, 7);

    // draw warning
    if (data->vesc_fault_code != FAULT_CODE_NONE) {
        uint16_t bg_color = _tft->setColor(150, 0, 0);
        _tft->fillRectangle(0, 60, 176, 83, bg_color);
        _tft->setFont(Terminal12x16);
        _tft->setBackgroundColor(bg_color);
        _tft->drawText(5, 65, vesc_fault_code_to_string(data->vesc_fault_code), COLOR_BLACK);
        _tft->setBackgroundColor(COLOR_BLACK);
    }

    _update_speed_indicator(data->speed_percent, _just_reset);
    _update_battery_indicator(data->battery_percent, _just_reset);

    _last_fault_code = data->vesc_fault_code;
    _just_reset = false;
}

void DavegaDefaultScreen::heartbeat(uint32_t duration_ms, bool successful_vesc_read) {
    uint16_t color = successful_vesc_read ? _tft->setColor(0, 150, 0) : _tft->setColor(150, 0, 0);
    _tft->fillRectangle(85, 155, 89, 159, color);
    delay(duration_ms);
    _tft->fillRectangle(85, 155, 89, 159, COLOR_BLACK);
}

void DavegaDefaultScreen::_draw_labels() {
    _tft->setFont(Terminal6x8);

    _tft->drawText(36, 48, "VOLTS", COLOR_WHITE);
    _tft->drawText(132, 48, "MAH", COLOR_WHITE);

    _tft->drawText(90, 131, _config->imperial_units ? "MPH" : "KPH", COLOR_WHITE);

    _tft->drawText(23, 175, "TRIP", COLOR_WHITE);
    _tft->drawText(97, 175, "TOTAL", COLOR_WHITE);

    if (_config->imperial_units) {
        _tft->drawText(50, 208, "MILES", COLOR_WHITE);
        _tft->drawText(119, 208, "MILES", COLOR_WHITE);
    }
    else {
        _tft->drawText(70, 208, "KM", COLOR_WHITE);
        _tft->drawText(139, 208, "KM", COLOR_WHITE);
    }
}

bool DavegaDefaultScreen::_draw_battery_cell(int index, bool filled, bool redraw = false) {
    uint16_t p_word = pgm_read_word_near(BATTERY_INDICATOR_CELLS + index);
    Point *p = (Point *) &p_word;
    if (filled || redraw) {
        uint8_t cell_count = LEN(BATTERY_INDICATOR_CELLS);
        uint8_t green = (uint8_t)(255.0 / (cell_count - 1) * index);
        uint8_t red = 255 - green;
        uint16_t color = _tft->setColor(red, green, 0);
        _tft->fillRectangle(
                p->x, p->y,
                p->x + BATTERY_INDICATOR_CELL_WIDTH, p->y + BATTERY_INDICATOR_CELL_HEIGHT,
                color
        );
    }
    if (!filled) {
        _tft->fillRectangle(
                p->x + 1, p->y + 1,
                p->x + BATTERY_INDICATOR_CELL_WIDTH - 1, p->y + BATTERY_INDICATOR_CELL_HEIGHT - 1,
                COLOR_BLACK
        );
    }
}

void DavegaDefaultScreen::_update_battery_indicator(float battery_percent, bool redraw = false) {
    int cells_to_fill = round(battery_percent * LEN(BATTERY_INDICATOR_CELLS));
    if (redraw) {
        for (int i = 0; i < LEN(BATTERY_INDICATOR_CELLS); i++)
            _draw_battery_cell(i, i < cells_to_fill, true);
    }
    else {
        if (cells_to_fill > _battery_cells_filled) {
            for (int i = _battery_cells_filled; i < cells_to_fill; i++)
                _draw_battery_cell(i, true);
        }
        if (cells_to_fill < _battery_cells_filled) {
            for (int i = _battery_cells_filled - 1; i >= cells_to_fill; i--)
                _draw_battery_cell(i, false);
        }
    }
    _battery_cells_filled = cells_to_fill;
}

void DavegaDefaultScreen::_draw_speed_cell(int index, bool filled, bool redraw = false) {
    uint16_t p_word = pgm_read_word_near(SPEED_INDICATOR_CELLS + index);
    Point *p = (Point *) &p_word;
    if (filled || redraw) {
        _tft->fillRectangle(
                p->x, p->y,
                p->x + SPEED_INDICATOR_CELL_WIDTH, p->y + SPEED_INDICATOR_CELL_HEIGHT,
                _tft->setColor(150, 150, 255)
        );
    }
    if (!filled) {
        _tft->fillRectangle(
                p->x + 1, p->y + 1,
                p->x + SPEED_INDICATOR_CELL_WIDTH - 1, p->y + SPEED_INDICATOR_CELL_HEIGHT - 1,
                COLOR_BLACK
        );
    }
}

void DavegaDefaultScreen::_update_speed_indicator(float speed_percent, bool redraw = false) {
    int cells_to_fill = round(speed_percent * LEN(SPEED_INDICATOR_CELLS));
    if (redraw) {
        for (int i = 0; i < LEN(SPEED_INDICATOR_CELLS); i++)
            _draw_speed_cell(i, i < cells_to_fill, true);
    }
    else {
        if (cells_to_fill > _speed_cells_filled) {
            for (int i = _speed_cells_filled; i < cells_to_fill; i++)
                _draw_speed_cell(i, true);
        }
        if (cells_to_fill < _speed_cells_filled) {
            for (int i = _speed_cells_filled - 1; i >= cells_to_fill; i--)
                _draw_speed_cell(i, false);
        }
    }
    _speed_cells_filled = cells_to_fill;
}
