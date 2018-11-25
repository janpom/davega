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
#include "davega_util.h"
#include "vesc_comm.h"
#include "tft_util.h"
#include <TFT_22_ILI9225.h>

DavegaTextScreen::DavegaTextScreen(TFT_22_ILI9225* tft, t_davega_screen_config* config) {
    _tft = tft;
    _config = config;
}

void DavegaTextScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);

    String s = String("FW version: ") + String(_config->fw_version);
    _write_line(&s, 0);
}

void DavegaTextScreen::update(t_davega_data *data) {
    int line = 1;
    String s;

    s = String("total voltage: ") + String(data->voltage) + String(" V");
    _write_line(&s, line++);

    s = String("avg cell voltage: ") + String(data->voltage / _config->battery_cells) + String(" V");
    _write_line(&s, line++);

    s = String("capacity: ") + String(data->mah) + String(" mAh");
    _write_line(&s, line++, progress_to_color(data->mah_reset_progress, _tft));

    s = String("capacity: ") + String(data->battery_percent * 100) + String("%");
    _write_line(&s, line++);

    // TODO: imperial
    s = String("trip: ") + String(data->trip_km) + String(" km");
    _write_line(&s, line++, progress_to_color(data->trip_reset_progress, _tft));

    // TODO: imperial
    s = String("total: ") + String(data->total_km) + String(" km");
    _write_line(&s, line++);

    // TODO: imperial
    s = String("speed: ") + String(data->speed_kph) + String(" km/h");
    _write_line(&s, line++);

    s = String("fault code: ") + String(vesc_fault_code_to_string(data->vesc_fault_code));
    _write_line(&s, line++);
}

void DavegaTextScreen::heartbeat(uint32_t duration_ms, bool successful_vesc_read) {
    // TODO: indicator
    delay(duration_ms);
}

void DavegaTextScreen::_write_line(String *text, int lineno, uint16_t color = COLOR_WHITE) {
    const int max_line_length = 30;
    char line_buffer[max_line_length + 1];
    String s = String("") + *text;
    while (s.length() < max_line_length)
        s.concat(String(" "));
    s.toCharArray(line_buffer, sizeof(line_buffer));

    int y = lineno * 12 + 5;
    _tft->setFont(Terminal6x8);
    _tft->drawText(5, y, line_buffer, color);
}
