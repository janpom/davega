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
#include "data.h"
#include "screen.h"

#define KM_PER_MILE 0.621371


uint16_t item_color(float speed_kph);

const char* make_fw_version(const char* fw_version, const char* revision_id);

/**
 * @brief Convert kilometers to miles if this is set in the configuration
 * 
 * @param kilometer
 * @param imperial_units 
 * @return float 
 */
float convert_km_to_miles(float kilometer, bool imperial_units);

float convert_temperature(float temp_celsius, bool imperial_units);
void format_total_distance(float total_distance, char* result);
const char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code);
int32_t rotations_to_meters(int32_t rotations);
float erpm_to_kph(uint32_t erpm);
float voltage_to_percent(float voltage);
void check_if_battery_charged(t_data* data);
bool was_battery_charged(float last_volts, float current_volts);
bool is_battery_full(float current_volts);


#endif //UTIL_H
