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
}

float VescCommUnity::get_temp_mosfet(uint8_t *vesc_packet) {
    return (((int16_t) get_word(vesc_packet, 3)) + ((int16_t) get_word(vesc_packet, 5))) / 2.0 / 10.0;
}

float VescCommUnity::get_temp_motor(uint8_t *vesc_packet) {
    return (((int16_t) get_word(vesc_packet, 7)) + ((int16_t) get_word(vesc_packet, 9))) / 2.0 / 10.0;
}

float VescCommUnity::get_motor_current(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 11)) + ((int32_t) get_long(vesc_packet, 15))) / 2.0 / 100.0;
}

float VescCommUnity::get_battery_current(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 19)) / 100.0;
}

float VescCommUnity::get_duty_cycle(uint8_t *vesc_packet) {
    return (get_word(vesc_packet, 39) + get_word(vesc_packet, 41)) / 2.0 / 1000.0;
}

int32_t VescCommUnity::get_rpm(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 43)) + ((int32_t) get_long(vesc_packet, 47))) / 2;
}

float VescCommUnity::get_voltage(uint8_t *vesc_packet) {
    return get_word(vesc_packet, 51) / 10.0;
}

float VescCommUnity::get_amphours_discharged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 53) / 10.0;
}

float VescCommUnity::get_amphours_charged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 57) / 10.0;
}

int32_t VescCommUnity::get_tachometer(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 69)) + ((int32_t) get_long(vesc_packet, 73)) / 2;
}

int32_t VescCommUnity::get_tachometer_abs(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 77)) + ((int32_t) get_long(vesc_packet, 81))) / 2;
}

vesc_comm_fault_code VescCommUnity::get_fault_code(uint8_t *vesc_packet) {
    return vesc_packet[85];
}
