/*
    This file is part of the Roxie firmware.

    Roxie firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Roxie firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Roxie firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UTIL_H
#define UTIL_H

#include "vesc_comm.h"

#define KM_PER_MILE 0.621371

const char* make_fw_version(const char* fw_version, const char* revision_id);
float convert_distance(float distance_km, bool imperial_units);
float convert_speed(float speed_kph, bool imperial_units);
float convert_temperature(float temp_celsius, bool imperial_units);
void format_total_distance(float total_distance, char* result);
const char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code);
int32_t rotations_to_meters(int32_t rotations);
float erpm_to_kph(uint32_t erpm);
float voltage_to_percent(float voltage);
bool was_battery_charged(float last_volts, float current_volts);
bool is_battery_full(float current_volts);


#endif //UTIL_H
