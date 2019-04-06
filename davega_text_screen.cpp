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
}

void DavegaTextScreen::update(t_davega_data *data) {
    uint16_t session_data_color = progress_to_color(data->session_reset_progress, _tft);
    uint16_t capacity_data_color = progress_to_color(data->mah_reset_progress, _tft);
    float avg_speed_kph;

    for (int i=0; i < _config->text_screen_items_count; i++) {
        switch (_config->text_screen_items[i]) {
            case SCR_FW_VERSION:
                _write_text_line(_config->fw_version, i);
                break;
            case SCR_MOSFET_TEMPERATURE:
                _write_numeric_line(
                        convert_temperature(data->mosfet_celsius, _config->use_fahrenheit),
                        _config->use_fahrenheit ? "'F" : "'C", "MOSFET", i);
                break;
            case SCR_MOTOR_TEMPERATURE:
                _write_numeric_line(
                        convert_temperature(data->motor_celsius, _config->use_fahrenheit),
                        _config->use_fahrenheit ? "'F" : "'C", "motor", i);
                break;
            case SCR_MOTOR_CURRENT:
                _write_numeric_line(data->motor_amps, "A", "motor", i);
                break;
            case SCR_BATTERY_CURRENT:
                _write_numeric_line(data->battery_amps, "A", "battery", i);
                break;
            case SCR_DUTY_CYCLE:
                _write_numeric_line(data->duty_cycle * 100, "%", "duty", i);
                break;
            case SCR_TOTAL_VOLTAGE:
                _write_numeric_line(data->voltage, "V", "pack", i);
                break;
            case SCR_MIN_TOTAL_VOLTAGE:
                _write_numeric_line(data->session->min_voltage, "V", "min pack", i);
                break;
            case SCR_AVG_CELL_VOLTAGE:
                _write_numeric_line(data->voltage / _config->battery_cells, "V", "avg cell", i);
                break;
            case SCR_BATTERY_CAPACITY_MAH:
                _write_numeric_line(data->mah, "mAh", "remaining", i, capacity_data_color);
                break;
            case SCR_BATTERY_CAPACITY_PERCENT:
                _write_numeric_line(data->battery_percent * 100, "%", "battery", i, capacity_data_color);
                break;
            case SCR_TRIP_DISTANCE:
                _write_numeric_line(
                        convert_distance(data->trip_km, _config->imperial_units),
                        _config->imperial_units ? "mi" : "km", "trip", i, session_data_color);
                break;
            case SCR_TOTAL_DISTANCE:
                _write_numeric_line(
                        convert_distance(data->total_km, _config->imperial_units),
                        _config->imperial_units ? "mi" : "km", "total", i, session_data_color);
                break;
            case SCR_SPEED:
                _write_numeric_line(
                        convert_distance(data->speed_kph, _config->imperial_units),
                        _config->imperial_units ? "mph" : "kph", "", i);
                break;
            case SCR_MAX_SPEED:
                _write_numeric_line(
                        convert_distance(data->session->max_speed_kph, _config->imperial_units),
                        _config->imperial_units ? "mph" : "kph", "max", i, session_data_color);
                break;
            case SCR_AVG_SPEED:
                avg_speed_kph = data->session->millis_riding > 0
                                ? 3600.0 * data->session->trip_meters / data->session->millis_riding
                                : 0;
                _write_numeric_line(
                        convert_distance(avg_speed_kph, _config->imperial_units),
                        _config->imperial_units ? "mph" : "kph", "avg", i, session_data_color);
                break;
            case SCR_TIME_ELAPSED:
                _write_time_line(data->session->millis_elapsed / 1000, "elapsed", i, session_data_color);
                break;
            case SCR_TIME_RIDING:
                _write_time_line(data->session->millis_riding / 1000, "riding", i, session_data_color);
                break;
            case SCR_FAULT_CODE:
                _write_text_line(vesc_fault_code_to_string(data->vesc_fault_code), i);
                break;
            default:
                _write_text_line("ERROR: unknown item", i);
        }
    }
}

void DavegaTextScreen::heartbeat(uint32_t duration_ms, bool successful_vesc_read) {
    uint16_t color = successful_vesc_read ? _tft->setColor(0, 150, 0) : _tft->setColor(150, 0, 0);
    _tft->fillRectangle(167, 5, 171, 9, color);
    delay(duration_ms);
    _tft->fillRectangle(167, 5, 171, 9, COLOR_BLACK);
}

void DavegaTextScreen::_write_numeric_line(
        float value, const char* units, const char* label, int lineno, uint16_t color = COLOR_WHITE) {
    for (int i=0; i < MAX_LINE_LENGTH; i++)
        _line_buffer[i] = ' ';
    dtostrf(value, 8, 2, _line_buffer);
    _line_buffer[8] = ' ';
    for (int i=0; i<strlen(units); i++)
        _line_buffer[i+9] = units[i];
    for (int i=0; i<strlen(label); i++)
        _line_buffer[i+13] = label[i];
    _line_buffer[MAX_LINE_LENGTH] = '\0';

    _write_line_buffer(lineno, color);
}

void DavegaTextScreen::_write_time_line(uint32_t seconds, const char* label, int lineno, uint16_t color = COLOR_WHITE) {
    for (int i=0; i < MAX_LINE_LENGTH; i++)
        _line_buffer[i] = ' ';
    uint32_t hours = seconds / 3600;
    seconds -= 3600 * hours;
    uint32_t minutes = seconds / 60;
    seconds -= 60 * minutes;
    dtostrf(hours, 2, 0, _line_buffer);
    _line_buffer[2] = ':';
    dtostrf(minutes, 2, 0, _line_buffer + 3);
    _line_buffer[5] = ':';
    dtostrf(seconds, 2, 0, _line_buffer + 6);
    _line_buffer[8] = ' ';
    for (int i=3; i<8; i++) {
        if (_line_buffer[i] == ' ')
            _line_buffer[i] = '0';
    }

    _line_buffer[9] = 'h';
    for (int i=0; i<strlen(label); i++)
        _line_buffer[i+13] = label[i];
    _line_buffer[MAX_LINE_LENGTH] = '\0';

    _write_line_buffer(lineno, color);
}

void DavegaTextScreen::_write_text_line(char* value, int lineno, uint16_t color = COLOR_WHITE) {
    for (int i=0; i<strlen(value); i++)
        _line_buffer[i] = value[i];
    _line_buffer[strlen(value)] = '\0';

    _write_line_buffer(lineno, color);
}

void DavegaTextScreen::_write_line_buffer(int lineno, uint16_t color = COLOR_WHITE) {
    // space padding
    for (int i=strlen(_line_buffer); i < MAX_LINE_LENGTH; i++)
        _line_buffer[i] = ' ';
    _line_buffer[MAX_LINE_LENGTH] = '\0';

    int y = lineno * (_config->big_font ? 19 : 12) + 5;
    _tft->setFont(_config->big_font ? Terminal11x16 : Terminal6x8, true);
    _tft->drawText(5, y, _line_buffer, color);
}
