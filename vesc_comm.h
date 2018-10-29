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

#include <Arduino.h>

#ifndef VESC_COMM_H
#define VESC_COMM_H

#define PACKET_MAX_LENGTH 64

typedef enum {
    FAULT_CODE_NONE = 0,
    FAULT_CODE_OVER_VOLTAGE,
    FAULT_CODE_UNDER_VOLTAGE,
    FAULT_CODE_DRV,
    FAULT_CODE_ABS_OVER_CURRENT,
    FAULT_CODE_OVER_TEMP_FET,
    FAULT_CODE_OVER_TEMP_MOTOR
} vesc_comm_fault_code;

void vesc_comm_init(uint32_t baud);
uint8_t vesc_comm_fetch_packet(uint8_t *vesc_packet);
uint8_t vesc_comm_receive_packet(uint8_t *vesc_packet);
bool vesc_comm_is_expected_packet(uint8_t *vesc_packet, uint8_t packet_length);
int32_t vesc_comm_get_rpm(uint8_t *vesc_packet);
float vesc_comm_get_voltage(uint8_t *vesc_packet);
float vesc_comm_get_amphours_discharged(uint8_t *vesc_packet);
float vesc_comm_get_amphours_charged(uint8_t *vesc_packet);
int32_t vesc_comm_get_tachometer(uint8_t *vesc_packet);
int32_t vesc_comm_get_tachometer_abs(uint8_t *vesc_packet);
vesc_comm_fault_code vesc_comm_get_fault_code(uint8_t *vesc_packet);

#endif //VESC_COMM_H
