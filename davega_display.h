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

#ifndef DAVEGA_DISPLAY_H
#define DAVEGA_DISPLAY_H

typedef struct {
    bool imperial_units;
    bool per_cell_voltage;
    uint8_t battery_cells;
} t_davega_display_config;

class DavegaDisplay {
public:
    virtual void reset() = 0;
    virtual void set_fw_version(char* fw_version) = 0;
    virtual void set_volts(float volts) = 0;
    virtual void set_mah(int32_t mah) = 0;
    virtual void set_mah_reset_progress(float progress) = 0;
    virtual void set_trip_distance(uint32_t meters) = 0;
    virtual void set_trip_reset_progress(float progress) = 0;
    virtual void set_total_distance(uint32_t meters) = 0;
    virtual void set_speed(uint8_t kph) = 0;
    virtual void update_battery_indicator(float battery_percent, bool redraw = false) = 0;
    virtual void update_speed_indicator(float speed_percent, bool redraw = false) = 0;
    virtual void indicate_read_success(uint32_t duration_ms) = 0;
    virtual void indicate_read_failure(uint32_t duration_ms) = 0;
    virtual void set_warning(char* warning) = 0;
};

#endif //DAVEGA_DISPLAY_H
