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

#ifndef DAVEGA_DATA_H
#define DAVEGA_DATA_H

#include "vesc_comm.h"

typedef struct {
    uint32_t millis_elapsed;
    uint32_t millis_riding;
    float max_speed_kph;
    float min_voltage;
    int32_t trip_meters;

    // TODO:
    // max_motor_temp
    // max_fet_temp
    // max_current
    // min_current
    // wh_spent
    // wh_per_km (derived)
    // range_km (derived)
} t_davega_session_data;

typedef struct {
    float mosfet_celsius;
    float motor_celsius;
    float motor_amps;
    float battery_amps;
    float duty_cycle;
    float voltage;
    float voltage_percent;
    int32_t mah;
    float mah_reset_progress;
    float mah_percent;
    float battery_percent;
    float speed_kph;
    float speed_percent;
    float trip_km;
    float session_reset_progress;
    float total_km;
    vesc_comm_fault_code vesc_fault_code = FAULT_CODE_NONE;
    t_davega_session_data* session;
} t_davega_data;

#endif //DAVEGA_DATA_H
