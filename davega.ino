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

#include "davega_display.h"
#include "davega_eeprom.h"
#include "vesc_comm.h"

//*********************************** <CONFIG> ***********************************//
#define VESC_COUNT 2  // number of controllers: 1 = single, 2 = dual
#define MOTOR_POLE_PAIRS 7
#define WHEEL_CIRCUMFERENCE_MM 604
#define GEAR_RATIO 4.8

// Affects the speed indicator. If MAX_SPEED_KPH is exceeded, no major disaster will happen.
// The speed indicator will merely indicate the current speed as the max speed (all blue rectangles
// filled). It will still show the correct number though.
#define MAX_SPEED_KPH 50

// Set to true to display the distance in miles and the speed in mph.
#define IMPERIAL_UNITS false

#define BATTERY_S 12  // number of battery cells
#define BATTERY_MAX_MAH 8000  // battery capacity in mAh
#define BATTERY_USABLE_CAPACITY 0.8  // [0.0, 1.0]

// The two options below are for detecting that battery has been fully charged (in which case
// we reset the available mAh to the max value). We think that the battery has been fully charged
// if the voltage has increased at least a little bit since the last remembered state AND the
// battery voltage is close to the max value (which is the last value in DISCHARGE_TICKS, see below).

// How much the battery voltage must increase since the last remembered state so that
// so that we think the battery has been charged.
#define FULL_CHARGE_MIN_INCREASE 0.01  // [0.0, 1.0]

// Minimal percentage of the max battery voltage to consider the battery fully charged.
// Example: With 0.97, if the max voltage is 50.4 V, then 48.888 V or more is considered fully charged.
#define FULL_CHARGE_THRESHOLD 0.97  // [0.0, 1.0]

// The DISCHARGE_TICKS can be used for configuring the battery discharge profile. The first value in the
// array is the voltage for fully discharged and the last value is the voltage for fully charged. The
// array may contain any number of values representing equidistant points of the discharge curve. For
// example, with 5 values, the voltages will be interpreted as 0%, 25%, 50%, 75%, and 100% of the battery
// capacity respectively.
const float DISCHARGE_TICKS[] = {
    3.5 * BATTERY_S,
    3.67 * BATTERY_S,
    3.73 * BATTERY_S,
    3.76 * BATTERY_S,
    3.80 * BATTERY_S,
    3.83 * BATTERY_S,
    3.86 * BATTERY_S,
    3.90 * BATTERY_S,
    4.0 * BATTERY_S,
    4.2 * BATTERY_S,
};

// Remaining battery capacity can be estimated from the battery voltage
// and from the VESC coulomb counter (number of mAh spent). The VOLTAGE_WEIGHT is
// the weight of the battery voltage in the calculation. (1.0 - VOLTAGE_WEIGHT) is
// then the weight of the coulomb counter.
//
// In particular,
// 1.0 = only use battery voltage for estimating remaining capacity.
// 0.0 = only use coulomb counter for estimating remaining capacity.
#define VOLTAGE_WEIGHT 0.4

// Changing the EEPROM_MAGIC_VALUE (to any value different from the current, e.g. 42 -> 43) will reset
// the EEPROM to the values defined below. This is especially handy for pre-setting the total distance
// traveled (EEPROM_INIT_VALUE_TOTAL_DISTANCE).
#define EEPROM_MAGIC_VALUE 42  // [1, 255]

#define EEPROM_INIT_VALUE_VOLTS 0
#define EEPROM_INIT_VALUE_MAH_SPENT 0
#define EEPROM_INIT_VALUE_TRIP_DISTANCE 0  // meters
#define EEPROM_INIT_VALUE_TOTAL_DISTANCE 0  // meters

// After how many meters traveled should the distance (and other values) be stored to EEPROM.
// The EEPROM lasts for 100,000 write cycles. With EEPROM_UPDATE_EACH_METERS=100, the EEPROM
// should last for 10,000 km. If the value is set lower, the EEPROM will die earlier.
// Note that EEPROM is also updated whenever the board comes to a stop (see below), so regardless
// of how EEPROM_UPDATE_EACH_METERS is set, there won't be missed meters unless DAVEga is accidentally
// reset before saving to EEPROM (which shouldn't happen under normal circumstances).
#define EEPROM_UPDATE_EACH_METERS 100

// If the board comes to stop, update EEPROM, unless it was already updated in less than
// EEPROM_UPDATE_MIN_DELAY_ON_STOP millis when the board stopped. This shouldn't happen
// under normal circumstance and is mainly a safeguard against destroying EEPROM in case
// unexpected oscillating speed readings are retrieved from the VESC.
//
// The EEPROM will still be updated if it was previously updated in less than
// EEPROM_UPDATE_MIN_DELAY_ON_STOP due to EEPROM_UPDATE_EACH_METERS. (Otherwise, meters would be missed.)
#define EEPROM_UPDATE_MIN_DELAY_ON_STOP 10000

// Hold button 1 for this time to reset trip distance.
#define TRIP_RESET_TIME 3000  // ms

// This corresponds (more or less) to how often data is read from VESC.
#define UPDATE_DELAY 50  // ms
//*********************************** </CONFIG> ***********************************//

#define REVISION_ID "$Id$"
#define FW_VERSION "v0.1"

//#define DEBUG
#ifdef DEBUG
#define D(x) Serial.println(x)
#else
#define D(x)
#endif

#define BUTTON_1_PIN A3
#define LEN(X) (sizeof(X) / sizeof(X[0]))

uint8_t vesc_packet[PACKET_MAX_LENGTH];

int16_t initial_mah_spent;
int32_t initial_trip_distance;
int32_t initial_total_distance;
int32_t last_eeprom_written_total_distance;
int32_t last_eeprom_update_on_stop;

int32_t last_rpm;
vesc_comm_fault_code last_fault_code;
uint32_t button_1_last_up_time = 0;

int32_t rotations_to_meters(int32_t rotations) {
    return (rotations / MOTOR_POLE_PAIRS / GEAR_RATIO) * WHEEL_CIRCUMFERENCE_MM / 1000;
}

float erpm_to_kph(uint32_t erpm) {
    float km_per_minute = rotations_to_meters(erpm) / 1000.0;
    return km_per_minute * 60.0;
}

float voltage_to_percent(float voltage) {
    if (voltage < DISCHARGE_TICKS[0])
        return 0.0;
    for (int i = 1; i < LEN(DISCHARGE_TICKS); i++) {
        float cur_voltage = DISCHARGE_TICKS[i];
        if (voltage < cur_voltage) {
            float prev_voltage = DISCHARGE_TICKS[i - 1];
            float interval_perc = (voltage - prev_voltage) / (cur_voltage - prev_voltage);
            float low = 1.0 * (i - 1) / (LEN(DISCHARGE_TICKS) - 1);
            float high = 1.0 * i / (LEN(DISCHARGE_TICKS) - 1);
            return low + (high - low) * interval_perc;
        }
    }
    return 1.0;
}

bool was_battery_fully_charged(float last_volts, float current_volts) {
    float max_volts = DISCHARGE_TICKS[LEN(DISCHARGE_TICKS) - 1];
    return (
            ((current_volts - last_volts) / current_volts > FULL_CHARGE_MIN_INCREASE) &&
            (current_volts / max_volts > FULL_CHARGE_THRESHOLD)
    );
}

char* vesc_fault_code_to_string(vesc_comm_fault_code fault_code) {
    switch (fault_code) {
        case FAULT_CODE_NONE:
            return "";
        case FAULT_CODE_OVER_VOLTAGE:
            return "OVER VOLTAGE";
        case FAULT_CODE_UNDER_VOLTAGE:
            return "UNDER VOLTAGE";
        case FAULT_CODE_DRV:
            return "DRV FAULT";
        case FAULT_CODE_ABS_OVER_CURRENT:
            return "OVER CURRENT";
        case FAULT_CODE_OVER_TEMP_FET:
            return "OVER TEMP FET";
        case FAULT_CODE_OVER_TEMP_MOTOR:
            return "OVER TEMP MOTOR";
        default:
            return "unexpected fault code";
    }
}

void setup() {
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);

#ifdef DEBUG
    Serial.begin(115200);
#endif
    vesc_comm_init(115200);

    if (!eeprom_is_initialized(EEPROM_MAGIC_VALUE)) {
        eeprom_initialize(EEPROM_MAGIC_VALUE);
        eeprom_write_volts(EEPROM_INIT_VALUE_VOLTS);
        eeprom_write_mah_spent(EEPROM_INIT_VALUE_MAH_SPENT);
        eeprom_write_trip_distance(EEPROM_INIT_VALUE_TRIP_DISTANCE);
        eeprom_write_total_distance(EEPROM_INIT_VALUE_TOTAL_DISTANCE);
    }

    display_init();
    display_set_imperial(IMPERIAL_UNITS);
    display_draw_labels();
    display_set_fw_version(FW_VERSION);
    display_update_battery_indicator(0.0, true);
    display_update_speed_indicator(0.0, true);
    display_set_volts(eeprom_read_volts());
    display_set_mah(BATTERY_MAX_MAH - eeprom_read_mah_spent());
    display_set_trip_distance(eeprom_read_trip_distance());
    display_set_total_distance(eeprom_read_total_distance());
    display_set_speed(0);

    uint8_t bytes_read = vesc_comm_fetch_packet(vesc_packet);
    while (!vesc_comm_is_expected_packet(vesc_packet, bytes_read)) {
        display_indicate_read_failure(UPDATE_DELAY);  // some delay between requests is necessary
        bytes_read = vesc_comm_fetch_packet(vesc_packet);
    }

    float last_volts = eeprom_read_volts();
    float current_volts = vesc_comm_get_voltage(vesc_packet);
    if (was_battery_fully_charged(last_volts, current_volts)) {
        // reset mAh spent
        eeprom_write_mah_spent(0);
    }

    // TODO: What to do with mAh spent if battery was only partially charged?

    initial_mah_spent = eeprom_read_mah_spent();
    initial_trip_distance = eeprom_read_trip_distance();
    initial_total_distance = eeprom_read_total_distance();

    last_eeprom_written_total_distance = initial_total_distance;
    last_eeprom_update_on_stop = millis();

    // Subtract current VESC values, which could be non-zero in case the display
    // got reset without resetting the VESC as well. The invariant is:
    //   current value = initial value + VESC value
    // and that works correctly with the default initial values in case the VESC values
    // start from 0. If that's not the case though we need to lower the initial values.
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm_get_amphours_discharged(vesc_packet) -
                                           vesc_comm_get_amphours_charged(vesc_packet));
    initial_mah_spent -= vesc_mah_spent;
    int32_t tachometer = rotations_to_meters(vesc_comm_get_tachometer(vesc_packet) / 6);
    initial_trip_distance -= tachometer;
    initial_total_distance -= tachometer;
}

void loop() {
    if (digitalRead(BUTTON_1_PIN) == HIGH)
        button_1_last_up_time = millis();

    uint8_t bytes_read = vesc_comm_fetch_packet(vesc_packet);

    if (!vesc_comm_is_expected_packet(vesc_packet, bytes_read)) {
        display_indicate_read_failure(UPDATE_DELAY);  // some delay between requests is necessary
        return;
    }

    vesc_comm_fault_code fault_code = vesc_comm_get_fault_code(vesc_packet);
    bool should_redraw = (fault_code == FAULT_CODE_NONE && last_fault_code != FAULT_CODE_NONE);

    if (should_redraw) {
        display_reset();
        display_draw_labels();
        display_set_fw_version(FW_VERSION);
    }

    float volts = vesc_comm_get_voltage(vesc_packet);
    display_set_volts(volts);

    // TODO: DRY
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm_get_amphours_discharged(vesc_packet) -
                                           vesc_comm_get_amphours_charged(vesc_packet));
    int32_t mah_spent = initial_mah_spent + vesc_mah_spent;
    int32_t mah = BATTERY_MAX_MAH - mah_spent;
    display_set_mah(mah);

    int32_t rpm = vesc_comm_get_rpm(vesc_packet);
    float kph = max(erpm_to_kph(rpm), 0);
    display_set_speed((int) (kph + 0.5));

    int32_t tachometer = rotations_to_meters(vesc_comm_get_tachometer(vesc_packet) / 6);

    uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if (button_1_down_elapsed > TRIP_RESET_TIME) {
        // reset trip distance
        eeprom_write_trip_distance(0);
        initial_trip_distance = -tachometer;
    }

    // dim trip distance if it's about to be reset
    uint16_t trip_distance_brightness = 255 - min(255.0 * button_1_down_elapsed / TRIP_RESET_TIME, 255);
    uint16_t trip_distance_color = display_make_color(
            trip_distance_brightness, trip_distance_brightness, trip_distance_brightness);

    int32_t trip_distance = initial_trip_distance + tachometer;
    int32_t total_distance = initial_total_distance + tachometer;
    display_set_trip_distance(trip_distance, trip_distance_color);
    display_set_total_distance(total_distance);

    float speed_percent = 1.0 * kph / MAX_SPEED_KPH;
    display_update_speed_indicator(speed_percent, should_redraw);

    float fully_discharged_mah = BATTERY_MAX_MAH * (1 - BATTERY_USABLE_CAPACITY);
    float mah_percent = 1.0 * (mah - fully_discharged_mah) / (BATTERY_MAX_MAH - fully_discharged_mah);
    float volts_percent = voltage_to_percent(volts);
    float battery_percent = VOLTAGE_WEIGHT * volts_percent + (1.0 - VOLTAGE_WEIGHT) * mah_percent;
    display_update_battery_indicator(battery_percent, should_redraw);

    D("volts = " + String(100 * volts_percent) + "%");
    D("mah = " + String(100 * mah_percent) + "%");
    D("mean = " + String(100 * battery_percent) + "%");

    if (fault_code != FAULT_CODE_NONE)
        display_set_warning(vesc_fault_code_to_string(fault_code));

    bool came_to_stop = (last_rpm != 0 && rpm == 0);
    bool traveled_enough_distance = (total_distance - last_eeprom_written_total_distance >= EEPROM_UPDATE_EACH_METERS);
    if (traveled_enough_distance || (came_to_stop && millis() - last_eeprom_update_on_stop > EEPROM_UPDATE_MIN_DELAY_ON_STOP)) {
        if (came_to_stop)
            last_eeprom_update_on_stop = millis();
        last_eeprom_written_total_distance = total_distance;
        eeprom_write_volts(volts);
        eeprom_write_mah_spent(mah_spent);
        eeprom_write_trip_distance(trip_distance);
        eeprom_write_total_distance(total_distance);
    }

    last_fault_code = fault_code;
    last_rpm = rpm;

    display_indicate_read_success(UPDATE_DELAY);  // some delay between requests is necessary
}
