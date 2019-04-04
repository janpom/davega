/*
    Copyright 2019 Jan Pomikalek <jan.pomikalek@gmail.com>

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

#include "vesc_comm_standard.h"

VescCommStandard::VescCommStandard() {
    _max_packet_length = 70;
    _packet = malloc(_max_packet_length * sizeof(*_packet));
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
    return get_long(31) / 10.0;
}

float VescCommStandard::get_amphours_charged() {
    return get_long(35) / 10.0;
}

int32_t VescCommStandard::get_tachometer() {
    return get_long(47);
}

int32_t VescCommStandard::get_tachometer_abs() {
    return get_long(51);
}

vesc_comm_fault_code VescCommStandard::get_fault_code() {
    return _packet[55];
}
