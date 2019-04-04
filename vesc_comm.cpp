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

#include "vesc_comm.h"
#include "crc.h"

// TODO: Make vesc_serial a parameter of vesc_comm_init.
#ifdef DEBUG
#define D(x) Serial.println(x)
#include "SoftwareSerial.h"
SoftwareSerial vesc_serial(13, 14); // RX, TX
#else
#define D(x)
HardwareSerial &vesc_serial = Serial;
#endif

#define PACKET_GET_VALUES_TYPE 4
#define PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT 2
#define PACKET_LENGTH_IDENTIFICATION_BYTE_LONG 3
#define PACKET_TERMINATION_BYTE 3

uint8_t GET_VALUES_PACKET[] = {0x02, 0x01, 0x04, 0x40, 0x84, 0x03};

uint8_t expected_packet_length(uint8_t payload_length) {
    return (1 + 1 + payload_length + 2 + 1);
}

void vesc_comm_init(uint32_t baud) {
    vesc_serial.begin(baud);
}

uint8_t vesc_comm_fetch_packet(uint8_t *vesc_packet, uint16_t timeout) {
    vesc_serial.write(GET_VALUES_PACKET, sizeof(GET_VALUES_PACKET));
    return vesc_comm_receive_packet(vesc_packet, timeout);
}

uint8_t vesc_comm_receive_packet(uint8_t *vesc_packet, uint16_t timeout) {
    int32_t start = millis();
    uint8_t bytes_read = 0;
    while (millis() - start < timeout) {
        if (vesc_serial.available())
            vesc_packet[bytes_read++] = vesc_serial.read();

        if (bytes_read >= PACKET_MAX_LENGTH)
            break;

        if (bytes_read >= 2 && vesc_packet[0] != PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT) {
            uint8_t payload_length = vesc_packet[1];
            if (bytes_read >= expected_packet_length(payload_length))
                break;
        }
    }
    // read any left-over bytes without storing
    while (vesc_serial.available()) {
        // TODO: warning
        vesc_serial.read();
    }
    return bytes_read;
}

uint16_t get_word(uint8_t *packet, uint8_t index) {
    return ((uint16_t) packet[index]) << 8 | ((uint16_t) packet[index + 1]);
}

uint32_t get_long(uint8_t *packet, uint8_t index) {
    return ((uint32_t) packet[index]) << 24 |
           ((uint32_t) packet[index + 1]) << 16 |
           ((uint32_t) packet[index + 2]) << 8 |
           ((uint32_t) packet[index + 3]);
}

bool vesc_comm_is_expected_packet(uint8_t *vesc_packet, uint8_t packet_length) {
    if (packet_length < 3) {
        D("packet too short (" + String(packet_length) + " bytes)");
        return false;
    }

    if (vesc_packet[0] != PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT) {
        D("unexpected length id byte: expected " + String(PACKET_LENGTH_IDENTIFICATION_BYTE_SHORT) +
          ", got " + String(vesc_packet[0]));
        return false;
    }

    if (vesc_packet[2] != PACKET_GET_VALUES_TYPE) {
        D("unexpected packet type: expected " + String(PACKET_GET_VALUES_TYPE) +
          ", got " + String(vesc_packet[2]));
        return false;
    }

    uint8_t payload_length = vesc_packet[1];
    if (packet_length != expected_packet_length(payload_length)) {
        D("packet length (" + String(payload_length) + ") does not correspond to the payload length (" +
          String(payload_length) + ")");
        return false;
    }

    uint16_t crc = get_word(vesc_packet, payload_length + 2);
    uint16_t expected_crc = crc16(&vesc_packet[2], payload_length);
    if (crc != expected_crc) {
        D("CRC error: expected " + String(expected_crc) + ", got " + String(crc));
        return false;
    }

    return true;
}

#ifndef FOCBOX_UNITY
float vesc_comm_get_temp_mosfet(uint8_t *vesc_packet) {
    return ((int16_t) get_word(vesc_packet, 3)) / 10.0;
}

float vesc_comm_get_temp_motor(uint8_t *vesc_packet) {
    return ((int16_t) get_word(vesc_packet, 5)) / 10.0;
}

float vesc_comm_get_motor_current(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 7)) / 100.0;
}

float vesc_comm_get_battery_current(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 11)) / 100.0;
}

float vesc_comm_get_duty_cycle(uint8_t *vesc_packet) {
    return get_word(vesc_packet, 23) / 1000.0;
}

int32_t vesc_comm_get_rpm(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 25);
}

float vesc_comm_get_voltage(uint8_t *vesc_packet) {
    return get_word(vesc_packet, 29) / 10.0;
}

float vesc_comm_get_amphours_discharged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 31) / 10.0;
}

float vesc_comm_get_amphours_charged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 35) / 10.0;
}

int32_t vesc_comm_get_tachometer(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 47);
}

int32_t vesc_comm_get_tachometer_abs(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 51);
}

vesc_comm_fault_code vesc_comm_get_fault_code(uint8_t *vesc_packet) {
    return vesc_packet[55];
}
#else  // FOCBOX_UNITY
float vesc_comm_get_temp_mosfet(uint8_t *vesc_packet) {
    return (((int16_t) get_word(vesc_packet, 3)) + ((int16_t) get_word(vesc_packet, 5))) / 2.0 / 10.0;
}

float vesc_comm_get_temp_motor(uint8_t *vesc_packet) {
    return (((int16_t) get_word(vesc_packet, 7)) + ((int16_t) get_word(vesc_packet, 9))) / 2.0 / 10.0;
}

float vesc_comm_get_motor_current(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 11)) + ((int32_t) get_long(vesc_packet, 15))) / 2.0 / 100.0;
}

float vesc_comm_get_battery_current(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 19)) / 100.0;
}

float vesc_comm_get_duty_cycle(uint8_t *vesc_packet) {
    return (get_word(vesc_packet, 39) + get_word(vesc_packet, 41)) / 2.0 / 1000.0;
}

int32_t vesc_comm_get_rpm(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 43)) + ((int32_t) get_long(vesc_packet, 47))) / 2;
}

float vesc_comm_get_voltage(uint8_t *vesc_packet) {
    return get_word(vesc_packet, 51) / 10.0;
}

float vesc_comm_get_amphours_discharged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 53) / 10.0;
}

float vesc_comm_get_amphours_charged(uint8_t *vesc_packet) {
    return get_long(vesc_packet, 57) / 10.0;
}

int32_t vesc_comm_get_tachometer(uint8_t *vesc_packet) {
    return ((int32_t) get_long(vesc_packet, 69)) + ((int32_t) get_long(vesc_packet, 73)) / 2;
}

int32_t vesc_comm_get_tachometer_abs(uint8_t *vesc_packet) {
    return (((int32_t) get_long(vesc_packet, 77)) + ((int32_t) get_long(vesc_packet, 81))) / 2;
}

vesc_comm_fault_code vesc_comm_get_fault_code(uint8_t *vesc_packet) {
    return vesc_packet[85];
}
#endif // FOCBOX_UNITY
