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

#ifndef DAVEGA_SCREEN_H
#define DAVEGA_SCREEN_H

#include <TFT_22_ILI9225.h>
#include "davega_data.h"

typedef enum {
    TSCR_FW_VERSION = 0,
    TSCR_MOSFET_TEMPERATURE,
    TSCR_MOTOR_TEMPERATURE,
    TSCR_MOTOR_CURRENT,
    TSCR_BATTERY_CURRENT,
    TSCR_DUTY_CYCLE,
    TSCR_TOTAL_VOLTAGE,
    TSCR_MIN_TOTAL_VOLTAGE,
    TSCR_AVG_CELL_VOLTAGE,
    TSCR_BATTERY_CAPACITY_MAH,
    TSCR_BATTERY_CAPACITY_PERCENT,
    TSCR_TRIP_DISTANCE,
    TSCR_TOTAL_DISTANCE,
    TSCR_SPEED,
    TSCR_MAX_SPEED,
    TSCR_AVG_SPEED,
    TSCR_TIME_ELAPSED,
    TSCR_TIME_RIDING,
    TSCR_FAULT_CODE
} t_text_screen_item;

typedef struct {
    char* fw_version;
    bool imperial_units;
    bool use_fahrenheit;
    bool per_cell_voltage;
    uint8_t battery_cells;
    bool big_font;
    t_text_screen_item* text_screen_items;
    uint8_t text_screen_items_count;
} t_davega_screen_config;

class DavegaScreen {
public:
    void init(TFT_22_ILI9225* tft, t_davega_screen_config* config) { _tft = tft; _config = config; };
    virtual void reset() = 0;
    virtual void update(t_davega_data* data) = 0;
    virtual void heartbeat(uint32_t duration_ms, bool successful_vesc_read) = 0;

protected:
    TFT_22_ILI9225* _tft;
    t_davega_screen_config* _config;
};

#endif //DAVEGA_SCREEN_H
