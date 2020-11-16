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

#include "roxie_config.h"
#include "roxie_eeprom.h"
#include "util.h"
#include "screen.h"
#include "vesc_comm.h"
#include "buttons.h"
#include "simple_vertical_screen.h"

unsigned long starting_time;

#ifdef FOCBOX_UNITY
#include "vesc_comm_unity.h"
VescCommUnity vesc_comm = VescCommUnity();
#else
#include "vesc_comm_standard.h"
VescCommStandard vesc_comm = VescCommStandard();
#endif

SimpleVerticalScreen simple_vertical_screen = SimpleVerticalScreen(SCR_SPEED);

Screen* screens[] = {
    &simple_vertical_screen,
};

t_screen_config screen_config = {
    make_fw_version(FW_VERSION, REVISION_ID),
    IMPERIAL_UNITS,
    USE_FAHRENHEIT,
    SHOW_AVG_CELL_VOLTAGE,
    BATTERY_S,
    SCREEN_ORIENTATION
};

int current_screen_index = 0;
Screen* scr;

t_data data;
t_session_data session_data;
int32_t initial_mah_spent;
int32_t initial_trip_meters;
int32_t initial_total_meters;
int32_t initial_millis_elapsed;
int32_t last_millis = 0;
int32_t last_eeprom_written_total_meters;
int32_t last_eeprom_update_on_stop;

int32_t last_rpm;
uint32_t button_1_last_up_time = 0;
uint32_t button_2_last_up_time = 0;

void setup() {
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), button1_changed, CHANGE);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), button2_pressed, FALLING);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3_PIN), button3_pressed, FALLING);

    Serial.begin(115200);
    vesc_comm.init(115200);

    for (uint8_t i=0; i<LEN(screens); i++)
        screens[i]->init(&screen_config);
    scr = screens[current_screen_index];

    if(eeprom_is_initialized(EEPROM_MAGIC_VALUE)){
        session_data = eeprom_read_session_data();
    }
    else {
        eeprom_initialize(EEPROM_MAGIC_VALUE, session_data, data);
    }
    eeprom_read_data(data, session_data);

    scr->reset();
    scr->update(&data);

    vesc_comm.fetch_packet();
    while (!vesc_comm.is_expected_packet()) {
        scr->heartbeat(UPDATE_DELAY, false);
        vesc_comm.fetch_packet();
    }

    check_if_battery_charged(&data);
    get_initial_values();
}

unsigned long start;
unsigned long loop_time;
void loop() {
    int32_t tachometer = rotations_to_meters(vesc_comm.get_tachometer() / 6);
    read_buttons(session_data, &initial_trip_meters, &tachometer, scr);
    //Serial.println("trip meters is " + String(session_data.trip_meters));

    vesc_comm.fetch_packet();
    if (!vesc_comm.is_expected_packet()) {
        scr->heartbeat(UPDATE_DELAY, false);
        return;
    }
    vesc_comm.process_packet(&data);

    calculate_mah();
    read_other_values();

    start = millis();

    scr->update(&data);
    scr->heartbeat(UPDATE_DELAY, true);

    loop_time = millis() - start;
    Serial.println("Loop time is " + String(loop_time));

}


void get_initial_values(){
    initial_mah_spent = eeprom_read_mah_spent();
    initial_trip_meters = session_data.trip_meters;
    initial_total_meters = eeprom_read_total_distance();
    initial_millis_elapsed = session_data.millis_elapsed;

    last_eeprom_written_total_meters = initial_total_meters;
    last_eeprom_update_on_stop = millis();

    // Subtract current VESC values, which could be non-zero in case the display
    // got reset without resetting the VESC as well. The invariant is:
    //   current value = initial value + VESC value
    // and that works correctly with the default initial values in case the VESC values
    // start from 0. If that's not the case though we need to lower the initial values.
    int32_t vesc_mah_spent = VESC_COUNT * (data.mah_discharged - data.mah_charged);
    initial_mah_spent -= vesc_mah_spent;
    int32_t tachometer = rotations_to_meters(data.tachometer / 6);
    initial_trip_meters -= tachometer;
    initial_total_meters -= tachometer;
}

void calculate_mah(){
        // TODO: DRY
    int32_t vesc_mah_spent = VESC_COUNT * (data.mah_discharged - data.mah_charged);
    int32_t mah_spent = initial_mah_spent + vesc_mah_spent;
    int32_t mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah_spent;

    // uint32_t button_2_down_elapsed = millis() - button_2_last_up_time;
/*     if (button2_down) {
        button2_down = LOW; */
        // reset coulomb counter
        mah = voltage_to_percent(data.voltage) * BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY;
        mah_spent = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah;
        eeprom_write_mah_spent(mah_spent);
        initial_mah_spent = mah_spent - vesc_mah_spent;
    //}

    data.mah = mah;
    data.mah_spent = data.mah_discharged;
}

void read_other_values(){
        // dim mAh if the counter is about to be reset
    // data.mah_reset_progress = min(1.0 * button_2_down_elapsed / COUNTER_RESET_TIME, 1.0);

    int32_t rpm = data.rpm;
    data.speed_kph = max(erpm_to_kph(rpm), float(0));

    int32_t tachometer = rotations_to_meters(data.tachometer / 6);

    session_data.trip_meters = initial_trip_meters + tachometer;
    int32_t total_meters = initial_total_meters + tachometer;

    // dim trip distance if it's about to be reset
    // data.session_reset_progress = min(1.0 * button_1_down_elapsed / COUNTER_RESET_TIME, 1.0);

    data.trip_km = session_data.trip_meters / 1000.0;
    data.total_km = total_meters / 1000.0;

    data.speed_percent = 1.0 * data.speed_kph / MAX_SPEED_KPH;

    data.mah_percent = 1.0 * data.mah / (BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY);
    data.voltage_percent = voltage_to_percent(data.voltage);
    data.battery_percent = VOLTAGE_WEIGHT * data.voltage_percent + (1.0 - VOLTAGE_WEIGHT) * data.mah_percent;

    // extreme values
    if (data.speed_kph > session_data.max_speed_kph)
        session_data.max_speed_kph = data.speed_kph;

    if (data.voltage < session_data.min_voltage)
        session_data.min_voltage = data.voltage;

    // time elapsed
    session_data.millis_elapsed = initial_millis_elapsed + millis();

    if (rpm > 0)
        session_data.millis_riding += millis() - last_millis;
    last_millis = millis();

    // update EEPROM
    bool came_to_stop = (last_rpm != 0 && rpm == 0);
    bool traveled_enough_distance = (total_meters - last_eeprom_written_total_meters >= EEPROM_UPDATE_EACH_METERS);
    if (traveled_enough_distance || (came_to_stop && millis() - last_eeprom_update_on_stop > EEPROM_UPDATE_MIN_DELAY_ON_STOP)) {
        if (came_to_stop)
            last_eeprom_update_on_stop = millis();
        last_eeprom_written_total_meters = total_meters;
        eeprom_write_volts(data.voltage);
        eeprom_write_mah_spent(data.mah_spent);
        eeprom_write_total_distance(total_meters);
        eeprom_write_session_data(session_data);
    }

    last_rpm = rpm;
}