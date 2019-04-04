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

#include "vesc_comm.h"

#ifndef VESC_COMM_STANDARD_H
#define VESC_COMM_STANDARD_H

class VescCommStandard: public VescComm {
public:
    VescCommStandard();
    float get_temp_mosfet(uint8_t *vesc_packet);
    float get_temp_motor(uint8_t *vesc_packet);
    float get_motor_current(uint8_t *vesc_packet);
    float get_battery_current(uint8_t *vesc_packet);
    float get_duty_cycle(uint8_t *vesc_packet);
    int32_t get_rpm(uint8_t *vesc_packet);
    float get_voltage(uint8_t *vesc_packet);
    float get_amphours_discharged(uint8_t *vesc_packet);
    float get_amphours_charged(uint8_t *vesc_packet);
    int32_t get_tachometer(uint8_t *vesc_packet);
    int32_t get_tachometer_abs(uint8_t *vesc_packet);
    vesc_comm_fault_code get_fault_code(uint8_t *vesc_packet);
};

#endif //VESC_COMM_STANDARD_H
