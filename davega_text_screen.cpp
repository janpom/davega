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

void DavegaTextScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);

    String s = String("FW version: ") + String(_config->fw_version);
    _write_line(&s, 0);
}

void DavegaTextScreen::update(t_davega_data *data) {
    int line = 1;
    String s;
    uint16_t session_data_color = progress_to_color(data->session_reset_progress, _tft);

    s = String("total voltage: ") + String(data->voltage) + String(" V");
    _write_line(&s, line++);

    s = String("min total voltage: ") + String(data->session->min_voltage) + String(" V");
    _write_line(&s, line++);

    s = String("avg cell voltage: ") + String(data->voltage / _config->battery_cells) + String(" V");
    _write_line(&s, line++);

    s = String("capacity: ") + String(data->mah) + String(" mAh");
    _write_line(&s, line++, progress_to_color(data->mah_reset_progress, _tft));

    s = String("capacity: ") + String(data->battery_percent * 100) + String("%");
    _write_line(&s, line++);

    s = (String("trip: ") + String(convert_distance(data->trip_km, _config->imperial_units))
            + String(" ") + String(_config->imperial_units ? "mi" : "km"));
    _write_line(&s, line++, session_data_color);

    s = (String("total: ") + String(convert_distance(data->total_km, _config->imperial_units))
            + String(" ") + String(_config->imperial_units ? "mi" : "km"));
    _write_line(&s, line++);

    s = (String("speed: ") + String(convert_distance(data->speed_kph, _config->imperial_units))
            + String(" ") + String(_config->imperial_units ? "mi/h" : "km/h"));
    _write_line(&s, line++);

    s = (String("max speed: ") + String(convert_distance(data->session->max_speed_kph, _config->imperial_units))
            + String(" ") + String(_config->imperial_units ? "mi/h" : "km/h"));
    _write_line(&s, line++, session_data_color);

    s = String("fault code: ") + String(vesc_fault_code_to_string(data->vesc_fault_code));
    _write_line(&s, line++);

    // TODO: format time hh:mm:ss
    s = String("time elapsed: ") + String(data->session->millis_elapsed / 1000) + String(" s");
    _write_line(&s, line++, session_data_color);

    s = String("time riding: ") + String(data->session->millis_riding / 1000) + String(" s");
    _write_line(&s, line++, session_data_color);
}

void DavegaTextScreen::heartbeat(uint32_t duration_ms, bool successful_vesc_read) {
    uint16_t color = successful_vesc_read ? _tft->setColor(0, 150, 0) : _tft->setColor(150, 0, 0);
    _tft->fillRectangle(167, 5, 171, 9, color);
    delay(duration_ms);
    _tft->fillRectangle(167, 5, 171, 9, COLOR_BLACK);
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
