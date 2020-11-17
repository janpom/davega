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
#include "vertical_screen.h"

#ifdef FOCBOX_UNITY
#include "vesc_comm_unity.h"
VescCommUnity vesc_comm = VescCommUnity();
#else
#include "vesc_comm_standard.h"
VescCommStandard vesc_comm = VescCommStandard();
#endif

VerticalScreen vertical_screen;
t_data data;
t_screen_config screen_config;

void setup() {
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), button1_changed, CHANGE);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), button2_pressed, FALLING);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3_PIN), button3_pressed, FALLING);

    // Initialize communication with computer for debugging and with vesc
    Serial.begin(115200);
    vesc_comm.init(115200);

    screen_config = { make_fw_version(FW_VERSION, REVISION_ID), IMPERIAL_UNITS, USE_FAHRENHEIT,
                    SHOW_AVG_CELL_VOLTAGE, BATTERY_S, SCREEN_ORIENTATION};
    vertical_screen = VerticalScreen();

    // Initialize the screen and draw te basic interface
    vertical_screen.init(&screen_config);
    vertical_screen.draw_basic();
}

void loop() {
    //read_buttons(session_data, &initial_trip_meters, &tachometer, screen);

    vesc_comm.fetch_packet();
    if (!vesc_comm.is_expected_packet()) {
        vertical_screen.heartbeat(UPDATE_DELAY, false);
        return;
    }
    vesc_comm.process_packet(&data);

    // DEBUG("Current : " + String(data.rpm));

    vertical_screen.update(&data);
    vertical_screen.heartbeat(UPDATE_DELAY, true);

}