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
#include "data.h"
#include "roxie_eeprom.h"
#include "util.h"
#include "screen.h"
#include "vesc_comm.h"

#define REVISION_ID ""
#define FW_VERSION "v1.0"

#ifdef DEBUG
#define D(x) Serial.println(x)
#else
#define D(x)
#endif

#define BUTTON_1_PIN A0
#define BUTTON_2_PIN A1
#define BUTTON_3_PIN A2


unsigned long starting_time;
volatile byte button1_down = LOW;
volatile byte button2_down = LOW;
volatile byte button3_down = LOW;

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

/*     bounce1.attach (BUTTON_1_PIN, INPUT_PULLUP );
    bounce1.interval(BOUNCE_TIMEOUT);
    //bounce2.attach (BUTTON_2_PIN, INPUT_PULLUP );
    //bounce2.interval(BOUNCE_TIMEOUT);
    bounce3.attach (BUTTON_3_PIN, INPUT_PULLUP );
    bounce3.interval(BOUNCE_TIMEOUT); */
    //pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    //pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), button1_pressed, FALLING);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), button2_pressed, FALLING);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3_PIN), button3_pressed, FALLING);

#ifdef DEBUG
    Serial.begin(115200);
#endif
    vesc_comm.init(115200);

    eeprom_initialize(EEPROM_MAGIC_VALUE, session_data);

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
    read_buttons();

    vesc_comm.fetch_packet();

    if (!vesc_comm.is_expected_packet()) {
        scr->heartbeat(UPDATE_DELAY, false);
        return;
    }

    vesc_comm.process_packet(&data);

    // TODO: DRY
    int32_t vesc_mah_spent = VESC_COUNT * (vesc_comm.get_amphours_discharged() -
                                           vesc_comm.get_amphours_charged());
    int32_t mah_spent = initial_mah_spent + vesc_mah_spent;
    int32_t mah = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah_spent;

    // uint32_t button_2_down_elapsed = millis() - button_2_last_up_time;
    if (button2_down) {
        D("button 2 pressed");
        button2_down = LOW;
        // reset coulomb counter
        mah = voltage_to_percent(data.voltage) * BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY;
        mah_spent = BATTERY_MAX_MAH * BATTERY_USABLE_CAPACITY - mah;
        eeprom_write_mah_spent(mah_spent);
        initial_mah_spent = mah_spent - vesc_mah_spent;
    }

    data.mah = mah;
    data.mah_spent = vesc_comm.get_amphours_discharged();

    // dim mAh if the counter is about to be reset
    // data.mah_reset_progress = min(1.0 * button_2_down_elapsed / COUNTER_RESET_TIME, 1.0);

    int32_t rpm = vesc_comm.get_rpm();
    data.speed_kph = max(erpm_to_kph(rpm), 0);

    int32_t tachometer = rotations_to_meters(vesc_comm.get_tachometer() / 6);

    // uint32_t button_1_down_elapsed = millis() - button_1_last_up_time;
    if (button1_down) {
        D("button 1 pressed");
        button1_down = LOW;
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
    // data.session_reset_progress = min(1.0 * button_1_down_elapsed / COUNTER_RESET_TIME, 1.0);

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

    starting_time = millis();
    scr->update(&data);
    scr->heartbeat(UPDATE_DELAY, true);
    //Serial.println("Redrawing screen took " + String(millis()-starting_time) + " milliseconds)");
}

void read_buttons(){

    if(button3_down){
        button3_down = LOW;
        D("Button 3 pressed " + String(millis()-starting_time) + " milliseconds");
        scr = screens[current_screen_index];
        scr->nextScreen();
        scr->reset();
    }

}


// ISR routines for button presses
void button1_pressed(){
    button1_down = HIGH;
}

void button2_pressed(){
    button2_down = HIGH;
}

void button3_pressed(){
    button3_down = HIGH;
}