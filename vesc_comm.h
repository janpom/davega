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

#include <Arduino.h>

#ifndef VESC_COMM_H
#define VESC_COMM_H

typedef enum {
    FAULT_CODE_NONE = 0,
    FAULT_CODE_OVER_VOLTAGE,
    FAULT_CODE_UNDER_VOLTAGE,
    FAULT_CODE_DRV,
    FAULT_CODE_ABS_OVER_CURRENT,
    FAULT_CODE_OVER_TEMP_FET,
    FAULT_CODE_OVER_TEMP_MOTOR
} vesc_comm_fault_code;

class VescComm {
public:
    ~VescComm();
    void init(uint32_t baud);
    uint8_t fetch_packet(uint16_t timeout = 100);
    uint8_t receive_packet(uint16_t timeout);
    bool is_expected_packet();
    virtual float get_temp_mosfet() = 0;
    virtual float get_temp_motor() = 0;
    virtual float get_motor_current() = 0;
    virtual float get_battery_current() = 0;
    virtual float get_duty_cycle() = 0;
    virtual int32_t get_rpm() = 0;
    virtual float get_voltage() = 0;
    virtual float get_amphours_discharged() = 0;
    virtual float get_amphours_charged() = 0;
    virtual int32_t get_tachometer() = 0;
    virtual int32_t get_tachometer_abs() = 0;
    virtual vesc_comm_fault_code get_fault_code() = 0;

protected:
    uint16_t get_word(uint8_t index);
    uint32_t get_long(uint8_t index);
    uint8_t *_packet = NULL;
    uint8_t _max_packet_length;
    uint8_t _bytes_read;

};

#endif //VESC_COMM_H
