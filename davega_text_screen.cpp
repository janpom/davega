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

String format_time(uint32_t milliseconds) {
    uint32_t seconds = milliseconds / 1000;
    uint32_t minutes = seconds / 60;
    seconds = seconds % 60;
    return String(minutes) + ":" + String(seconds < 10 ? "0" : "") + String(seconds);
}

void DavegaTextScreen::reset() {
    _tft->fillRectangle(0, 0, _tft->maxX() - 1, _tft->maxY() - 1, COLOR_BLACK);
}

void DavegaTextScreen::update(t_davega_data *data) {
    String s;
    uint16_t session_data_color = progress_to_color(data->session_reset_progress, _tft);
    uint16_t capacity_data_color = progress_to_color(data->mah_reset_progress, _tft);
    float avg_speed_kph;

    for (int i=0; i < _config->text_screen_items_count; i++) {
        uint16_t color = COLOR_WHITE;

        switch (_config->text_screen_items[i]) {
            case SCR_FW_VERSION:
                s = String("FW version: ") + String(_config->fw_version);
                break;
            case SCR_MOSFET_TEMPERATURE:
                s = String("MOSFET temp: ") + String(convert_temperature(data->mosfet_celsius, _config->use_fahrenheit))
                        + String(" ") + String(_config->use_fahrenheit ? "'F" : "'C");
                break;
            case SCR_MOTOR_TEMPERATURE:
                s = String("motor temp: ") + String(convert_temperature(data->motor_celsius, _config->use_fahrenheit))
                        + String(" ") + String(_config->use_fahrenheit ? "'F" : "'C");
                break;
            case SCR_MOTOR_CURRENT:
                s = String("motor current: ") + String(data->motor_amps) + String(" A");
                break;
            case SCR_BATTERY_CURRENT:
                s = String("battery current: ") + String(data->battery_amps) + String(" A");
                break;
            case SCR_DUTY_CYCLE:
                s = String("duty cycle: ") + String(data->duty_cycle);
                break;
            case SCR_TOTAL_VOLTAGE:
                s = String("total voltage: ") + String(data->voltage) + String(" V");
                break;
            case SCR_MIN_TOTAL_VOLTAGE:
                s = String("min total voltage: ") + String(data->session->min_voltage) + String(" V");
                break;
            case SCR_AVG_CELL_VOLTAGE:
                s = String("avg cell voltage: ") + String(data->voltage / _config->battery_cells) + String(" V");
                break;
            case SCR_BATTERY_CAPACITY_MAH:
                s = String("capacity: ") + String(data->mah) + String(" mAh");
                color = capacity_data_color;
                break;
            case SCR_BATTERY_CAPACITY_PERCENT:
                s = String("capacity: ") + String(data->battery_percent * 100) + String("%");
                color = capacity_data_color;
                break;
            case SCR_TRIP_DISTANCE:
                s = (String("trip: ") + String(convert_distance(data->trip_km, _config->imperial_units))
                     + String(" ") + String(_config->imperial_units ? "mi" : "km"));
                color = session_data_color;
                break;
            case SCR_TOTAL_DISTANCE:
                s = (String("total: ") + String(convert_distance(data->total_km, _config->imperial_units))
                     + String(" ") + String(_config->imperial_units ? "mi" : "km"));
                break;
            case SCR_SPEED:
                s = (String("speed: ") + String(convert_distance(data->speed_kph, _config->imperial_units))
                     + String(" ") + String(_config->imperial_units ? "mi/h" : "km/h"));
                break;
            case SCR_MAX_SPEED:
                s = (String("max speed: ") + String(convert_distance(data->session->max_speed_kph, _config->imperial_units))
                     + String(" ") + String(_config->imperial_units ? "mi/h" : "km/h"));
                break;
            case SCR_AVG_SPEED:
                avg_speed_kph = data->session->millis_riding > 0
                                ? 3600.0 * data->session->trip_meters / data->session->millis_riding
                                : 0;
                s = (String("avg speed: ") + String(convert_distance(avg_speed_kph, _config->imperial_units))
                     + String(" ") + String(_config->imperial_units ? "mi/h" : "km/h"));
                color = session_data_color;
                break;
            case SCR_TIME_ELAPSED:
                s = String("time elapsed: ") + format_time(data->session->millis_elapsed) + String(" mins");
                color = session_data_color;
                break;
            case SCR_TIME_RIDING:
                s = String("time riding: ") + format_time(data->session->millis_riding) + String(" mins");
                color = session_data_color;
                break;
            case SCR_FAULT_CODE:
                s = String("fault code: ") + String(vesc_fault_code_to_string(data->vesc_fault_code));
                break;
            default:
                s = String("ERROR: unknown item");
        }

        _write_line(&s, i, color);
    }


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

    int y = lineno * (_config->big_font ? 19 : 12) + 5;
    _tft->setFont(_config->big_font ? Terminal11x16 : Terminal6x8);
    _tft->drawText(5, y, line_buffer, color);
}
