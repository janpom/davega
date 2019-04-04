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
    float get_temp_mosfet();
    float get_temp_motor();
    float get_motor_current();
    float get_battery_current();
    float get_duty_cycle();
    int32_t get_rpm();
    float get_voltage();
    float get_amphours_discharged();
    float get_amphours_charged();
    int32_t get_tachometer();
    int32_t get_tachometer_abs();
    vesc_comm_fault_code get_fault_code();
};

#endif //VESC_COMM_STANDARD_H
