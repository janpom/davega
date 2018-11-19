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

#include "davega_config.h"
#include "davega_screen.h"
#include "davega_default_screen.h"
#include "davega_eeprom.h"
#include "vesc_comm.h"

#define REVISION_ID "$Id$"
#define FW_VERSION "master"

//#define DEBUG
#ifdef DEBUG
#define D(x) Serial.println(x)
#else
#define D(x)
#endif

#define BUTTON_1_PIN A3
#define BUTTON_2_PIN A2
#define BUTTON_3_PIN A1

#define TFT_RST 12
#define TFT_RS  9
#define TFT_CS  10  // SS
#define TFT_SDI 11  // MOSI
#define TFT_CLK 13  // SCK
#define TFT_LED 0

#define LEN(X) (sizeof(X) / sizeof(X[0]))

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, 200);

t_davega_screen_config screen_config = {
    IMPERIAL_UNITS,
    SHOW_AVG_CELL_VOLTAGE,
    BATTERY_S,
};

DavegaDefaultScreen screen0 = DavegaDefaultScreen(&tft, &screen_config);
DavegaScreen* screens[] = {&screen0};
int current_screen_index = 0;
DavegaScreen* scr = screens[current_screen_index];

const float discharge_ticks[] = DISCHARGE_TICKS;

uint8_t vesc_packet[PACKET_MAX_LENGTH];

int16_t initial_mah_spent;
int32_t initial_trip_distance;
int32_t initial_total_distance;
int32_t last_eeprom_written_total_distance;
int32_t last_eeprom_update_on_stop;

int32_t last_rpm;
vesc_comm_fault_code last_fault_code;
uint32_t button_1_last_up_time = 0;
uint32_t button_2_last_up_time = 0;

char fw_version_buffer[6];

int32_t rotations_to_meters(int32_t rotations) {
    float gear_ratio = float(WHEEL_PULLEY_TEETH) / float(MOTOR_PULLEY_TEETH);
    return (rotations / MOTOR_POLE_PAIRS / gear_ratio) * WHEEL_CIRCUMFERENCE_MM / 1000;
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

bool was_battery_fully_charged(float last_volts, float current_volts) {
    float max_volts = discharge_ticks[LEN(discharge_ticks) - 1];
    return (
            ((current_volts - last_volts) / current_volts > FULL_CHARGE_MIN_INCREASE) &&
            (current_volts / max_volts > FULL_CHARGE_THRESHOLD)
    );
}

char* fw_version() {
    if (FW_VERSION[0] == 'v') {
        return FW_VERSION;
    }
    else {
        String r = String("r");
        String upper_rev_id = String(REVISION_ID).substring(5, 9);
        upper_rev_id.toUpperCase();
        r.concat(upper_rev_id);
        r.toCharArray(fw_version_buffer, sizeof(fw_version_buffer));
        return fw_version_buffer;
    }
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
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);

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

    tft.begin();
    tft.setOrientation(0);
    tft.setBackgroundColor(COLOR_BLACK);

    scr->reset();
    scr->set_fw_version(fw_version());
    scr->update_battery_indicator(0.0, true);
    scr->update_speed_indicator(0.0, true);
    scr->set_volts(eeprom_read_volts());
    scr->set_mah(BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - eeprom_read_mah_spent());
    scr->set_trip_distance(eeprom_read_trip_distance());
    scr->set_total_distance(eeprom_read_total_distance());
    scr->set_speed(0);

    uint8_t bytes_read = vesc_comm_fetch_packet(vesc_packet);
    while (!vesc_comm_is_expected_packet(vesc_packet, bytes_read)) {
        scr->indicate_read_failure(UPDATE_DELAY);  // some delay between requests is necessary
        bytes_read = vesc_comm_fetch_packet(vesc_packet);
    }

    float last_volts = eeprom_read_volts();
    float current_volts = vesc_comm_get_voltage(vesc_packet);
    if (was_battery_fully_charged(last_volts, current_volts)) {
        // reset mAh spent
        eeprom_write_mah_spent(0);
        eeprom_write_volts(current_volts);
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

    if (digitalRead(BUTTON_2_PIN) == HIGH)
        button_2_last_up_time = millis();

    uint8_t bytes_read = vesc_comm_fetch_packet(vesc_packet);

    if (!vesc_comm_is_expected_packet(vesc_packet, bytes_read)) {
        scr->indicate_read_failure(UPDATE_DELAY);  // some delay between requests is necessary
        return;
    }

    vesc_comm_fault_code fault_code = vesc_comm_get_fault_code(vesc_packet);
    bool should_redraw = (fault_code == FAULT_CODE_NONE && last_fault_code != FAULT_CODE_NONE);

    if (should_redraw) {
        scr->reset();
        scr->set_fw_version(fw_version());
    }

    float volts = vesc_comm_get_voltage(vesc_packet);
    scr->set_volts(volts);

    // TODO: DRY
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm_get_amphours_discharged(vesc_packet) -
                                           vesc_comm_get_amphours_charged(vesc_packet));
    int32_t mah_spent = initial_mah_spent + vesc_mah_spent;
    int32_t mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah_spent;

    uint32_t button_2_down_elapsed = millis() - button_2_last_up_time;
    if (button_2_down_elapsed > COUNTER_RESET_TIME) {
        // reset coulomb counter
        mah = voltage_to_percent(volts) * BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY;
        mah_spent = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah;
        eeprom_write_mah_spent(mah_spent);
        initial_mah_spent = mah_spent - vesc_mah_spent;
    }

    scr->set_mah(mah);
    // dim mAh if the counter is about to be reset
    scr->set_mah_reset_progress(min(1.0 * button_2_down_elapsed / COUNTER_RESET_TIME, 1.0));

    int32_t rpm = vesc_comm_get_rpm(vesc_packet);
    float kph = max(erpm_to_kph(rpm), 0);
    scr->set_speed((int) (kph + 0.5));

    int32_t tachometer = rotations_to_meters(vesc_comm_get_tachometer(vesc_packet) / 6);

    uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if (button_1_down_elapsed > COUNTER_RESET_TIME) {
        // reset trip distance
        eeprom_write_trip_distance(0);
        initial_trip_distance = -tachometer;
    }

    int32_t trip_distance = initial_trip_distance + tachometer;
    int32_t total_distance = initial_total_distance + tachometer;
    scr->set_trip_distance(trip_distance);
    // dim trip distance if it's about to be reset
    scr->set_mah_reset_progress(min(1.0 * button_1_down_elapsed / COUNTER_RESET_TIME, 1.0));
    scr->set_total_distance(total_distance);

    float speed_percent = 1.0 * kph / MAX_SPEED_KPH;
    scr->update_speed_indicator(speed_percent, should_redraw);

    float mah_percent = 1.0 * mah / (BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY);
    float volts_percent = voltage_to_percent(volts);
    float battery_percent = VOLTAGE_WEIGHT * volts_percent + (1.0 - VOLTAGE_WEIGHT) * mah_percent;
    scr->update_battery_indicator(battery_percent, should_redraw);

    D("volts = " + String(100 * volts_percent) + "%");
    D("mah = " + String(100 * mah_percent) + "%");
    D("mean = " + String(100 * battery_percent) + "%");

    if (fault_code != FAULT_CODE_NONE)
        scr->set_warning(vesc_fault_code_to_string(fault_code));

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

    scr->indicate_read_success(UPDATE_DELAY);  // some delay between requests is necessary
}
