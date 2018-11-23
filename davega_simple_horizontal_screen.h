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

#ifndef DAVEGA_SIMPLE_HORIZONTAL_SCREEN_H
#define DAVEGA_SIMPLE_HORIZONTAL_SCREEN_H

#include <TFT_22_ILI9225.h>
#include "davega_screen.h"

class DavegaSimpleHorizontalScreen: public DavegaScreen {
public:
    DavegaSimpleHorizontalScreen(TFT_22_ILI9225* tft, t_davega_screen_config* config);
    void reset();
    void set_fw_version(char* fw_version);
    void set_volts(float volts);
    void set_mah(int32_t mah);
    void set_mah_reset_progress(float progress);
    void set_trip_distance(uint32_t meters);
    void set_trip_reset_progress(float progress);
    void set_total_distance(uint32_t meters);
    void set_speed(uint8_t kph);
    void update_battery_indicator(float battery_percent, bool redraw = false);
    void update_speed_indicator(float speed_percent, bool redraw = false);
    void indicate_read_success(uint32_t duration_ms);
    void indicate_read_failure(uint32_t duration_ms);
    void set_warning(char* warning);

protected:
    TFT_22_ILI9225* _tft;
    t_davega_screen_config* _config;

    float _distance_speed_multiplier = 1.0;
    uint32_t _trip_distance;
    uint32_t _mah;

    void _draw_volts(float volts, uint8_t decimals);
    void _draw_mah(int32_t mah, uint16_t color);
    void _draw_trip_distance(uint32_t meters, uint16_t color);
    void _write_line(String *text, int lineno, uint16_t color = COLOR_WHITE);
};

#endif //DAVEGA_SIMPLE_HORIZONTAL_SCREEN_H
