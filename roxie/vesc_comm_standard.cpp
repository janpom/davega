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

#include "data.h"
#include "vesc_comm_standard.h"
#include "roxie_config.h"
#include "util.h"

VescCommStandard::VescCommStandard() {
    _max_packet_length = 78;
    #ifndef SIM_VALUES
        _packet = (uint8_t*) malloc(_max_packet_length * sizeof(*_packet));
    #endif
}

void VescCommStandard::process_packet(t_data* data){
    data->mosfet_celsius = get_temp_mosfet();
    data->motor_celsius = get_temp_motor();
    data->motor_amps = get_motor_current();
    data->battery_amps = get_battery_current() * VESC_COUNT;
    data->duty_cycle = get_duty_cycle();
    data->vesc_fault_code = get_fault_code();
    data->voltage = get_voltage();
    data->wh_spent = get_watthours_discharged();
    data->mah_charged = get_amphours_charged();
    data->mah_discharged = get_amphours_discharged();
    data->tachometer = get_tachometer();
    data->rpm = get_rpm();
    data->speed_kph = max(erpm_to_kph(data->rpm), float(0));
    data->voltage_percent = voltage_to_percent(data->voltage);
    data->battery_percent = VOLTAGE_WEIGHT * data->voltage_percent;
}

float VescCommStandard::get_temp_mosfet() {
    return ((int16_t) get_word(3)) / 10.0;
}

float VescCommStandard::get_temp_motor() {
    return ((int16_t) get_word(5)) / 10.0;
}

float VescCommStandard::get_motor_current() {
    return ((int32_t) get_long(7)) / 100.0;
}

float VescCommStandard::get_battery_current() {
    return ((int32_t) get_long(11)) / 100.0;
}

float VescCommStandard::get_duty_cycle() {
    return get_word(23) / 1000.0;
}

int32_t VescCommStandard::get_rpm() {
    return get_long(25);
}

float VescCommStandard::get_voltage() {
    return get_word(29) / 10.0;
}

float VescCommStandard::get_amphours_discharged() {
    return get_long(31) / 10000.0;
}

float VescCommStandard::get_amphours_charged() {
    return get_long(35) / 10000.0;
}

float VescCommStandard::get_watthours_discharged() {
    return get_long(39) / 10000.0;
}

float VescCommStandard::get_watthours_charged() {
    return get_long(43) / 10000.0;
}

int32_t VescCommStandard::get_tachometer() {
    return get_long(47);
}

int32_t VescCommStandard::get_tachometer_abs() {
    return get_long(51);
}

vesc_comm_fault_code VescCommStandard::get_fault_code() {
    return (vesc_comm_fault_code) _packet[55];
}
