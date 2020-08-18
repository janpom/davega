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

#include "roxie_eeprom.h"
#include "data.h"
#include <EEPROM.h>

#define EEPROM_ADDRESS_MAGIC_BYTE 0
#define EEPROM_ADDRESS_VOLTS 1
#define EEPROM_ADDRESS_MAH_SPENT 5
#define EEPROM_ADDRESS_TOTAL_DISTANCE 13
#define EEPROM_ADDRESS_SESSION_DATA 17

#define LEN(X) (sizeof(X) / sizeof(X[0]))

bool eeprom_is_initialized(uint8_t magic_value) {
    return EEPROM.read(EEPROM_ADDRESS_MAGIC_BYTE) == magic_value;
}

void eeprom_initialize(uint8_t magic_value, t_session_data session_data) {
    EEPROM.write(EEPROM_ADDRESS_MAGIC_BYTE, magic_value);
    eeprom_write_volts(EEPROM_INIT_VALUE_VOLTS);
    eeprom_write_mah_spent(EEPROM_INIT_VALUE_MAH_SPENT);
    eeprom_write_total_distance(EEPROM_INIT_VALUE_TOTAL_DISTANCE);
    session_data.max_speed_kph = EEPROM_INIT_VALUE_MAX_SPEED;
    session_data.millis_elapsed = EEPROM_INIT_VALUE_MILLIS_ELAPSED;
    session_data.millis_riding = EEPROM_INIT_VALUE_MILLIS_RIDING;
    session_data.min_voltage = EEPROM_INIT_VALUE_MIN_VOLTAGE;
    session_data.trip_meters = EEPROM_INIT_VALUE_TRIP_DISTANCE;
    eeprom_write_session_data(session_data);

    session_data = eeprom_read_session_data();
/*     data.voltage = eeprom_read_volts();
    data.mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - eeprom_read_mah_spent();
    data.trip_km = session_data.trip_meters / 1000.0;
    data.total_km = eeprom_read_total_distance() / 1000.0;
    data.session = &session_data; */

}

float eeprom_read_volts() {
    float volts;
    EEPROM.get(EEPROM_ADDRESS_VOLTS, volts);
    return volts;
}

void eeprom_write_volts(float volts) {
    EEPROM.put(EEPROM_ADDRESS_VOLTS, volts);
}

uint16_t eeprom_read_mah_spent() {
    uint16_t mah_spent;
    EEPROM.get(EEPROM_ADDRESS_MAH_SPENT, mah_spent);
    return mah_spent;
}

void eeprom_write_mah_spent(uint16_t mah_spent) {
    EEPROM.put(EEPROM_ADDRESS_MAH_SPENT, mah_spent);
}

uint32_t eeprom_read_total_distance() {
    uint32_t meters;
    EEPROM.get(EEPROM_ADDRESS_TOTAL_DISTANCE, meters);
    return meters;
}

void eeprom_write_total_distance(uint32_t meters) {
    EEPROM.put(EEPROM_ADDRESS_TOTAL_DISTANCE, meters);
}

t_session_data eeprom_read_session_data() {
    t_session_data session_data;
    EEPROM.get(EEPROM_ADDRESS_SESSION_DATA, session_data);
    return session_data;
}

void eeprom_write_session_data(t_session_data session_data) {
    EEPROM.put(EEPROM_ADDRESS_SESSION_DATA, session_data);
}
