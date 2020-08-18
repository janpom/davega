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

#include "util.h"
#include "vesc_comm.h"

#define LEN(X) (sizeof(X) / sizeof(X[0]))
const float discharge_ticks[] = DISCHARGE_TICKS;

char fw_version_buffer[6];

const char* make_fw_version(const char* fw_version, const char* revision_id) {
    if (fw_version[0] == 'v') {
        return fw_version;
    }
    else {
        String r = String("r");
        String upper_rev_id = String(revision_id).substring(5, 9);
        upper_rev_id.toUpperCase();
        r.concat(upper_rev_id);
        r.toCharArray(fw_version_buffer, sizeof(fw_version_buffer));
        return fw_version_buffer;
    }
}

float convert_distance(float distance_km, bool imperial_units)
{
    if (imperial_units)
        return distance_km * KM_PER_MILE;
    else
        return distance_km;
}

float convert_speed(float speed_kph, bool imperial_units)
{
    return convert_distance(speed_kph, imperial_units);
}

float convert_temperature(float temp_celsius, bool imperial_units) {
    if (imperial_units)
        return temp_celsius * 9.0 / 5.0 + 32;
    else
        return temp_celsius;
}

void format_total_distance(float total_distance, char* result) {
    if (total_distance >= 1000)
        dtostrf(((int) round(total_distance)) % 10000, 4, 0, result);
    else
        dtostrf(total_distance, 5, 1, result);
}

const char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code) {
    switch (fault_code) {
        case FAULT_CODE_NONE:
            return "FAULT CODE NONE";
        case FAULT_CODE_OVER_VOLTAGE:
            return "OVER VOLTAGE";
        case FAULT_CODE_UNDER_VOLTAGE:
            return "UNDER VOLTAGE";
        case FAULT_CODE_DRV:
            return "DRV FAULT";
        case FAULT_CODE_ABS_OVER_CURRENT:
            return "OVER CURRENT";
        case FAULT_CODE_OVER_TEMP_FET:
            return "OVER TEMP FET";
        case FAULT_CODE_OVER_TEMP_MOTOR:
            return "OVER TEMP MOTOR";
        default:
            return "unexpected fault code";
    }
}

int32_t rotations_to_meters(int32_t rotations) {
    float gear_ratio = float(WHEEL_PULLEY_TEETH) / float(MOTOR_PULLEY_TEETH);
    return (rotations / MOTOR_POLE_PAIRS / gear_ratio) * WHEEL_DIAMETER_MM * PI / 1000;
}

float erpm_to_kph(uint32_t erpm) {
    float km_per_minute = rotations_to_meters(erpm) / 1000.0;
    return km_per_minute * 60.0;
}

float voltage_to_percent(float voltage) {
    if (voltage < discharge_ticks[0])
        return 0.0;
    for (int i = 1; i < LEN(discharge_ticks); i++) {
        float cur_voltage = discharge_ticks[i];
        if (voltage < cur_voltage) {
            float prev_voltage = discharge_ticks[i - 1];
            float interval_perc = (voltage - prev_voltage) / (cur_voltage - prev_voltage);
            float low = 1.0 * (i - 1) / (LEN(discharge_ticks) - 1);
            float high = 1.0 * i / (LEN(discharge_ticks) - 1);
            return low + (high - low) * interval_perc;
        }
    }
    return 1.0;
}

bool was_battery_charged(float last_volts, float current_volts) {
    return (current_volts - last_volts) / current_volts > FULL_CHARGE_MIN_INCREASE;
}

bool is_battery_full(float current_volts) {
    float max_volts = discharge_ticks[LEN(discharge_ticks) - 1];
    return current_volts / max_volts > FULL_CHARGE_THRESHOLD;
}
