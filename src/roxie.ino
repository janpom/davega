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
#include "eeprom.h"
#include "data.h"
#include "util.h"
#include "screen.h"
#include "vesc_comm.h"

#define REVISION_ID ""
#define FW_VERSION "v1.0"

//#define DEBUG
#ifdef DEBUG
#define D(x) Serial.println(x)
#else
#define D(x)
#endif

#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 3
#define BUTTON_3_PIN 4

#define LEN(X) (sizeof(X) / sizeof(X[0]))

#ifdef FOCBOX_UNITY
#include "vesc_comm_unity.h"
VescCommUnity vesc_comm = VescCommUnity();
#else
#include "vesc_comm_standard.h"
VescCommStandard vesc_comm = VescCommStandard();
#endif

#ifdef DEFAULT_SCREEN_ENABLED
#include "default_screen.h"
DefaultScreen default_screen = DefaultScreen();
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_ENABLED
#include "simple_horizontal_screen.h"
SimpleHorizontalScreen simple_horizontal_screen = SimpleHorizontalScreen(SCR_SPEED);
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED
#include "simple_horizontal_screen.h"
SimpleHorizontalScreen simple_horizontal_screen_with_battery_current = SimpleHorizontalScreen(SCR_BATTERY_CURRENT);
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED
#include "simple_horizontal_screen.h"
SimpleHorizontalScreen _simple_horizontal_screen_with_motor_current = SimpleHorizontalScreen(SCR_MOTOR_CURRENT);
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_ENABLED
#include "simple_vertical_screen.h"
SimpleVerticalScreen simple_vertical_screen = SimpleVerticalScreen(SCR_SPEED);
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED
#include "simple_vertical_screen.h"
SimpleVerticalScreen simple_vertical_screen_with_battery_current = SimpleVerticalScreen(SCR_BATTERY_CURRENT);
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED
#include "simple_vertical_screen.h"
SimpleVerticalScreen simple_vertical_screen_with_motor_current = SimpleVerticalScreen(SCR_MOTOR_CURRENT);
#endif
#ifdef TEXT_SCREEN_ENABLED
#include "text_screen.h"
TextScreen text_screen = TextScreen();
#endif

Screen* screens[] = {
#ifdef DEFAULT_SCREEN_ENABLED
    &default_screen,
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_ENABLED
    &simple_horizontal_screen,
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED
    &simple_horizontal_screen_with_battery_current,
#endif
#ifdef SIMPLE_HORIZONTAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED
    &simple_horizontal_screen_with_motor_current,
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_ENABLED
    &simple_vertical_screen,
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_WITH_BATTERY_CURRENT_ENABLED
    &simple_vertical_screen_with_battery_current,
#endif
#ifdef SIMPLE_VERTICAL_SCREEN_WITH_MOTOR_CURRENT_ENABLED
    &simple_vertical_screen_with_motor_current,
#endif
#ifdef TEXT_SCREEN_ENABLED
    &text_screen,
#endif
};

t_screen_item text_screen_items[] = TEXT_SCREEN_ITEMS;

t_screen_config screen_config = {
    make_fw_version(FW_VERSION, REVISION_ID),
    IMPERIAL_UNITS,
    USE_FAHRENHEIT,
    SHOW_AVG_CELL_VOLTAGE,
    BATTERY_S,
    TEXT_SCREEN_BIG_FONT,
    text_screen_items,
    LEN(text_screen_items),
    SCREEN_ORIENTATION
};

int current_screen_index = 0;
Screen* scr;

const float discharge_ticks[] = DISCHARGE_TICKS;

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

int32_t rotations_to_meters(int32_t rotations) {
    float gear_ratio = float(WHEEL_PULLEY_TEETH) / float(MOTOR_PULLEY_TEETH);
    return (rotations / MOTOR_POLE_PAIRS / gear_ratio) * WHEEL_DIAMETER_MM * PI / 1000;
}

float erpm_to_kph(uint32_t erpm) {
    float km_per_minute = rotations_to_meters(erpm) / 1000.0;
    return km_per_minute * 60.0;
}

float voltage_to_percent(float voltage) {
    if (voltage < discharge_ticks[0])
        return 0.0;
    for (int i = 1; i < LEN(discharge_ticks); i++) {
        float cur_voltage = discharge_ticks[i];
        if (voltage < cur_voltage) {
            float prev_voltage = discharge_ticks[i - 1];
            float interval_perc = (voltage - prev_voltage) / (cur_voltage - prev_voltage);
            float low = 1.0 * (i - 1) / (LEN(discharge_ticks) - 1);
            float high = 1.0 * i / (LEN(discharge_ticks) - 1);
            return low + (high - low) * interval_perc;
        }
    }
    return 1.0;
}

bool was_battery_charged(float last_volts, float current_volts) {
    return (current_volts - last_volts) / current_volts > FULL_CHARGE_MIN_INCREASE;
}

bool is_battery_full(float current_volts) {
    float max_volts = discharge_ticks[LEN(discharge_ticks) - 1];
    return current_volts / max_volts > FULL_CHARGE_THRESHOLD;
}

void setup() {
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);

#ifdef DEBUG
    Serial.begin(115200);
#endif
    vesc_comm.init(115200);

    if (!eeprom_is_initialized(EEPROM_MAGIC_VALUE)) {
        eeprom_initialize(EEPROM_MAGIC_VALUE);
        eeprom_write_volts(EEPROM_INIT_VALUE_VOLTS);
        eeprom_write_mah_spent(EEPROM_INIT_VALUE_MAH_SPENT);
        eeprom_write_total_distance(EEPROM_INIT_VALUE_TOTAL_DISTANCE);
        session_data.max_speed_kph = EEPROM_INIT_VALUE_MAX_SPEED;
        session_data.millis_elapsed = EEPROM_INIT_VALUE_MILLIS_ELAPSED;
        session_data.millis_riding = EEPROM_INIT_VALUE_MILLIS_RIDING;
        session_data.min_voltage = EEPROM_INIT_VALUE_MIN_VOLTAGE;
        session_data.trip_meters = EEPROM_INIT_VALUE_TRIP_DISTANCE;
        eeprom_write_session_data(session_data);
    }

    for (int i=0; i<LEN(screens); i++)
        screens[i]->init(&screen_config);
    scr = screens[current_screen_index];

    session_data = eeprom_read_session_data();
    data.voltage = eeprom_read_volts();
    data.mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - eeprom_read_mah_spent();
    data.trip_km = session_data.trip_meters / 1000.0;
    data.total_km = eeprom_read_total_distance() / 1000.0;
    data.session = &session_data;

    scr->reset();
    scr->update(&data);

    vesc_comm.fetch_packet();
    while (!vesc_comm.is_expected_packet()) {
        scr->heartbeat(UPDATE_DELAY, false);
        vesc_comm.fetch_packet();
    }

    float last_volts = eeprom_read_volts();
    float current_volts = vesc_comm.get_voltage();
    if (was_battery_charged(last_volts, current_volts)) {
        // reset mAh spent
        if (is_battery_full(current_volts)) {
            eeprom_write_mah_spent(0);
        }
        else {
            float soc = voltage_to_percent(current_volts);
            uint16_t mah_spent = (uint16_t) (BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY * (1 - soc));
            eeprom_write_mah_spent(mah_spent);
        }
        eeprom_write_volts(current_volts);
    }

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
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm.get_amphours_discharged() -
                                           vesc_comm.get_amphours_charged());
    initial_mah_spent -= vesc_mah_spent;
    int32_t tachometer = rotations_to_meters(vesc_comm.get_tachometer() / 6);
    initial_trip_meters -= tachometer;
    initial_total_meters -= tachometer;
}

void loop() {
    if (digitalRead(BUTTON_3_PIN) == LOW) {
        current_screen_index = (current_screen_index + 1) % LEN(screens);
        scr = screens[current_screen_index];
        scr->reset();
        delay(UPDATE_DELAY);
    }

    if (digitalRead(BUTTON_1_PIN) == HIGH)
        button_1_last_up_time = millis();

    if (digitalRead(BUTTON_2_PIN) == HIGH)
        button_2_last_up_time = millis();

    vesc_comm.fetch_packet();

    if (!vesc_comm.is_expected_packet()) {
        scr->heartbeat(UPDATE_DELAY, false);
        return;
    }

    data.mosfet_celsius = vesc_comm.get_temp_mosfet();
    data.motor_celsius = vesc_comm.get_temp_motor();
    data.motor_amps = vesc_comm.get_motor_current();
    data.battery_amps = vesc_comm.get_battery_current() * VESC_COUNT;
    data.duty_cycle = vesc_comm.get_duty_cycle();
    data.vesc_fault_code = vesc_comm.get_fault_code();
    data.voltage = vesc_comm.get_voltage();

    // TODO: DRY
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm.get_amphours_discharged() -
                                           vesc_comm.get_amphours_charged());
    int32_t mah_spent = initial_mah_spent + vesc_mah_spent;
    int32_t mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah_spent;

    uint32_t button_2_down_elapsed = millis() - button_2_last_up_time;
    if (button_2_down_elapsed > COUNTER_RESET_TIME) {
        // reset coulomb counter
        mah = voltage_to_percent(data.voltage) * BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY;
        mah_spent = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah;
        eeprom_write_mah_spent(mah_spent);
        initial_mah_spent = mah_spent - vesc_mah_spent;
    }

    data.mah = mah;

    // dim mAh if the counter is about to be reset
    data.mah_reset_progress = min(1.0 * button_2_down_elapsed / COUNTER_RESET_TIME, 1.0);

    int32_t rpm = vesc_comm.get_rpm();
    data.speed_kph = max(erpm_to_kph(rpm), 0);

    int32_t tachometer = rotations_to_meters(vesc_comm.get_tachometer() / 6);

    uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if (button_1_down_elapsed > COUNTER_RESET_TIME) {
        // reset session
        session_data.trip_meters = 0;
        session_data.max_speed_kph = 0;
        session_data.millis_elapsed = 0;
        session_data.millis_riding = 0;
        session_data.min_voltage = data.voltage;
        eeprom_write_session_data(session_data);
        initial_trip_meters = -tachometer;
        initial_millis_elapsed = -millis();
    }

    session_data.trip_meters = initial_trip_meters + tachometer;
    int32_t total_meters = initial_total_meters + tachometer;

    // dim trip distance if it's about to be reset
    data.session_reset_progress = min(1.0 * button_1_down_elapsed / COUNTER_RESET_TIME, 1.0);

    data.trip_km = session_data.trip_meters / 1000.0;
    data.total_km = total_meters / 1000.0;

    data.speed_percent = 1.0 * data.speed_kph / MAX_SPEED_KPH;

    data.mah_percent = 1.0 * mah / (BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY);
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
        eeprom_write_mah_spent(mah_spent);
        eeprom_write_total_distance(total_meters);
        eeprom_write_session_data(session_data);
    }

    last_rpm = rpm;

    scr->update(&data);
    scr->heartbeat(UPDATE_DELAY, true);
}