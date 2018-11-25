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
    float voltage;
    float voltage_percent;
    int32_t mah;
    float mah_reset_progress;
    float mah_percent;
    float battery_percent;
    float speed_kph;
    float speed_percent;
    float trip_km;
    float trip_reset_progress;
    float total_km;
    vesc_comm_fault_code vesc_fault_code = FAULT_CODE_NONE;
} t_davega_data;

#endif //DAVEGA_DATA_H
