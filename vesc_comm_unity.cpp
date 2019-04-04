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

#include "vesc_comm_unity.h"

VescCommUnity::VescCommUnity() {
    _max_packet_length = 89;
    _packet = malloc(_max_packet_length * sizeof(*_packet));
}

float VescCommUnity::get_temp_mosfet() {
    return (((int16_t) get_word(3)) + ((int16_t) get_word(5))) / 2.0 / 10.0;
}

float VescCommUnity::get_temp_motor() {
    return (((int16_t) get_word(7)) + ((int16_t) get_word(9))) / 2.0 / 10.0;
}

float VescCommUnity::get_motor_current() {
    return (((int32_t) get_long(11)) + ((int32_t) get_long(15))) / 2.0 / 100.0;
}

float VescCommUnity::get_battery_current() {
    return ((int32_t) get_long(19)) / 100.0;
}

float VescCommUnity::get_duty_cycle() {
    return (get_word(39) + get_word(41)) / 2.0 / 1000.0;
}

int32_t VescCommUnity::get_rpm() {
    return (((int32_t) get_long(43)) + ((int32_t) get_long(47))) / 2;
}

float VescCommUnity::get_voltage() {
    return get_word(51) / 10.0;
}

float VescCommUnity::get_amphours_discharged() {
    return get_long(53) / 10.0;
}

float VescCommUnity::get_amphours_charged() {
    return get_long(57) / 10.0;
}

int32_t VescCommUnity::get_tachometer() {
    return ((int32_t) get_long(69)) + ((int32_t) get_long(73)) / 2;
}

int32_t VescCommUnity::get_tachometer_abs() {
    return (((int32_t) get_long(77)) + ((int32_t) get_long(81))) / 2;
}

vesc_comm_fault_code VescCommUnity::get_fault_code() {
    return _packet[85];
}
