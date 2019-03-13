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

#include "davega_data.h"

typedef enum {
    SCR_FW_VERSION = 0,
    SCR_MOSFET_TEMPERATURE,
    SCR_MOTOR_TEMPERATURE,
    SCR_MOTOR_CURRENT,
    SCR_BATTERY_CURRENT,
    SCR_DUTY_CYCLE,
    SCR_TOTAL_VOLTAGE,
    SCR_MIN_TOTAL_VOLTAGE,
    SCR_AVG_CELL_VOLTAGE,
    SCR_BATTERY_CAPACITY_MAH,
    SCR_BATTERY_CAPACITY_PERCENT,
    SCR_TRIP_DISTANCE,
    SCR_TOTAL_DISTANCE,
    SCR_SPEED,
    SCR_MAX_SPEED,
    SCR_AVG_SPEED,
    SCR_TIME_ELAPSED,
    SCR_TIME_RIDING,
    SCR_FAULT_CODE
} t_screen_item;

typedef struct {
    char* fw_version;
    bool imperial_units;
    bool use_fahrenheit;
    bool per_cell_voltage;
    uint8_t battery_cells;
    bool big_font;
    t_screen_item* text_screen_items;
    uint8_t text_screen_items_count;
    uint8_t orientation;
} t_davega_screen_config;

class DavegaScreen {
public:
    virtual void init(t_davega_screen_config* config) { _config = config; };
    virtual void reset() = 0;
    virtual void update(t_davega_data* data) = 0;
    virtual void heartbeat(uint32_t duration_ms, bool successful_vesc_read) = 0;

protected:
    t_davega_screen_config* _config;
};

#endif //DAVEGA_SCREEN_H
