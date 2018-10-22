/*
    Copyright 2018 Jan Pomikalek <jan.pomikalek@gmail.com>

    This file is part of the VeGa firmware.

    VeGa firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VeGa firmware.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Arduino.h>

#ifndef VESC_COMM_H
#define VESC_COMM_H

#define PACKET_MAX_LENGTH 64

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

#endif //VESC_COMM_H
